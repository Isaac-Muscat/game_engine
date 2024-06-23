#pragma once
#include "pch.h"
#include "Platform.h"
#include "scene/Scene.h"

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
	int m_current_scene_index = 0;
	std::vector<std::shared_ptr<Scene>> m_scenes;
	Application(ApplicationInfo info);
	~Application();

	void Run();
	void OnCreate();
	void OnUpdate();
	void OnDestroy();

    inline void Exit() { m_user_says_close = 1; };

private:
    bool m_user_says_close = 0;
};
