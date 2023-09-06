#include "core/EntryPoint.h"
#include "ecs/EntityComponentSystem.h"
#include "ecs/Entity.h"
#include "hid/Input.h"

void Application::OnCreate() {

	/* Goal API for ECS
		Entity player = g_ecs.GetEntityByName("Player");
		Transform t = player.GetComponent<Transform>();
		Mesh m = Mesh("path/to/file.obj");
		player.AddComponent<Mesh>(m);
	*/
	EntityComponentSystem scene;
	scene.RegisterView<TransformComponent, ScriptComponent>();

	for (int i = 0; i < 2; i++) {
		Entity entity = scene.CreateEntity();
		TransformComponent transform = { glm::vec3(10, 20, 30) + glm::vec3(i)};
		entity.AddComponent<TransformComponent>(transform);
		ScriptComponent script = {};
		entity.AddComponent<ScriptComponent>(script);
		entity.SetTag("Entity" + std::to_string(i));
	}

	Entity e0 = scene.FindEntityByTag("Entity0");
	Entity e1 = scene.FindEntityByTag("Entity1");
	Entity e2 = scene.FindEntityByTag("Entity2");

	const auto& entities_with_transforms_and_scripts = scene.GetView<TransformComponent, ScriptComponent>();

	for (const auto& e : scene.GetView<TransformComponent, ScriptComponent>()) {
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

	std::cout << "After destroying " << entity1 << std::endl;
	for (const auto& e : scene.GetView<TransformComponent, ScriptComponent>()) {
		std::cout << e << std::endl;
		std::cout << scene.GetComponent<TransformComponent>(e).position.x << std::endl;
		std::cout << scene.GetComponent<TransformComponent>(e).position.y << std::endl;
		std::cout << scene.GetComponent<TransformComponent>(e).position.z << std::endl;
	}

	uint32_t tid = EntityComponentSystem::GetComponentID<TransformComponent>();
	uint32_t mid = EntityComponentSystem::GetComponentID<MeshComponent>();
	uint32_t sid = EntityComponentSystem::GetComponentID<ScriptComponent>();
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