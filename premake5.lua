
-- Get libs and dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")
VULKAN_LIB = "%{VULKAN_SDK}/Lib/vulkan-1.lib"

VULKAN_INCLUDE_DIR = "%{VULKAN_SDK}/Include"
GLFW_INCLUDE_DIR = "%{wks.location}/dependencies/glfw/include"
GLM_INCLUDE_DIR = "%{wks.location}/dependencies/glm"
STB_INCLUDE_DIR = "%{wks.location}/dependencies/stb"
TINY_OBJ_LOADER_INCLUDE_DIR = "%{wks.location}/dependencies/tiny_obj_loader"
TINY_GLTF_INCLUDE_DIR = "%{wks.location}/dependencies/tinygltf"

workspace "game_engine"
	architecture "x86_64"
	configurations { "Debug", "Release" }
	startproject "sandbox"

group "Dependencies"
	include "dependencies"
group ""

group "Engine"
	include "game_engine"
group ""

group "Apps"
	include "sandbox"
group ""
