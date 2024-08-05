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

The code is written with C++ and uses polymorphism and smart pointers.
I will only stop using these fancy C++ features if I need more performance for the engine.
I think they hit the sweet spot in terms of performance and ease of use.

## What was I Doing Last
Make separate list for each light type
Variable size light array. In the middle of changing uniform to storage bufffer but not sure if this is best.
Just added 2nd light so need to make size of lights array passed to shader dynamic.
Quuick profiling hack on render time taken and update time taken (Not Accurate)

## Current Tasks
Automatic shader recompliation on change
1. Scene Graph Component
    - This will be optionally added to an entity to allow for that entity to enter the scene graph
    - This is not super performant, but it can allow helpful queries for the developer
2. Material System
    - This will combine descriptors and pipelines into one object
    - Need to figure out where shaders fit in
3. Lights
    - Use Storage Buffers that are dynamically changing in size if more lights are added or removed
    - Light component with different types
4. PBR pipeline
5. Animation
6. Start procedural world/terrain generation (In sandbox as target game)

## To Be Implemented
* Compute Shader frustrum culling by uploading scene onto GPU with AABBs and/or Binary Space Partitioning
* Logging (Remove exceptions)
	* Logginng uses asserts (Maybe add color?)
* Renderer Cleanup
* Memory allocators
	* Pool allocator
	* Stack allocator
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

### Misc
* Set Platform enums on startup
	* Use switch statement for input with cases using platform
