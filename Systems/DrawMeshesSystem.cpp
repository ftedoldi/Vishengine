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

    _drawMeshes();
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

void DrawMeshesSystem::_drawMeshes() {
    auto view{_registry.view<MeshObject, Transform>()};

    for(const auto& [_, meshObject, transform]: view.each()) {
        for(auto& mesh : meshObject.Meshes) {
            _setUniformColors(*mesh);
            _bindTextures(*mesh);
            _drawMesh(*mesh, transform);
        }
    }
}

void DrawMeshesSystem::_drawMesh(const Mesh& mesh, Transform& transform) {
    // TODO: be able to also draw on a camera with an orthogonal matrix when needed.
    const auto& cameraComponents{_registry.get<Camera, Transform, Perspective>(_currentCameraToRender)};

    const auto& cameraTransform{std::get<Transform&>(cameraComponents)};
    const auto& perspective{std::get<Perspective&>(cameraComponents)};

    auto viewTransform{transform.CumulateWith(cameraTransform.Inverse())};

    assert(_shader);
    _shader->SetUniformVec3("Translation", viewTransform.Translation);
    _shader->SetUniformQuat("Rotation", viewTransform.Rotation);
    _shader->SetUniformMat4("Perspective", perspective.Matrix);
    _shader->SetBool("HasTextureDiffuse", mesh.GetHasTextureDiffuse());
    _shader->SetBool("HasTextureSpecular", mesh.GetHasTextureSpecular());

    glBindVertexArray(mesh.Vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.Indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void DrawMeshesSystem::_setUniformColors(const Mesh& mesh) {
    _shader->SetUniformVec4("DiffuseColor", mesh.GetColorDiffuse());
    _shader->SetUniformVec3("SpecularColor", mesh.GetColorSpecular());
}
