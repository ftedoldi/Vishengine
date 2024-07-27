#pragma once

#include "Components/CameraComponents/Camera.h"
#include "Components/Mesh.h"
#include "Components/TextureList.h"
#include "Shaders/Shader.h"

#include <entt/entt.hpp>

#include <memory>

class DrawMeshesSystem {
public:
    explicit DrawMeshesSystem(entt::registry& registry, Shader* shader, entt::entity currentCamera);

    void SetCurrentCamera(entt::entity camera);

    void Update(float deltaTime);

private:
    void _bindTextures(const Mesh& mesh);
    void _drawMeshes();
    void _drawMesh(const Mesh& mesh, Transform& transform);

    void _setUniformColors(const Mesh& mesh);

    entt::entity _currentCameraToRender{};
    entt::registry& _registry;
    Shader* _shader;
};