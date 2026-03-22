#include "RendererSystem.h"

#include "Components/Lights/DirectionalLight.h"
#include "Components/Lights/PointLight.h"
#include "Components/Mesh.h"
#include "Components/Position.h"
#include "Components/Relationship.h"
#include "Components/Rotation.h"
#include "Components/Scale.h"
#include "Components/WorldTransform.h"

#include "Components/Camera/EditorCameraTag.h"
#include "Math/Math.h"

RendererSystem::RendererSystem(Shader* const shader)
    : _shader{shader}{
}

void RendererSystem::Update(float, entt::registry& registry, const MaterialController& materialController, const MeshController& meshController) const {
    assert(_shader);
    _shader->UseProgram();

    std::unordered_map<uint32_t, std::vector<Transform>> meshesTransforms{};

    // If the mesh has no material it means it is an instance that needs to be drawn using its world location.
    auto meshView{registry.view<Mesh, WorldTransform>()};
    for (const auto& [entity, mesh, worldTransform] : meshView.each()) {
        meshesTransforms[mesh.meshID].push_back(worldTransform.Value);
    }

    // If a mesh has a materiali it represents the actual geometry.
    auto meshMaterialView{registry.view<Mesh, Material>()};

    auto cameraView{registry.view<Camera, Position, Rotation, Scale, EditorCameraTag>()};
    for(const auto& [cameraEntity, camera, cameraPosition, cameraRotation, cameraScale]: cameraView.each()) {
        _shader->SetUniformMat4("Perspective", camera.ProjectionMatrix);

        Transform worldSpaceCameraTransform{cameraPosition.Vector, cameraRotation.Quaternion, cameraScale.Value};

        for (const auto& [meshEntity, mesh, material] : meshMaterialView.each()) {
            const auto& materialData{materialController.GetMaterialData(material.materialID)};
            _setUniformColors(materialData.ColorDiffuse, materialData.ColorSpecular);
            _bindTextures(materialData.TexturesDiffuse, materialData.TexturesSpecular, materialData.TexturesNormal);

            // Transform all instances of this mesh from world to view space.
            const auto& instanceWorldTransforms{meshesTransforms.at(mesh.meshID)};
            std::vector<Transform> viewTransforms{};
            viewTransforms.reserve(instanceWorldTransforms.size());
            for (const auto& worldTransform : instanceWorldTransforms) {
                viewTransforms.push_back(camera.ViewTransform.Cumulate(worldTransform));
            }

            const auto& meshData{meshController.GetMeshData(mesh.meshID)};
            _drawMesh(viewTransforms, meshData.MeshGpuData, meshData.RawMeshData.Indices);
        }

        // TODO: pass here the view transform directly.
        _drawLights(worldSpaceCameraTransform, registry);
    }
}

void RendererSystem::_bindTextures(const std::vector<Texture>& diffuseTextures, const std::vector<Texture>& specularTextures, const std::vector<Texture>&) const {
    assert(_shader);
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

    /*for (unsigned i{0}; i < normalTextures.size(); ++i) {
        _shader->SetUniformInt("TextureNormal" + std::to_string(i), i);
        normalTextures.at(i).BindTexture(i);
    }*/
}

void RendererSystem::_drawMesh(const std::vector<Transform>& instanceTransforms, const MeshGpuData& gpuData, const std::vector<uint32_t>& indices) const {
    assert(_shader);

    std::vector<InstanceData> instanceData{};
    instanceData.reserve(instanceTransforms.size());
    for (const auto& t : instanceTransforms) {
        instanceData.push_back({
            t.Position,
            t.Scale,
            glm::vec4{t.Rotation.x, t.Rotation.y, t.Rotation.z, t.Rotation.w},
        });
    }

    // Upload instance data to the instance VBO (orphan + re-upload each frame)
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

void RendererSystem::_drawLights(const Transform& cameraTransform, entt::registry& registry) const {
    _drawDirectionalLights(cameraTransform, registry);
    _drawPointLights(cameraTransform, registry);
}

void RendererSystem::_drawPointLights(const Transform& cameraTransform, entt::registry& registry) const {
    assert(_shader);
    auto view{registry.view<PointLight, Position>()};

    view.each([this, &cameraTransform = std::as_const(cameraTransform)](const PointLight& pointLight, const Position& lightPosition) {
        const auto invertedCameraTransform{cameraTransform.Invert()};

        const auto lightViewPosition{Math::RotateVectorByQuaternion(invertedCameraTransform.Rotation, lightPosition.Vector)};

        _shader->SetUniformVec3("pointLight.Position", lightViewPosition + invertedCameraTransform.Position);
        _shader->SetUniformVec3("pointLight.Diffuse", pointLight.Diffuse);
        _shader->SetUniformVec3("pointLight.Ambient", pointLight.Ambient);
        _shader->SetUniformVec3("pointLight.Specular", pointLight.Specular);

        _shader->SetUniformFloat("pointLight.Constant", pointLight.Constant);
        _shader->SetUniformFloat("pointLight.Linear", pointLight.Linear);
        _shader->SetUniformFloat("pointLight.Quadratic", pointLight.Quadratic);
    });
}

void RendererSystem::_drawDirectionalLights(const Transform& cameraTransform, entt::registry& registry) const {
    assert(_shader);

    auto view{registry.view<DirectionalLight>()};

    view.each([this, &cameraTransform = std::as_const(cameraTransform)](const DirectionalLight& dirLight) {
        const auto invertedCameraTransform{cameraTransform.Invert()};
        const auto viewDirection{Math::RotateVectorByQuaternion(invertedCameraTransform.Rotation, dirLight.Direction)};

        _shader->SetUniformVec3("dirLight.Direction", viewDirection);

        _shader->SetUniformVec3("dirLight.Diffuse", dirLight.Diffuse);
        _shader->SetUniformVec3("dirLight.Ambient", dirLight.Ambient);
        _shader->SetUniformVec3("dirLight.Specular", dirLight.Specular);
    });
}

void RendererSystem::_setUniformColors(const glm::vec4& colorDiffuse, const glm::vec3& colorSpecular) const {
    assert(_shader);

    _shader->SetUniformVec4("DiffuseColor", colorDiffuse);
    _shader->SetUniformVec3("SpecularColor", colorSpecular);
}
