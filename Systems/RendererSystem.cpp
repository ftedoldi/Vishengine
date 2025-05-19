#include "RendererSystem.h"

#include "Components/MeshObject.h"
#include "Components/Lights/PointLight.h"
#include "Components/Lights/DirectionalLight.h"
#include "Components/Position.h"
#include "Components/Rotation.h"
#include "Components/Scale.h"
#include "Components/Relationship.h"

#include "Math/Math.h"

RendererSystem::RendererSystem(entt::registry& registry, Shader* const shader, const entt::entity currentCamera)
    : _currentCameraToRender{currentCamera}, _registry{registry}, _shader{shader}{
}

void RendererSystem::SetCurrentCamera(const entt::entity camera) {
    _currentCameraToRender = camera;
}

void RendererSystem::Update(float) {
    assert(_shader);
    _shader->UseProgram();

    const auto& cameraComponents{_registry.get<Camera, Position, Rotation, Scale>(_currentCameraToRender)};

    const auto& camera{std::get<Camera&>(cameraComponents)};
    // TODO: this is hardcoded find a better way
    _shader->SetUniformMat4("Perspective", camera.ProjectionMatrix);

    const auto& cameraPosition{std::get<Position&>(cameraComponents)};
    const auto& cameraRotation{std::get<Rotation&>(cameraComponents)};
    const auto& cameraScale{std::get<Scale&>(cameraComponents)};

    Transform cameraTransform{cameraPosition.Vector, cameraRotation.Quaternion, cameraScale.Value};

    _drawMeshes(cameraTransform);
    _drawLights(cameraTransform);
}

void RendererSystem::_bindTextures(const Mesh& mesh) {
    assert(_shader);
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

void RendererSystem::_drawMeshes(const Transform& cameraTransform) {
    auto view{_registry.view<MeshObject, Position, Rotation, Scale>()};

    for(const auto& [meshEntity, meshObject, position, rotation, scale]: view.each()) {
        for(const auto& mesh : meshObject.Meshes) {
            _setUniformColors(*mesh);
            _bindTextures(*mesh);

            //glm::vec3 vecFbx{position.Vector.x / 100.f, position.Vector.y / 100.f, position.Vector.z / 100.f};
            //Transform meshTransform{vecFbx, rotation.Quaternion, scale.Value / 100.f};

            Transform meshTransform{position.Vector, rotation.Quaternion, scale.Value};
            auto worldTransform{_calculateWorldTransform(meshEntity, meshTransform)};

            _drawMesh(*mesh, worldTransform, cameraTransform);
        }
    }
}

void RendererSystem::_drawMesh(const Mesh& mesh, const Transform& meshTransform, const Transform& cameraTransform) {
    // TODO: be able to also draw on a camera with an orthogonal matrix when needed.
    const auto viewTransform{_calculateViewTransform(meshTransform, cameraTransform)};

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

void RendererSystem::_drawLights(const Transform& cameraTransform) {
    _drawDirectionalLights(cameraTransform);
    _drawPointLights(cameraTransform);
}

void RendererSystem::_drawPointLights(const Transform& cameraTransform) {
    assert(_shader);
    auto view{_registry.view<PointLight, Position>()};

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

void RendererSystem::_drawDirectionalLights(const Transform& cameraTransform) {
    assert(_shader);

    auto view{_registry.view<DirectionalLight>()};

    view.each([this, &cameraTransform = std::as_const(cameraTransform)](const DirectionalLight dirLight) {
        const auto invertedCameraTransform{cameraTransform.Invert()};
        const auto viewDirection{Math::RotateVectorByQuaternion(invertedCameraTransform.Rotation, dirLight.Direction)};

        _shader->SetUniformVec3("dirLight.Direction", viewDirection);

        _shader->SetUniformVec3("dirLight.Diffuse", dirLight.Diffuse);
        _shader->SetUniformVec3("dirLight.Ambient", dirLight.Ambient);
        _shader->SetUniformVec3("dirLight.Specular", dirLight.Specular);
    });
}

void RendererSystem::_setUniformColors(const Mesh& mesh) {
    assert(_shader);

    _shader->SetUniformVec4("DiffuseColor", mesh.GetColorDiffuse());
    _shader->SetUniformVec3("SpecularColor", mesh.GetColorSpecular());
}

Transform RendererSystem::_calculateWorldTransform(const entt::entity entity, const Transform& transform) {
    auto& entityRelationship{_registry.get<Relationship>(entity)};

    if(entityRelationship.parent == entt::null) {
        return transform;
    }

    auto& position{_registry.get<Position>(entityRelationship.parent)};
    auto& rotation{_registry.get<Rotation>(entityRelationship.parent)};
    auto& scale{_registry.get<Scale>(entityRelationship.parent)};

    Transform parentTransform{position.Vector, rotation.Quaternion, scale.Value};

    const auto cumulatedTransform{transform.Cumulate(parentTransform)};

    return _calculateWorldTransform(entityRelationship.parent, cumulatedTransform);
}

Transform RendererSystem::_calculateViewTransform(const Transform& transform, const Transform& cameraTransform) {
    const auto inverseCameraTransform{cameraTransform.Invert()};

    return transform.Cumulate(inverseCameraTransform);
}
