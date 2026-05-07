#include "RendererSystem.h"

#include "Components/BoundingBox.h"
#include "Platform/Framebuffer.h"

#include "Components/Camera/Camera.h"
#include "Components/Camera/EditorCameraTag.h"
#include "Components/InstancedMeshTag.h"
#include "Components/Lights/DirectionalLight.h"
#include "Components/Lights/PointLight.h"
#include "Components/Mesh.h"
#include "Components/Relationship.h"
#include "Components/RenderableTag.h"
#include "Components/RenderingComponents.h"
#include "Components/Transforms/RelativeTransform.h"
#include "Components/Transforms/WorldTransform.h"
#include "Coordinates/CoordinateUtils.h"
#include "RenderingComponents/LineDrawer.h"
#include "Shaders/Shader.h"

#include <glad/gl.h>

namespace {

void DrawMesh(const std::vector<Transform>& instanceTransforms,
              const MeshGpuData& gpuData,
              const std::vector<uint32_t>& indices) {
    std::vector<InstanceData> instanceData{};
    instanceData.reserve(instanceTransforms.size());
    for (const auto& t : instanceTransforms) {
        instanceData.push_back({
            t.Position,
            t.Scale,
            glm::vec4{t.Rotation.x, t.Rotation.y, t.Rotation.z, t.Rotation.w},
        });
    }

    const auto dataSize{static_cast<GLsizeiptr>(instanceData.size() * sizeof(InstanceData))};
    glNamedBufferData(gpuData.InstanceVbo, dataSize, instanceData.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(gpuData.Vao);
    glDrawElementsInstanced(GL_TRIANGLES,
                            static_cast<GLsizei>(indices.size()),
                            GL_UNSIGNED_INT,
                            nullptr,
                            static_cast<GLsizei>(instanceTransforms.size()));
    glBindVertexArray(0);
}

}

RendererSystem::RendererSystem(entt::dispatcher& windowDispatcher,
                               MaterialController* const materialController,
                               MeshController* const meshController,
                               ShadersController* const shadersController,
                               FramebuffersController* const framebuffersController)
    : _materialController{materialController},
      _meshController{meshController},
      _shadersController{shadersController},
      _framebuffersController{framebuffersController} {
    windowDispatcher.sink<WindowsEvents::FrameBufferSizeChangedEvent>().connect<&RendererSystem::_onFramebufferSizeChanged>(this);
}

void RendererSystem::AddPass(std::unique_ptr<IRenderPass> pass) {
    _passes.push_back(std::move(pass));
}

void RendererSystem::Update(entt::registry& registry) const {
    std::unordered_map<uint32_t, std::vector<Transform>> transformsByMeshID{};
    const auto allMeshView{registry.view<Mesh, Relationship>()};
    for (const auto& [meshEntity, mesh, relationship] : allMeshView.each()) {
        // The parent of the mesh in the abstract mesh object that contains the transform.
        if (!registry.all_of<RenderableTag>(relationship.Parent)) {
            continue;
        }
        const auto& worldTransform{registry.get<WorldTransform>(relationship.Parent).Value};
        transformsByMeshID[mesh.meshID].push_back(worldTransform);
    }

    const auto viewEntities{registry.view<Camera, RenderTarget>()};
    for (const auto viewEntity : viewEntities) {
        const auto renderTarget{registry.get<RenderTarget>(viewEntity)};
        const auto* const framebuffer{_framebuffersController->GetFramebuffer(renderTarget.FramebufferHandle)};
        assert(framebuffer);
        framebuffer->Bind();

        for (const auto pass : renderTarget.Passes) {
            if (pass.RenderLayers.Layers.test(static_cast<size_t>(RenderLayer::SceneMeshes))) {
                _drawSceneMeshes(viewEntity, pass.ShaderHandle, registry, transformsByMeshID);
            }

            if (pass.RenderLayers.Layers.test(static_cast<size_t>(RenderLayer::DebugFrustumIntersections))) {
                _drawDebugFrustumIntersections(viewEntity, pass.ShaderHandle, registry);
            }
        }
    }

    for (const auto& pass : _passes) {
        pass->Execute();
    }
}

void RendererSystem::_drawSceneMeshes(const entt::entity viewEntity,
                                    const ShaderID shaderId,
                                    entt::registry& registry,
                                    const std::unordered_map<uint32_t, std::vector<Transform>>& transformsByMeshID) const {
    const auto& camera{registry.get<Camera>(viewEntity)};

    const auto* const shader{_shadersController->GetShader(shaderId)};
    assert(shader);
    shader->UseProgram();
    shader->SetUniformMat4("Perspective", camera.ProjectionMatrix);

    if (registry.all_of<LitPassTag>(viewEntity)) {
        if (const auto* const cameraWorld{registry.try_get<WorldTransform>(viewEntity)}) {
            _setupLighting(shader, cameraWorld->Value, registry);
        }
    }

    const auto actualMeshView{registry.view<Mesh, Relationship>(entt::exclude<InstancedMeshTag>)};
    for (const auto& [meshEntity, mesh, relationship] : actualMeshView.each()) {
        const auto it{transformsByMeshID.find(mesh.meshID)};
        if (it == transformsByMeshID.end()) {
            continue;
        }
        const auto& instanceWorldTransforms{it->second};

        const auto& materialData{_materialController->GetMaterialData(mesh.meshID)};
        shader->SetUniformVec4("DiffuseColor", materialData.ColorDiffuse);
        shader->SetUniformVec3("SpecularColor", materialData.ColorSpecular);
        _bindTextures(shader, materialData.TexturesDiffuse, materialData.TexturesSpecular);

        std::vector<Transform> viewTransforms{};
        viewTransforms.reserve(instanceWorldTransforms.size());
        for (const auto& wt : instanceWorldTransforms) {
            viewTransforms.push_back(camera.ViewTransform.Cumulate(wt));
        }

        const auto& meshData{_meshController->GetMeshData(mesh.meshID)};
        DrawMesh(viewTransforms, meshData.MeshGpuData, meshData.RawMeshData.Indices);
    }
}

void RendererSystem::_drawDebugFrustumIntersections(const entt::entity viewEntity, const ShaderID shaderId, entt::registry& registry) const {
    const auto& debugFrustumCamera{registry.get<Camera>(viewEntity)};

    const auto* const shader{_shadersController->GetShader(shaderId)};
    shader->UseProgram();
    shader->SetUniformMat4("Perspective", debugFrustumCamera.ProjectionMatrix);

    const auto editorCamView{registry.view<Camera, EditorCameraTag>()};

    editorCamView.each([&debugFrustumCamera, &registry](entt::entity, const Camera& editorCamera) {
        auto insideBatch{LineDrawer::Get().BeginBatch(debugFrustumCamera, {1.f, 0.f, 0.f})};
        auto outsideBatch{LineDrawer::Get().BeginBatch(debugFrustumCamera, {0.f, 1.f, 0.f})};

        for (auto entity : registry.view<BoundingBox, WorldTransform>()) {
            const auto worldBox = CoordUtils::ComputeWorldSpaceBox(entity, registry);
            auto& target{FrustumUtils::IsAABBInsideFrustum(worldBox, editorCamera.ViewFrustum)
                           ? insideBatch : outsideBatch};
            target.AddBox(worldBox.Min, worldBox.Max);
        }
    });
}

void RendererSystem::_bindTextures(const Shader* const shader,
                                   const std::vector<Texture>& diffuseTextures,
                                   const std::vector<Texture>& specularTextures) const {
    int currentTextureIndex{0};

    shader->SetBool("HasTextureDiffuse", !diffuseTextures.empty());
    shader->SetBool("HasTextureSpecular", !specularTextures.empty());

    for (unsigned i{0}; i < diffuseTextures.size(); ++i) {
        shader->SetUniformInt("TextureDiffuse" + std::to_string(i), currentTextureIndex);
        diffuseTextures.at(i).BindTexture(currentTextureIndex++);
    }
    for (unsigned i{0}; i < specularTextures.size(); ++i) {
        shader->SetUniformInt("TextureSpecular" + std::to_string(i), currentTextureIndex);
        specularTextures.at(i).BindTexture(currentTextureIndex++);
    }
}

void RendererSystem::_setupLighting(const Shader* const shader,
                                    const Transform& cameraWorldTransform,
                                    entt::registry& registry) const {
    const auto pointView{registry.view<PointLight, RelativeTransform>()};
    pointView.each([shader, &cameraWorldTransform = std::as_const(cameraWorldTransform)]
                   (const PointLight& pointLight, const RelativeTransform& relativeTransform) {
        const auto invertedCameraTransform{cameraWorldTransform.Invert()};
        const auto lightViewPosition{CoordUtils::RotateVectorByQuaternion(
            invertedCameraTransform.Rotation, relativeTransform.Value.Position)};

        shader->SetUniformVec3("pointLight.Position", lightViewPosition + invertedCameraTransform.Position);
        shader->SetUniformVec3("pointLight.Diffuse",  pointLight.Diffuse);
        shader->SetUniformVec3("pointLight.Ambient",  pointLight.Ambient);
        shader->SetUniformVec3("pointLight.Specular", pointLight.Specular);

        shader->SetUniformFloat("pointLight.Constant",  pointLight.Constant);
        shader->SetUniformFloat("pointLight.Linear",    pointLight.Linear);
        shader->SetUniformFloat("pointLight.Quadratic", pointLight.Quadratic);
    });

    const auto dirView{registry.view<DirectionalLight>()};
    dirView.each([shader, &cameraWorldTransform = std::as_const(cameraWorldTransform)]
                 (const DirectionalLight& dirLight) {
        const auto invertedCameraTransform{cameraWorldTransform.Invert()};
        const auto viewDirection{CoordUtils::RotateVectorByQuaternion(
            invertedCameraTransform.Rotation, dirLight.Direction)};

        shader->SetUniformVec3("dirLight.Direction", viewDirection);
        shader->SetUniformVec3("dirLight.Diffuse",   dirLight.Diffuse);
        shader->SetUniformVec3("dirLight.Ambient",   dirLight.Ambient);
        shader->SetUniformVec3("dirLight.Specular",  dirLight.Specular);
    });
}

void RendererSystem::_onFramebufferSizeChanged(const WindowsEvents::FrameBufferSizeChangedEvent frameBufferSizeChangedEvent) const {
    // TODO: is this what should really happens as if I have many different framebuffers, do I want to do this?
    // probably not correct because I can have framebuffers in panels that are not docked with the main window.
    _framebuffersController->ResizeAll(static_cast<int32_t>(frameBufferSizeChangedEvent.Width),
                                       static_cast<int32_t>(frameBufferSizeChangedEvent.Height));
}