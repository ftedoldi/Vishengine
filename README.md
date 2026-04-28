# Vishengine

A 3D engine written in modern C++20, built on OpenGL with an
entity–component–system (ECS) architecture and an integrated ImGui-based editor.

<!-- TODO: replace with a screenshot or short GIF of the editor in action -->

## Features

- **OpenGL 4.x renderer** via the [GLAD](https://github.com/Dav1dde/glad) loader
- **Entity–Component–System** architecture powered by [EnTT](https://github.com/skypjack/entt)
- **Integrated editor** built with [Dear ImGui](https://github.com/ocornut/imgui)
  and [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo) for in-viewport
  object manipulation
- **Model loading** through [Assimp](https://github.com/assimp/assimp)
  — supports FBX, OBJ, glTF and many others
- **Texture loading** with [stb_image](https://github.com/nothings/stb)
- **Math library** built on [GLM](https://github.com/g-truc/glm) with additional
  helpers under `Math/`
- **Windowing & input** through [GLFW](https://github.com/glfw/glfw)
- **Component library** covering cameras, lights, transforms, meshes and materials
- **GLSL shader pipeline** with automatic shader copying on build

## Project layout

```
.
├── Assets/               # Models, textures and other runtime assets
├── Camera/               # Camera abstractions
├── Components/           # ECS components (Camera, Lights, Transforms, ...)
├── Core/                 # Engine core (application loop, lifecycle)
├── DataStructures/       # Generic containers and helpers
├── Editor/               # ImGui + ImGuizmo editor layer
├── Events/               # Event system
├── Libs/                 # Third-party dependencies (git submodules)
├── Material/             # Material definitions
├── Math/                 # Math utilities on top of GLM
├── Mesh/                 # Mesh data and loading
├── Platform/             # Platform / windowing abstraction
├── RenderingComponents/  # Renderer-side component data
├── Shaders/GlslShaders/  # GLSL vertex/fragment/geometry/compute shaders
├── Systems/              # ECS systems (rendering, input, ...)
├── CMakeLists.txt
└── main.cpp
```

## Requirements

- A C++20 compiler (GCC 11+, Clang 13+, MSVC 19.30+)
- [CMake](https://cmake.org/) **3.28** or newer
- An OpenGL 4.x capable GPU and up-to-date drivers
- Git (with submodule support)

On Linux you will also need the usual GLFW build dependencies, e.g. on
Debian/Ubuntu:

```bash
sudo apt install build-essential cmake xorg-dev libgl1-mesa-dev
```

## Building

Clone the repository **with submodules** — all third-party libraries live under
`Libs/` and are pulled in via `git submodule`:

```bash
git clone --recurse-submodules https://github.com/ftedoldi/Vishengine.git
cd Vishengine
```

If you already cloned without `--recurse-submodules`:

```bash
git submodule update --init --recursive
```

Then configure and build with CMake:

```bash
cmake -S . -B build
cmake --build build --config Release -j
```

The executable is placed at:

```
build/Vishengine/Vishengine        # (or Vishengine.exe on Windows)
```

The build step automatically copies `Shaders/` next to the executable so they
are available at runtime.

## Running

From the project root:

```bash
./build/Vishengine/Vishengine
```

## Dependencies

All third-party code is vendored as a submodule under `Libs/`:

| Library    | Purpose                          |
| ---------- | -------------------------------- |
| GLFW       | Windowing & input                |
| GLAD       | OpenGL function loader           |
| GLM        | Math (vectors, matrices, quats)  |
| Assimp     | Model importing                  |
| EnTT       | Entity–component–system          |
| Dear ImGui | Immediate-mode editor UI         |
| ImGuizmo   | 3D manipulation gizmos           |
| stb        | Image loading (`stb_image.h`)    |

## License

<!-- TODO: pick a license (MIT / Apache-2.0 / etc.) and add a LICENSE file.
     Libraries under Libs/ keep their own respective licenses. -->
No license has been chosen yet.

## Author

Made by [@ftedoldi](https://github.com/ftedoldi).
