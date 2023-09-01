#pragma once
#include "pch.h"

class Renderer {
public:
	virtual void draw() = 0;
	virtual void destroy() = 0;
};

extern std::unique_ptr<Renderer> g_renderer;
