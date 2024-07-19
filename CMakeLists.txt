cmake_minimum_required(VERSION "3.28.1")
project(Vishengine)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(Libs/glfw)

option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(Libs/assimp)

add_subdirectory(Libs/entt)

find_package(OpenGL REQUIRED)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 ")
    set (CMAKE_CXX_STANDARD 20)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++20")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

add_definitions(-DGLM_FORCE_QUAT_DATA_XYZW)

include_directories(Libs/assimp/include/
                    Libs/glfw/include/
                    Libs/glad/include/
                    Libs/stb/
                    Libs/glm/
                    ${CMAKE_SOURCE_DIR})

file(GLOB LIBS_SOURCES Libs/glad/src/gl.c)

file(GLOB PROJECT_HEADERS Core/*.h
        Shaders/*.h
        Texture/*.h
        Platform/*.h
        Components/*.h Components/EventComponents/*.h Components/CameraComponents/*.h
        Systems/*.h
        InputManager/*.h)

file(GLOB PROJECT_SOURCES
        Core/*.cpp
        Shaders/*.cpp
        Texture/*.cpp
        Platform/*.cpp
        Components/*.cpp Components/CameraComponents/*.cpp
        Systems/*.cpp
        InputManager/*.cpp
        ./*.cpp)

file(GLOB PROJECT_SHADERS Shaders/GlslShaders/*.comp
                          Shaders/GlslShaders/*.frag
                          Shaders/GlslShaders/*.geom
                          Shaders/GlslShaders/*.vert)
file(GLOB PROJECT_CONFIGS CMakeLists.txt
        Readme.md
        .gitattributes
        .gitignore
        .gitmodules)

add_definitions(-DGLFW_INCLUDE_NONE
        -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS}
        ${PROJECT_SHADERS} ${PROJECT_CONFIGS}
        ${LIBS_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE Libs/entt/src)

target_link_libraries(${PROJECT_NAME} assimp glfw EnTT::EnTT
        ${GLFW_LIBRARIES} ${GLAD_LIBRARIES} ${OPENGL_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>
        DEPENDS ${PROJECT_SHADERS})