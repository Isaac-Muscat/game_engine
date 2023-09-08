#pragma once
#include "pch.h"
#include "Camera.h"

class Renderer {
public:
	virtual void Draw(const Camera& camera) = 0;
	virtual void SwapBuffers() = 0;
};

extern std::unique_ptr<Renderer> g_renderer;
