project "sandbox"
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
		"%{wks.location}/game_engine/src",
		VULKAN_INCLUDE_DIR,
		GLFW_INCLUDE_DIR,
		GLM_INCLUDE_DIR,
		STB_INCLUDE_DIR,
		TINY_OBJ_LOADER_INCLUDE_DIR
	}

	links {
		"game_engine"
	}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"