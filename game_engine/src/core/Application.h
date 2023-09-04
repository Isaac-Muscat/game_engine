#pragma once
#include "pch.h"
#include "Platform.h"

struct ApplicationInfo {
	Platform platform;
	PlatformGraphicsAPI graphics_api;
	uint32_t screen_width;
	uint32_t screen_height;
	std::string name;
};

class Application {
public:
	static Application* CreateApplication();
	Application(ApplicationInfo info);
	~Application();
	void Run();

	void OnCreate();
	void OnUpdate();
	void OnDestroy();

};