#include "GUIDrawer.h"

#include "Components/Mesh.h"
#include "Components/Position.h"
#include "Components/Relationship.h"
#include "Components/Rotation.h"
#include "Components/Scale.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

GUIDrawer::GUIDrawer(GLFWwindow* const window) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
}


void GUIDrawer::StartFrame(entt::registry& registry) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    //ImGui::ShowDemoWindow(); // Show demo window! :)

    auto view{registry.view<Position, Rotation, Scale, Relationship>()};

    if (ImGui::TreeNode("Mesh hierarchy")) {
        bool firstTimeInLoop{true};
        uint32_t id{};
        view.each([&firstTimeInLoop, &id](const Position& pos, const auto& rot, const auto& scale, const Relationship& rel) {
            // Use SetNextItemOpen() so set the default state of a node to be open. We could
            // also use TreeNodeEx() with the ImGuiTreeNodeFlags_DefaultOpen flag to achieve the same thing!
            if (firstTimeInLoop) {
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                firstTimeInLoop = false;
            }

            if (ImGui::TreeNode((void*)(intptr_t)id, "Mesh")) {
                ImGui::Text("Position: %f, %f, %f", pos.Vector.x, pos.Vector.y, pos.Vector.z);
                ImGui::SameLine();
                if (ImGui::SmallButton("button")) {}
                ImGui::TreePop();
                id++;
            }

        });
        ImGui::TreePop();
    }

    //ImGui::Checkbox("Demo Window", &show_demo_window);
}

void GUIDrawer::Render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUIDrawer::Clear() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
