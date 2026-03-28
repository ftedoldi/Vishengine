#include "GUIDrawer.h"

#include "Editor/EditorStyle.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

#include <glad/gl.h>

GUIDrawer::GUIDrawer(GLFWwindow* const window,
                     const std::shared_ptr<Framebuffer>& sceneFramebuffer,
                     const std::filesystem::path& assetsRoot)
    : _editorLayer{sceneFramebuffer, assetsRoot}
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
    // Disable depth test so screen-space quad isn't discarded due to depth test.
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUIDrawer::Clear() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
