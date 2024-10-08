#include "RendererSystem.h"

#include "Components/CameraComponents/Perspective.h"
#include "Components/MeshObject.h"
#include "Components/Light.h"
#include "Components/Position.h"
#include "Components/Rotation.h"
#include "Components/Scale.h"
#include "Components/Relationship.h"

namespace RendererUtils {

Transform CumulateTransforms(const Transform& firstTransform, const Transform& otherTransform) {
    const auto position{firstTransform.Rotation * (otherTransform.Position * firstTransform.Scale) + firstTransform.Position};
    const auto rotation{firstTransform.Rotation * otherTransform.Rotation};
    const auto scale{firstTransform.Scale * otherTransform.Scale};

    return Transform{position, rotation, scale};
}

Transform InvertTransform(const Transform& transform) {
    const auto scale{1.f / transform.Scale};
    const auto rotation{glm::inverse(transform.Rotation)};
    const auto position{rotation * (-transform.Position * scale)};

    return Transform{position, rotation, scale};
}
}

RendererSystem::RendererSystem(entt::registry& registry, Shader* const shader, const entt::entity currentCamera)
    : _currentCameraToRender{currentCamera}, _registry{registry}, _shader{shader}{
}

void RendererSystem::SetCurrentCamera(const entt::entity camera) {
    _currentCameraToRender = camera;
}

void RendererSystem::Update(float) {
    assert(_shader);
    _shader->UseProgram();

    const auto& cameraComponents{_registry.get<Camera, Position, Rotation, Scale, Perspective>(_currentCameraToRender)};

    const auto& perspective{std::get<Perspective&>(cameraComponents)};
    _shader->SetUniformMat4("Perspective", perspective.Matrix);

    const auto& cameraPosition{std::get<Position&>(cameraComponents)};
    const auto& cameraRotation{std::get<Rotation&>(cameraComponents)};
    const auto& cameraScale{std::get<Scale&>(cameraComponents)};

    RendererUtils::Transform cameraTransform{cameraPosition.Vector, cameraRotation.Quaternion, cameraScale.Value};

    _drawMeshes(cameraTransform);
    _drawLights(cameraTransform);
}

void RendererSystem::_bindTextures(const Mesh& mesh) {
    int currentTextureIndex{0};

    for (unsigned i{0}; i < mesh.TexturesDiffuse.size(); ++i) {
        _shader->SetUniformInt("TextureDiffuse" + std::to_string(i), currentTextureIndex);
        mesh.TexturesDiffuse.at(i).BindTexture(currentTextureIndex++);
    }

    for (unsigned i{0}; i < mesh.TexturesSpecular.size(); ++i) {
        _shader->SetUniformInt("TextureSpecular" + std::to_string(i), currentTextureIndex);
        mesh.TexturesSpecular.at(i).BindTexture(currentTextureIndex++);
    }

    /*for (unsigned i{0}; i < mesh.TexturesNormal.size(); ++i) {
        _shader->SetUniformInt("TextureNormal" + std::to_string(i), i);
        mesh.TexturesNormal.at(i).BindTexture(i);
    }*/
}

void RendererSystem::_drawMeshes(const RendererUtils::Transform& cameraTransform) {
    auto view{_registry.view<MeshObject, Position, Rotation, Scale>()};

    for(const auto& [meshEntity, meshObject, position, rotation, scale]: view.each()) {
        for(const auto& mesh : meshObject.Meshes) {

            _setUniformColors(*mesh);
            _bindTextures(*mesh);

            glm::vec3 vecFbx{position.Vector.x / 100.f, position.Vector.y / 100.f, position.Vector.z / 100.f};
            RendererUtils::Transform meshTransform{vecFbx, rotation.Quaternion, scale.Value / 100.f};

            //RendererUtils::Transform meshTransform{position.Vector, rotation.Quaternion, scale.Value};
            auto worldTransform{_calculateWorldTransform(meshEntity, meshTransform)};

            _drawMesh(*mesh, worldTransform, cameraTransform);
        }
    }
}

void RendererSystem::_drawMesh(const Mesh& mesh, const RendererUtils::Transform& meshTransform, const RendererUtils::Transform& cameraTransform) {
    // TODO: be able to also draw on a camera with an orthogonal matrix when needed.

    const auto inverseCameraTransform{RendererUtils::InvertTransform(cameraTransform)};
    const auto viewTransform{RendererUtils::CumulateTransforms(inverseCameraTransform, meshTransform)};

    assert(_shader);
    _shader->SetUniformVec3("ViewPosition", viewTransform.Position);
    _shader->SetUniformQuat("ViewRotation", viewTransform.Rotation);
    _shader->SetUniformFloat("Scale", viewTransform.Scale);

    _shader->SetBool("HasTextureDiffuse", mesh.GetHasTextureDiffuse());
    _shader->SetBool("HasTextureSpecular", mesh.GetHasTextureSpecular());

    glBindVertexArray(mesh.Vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.Indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void RendererSystem::_drawLights(const RendererUtils::Transform& cameraTransform) {
    auto view{_registry.view<Light, Position>()};

    view.each([this, &cameraTransform = std::as_const(cameraTransform)](const Light light, const Position lightPosition) {
        _shader->SetUniformVec3("LightColor", light.Diffuse);

        RendererUtils::Transform lightTransform{lightPosition.Vector};
        const auto inverseCameraTransform{RendererUtils::InvertTransform(cameraTransform)};
        const auto viewTransform{RendererUtils::CumulateTransforms(inverseCameraTransform, lightTransform)};

        _shader->SetUniformVec3("LightPosition", viewTransform.Position);
    });
}

void RendererSystem::_setUniformColors(const Mesh& mesh) {
    _shader->SetUniformVec4("DiffuseColor", mesh.GetColorDiffuse());
    _shader->SetUniformVec3("SpecularColor", mesh.GetColorSpecular());
}

RendererUtils::Transform RendererSystem::_calculateWorldTransform(const entt::entity entity, const RendererUtils::Transform& transform) {
    auto& entityRelationship{_registry.get<Relationship>(entity)};

    if(entityRelationship.parent == entt::null) {
        return transform;
    }

    auto& position{_registry.get<Position>(entityRelationship.parent)};
    auto& rotation{_registry.get<Rotation>(entityRelationship.parent)};
    auto& scale{_registry.get<Scale>(entityRelationship.parent)};

    RendererUtils::Transform parentTransform{position.Vector, rotation.Quaternion, scale.Value};

    const auto cumulatedTransform{RendererUtils::CumulateTransforms(transform, parentTransform)};

    return _calculateWorldTransform(entityRelationship.parent, cumulatedTransform);
}
