# Voxel Engine

A C++ voxel rendering engine built with **OpenGL**. The project explores the core building blocks of a Minecraft-style voxel renderer, including chunk-based terrain generation, block definitions, mesh generation, texture mapping, lighting, transparency, and first-person camera controls.

## Features

* **Chunk-based terrain generation** using a deterministic world seed
* **Voxel block registry** defined through JSON configuration
* **Voxel-to-mesh generation** for rendering chunk geometry
* **Texture mapping** using a shared texture atlas
* **Transparent block support** using OpenGL alpha blending
* **First-person camera** with WASD movement and mouse look
* **Perspective 3D rendering** with depth testing
* **OpenGL shader pipeline** with vertex and fragment shaders
* **FPS counter** displayed in the application window
* **Makefile-based build workflow**

## Tech Stack

| Technology    | Purpose                                               |
| ------------- | ----------------------------------------------------- |
| C++           | Core engine implementation                            |
| OpenGL 3.3    | 3D rendering                                          |
| GLFW          | Window creation, input, and OpenGL context            |
| GLAD          | OpenGL function loading                               |
| GLM           | Mathematics, vectors, matrices, and camera transforms |
| stb_image     | Texture loading                                       |
| nlohmann/json | JSON-based block configuration                        |
| Make          | Build and run automation                              |

## How It Works

The engine generates voxel data for a fixed-size chunk using a deterministic world seed. Each voxel is converted into a block representation using the block registry, which maps numeric block IDs to block types such as stone, dirt, grass, sand, water, and snow.

The generated block data is then passed to the mesh-building stage, which produces the vertex and index data used for rendering. The resulting mesh is drawn with OpenGL using a texture atlas and configurable shaders.

The renderer also supports transparent blocks through alpha blending and provides a first-person camera for navigating the generated terrain.

## Controls

| Input | Action        |
| ----- | ------------- |
| `W`   | Move forward  |
| `S`   | Move backward |
| `A`   | Move left     |
| `D`   | Move right    |
| Mouse | Look around   |

The mouse cursor is captured while the application is running.

## Build and Run

### Requirements

A C++ development environment with the following libraries/components:

* OpenGL 3.3
* GLFW
* GLAD
* GLM
* stb_image
* nlohmann/json

The current Makefile is configured for the Windows OpenGL libraries (`opengl32` and `gdi32`).

### Build

From the repository root:

```bash
make
```

The Makefile compiles the current renderer and launches the resulting application.

## Project Structure

```text
Voxel-Engine/
├── include/                  # Engine and rendering headers
├── lib/                     # External libraries
├── src/
│   ├── cube_renderer.cpp    # Main renderer entry point
│   └── *.glsl               # Vertex and fragment shaders
├── textures/
│   ├── textures.png         # Texture atlas
│   └── block_registry.json  # Block ID/type registry
├── main.cpp                 # Earlier rendering prototype
├── Makefile                 # Build and run commands
└── README.md
```

## Current Status

This project is an **active learning/prototyping engine** focused on understanding voxel rendering and OpenGL architecture. The current implementation renders generated terrain from a single chunk and establishes the foundation for a larger voxel world.

## Roadmap

* [ ] Multiple chunk loading and unloading
* [ ] Procedural terrain generation across larger worlds
* [ ] Chunk streaming around the player
* [ ] Face culling and mesh optimization
* [ ] Block placement and removal
* [ ] Collision detection and gravity
* [ ] Improved lighting and ambient occlusion
* [ ] More configurable block properties
* [ ] Performance profiling and rendering optimizations

## Why This Project?

The project is designed as a hands-on exploration of low-level 3D rendering concepts, including:

* OpenGL rendering pipelines
* GPU vertex/index buffers
* Shader programming
* Camera mathematics
* Texture mapping
* Voxel data representation
* Procedural terrain generation
* Mesh construction
* Real-time rendering loops
