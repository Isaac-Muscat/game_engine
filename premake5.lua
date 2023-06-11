
-- Get libs and dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")
VULKAN_LIB = "%{VULKAN_SDK}/Lib/vulkan-1.lib"

VULKAN_INCLUDE_DIR = "%{VULKAN_SDK}/Include"
GLFW_INCLUDE_DIR = "%{wks.location}/dependencies/glfw/include"
GLM_INCLUDE_DIR = "%{wks.location}/dependencies/glm"
STB_INCLUDE_DIR = "%{wks.location}/dependencies/stb"

workspace "VulkanGameEngine"
	architecture "x86_64"
	configurations { "Debug", "Release" }
	startproject "VulkanGameEngine"

group "Dependencies"
	include "dependencies"
group ""

include "VulkanGameEngine"