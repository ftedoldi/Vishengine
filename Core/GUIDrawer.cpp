#include "GUIDrawer.h"

#include "Editor/EditorStyle.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

#include <glad/gl.h>

GUIDrawer::GUIDrawer(GLFWwindow* const window,
                     std::shared_ptr<Framebuffer> sceneFramebuffer,
                     const std::filesystem::path& assetsRoot)
    : _editorLayer{std::move(sceneFramebuffer), assetsRoot}
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io{ImGui::GetIO()};
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    EditorStyle::ApplyDarkTheme();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}

void GUIDrawer::BeginFrame() {
    // Bind the default framebuffer and clear it so the editor background
    // is a solid dark colour.  The scene is rendered into the offscreen
    // framebuffer AFTER this call, before DrawUI().
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GUIDrawer::DrawUI(entt::registry& registry) {
    // The scene has already been rendered into the offscreen framebuffer by
    // this point, so ScenePanel::OnRender() will sample the correct texture.
    _editorLayer.OnRender(registry);
}

void GUIDrawer::Render() {
    // Make sure we are rendering ImGui onto the default framebuffer.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUIDrawer::Clear() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
