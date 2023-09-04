#include "core/EntryPoint.h"
#include "ecs/Entity.h"
#include "ecs/Components.h"

void Application::OnCreate() {
	Entity entity{};

void Application::OnUpdate() {
	if (Input::GetKeyPressed(GLFW_KEY_A)) {
		std::cout << "Mouse x: " << Input::GetMousePosition().x << std::endl;
		std::cout << "Mouse y: " << Input::GetMousePosition().y << std::endl;
	}
}

void Application::OnDestroy() {}

Application* Application::CreateApplication() {
	ApplicationInfo info{};
	info.platform = Platform::Win32;
	info.graphics_api = PlatformGraphicsAPI::Vulkan;
	info.name = "Game Engine";
	info.screen_width = 1600;
	info.screen_height = 900;
	return new Application(info);
}