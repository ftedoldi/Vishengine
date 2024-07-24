#include "DrawMeshesSystem.h"

#include "Components/CameraComponents/Perspective.h"
#include "Components/MeshObject.h"

DrawMeshesSystem::DrawMeshesSystem(entt::registry& registry, Shader* const shader, const entt::entity currentCamera)
    : _currentCameraToRender{currentCamera}, _registry{registry}, _shader{shader}{
}

void DrawMeshesSystem::SetCurrentCamera(const entt::entity camera) {
    _currentCameraToRender = camera;
}

void DrawMeshesSystem::Update(float) {
    assert(&_currentCameraToRender);

    _drawTexturedMeshes();
    //_drawNonTexturedMeshes();
}

void DrawMeshesSystem::_bindTextures(const Mesh& mesh) {
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

void DrawMeshesSystem::_drawNonTexturedMeshes() {
    auto view{_registry.view<Mesh, Transform>()};
    for(const auto& [_, mesh, transform]: view.each()) {
        _drawMesh(mesh, transform);
    }
}

void DrawMeshesSystem::_drawTexturedMeshes() {
    auto view{_registry.view<MeshObject, Transform>()};

    for(const auto& [_, meshObject, transform]: view.each()) {
        for(auto& mesh : meshObject.Meshes) {
            _bindTextures(*mesh);
            _drawMesh(*mesh, transform);
        }
    }
}

void DrawMeshesSystem::_drawMesh(Mesh& mesh, Transform& transform) {
    // TODO: be able to also draw on a camera with an orthogonal matrix when needed.
    const auto& cameraComponents{_registry.get<Camera, Transform, Perspective>(_currentCameraToRender)};

    const auto& cameraTransform{std::get<Transform&>(cameraComponents)};
    const auto& perspective{std::get<Perspective&>(cameraComponents)};

    auto viewTransform{transform.CumulateWith(cameraTransform.Inverse())};

    assert(_shader);
    _shader->SetUniformVec3("Translation", viewTransform.Translation);
    _shader->SetUniformQuat("Rotation", viewTransform.Rotation);
    _shader->SetUniformMat4("Perspective", perspective.Matrix);

    glBindVertexArray(mesh.Vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.Indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
