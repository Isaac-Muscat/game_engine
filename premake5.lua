
-- Get libs and dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")
VULKAN_LIB = "%{VULKAN_SDK}/Lib/vulkan-1.lib"

VULKAN_INCLUDE_DIR = "%{VULKAN_SDK}/Include"
GLFW_INCLUDE_DIR = "%{wks.location}/dependencies/glfw/include"
GLM_INCLUDE_DIR = "%{wks.location}/dependencies/glm"
STB_INCLUDE_DIR = "%{wks.location}/dependencies/stb"

workspace "game_engine"
	architecture "x86_64"
	configurations { "Debug", "Release" }
	startproject "game_engine"

group "Dependencies"
	include "dependencies"
group ""

project "game_engine"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	targetdir "%{wks.location}/bin/%{cfg.buildcfg}/%{prj.name}"
	objdir "%{wks.location}/bin/obj/%{cfg.buildcfg}/%{prj.name}"

	files {
		"src/**.h",
		"src/**.cpp"
	}

	includedirs {
		"src",
		VULKAN_INCLUDE_DIR,
		GLFW_INCLUDE_DIR,
		GLM_INCLUDE_DIR,
		STB_INCLUDE_DIR
	}

	links {
		VULKAN_LIB,
		"glfw"
	}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
