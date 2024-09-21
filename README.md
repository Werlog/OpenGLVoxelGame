# OpenGL Voxel Game

A simple voxel minecraft-like game I'm making in C++ and OpenGL.
I am also using this GLFW CMake [project setup](https://github.com/meemknight/GLFWCMakeSetup).

## Controls

WASD - Move

Space - Jump

Left Click - Destroy block

Right Click - Place block

Scroll - swap between blocks

## Current Features

- Procedural World Generation
	- Infinite world generation
	- Caves
	- Trees
- Block Placing and Breaking
- Player Movement and Player collision
- Diffuse Lighting

## Planned Features

- Improved performance when generating chunks
- Improved terrain generation (using splines)
- HUD (showing current picked block and a crosshair)

## How to build
Open the project folder with Visual Studio, open CMakeLists.txt and press <kbd>Ctrl + S</kbd>.
Select **VoxelGame.exe** from the run dropdown, then press <kbd>Ctrl + F5</kbd> to build and run.

## Screenshots

![In-Game Screenshot](https://github.com/Werlog/OpenGLVoxelGame/blob/d8dcd0f87089f48ca15c8244f2cc827de91ca858/images/screenshot.png)
