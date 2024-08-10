#include "ImGuiHandlerSystem.h"

#include "Components/Window.h"

#include "Components/MeshObject.h"
#include "Components/Position.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

ImGuiHandlerSystem::ImGuiHandlerSystem(entt::registry &registry, const entt::entity windowEntity) :_registry{registry}, _window{windowEntity} {
    auto& window{_registry.get<Window>(_window)};

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window.GlfwWindow, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
}


void ImGuiHandlerSystem::StartFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    //ImGui::ShowDemoWindow(); // Show demo window! :)

    auto view{_registry.view<MeshObject, Position>()};

    if (ImGui::TreeNode("Mesh hierarchy"))
    {
        bool firstTimeInLoop{true};
        uint32_t id{};
        view.each([&firstTimeInLoop, &id](const MeshObject& meshObject, const Position& position){
            // Use SetNextItemOpen() so set the default state of a node to be open. We could
            // also use TreeNodeEx() with the ImGuiTreeNodeFlags_DefaultOpen flag to achieve the same thing!
            if (firstTimeInLoop) {
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                firstTimeInLoop = false;
            }

            if (ImGui::TreeNode((void*)(intptr_t)id, "Mesh"))
            {
                ImGui::Text("Position: %f, %f, %f", position.Vector.x, position.Vector.y, position.Vector.z);
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

void ImGuiHandlerSystem::Render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiHandlerSystem::Clear() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
