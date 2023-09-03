# Game Engine
Games (and the engines that create them) are one of the most complicated software applications to create.
Game engines can seemingly utilize every aspect of programming, so I see building one as the ultimate long term project.
I started out supporting only Windows and Vulkan, but I hope to support other platforms eventually.

Currently the only libraries being used are:
* C++ STL
* tiny_obj_loader
* glm
* GLFW
* stb_image

The code is written with C++, and mainly uses polymorphism, smart pointers.
I will only stop using these fancy C++ features if I need more performance for the engine.
I think they hit the sweet spot in terms of performance and ease of use.

## To Be Implemented
* Input
* Logging (Remove exceptions)
	* Logginng uses asserts (Maybe add color?)
* Camera
* Renderer Cleanup
* ECS
* Scenes
* Make editor for engine
	* 2D rendering
	* Text rendering
	* Custom UI engine
* Physics
* 3D Renderer (Infinite features to add)
	* Materials
	* Lighting
	* Shadows
	* SoA
	* Environment Maps
	* Etc, etc, ...

* Platform Specific Code
	* Win32 window to replace glfw
* Audio

## Notes for Dev
### Coding Style
* Classes --> ThisIsAClass
* Variables --> regular_variable, m_member_variable, s_static_variable, g_global_variable, p_pointer
* Functions --> ThisIsAFunction
