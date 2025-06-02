#include "RendererSystem.h"

#include "Components/Lights/DirectionalLight.h"
#include "Components/Lights/PointLight.h"
#include "Components/Mesh.h"
#include "Components/Position.h"
#include "Components/Relationship.h"
#include "Components/Rotation.h"
#include "Components/Scale.h"

#include "Components/Camera/EditorCameraTag.h"
#include "Math/Math.h"

namespace RS::Detail {

    Transform CalculateViewTransform(const Transform& transform, const Transform& cameraTransform) {
        const auto inverseCameraTransform{cameraTransform.Invert()};

        return transform.Cumulate(inverseCameraTransform);
    }

}

RendererSystem::RendererSystem(Shader* const shader)
    : _shader{shader}{
}

void RendererSystem::Update(float, entt::registry& registry, const MaterialController& materialController, const MeshController& meshController) {
    assert(_shader);
    _shader->UseProgram();

    auto cameraView{registry.view<Camera, Position, Rotation, Scale, EditorCameraTag>()};
    for(const auto entity: cameraView) {
        const auto& cameraComponent{cameraView.get<Camera>(entity)};
        const auto& positionComponent{cameraView.get<Position>(entity)};
        const auto& rotationComponent{cameraView.get<Rotation>(entity)};
        const auto& scaleComponent{cameraView.get<Scale>(entity)};
        _shader->SetUniformMat4("Perspective", cameraComponent.ProjectionMatrix);

        Transform worldSpaceCameraTransform{positionComponent.Vector, rotationComponent.Quaternion, scaleComponent.Value};
        auto meshView{registry.view<Mesh, Material>()};

        for(const auto& [meshEntity, mesh, material]: meshView.each()) {
            const auto& materialData{materialController.GetMaterialData(material.materialID)};

            _setUniformColors(materialData.ColorDiffuse, materialData.ColorSpecular);
            _bindTextures(materialData.TexturesDiffuse, materialData.TexturesSpecular, materialData.TexturesNormal);

            const auto worldSpaceMeshTransform{_calculateWorldTransform(meshEntity, registry, Transform{{0,0,0}})};
            const auto meshViewTransform{RS::Detail::CalculateViewTransform(worldSpaceMeshTransform, worldSpaceCameraTransform)};

            const auto& meshData{meshController.GetMeshData(mesh.meshID)};
            _drawMesh(meshViewTransform, meshData.MeshGpuData.Vao, meshData.RawMeshData.Indices);
        }

        _drawLights(worldSpaceCameraTransform, registry);
    }
}

void RendererSystem::_bindTextures(const std::vector<Texture>& diffuseTextures, const std::vector<Texture>& specularTextures, const std::vector<Texture>&) {
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

void RendererSystem::_drawMesh(const Transform& meshViewTransform, const uint32_t meshVao, const std::vector<uint32_t>& indices) {
    assert(_shader);
    _shader->SetUniformVec3("ViewPosition", meshViewTransform.Position);
    _shader->SetUniformQuat("ViewRotation", meshViewTransform.Rotation);
    _shader->SetUniformFloat("Scale", meshViewTransform.Scale);

    glBindVertexArray(meshVao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void RendererSystem::_drawLights(const Transform& cameraTransform, entt::registry& registry) {
    _drawDirectionalLights(cameraTransform, registry);
    _drawPointLights(cameraTransform, registry);
}

void RendererSystem::_drawPointLights(const Transform& cameraTransform, entt::registry& registry) {
    assert(_shader);
    auto view{registry.view<PointLight, Position>()};

    view.each([this, &cameraTransform = std::as_const(cameraTransform)](const PointLight pointLight, const Position lightPosition) {
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

void RendererSystem::_drawDirectionalLights(const Transform& cameraTransform, entt::registry& registry) {
    assert(_shader);

    auto view{registry.view<DirectionalLight>()};

    view.each([this, &cameraTransform = std::as_const(cameraTransform)](const DirectionalLight dirLight) {
        const auto invertedCameraTransform{cameraTransform.Invert()};
        const auto viewDirection{Math::RotateVectorByQuaternion(invertedCameraTransform.Rotation, dirLight.Direction)};

        _shader->SetUniformVec3("dirLight.Direction", viewDirection);

        _shader->SetUniformVec3("dirLight.Diffuse", dirLight.Diffuse);
        _shader->SetUniformVec3("dirLight.Ambient", dirLight.Ambient);
        _shader->SetUniformVec3("dirLight.Specular", dirLight.Specular);
    });
}

void RendererSystem::_setUniformColors(const glm::vec4& colorDiffuse, const glm::vec3& colorSpecular) {
    assert(_shader);

    _shader->SetUniformVec4("DiffuseColor", colorDiffuse);
    _shader->SetUniformVec3("SpecularColor", colorSpecular);
}

Transform RendererSystem::_calculateWorldTransform(const entt::entity entity, entt::registry& registry, const Transform& transform) {
    auto& entityRelationship{registry.get<Relationship>(entity)};

    if(entityRelationship.parent == entt::null) {
        return transform;
    }

    auto& position{registry.get<Position>(entityRelationship.parent)};
    auto& rotation{registry.get<Rotation>(entityRelationship.parent)};
    auto& scale{registry.get<Scale>(entityRelationship.parent)};

    Transform parentTransform{position.Vector, rotation.Quaternion, scale.Value};

    const auto cumulatedTransform{transform.Cumulate(parentTransform)};

    return _calculateWorldTransform(entityRelationship.parent, registry, cumulatedTransform);
}
