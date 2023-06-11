project "VulkanGameEngine"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
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