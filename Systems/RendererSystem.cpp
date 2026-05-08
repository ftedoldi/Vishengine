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

#include <algorithm>
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

void RendererSystem::Update(entt::registry& registry) {
    _meshInstances.clear();
    const auto allMeshView{registry.view<Mesh, Relationship>(entt::exclude<InstancedMeshTag>)};
    for (const auto& [meshEntity, mesh, relationship] : allMeshView.each()) {
        _meshInstances.push_back({
            mesh.meshID,
            registry.get<WorldTransform>(relationship.Parent).Value,
            registry.all_of<RenderableTag>(relationship.Parent),
        });
    }
    std::ranges::sort(_meshInstances,
              [](const MeshInstance& a, const MeshInstance& b){ return a.meshID < b.meshID; });

    const auto viewEntities{registry.view<Camera, RenderTarget>()};
    for (const auto viewEntity : viewEntities) {
        const auto renderTarget{registry.get<RenderTarget>(viewEntity)};
        const auto* const framebuffer{_framebuffersController->GetFramebuffer(renderTarget.FramebufferHandle)};
        assert(framebuffer);
        framebuffer->Bind();

        for (const auto pass : renderTarget.Passes) {
            if (pass.RenderLayers.Layers.test(static_cast<size_t>(RenderLayer::SceneMeshes))) {
                _drawSceneMeshes(viewEntity, pass.ShaderHandle, registry, pass.Meshes);
            }
            if (pass.RenderLayers.Layers.test(static_cast<size_t>(RenderLayer::DebugFrustumIntersections))) {
                _drawDebugFrustumIntersections(viewEntity, pass.ShaderHandle, registry);
            }
        }
    }
}

void RendererSystem::_drawSceneMeshes(const entt::entity viewEntity,
                                    const ShaderID shaderId,
                                    entt::registry& registry,
                                    const MeshSet meshSet) {
    const auto& camera{registry.get<Camera>(viewEntity)};

    const auto* const shader{_shadersController->GetShader(shaderId)};
    assert(shader);
    shader->UseProgram();
    shader->SetUniformMat4("Perspective", camera.ProjectionMatrix);

    if (registry.all_of<LitPassTag>(viewEntity)) {
        _setupLighting(shader, camera.ViewTransform, registry);
    }

    std::vector<Transform> viewTransforms{};
    auto it{_meshInstances.cbegin()};
    while (it != _meshInstances.cend()) {
        const uint32_t currentMeshID{it->meshID};

        auto groupEnd{it};
        while (groupEnd != _meshInstances.cend() && groupEnd->meshID == currentMeshID) {
            ++groupEnd;
        }

        viewTransforms.clear();
        for (auto cur{it}; cur != groupEnd; ++cur) {
            if (meshSet == MeshSet::Visible && !cur->renderable) {
                continue;
            }
            viewTransforms.push_back(camera.ViewTransform.Cumulate(cur->transform));
        }

        if (!viewTransforms.empty()) {
            const auto& materialData{_materialController->GetMaterialData(currentMeshID)};
            shader->SetUniformVec4("DiffuseColor", materialData.ColorDiffuse);
            shader->SetUniformVec3("SpecularColor", materialData.ColorSpecular);
            _bindTextures(shader, materialData.TexturesDiffuse, materialData.TexturesSpecular);

            const auto& meshData{_meshController->GetMeshData(currentMeshID)};
            DrawMesh(viewTransforms, meshData.MeshGpuData, meshData.RawMeshData.Indices);
        }

        it = groupEnd;
    }
}

void RendererSystem::_drawDebugFrustumIntersections(const entt::entity viewEntity, const ShaderID shaderId, entt::registry& registry) {
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
                                   const std::vector<Texture>& specularTextures) {
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
                                    const Transform& cameraViewTransform,
                                    entt::registry& registry) {
    constexpr int maxPointLights{16};
    int pointLightCount{0};
    const auto pointView{registry.view<PointLight, WorldTransform>()};
    pointView.each([shader, &pointLightCount, &cameraViewTransform](const PointLight& pointLight, const WorldTransform& lightWorldTransform) {
        if (pointLightCount >= maxPointLights) {
            return;
        }
        const std::string p{"pointLights[" + std::to_string(pointLightCount) + "]."};
        shader->SetUniformVec3(p + "Position", lightWorldTransform.Value.Cumulate(cameraViewTransform).Position);
        shader->SetUniformVec3(p + "Diffuse",  pointLight.Diffuse);
        shader->SetUniformVec3(p + "Ambient",  pointLight.Ambient);
        shader->SetUniformVec3(p + "Specular", pointLight.Specular);
        shader->SetUniformFloat(p + "Constant",  pointLight.Constant);
        shader->SetUniformFloat(p + "Linear",    pointLight.Linear);
        shader->SetUniformFloat(p + "Quadratic", pointLight.Quadratic);
        ++pointLightCount;
    });
    shader->SetUniformInt("NumPointLights", pointLightCount);

    constexpr int maxDirLights{4};
    int dirLightCount{0};
    const auto dirView{registry.view<DirectionalLight>()};
    dirView.each([shader, &dirLightCount, &cameraViewTransform](const DirectionalLight& dirLight) {
        if (dirLightCount >= maxDirLights) {
            return;
        }
        const std::string p{"dirLights[" + std::to_string(dirLightCount) + "]."};
        const auto viewDir{CoordUtils::RotateVectorByQuaternion(cameraViewTransform.Rotation, dirLight.Direction)};
        shader->SetUniformVec3(p + "Direction", viewDir);
        shader->SetUniformVec3(p + "Diffuse",   dirLight.Diffuse);
        shader->SetUniformVec3(p + "Ambient",   dirLight.Ambient);
        shader->SetUniformVec3(p + "Specular",  dirLight.Specular);
        ++dirLightCount;
    });
    shader->SetUniformInt("NumDirLights", dirLightCount);
}

void RendererSystem::_onFramebufferSizeChanged(const WindowsEvents::FrameBufferSizeChangedEvent frameBufferSizeChangedEvent) const {
    // TODO: is this what should really happens as if I have many different framebuffers, do I want to do this?
    // probably not correct because I can have framebuffers in panels that are not docked with the main window.
    _framebuffersController->ResizeAll(static_cast<int32_t>(frameBufferSizeChangedEvent.Width),
                                       static_cast<int32_t>(frameBufferSizeChangedEvent.Height));
}