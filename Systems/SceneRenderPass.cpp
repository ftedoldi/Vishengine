#include "SceneRenderPass.h"

#include "Components/Camera/Camera.h"
#include "Components/Camera/EditorCameraTag.h"
#include "Components/InstancedMeshTag.h"
#include "Components/Lights/DirectionalLight.h"
#include "Components/Lights/PointLight.h"
#include "Components/Mesh.h"
#include "Components/Relationship.h"
#include "Components/Transforms/RelativeTransform.h"
#include "Components/Transforms/WorldTransform.h"
#include "Math/Math.h"

#include <unordered_map>

namespace {

    void DrawMesh(const std::vector<Transform>& instanceTransforms, const MeshGpuData& gpuData, const std::vector<uint32_t>& indices) {
        // This should somehow optimized by using AOS instead of SOA
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

SceneRenderPass::SceneRenderPass(
    std::unique_ptr<Shader> shader,
    entt::registry& registry,
    const std::shared_ptr<MaterialController>& materialController,
    const std::shared_ptr<MeshController>& meshController)
    : _shader{std::move(shader)}, _registry{registry}, _materialController{materialController}, _meshController {meshController} {}

SceneRenderPass::~SceneRenderPass() {
    _shader->DeleteProgram();
}

void SceneRenderPass::Execute() {
    _render();
}

void SceneRenderPass::_render() const {
    _shader->UseProgram();

    const auto allMeshView{_registry.view<Mesh, Relationship>()};
    std::unordered_map<uint32_t, std::vector<Transform>> transformsByMeshID{};
    for (const auto& [meshEntity, mesh, relationship] : allMeshView.each()) {
        const auto& worldTransform{_registry.get<WorldTransform>(relationship.Parent).Value};
        transformsByMeshID[mesh.meshID].push_back(worldTransform);
    }

    // Loop only on the actual meshes (not the instances).
    const auto actualMeshView{_registry.view<Mesh, Relationship>(entt::exclude<InstancedMeshTag>)};
    auto cameraView{_registry.view<Camera, WorldTransform, EditorCameraTag>()};
    for (const auto& [cameraEntity, camera, worldTransform] : cameraView.each()) {
        _shader->SetUniformMat4("Perspective", camera.ProjectionMatrix);

        for (const auto& [meshEntity, mesh, relationship] : actualMeshView.each()) {
            const auto& materialData{_materialController->GetMaterialData(mesh.meshID)};
            _setUniformColors(materialData.ColorDiffuse, materialData.ColorSpecular);
            _bindTextures(materialData.TexturesDiffuse, materialData.TexturesSpecular, materialData.TexturesNormal);

            // Transform only the instances of THIS mesh from world space to view space.
            const auto& instanceWorldTransforms{transformsByMeshID[mesh.meshID]};
            std::vector<Transform> viewTransforms{};
            viewTransforms.reserve(instanceWorldTransforms.size());
            for (const auto& instanceWorldTransform : instanceWorldTransforms) {
                viewTransforms.push_back(camera.ViewTransform.Cumulate(instanceWorldTransform));
            }

            const auto& meshData{_meshController->GetMeshData(mesh.meshID)};
            DrawMesh(viewTransforms, meshData.MeshGpuData, meshData.RawMeshData.Indices);
        }

        _drawLights(worldTransform.Value, _registry);
    }
}

void SceneRenderPass::_bindTextures(const std::vector<Texture>& diffuseTextures,
                                    const std::vector<Texture>& specularTextures,
                                    const std::vector<Texture>&) const {
    int currentTextureIndex{0};

    _shader->SetBool("HasTextureDiffuse", !diffuseTextures.empty());
    _shader->SetBool("HasTextureSpecular", !specularTextures.empty());

    for (unsigned i{0}; i < diffuseTextures.size(); ++i) {
        _shader->SetUniformInt("TextureDiffuse" + std::to_string(i), currentTextureIndex);
        diffuseTextures.at(i).BindTexture(currentTextureIndex++);
    }

    for (unsigned i{0}; i < specularTextures.size(); ++i) {
        _shader->SetUniformInt("TextureSpecular" + std::to_string(i), currentTextureIndex);
        specularTextures.at(i).BindTexture(currentTextureIndex++);
    }
}

void SceneRenderPass::_drawLights(const Transform& cameraTransform, entt::registry& registry) const {
    _drawDirectionalLights(cameraTransform, registry);
    _drawPointLights(cameraTransform, registry);
}

void SceneRenderPass::_drawPointLights(const Transform& cameraTransform, entt::registry& registry) const {
    assert(_shader);
    auto view{registry.view<PointLight, RelativeTransform>()};

    view.each([this, &cameraTransform = std::as_const(cameraTransform)](const PointLight& pointLight, const RelativeTransform& relativeTransform) {
        const auto invertedCameraTransform{cameraTransform.Invert()};
        const auto lightViewPosition{Math::RotateVectorByQuaternion(invertedCameraTransform.Rotation, relativeTransform.Value.Position)};

        _shader->SetUniformVec3("pointLight.Position", lightViewPosition + invertedCameraTransform.Position);
        _shader->SetUniformVec3("pointLight.Diffuse",  pointLight.Diffuse);
        _shader->SetUniformVec3("pointLight.Ambient",  pointLight.Ambient);
        _shader->SetUniformVec3("pointLight.Specular", pointLight.Specular);

        _shader->SetUniformFloat("pointLight.Constant",  pointLight.Constant);
        _shader->SetUniformFloat("pointLight.Linear",    pointLight.Linear);
        _shader->SetUniformFloat("pointLight.Quadratic", pointLight.Quadratic);
    });
}

void SceneRenderPass::_drawDirectionalLights(const Transform& cameraTransform, entt::registry& registry) const {
    assert(_shader);
    auto view{registry.view<DirectionalLight>()};

    view.each([this, &cameraTransform = std::as_const(cameraTransform)](const DirectionalLight& dirLight) {
        const auto invertedCameraTransform{cameraTransform.Invert()};
        const auto viewDirection{Math::RotateVectorByQuaternion(invertedCameraTransform.Rotation, dirLight.Direction)};

        _shader->SetUniformVec3("dirLight.Direction", viewDirection);
        _shader->SetUniformVec3("dirLight.Diffuse",   dirLight.Diffuse);
        _shader->SetUniformVec3("dirLight.Ambient",   dirLight.Ambient);
        _shader->SetUniformVec3("dirLight.Specular",  dirLight.Specular);
    });
}

void SceneRenderPass::_setUniformColors(const glm::vec4& colorDiffuse, const glm::vec3& colorSpecular) const {
    assert(_shader);
    _shader->SetUniformVec4("DiffuseColor",  colorDiffuse);
    _shader->SetUniformVec3("SpecularColor", colorSpecular);
}


