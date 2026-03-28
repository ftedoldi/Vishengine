#pragma once

#include "Editor/EditorLayer.h"
#include "Platform/Framebuffer.h"

#include "GLFW/glfw3.h"
#include <entt/entt.hpp>

#include <filesystem>
#include <memory>

/**
 * @brief Owns the ImGui context and drives the EditorLayer each frame.
 *
 * Correct per-frame call order (from Game::Update):
 *   1. BeginFrame()          — clear screen, call ImGui::NewFrame()
 *   2. _rendererSystem->Update() — render scene into offscreen framebuffer
 *   3. DrawUI(registry)      — render all editor panels (reads framebuffer texture)
 *   4. Render()              — ImGui::Render() + RenderDrawData
 */
class GUIDrawer {
public:
    GUIDrawer(GLFWwindow* window,
              const std::shared_ptr<Framebuffer>& sceneFramebuffer,
              const std::filesystem::path& assetsRoot);

    /** Step 1: clear the default framebuffer and start a new ImGui frame. */
    void BeginFrame();

    /** Step 3: build all ImGui panel draw-lists (call AFTER the renderer). */
    void DrawUI(entt::registry& registry);

    /** Step 4: submit ImGui draw-lists to OpenGL. */
    void Render();

    void Clear();

    EditorLayer& GetEditorLayer() { return _editorLayer; }

private:
    EditorLayer _editorLayer;
};
