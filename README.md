# Spectre
## Vulkan-based 3D renderer

The purpose is to develop an API-friendly 3D renderer that supports Windows, Linux, and Android platforms.  
At present, Vulkan is used at the bottom layer, and support for OpenGL will be considered after it is gradually improved.  
The project is currently in its infancy, and the code is in an extremely unstable stage. It is expected to form a stable version by the end of 2022.  
## Building
- C++ 17
- CMake3.2
Linux and Android platforms are currently not supported.
## Features
- [X] Automatic resource recycling
- [X] Static Batching
- [X] Decriptor magagement 
- [X] Scene management
- [X] PhongMaterial
- [ ] Multithreaded rendering 
- [ ] GLTF loader
- [ ] LOD
- [ ] PBR
