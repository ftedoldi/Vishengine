#include "DrawMeshesSystem.h"

DrawMeshesSystem::DrawMeshesSystem(entt::registry& registry, Shader* const shader, const entt::entity currentCamera)
    : _currentCameraToRender{currentCamera}, _registry{registry}, _shader{shader}{
}

void DrawMeshesSystem::SetCurrentCamera(const entt::entity camera) {
    _currentCameraToRender = camera;
}

void DrawMeshesSystem::Update(float) {
    assert(&_currentCameraToRender);

    _drawTexturedMeshes();
    _drawNonTexturedMeshes();
}

void DrawMeshesSystem::_bindTextures(TextureList& textureList) {
    assert(_shader);
    _shader->UseProgram();

    for (unsigned i{0}; i < textureList.Textures.size(); ++i) {
        _shader->SetUniformInt("texture" + std::to_string(i), i);
        textureList.Textures.at(i).bindTexture(i);
    }
}

void DrawMeshesSystem::_drawNonTexturedMeshes() {
    auto view{_registry.view<Mesh, Transform>()};
    for(const auto& [_, mesh, transform]: view.each()) {
        _drawMesh(mesh, transform);
    }
}

void DrawMeshesSystem::_drawTexturedMeshes() {
    auto view{_registry.view<Mesh, Transform, TextureList>()};

    for(const auto& [_, mesh, transform, textureList]: view.each()) {
        _bindTextures(textureList);
        _drawMesh(mesh, transform);
    }
}

void DrawMeshesSystem::_drawMesh(Mesh& mesh, Transform& transform) {
    const auto& cameraComponents{_registry.get<Camera, Transform>(_currentCameraToRender)};
    auto& camera{std::get<Camera&>(cameraComponents)};
    const auto& cameraTransform{std::get<Transform&>(cameraComponents)};

    auto viewTransform{transform.CumulateWith(cameraTransform.Inverse())};

    assert(_shader);
    _shader->SetUniformVec3("Translation", viewTransform.Translation);
    _shader->SetUniformQuat("Rotation", viewTransform.Rotation);
    _shader->SetUniformMat4("Perspective", camera.PerspectiveMatrix);

    glBindVertexArray(mesh.Vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.Indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
