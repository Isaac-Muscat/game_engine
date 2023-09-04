#include "core/EntryPoint.h"
#include "ecs/Entity.h"
#include "ecs/Components.h"
#include "hid/Input.h"

void Application::OnCreate() {
	Entity entity1 = 100;
	Entity entity2 = 200;
	Entity entity3 = 300;
	Entity entity4 = 400;
	TransformComponent transform1 = { glm::vec3(1), {}, {} };
	TransformComponent transform2 = { glm::vec3(2), {}, {} };
	TransformComponent transform3 = { glm::vec3(3), {}, {} };
	TransformComponent transform4 = { glm::vec3(4), {}, {} };
	TransformComponent t = {};
	ComponentArray<TransformComponent> transform_components;

	transform_components.AddComponentToEntity(entity1, transform1);
	transform_components.AddComponentToEntity(entity2, transform2);
	transform_components.AddComponentToEntity(entity3, transform3);

	transform_components.DestroyEntity(entity2);

	transform_components.AddComponentToEntity(entity4, transform4);
	t = transform_components.GetComponent(entity1);
	t = transform_components.GetComponent(entity3);
	t = transform_components.GetComponent(entity4);

	transform_components.DestroyEntity(entity4);

	transform_components.AddComponentToEntity(entity2, transform2);
	t = transform_components.GetComponent(entity1);
	t = transform_components.GetComponent(entity2);
	t = transform_components.GetComponent(entity3);

	transform_components.DestroyEntity(entity1);
	transform_components.DestroyEntity(entity2);
	t = transform_components.GetComponent(entity3);
	transform_components.DestroyEntity(entity3);
}

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