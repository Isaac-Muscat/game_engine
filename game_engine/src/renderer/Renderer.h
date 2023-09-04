#pragma once
#include "pch.h"

class Renderer {
public:
	virtual void Draw() = 0;
	virtual void SwapBuffers() = 0;
};

extern std::unique_ptr<Renderer> g_renderer;
