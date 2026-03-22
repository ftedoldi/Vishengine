cmake_minimum_required(VERSION 3.28)
project(Vishengine)

# -----------------------------
# Options for dependencies
# -----------------------------
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)

option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)

# -----------------------------
# Dependencies
# -----------------------------
add_subdirectory(Libs/glfw)
add_subdirectory(Libs/assimp)
add_subdirectory(Libs/entt)

find_package(OpenGL REQUIRED)

# -----------------------------
# GLAD
# -----------------------------
add_library(glad Libs/glad/src/gl.c)
target_include_directories(glad PUBLIC Libs/glad/include)

if(NOT WIN32)
    target_link_libraries(glad PUBLIC dl)
endif()

# -----------------------------
# ImGui
# -----------------------------
add_library(imgui
        Libs/imgui/imgui.cpp
        Libs/imgui/imgui_draw.cpp
        Libs/imgui/imgui_tables.cpp
        Libs/imgui/imgui_widgets.cpp
)

target_include_directories(imgui PUBLIC Libs/imgui)

target_sources(imgui PRIVATE
        Libs/imgui/backends/imgui_impl_glfw.cpp
        Libs/imgui/backends/imgui_impl_opengl3.cpp
)

target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

# -----------------------------
# Project files
# -----------------------------
file(GLOB PROJECT_HEADERS
        Camera/*.h
        Core/*.h
        Utils/*.h
        Math/*.h
        Shaders/*.h
        Material/*.h
        Platform/*.h
        Components/*.h
        Components/Camera/*.h
        Components/Lights/*.h
        Systems/*.h
        Mesh/*.h
        InputManager/*.h
)

file(GLOB PROJECT_SOURCES
        Camera/*.cpp
        Core/*.cpp
        Utils/*.cpp
        Math/*.cpp
        Shaders/*.cpp
        Material/*.cpp
        Platform/*.cpp
        Components/*.cpp
        Components/Camera/*.cpp
        Components/Lights/*.cpp
        Systems/*.cpp
        Mesh/*.cpp
        InputManager/*.cpp
        ./*.cpp
)

file(GLOB PROJECT_SHADERS
        Shaders/GlslShaders/*.comp
        Shaders/GlslShaders/*.frag
        Shaders/GlslShaders/*.geom
        Shaders/GlslShaders/*.vert
)

file(GLOB PROJECT_CONFIGS
        CMakeLists.txt
        Readme.md
        .gitattributes
        .gitignore
        .gitmodules
)

# -----------------------------
# Executable
# -----------------------------
add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
        ${PROJECT_SHADERS}
        ${PROJECT_CONFIGS}
        Components/WorldTransform.h
        Systems/TransformSystem.cpp
        Systems/TransformSystem.h
        Systems/CameraSystem.cpp
        Systems/CameraSystem.h
        Components/InstancedMesh.h
)

# -----------------------------
# Include directories
# -----------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}
        Libs/glfw/include
        Libs/assimp/include
        Libs/stb
        Libs/glm
        Libs/entt/src
)

# -----------------------------
# Compiler settings
# -----------------------------
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# -----------------------------
# Definitions
# -----------------------------
target_compile_definitions(${PROJECT_NAME} PRIVATE
        GLFW_INCLUDE_NONE
        PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}"
        GLM_FORCE_QUAT_DATA_XYZW
)

# -----------------------------
# Linking
# -----------------------------
target_link_libraries(${PROJECT_NAME} PRIVATE
        assimp
        glfw
        EnTT::EnTT
        imgui
        glad
        OpenGL::GL
)

# -----------------------------
# Output directory
# -----------------------------
set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
)

# -----------------------------
# Copy shaders after build
# -----------------------------
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        DEPENDS ${PROJECT_SHADERS}
)