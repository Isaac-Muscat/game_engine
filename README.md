# Base Vulkan Setup
It is tedious to set up a C++ project, so this is a template repository that can be used to create new projects without having to worry about the initial setup.

## Notes on Use
1. Only supports windows VS2022 build for now
2. Make sure to clone this repository recursively to also download glfw, glm, and stb_image which are common libraries for graphics projects
3. Run genVSProj.bat to create the Solution
4. This uses premake (https://premake.github.io/) as its build system, so to build for other platforms, just download the premake5 executable and run the build for the target platform
5. This uses Vulkan so it is assumed that you have the Vulkan SDK (https://vulkan.lunarg.com/) installed