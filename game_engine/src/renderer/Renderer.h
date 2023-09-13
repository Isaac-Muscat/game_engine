#pragma once
#include "pch.h"

// Example of wrapping rendering classes to support multiple apis in the future.
// For now just using vulkan renderer.
class Renderer {
public:
	virtual void Draw() = 0;
	virtual void SwapBuffers() = 0;
};
