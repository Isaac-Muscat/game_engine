#include "core/EntryPoint.h"
#include "ecs/Entity.h"
#include "ecs/Components.h"
#include "ecs/Scene.h"
#include "hid/Input.h"

void Application::OnCreate() {

	/* Goal API for ECS
		Entity player = g_ecs.GetEntityByName("Player");
		Transform t = player.GetComponent<Transform>();
		Mesh m = Mesh("path/to/file.obj");
		player.AddComponent<Mesh>(m);
	*/
	Scene scene;
	scene.RegisterView<TransformComponent, ScriptComponent>();

	for (int i = 0; i < 2; i++) {
		EntityID entity = scene.CreateEntity();
		TransformComponent transform = { glm::vec3(10, 20, 30) + glm::vec3(i)};
		scene.AddComponent<TransformComponent>(entity, transform);
		ScriptComponent script = {};
		scene.AddComponent<ScriptComponent>(entity, script);
	}

	const auto& entities_with_transforms_and_scripts = scene.GetView<TransformComponent, ScriptComponent>();

	for (auto e : entities_with_transforms_and_scripts) {
		std::cout << e << std::endl;
		std::cout << scene.GetComponent<TransformComponent>(e).position.x << std::endl;
		std::cout << scene.GetComponent<TransformComponent>(e).position.y << std::endl;
		std::cout << scene.GetComponent<TransformComponent>(e).position.z << std::endl;
	}

	EntityID entity1 = *entities_with_transforms_and_scripts.begin();
	TransformComponent& t = scene.GetComponent<TransformComponent>(entity1);
	t.position *= 3;
	t = scene.GetComponent<TransformComponent>(entity1);
	
	scene.DeleteComponent<ScriptComponent>(entity1);


	scene.DestroyEntity(entity1);

	uint32_t tid = Scene::GetComponentID<TransformComponent>();
	uint32_t mid = Scene::GetComponentID<MeshComponent>();
	uint32_t sid = Scene::GetComponentID<ScriptComponent>();
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