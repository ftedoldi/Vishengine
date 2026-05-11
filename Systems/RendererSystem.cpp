#include "RendererSystem.h"

#include "Components/BoundingBox.h"
#include "Components/Camera/Camera.h"
#include "Components/Camera/EditorCameraTag.h"
#include "Components/Lights/DirectionalLight.h"
#include "Components/Lights/PointLight.h"
#include "Components/Mesh.h"
#include "Components/RenderableTag.h"
#include "Components/RenderingComponents.h"
#include "Components/Transforms/WorldTransform.h"
#include "Coordinates/CoordinateUtils.h"
#include "Platform/Framebuffer.h"
#include "RenderingComponents/LineDrawer.h"
#include "ScopedTimer.h"
#include "Shaders/Shader.h"

#include <glad/gl.h>

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
    const auto viewEntities{registry.view<Camera, RenderTarget>()};
    for (const auto viewEntity : viewEntities) {
        const auto& renderTarget{registry.get<RenderTarget>(viewEntity)};
        const auto* const framebuffer{_framebuffersController->GetFramebuffer(renderTarget.FramebufferHandle)};
        assert(framebuffer);
        framebuffer->Bind();

        for (const auto& pass : renderTarget.Passes) {
            if (pass.RenderLayers.Layers.test(static_cast<size_t>(RenderLayer::SceneMeshes))) {
                _drawSceneMeshes(viewEntity, pass.ShaderHandle, registry);
            }
            if (pass.RenderLayers.Layers.test(static_cast<size_t>(RenderLayer::DebugFrustumIntersections))) {
                _drawDebugFrustumIntersections(viewEntity, pass.ShaderHandle, registry);
            }
        }
    }
}

void RendererSystem::_drawSceneMeshes(const entt::entity viewEntity,
                                      const ShaderID shaderId,
                                      entt::registry& registry) {
    const auto& camera{registry.get<Camera>(viewEntity)};

    const auto* const shader{_shadersController->GetShader(shaderId)};
    assert(shader);
    shader->UseProgram();
    shader->SetUniformMat4("Perspective", camera.ProjectionMatrix);

    if (registry.all_of<LitPassTag>(viewEntity)) {
        _setupLighting(shader, camera.ViewTransform, registry);
    }

    const auto meshGroup = registry.group<Mesh, WorldTransform, RenderableTag>();

    std::unordered_map<uint32_t, std::vector<InstanceData>> perMeshData{};

    perMeshData.reserve(meshGroup.size());

    auto makeInstanceData = [](const auto& vt) {
        return InstanceData{
            glm::vec4{vt.Position, vt.Scale},
            glm::vec4{vt.Rotation.x, vt.Rotation.y, vt.Rotation.z, vt.Rotation.w}
        };
    };

    {
        PROFILE_SCOPE("BuildInstances");
        meshGroup.each([&](const Mesh& mesh, const WorldTransform& meshWorldTransform) {
            auto& vec = perMeshData[mesh.MeshID];
            vec.push_back(makeInstanceData(camera.ViewTransform.Cumulate(meshWorldTransform.Value)));
        });
    }

    {
        PROFILE_SCOPE("DrawLoop");
        using clock = std::chrono::steady_clock;
        clock::duration uniformsTotal{}, uploadTotal{}, drawTotal{};
        size_t totalInstances{0};
        size_t uploadedBytes{0};
        for (const auto& [meshID, viewTransforms] : perMeshData) {
            const auto& materialData = _materialController->GetMaterialData(meshID);

            const auto t0 = clock::now();
            shader->SetUniformVec4("DiffuseColor", materialData.ColorDiffuse);
            shader->SetUniformVec3("SpecularColor", materialData.ColorSpecular);
            _bindTextures(shader, materialData.TexturesDiffuse, materialData.TexturesSpecular);

            const auto t1 = clock::now();
            const auto& gpuData{_meshController->GetMeshGpuData(meshID)};
            const auto dataSize{static_cast<GLsizeiptr>(viewTransforms.size() * sizeof(InstanceData))};
            glNamedBufferData(gpuData.InstanceSsbo,
                                 dataSize,
                                 viewTransforms.data(),
                                 GL_DYNAMIC_DRAW);

            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, gpuData.InstanceSsbo);

            const auto t2 = clock::now();
            glBindVertexArray(gpuData.Vao);
            glDrawElementsInstanced(GL_TRIANGLES,
                                    static_cast<GLsizei>(gpuData.IndexCount),
                                    GL_UNSIGNED_INT,
                                    nullptr,
                                    static_cast<GLsizei>(viewTransforms.size()));
            const auto t3 = clock::now();

            uniformsTotal += t1 - t0;
            uploadTotal   += t2 - t1;
            drawTotal     += t3 - t2;
            totalInstances += viewTransforms.size();
            uploadedBytes += static_cast<size_t>(dataSize);
        }
        const auto us = [](auto d) {
            return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
        };
        std::cout << "  uniqueMeshes=" << perMeshData.size()
                  << " totalInstances=" << totalInstances
                  << " uploadedBytes=" << uploadedBytes
                  << " | Uniforms+Tex=" << us(uniformsTotal) << "us"
                  << " Upload=" << us(uploadTotal) << "us"
                  << " Draw=" << us(drawTotal) << "us\n";
    }
    glBindVertexArray(0);
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
            auto& target{FrustumUtils::IntersectsFrustum(worldBox, editorCamera.ViewFrustum)
                           ? insideBatch : outsideBatch};
            target.AddBox(worldBox.Min, worldBox.Max);
        }
    });
}

void RendererSystem::_bindTextures(const Shader* const shader,
                                   const std::vector<std::shared_ptr<Texture>>& diffuseTextures,
                                   const std::vector<std::shared_ptr<Texture>>& specularTextures) {
    int currentTextureIndex{0};

    shader->SetBool("HasTextureDiffuse", !diffuseTextures.empty());
    shader->SetBool("HasTextureSpecular", !specularTextures.empty());

    for (unsigned i{0}; i < diffuseTextures.size(); ++i) {
        shader->SetUniformInt("TextureDiffuse" + std::to_string(i), currentTextureIndex);
        diffuseTextures.at(i)->BindTexture(currentTextureIndex++);
    }
    for (unsigned i{0}; i < specularTextures.size(); ++i) {
        shader->SetUniformInt("TextureSpecular" + std::to_string(i), currentTextureIndex);
        specularTextures.at(i)->BindTexture(currentTextureIndex++);
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
        shader->SetUniformVec3(p + "Position", cameraViewTransform.Cumulate(lightWorldTransform.Value).Position);
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
    _framebuffersController->ResizeAll(static_cast<int32_t>(frameBufferSizeChangedEvent.Width),
                                       static_cast<int32_t>(frameBufferSizeChangedEvent.Height));
}