#pragma once

#include "Controllers/FramebuffersController.h"
#include "Editor/EditorLayer.h"

#include "GLFW/glfw3.h"
#include <entt/entt.hpp>

#include <filesystem>

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
              const FramebuffersController* framebuffersController,
              const std::filesystem::path& assetsRoot);

    ~GUIDrawer();

    /** Step 1: clear the default framebuffer and start a new ImGui frame. */
    void BeginFrame();

    /** Step 3: build all ImGui panel draw-lists (call AFTER the renderer). */
    void DrawUI(entt::dispatcher& dispatcher, entt::registry& registry);

    /** Step 4: submit ImGui draw-lists to OpenGL. */
    void Render();

private:
    EditorLayer _editorLayer;
};
