#include "RendererSystem.h"

#include "Components/CameraComponents/Perspective.h"
#include "Components/MeshObject.h"
#include "Components/Light.h"
#include "Components/Position.h"
#include "Components/Rotation.h"
#include "Components/Scale.h"

namespace RendererUtils {

Transform CumulateTransforms(const Transform& firstTransform, const Transform& otherTransform) {
    const auto position{otherTransform.Rotation * (firstTransform.Position * otherTransform.Scale) + otherTransform.Position};
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
    assert(&_currentCameraToRender);

    _drawMeshes();
    _drawLights();
}

void RendererSystem::_bindTextures(const Mesh& mesh) {
    assert(_shader);
    _shader->UseProgram();

    for (unsigned i{0}; i < mesh.TexturesDiffuse.size(); ++i) {
        _shader->SetUniformInt("TextureDiffuse" + std::to_string(i), i);
        mesh.TexturesDiffuse.at(i).BindTexture(i);
    }

    /*for (unsigned i{0}; i < mesh.TexturesSpecular.size(); ++i) {
        _shader->SetUniformInt("TextureSpecular" + std::to_string(i), i);
        mesh.TexturesSpecular.at(i).BindTexture(i);
    }

    for (unsigned i{0}; i < mesh.TexturesNormal.size(); ++i) {
        _shader->SetUniformInt("TextureNormal" + std::to_string(i), i);
        mesh.TexturesNormal.at(i).BindTexture(i);
    }*/
}

void RendererSystem::_drawMeshes() {
    auto view{_registry.view<MeshObject, Position, Rotation, Scale>()};

    for(const auto& [_, meshObject, position, rotation, scale]: view.each()) {
        for(auto& mesh : meshObject.Meshes) {
            _setUniformColors(*mesh);
            _bindTextures(*mesh);

            RendererUtils::Transform meshTransform{position.Vector, rotation.Quaternion, scale.Value};
            _drawMesh(*mesh, meshTransform);
        }
    }
}

void RendererSystem::_drawMesh(const Mesh& mesh, const RendererUtils::Transform& meshTransform) {
    // TODO: be able to also draw on a camera with an orthogonal matrix when needed.
    const auto& cameraComponents{_registry.get<Camera, Position, Rotation, Scale, Perspective>(_currentCameraToRender)};

    const auto& cameraPosition{std::get<Position&>(cameraComponents)};
    const auto& cameraRotation{std::get<Rotation&>(cameraComponents)};
    const auto& cameraScale{std::get<Scale&>(cameraComponents)};

    const auto& perspective{std::get<Perspective&>(cameraComponents)};

    RendererUtils::Transform cameraTransform{cameraPosition.Vector, cameraRotation.Quaternion, cameraScale.Value};

    const auto inverseCameraTransform{RendererUtils::InvertTransform(cameraTransform)};
    const auto viewTransform{RendererUtils::CumulateTransforms(meshTransform, inverseCameraTransform)};

    assert(_shader);
    _shader->SetUniformVec3("Translation", viewTransform.Position);
    _shader->SetUniformQuat("Rotation", viewTransform.Rotation);
    // TODO: Set float
    _shader->SetUniformMat4("Perspective", perspective.Matrix);
    _shader->SetBool("HasTextureDiffuse", mesh.GetHasTextureDiffuse());
    _shader->SetBool("HasTextureSpecular", mesh.GetHasTextureSpecular());

    glBindVertexArray(mesh.Vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.Indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void RendererSystem::_drawLights() {
    auto view{_registry.view<Light>()};

    view.each([this](Light& light) {
        _shader->SetUniformVec3("LightColor", light.Diffuse);
    });

}

void RendererSystem::_setUniformColors(const Mesh& mesh) {
    _shader->SetUniformVec4("DiffuseColor", mesh.GetColorDiffuse());
    _shader->SetUniformVec3("SpecularColor", mesh.GetColorSpecular());
}

