#pragma once

#include "Camera/Camera.h"
#include "Components/Mesh.h"
#include "Shaders/Shader.h"
#include "Components/TextureList.h"

#include <entt/entt.hpp>

#include <memory>

class DrawMeshesSystem {
public:
    explicit DrawMeshesSystem(entt::registry& registry, Shader* shader, entt::entity currentCamera);

    void SetCurrentCamera(entt::entity camera);

    void Update(float deltaTime);

private:
    void _bindTextures(TextureList& textureList);
    void _drawNonTexturedMeshes();
    void _drawTexturedMeshes();
    void _drawMesh(Mesh& mesh, Transform& transform);

    entt::entity _currentCameraToRender{};
    entt::registry& _registry;
    Shader* _shader;
};