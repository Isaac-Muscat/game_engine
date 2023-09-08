#include "core/EntryPoint.h"
#include "ecs/EntityComponentSystem.h"
#include "ecs/Entity.h"
#include "hid/Input.h"

#include "PlayerController.h"
#include "scripts/CameraController.h"

void Application::OnCreate() {
	// Test Workspace

	/* Goal API for ECS
		Entity player = g_ecs.GetEntityByName("Player");
		Transform t = player.GetComponent<Transform>();
		Mesh m = Mesh("path/to/file.obj");
		player.AddComponent<Mesh>(m);
	*/
	EntityComponentSystem g_ecs;
	g_ecs.RegisterView<TransformComponent, NameComponent>();

	for (int i = 0; i < 2; i++) {
		Entity entity = g_ecs.CreateEntity();
		TransformComponent transform = { glm::vec3(10, 20, 30) + glm::vec3(i) };
		entity.AddComponent<TransformComponent>(transform);
		entity.AddComponent<NameComponent>({ "Test" });
		entity.SetTag("Entity" + std::to_string(i));
	}

	Entity e0 = g_ecs.FindEntityByTag("Entity0");
	Entity e1 = g_ecs.FindEntityByTag("Entity1");
	Entity e2 = g_ecs.FindEntityByTag("Entity2");

	const auto& entities_with_transforms_and_scripts = g_ecs.GetView<TransformComponent, NameComponent>();

	for (const auto& e : g_ecs.GetView<TransformComponent, NameComponent>()) {
		std::cout << e << std::endl;
		std::cout << g_ecs.GetComponent<TransformComponent>(e).position.x << std::endl;
		std::cout << g_ecs.GetComponent<TransformComponent>(e).position.y << std::endl;
		std::cout << g_ecs.GetComponent<TransformComponent>(e).position.z << std::endl;
	}

	EntityID entity1 = *entities_with_transforms_and_scripts.begin();
	TransformComponent& t = g_ecs.GetComponent<TransformComponent>(entity1);
	t.position *= 3;
	t = g_ecs.GetComponent<TransformComponent>(entity1);

	g_ecs.DeleteComponent<NameComponent>(entity1);

	g_ecs.DestroyEntity(entity1);

	std::cout << "After destroying " << entity1 << std::endl;
	for (const auto& e : g_ecs.GetView<TransformComponent, NameComponent>()) {
		std::cout << e << std::endl;
		std::cout << g_ecs.GetComponent<TransformComponent>(e).position.x << std::endl;
		std::cout << g_ecs.GetComponent<TransformComponent>(e).position.y << std::endl;
		std::cout << g_ecs.GetComponent<TransformComponent>(e).position.z << std::endl;
	}

	uint32_t tid = EntityComponentSystem::GetComponentID<TransformComponent>();
	uint32_t mid = EntityComponentSystem::GetComponentID<MeshComponent>();
	uint32_t sid = EntityComponentSystem::GetComponentID<ScriptComponent>();

	// Create Scenes. Eventually will have an editor with scene serialization and deserialization.
	// For now, manually define scenes in Application::OnCreate function.
	std::shared_ptr<Scene> scene = std::make_shared<Scene>();
	for (int i = 0; i < 100; i++) {
		Entity player = scene->m_ecs->CreateEntity();
		ScriptComponent player_controller = { new PlayerController };
		player_controller.script->m_entity = player;
		player.AddComponent<ScriptComponent>(player_controller);
		player.AddComponent<TransformComponent>({});
		player.SetTag("Player" + std::to_string(i));
		player.AddComponent<NameComponent>({ "Player" + std::to_string(i) });
	}

	// Create First Person Camera.
	Entity camera_entity = scene->m_ecs->CreateEntity();
	camera_entity.AddComponent<CameraComponent>({ Camera(glm::vec3(2.0f)) });
	ScriptComponent camera_controller = { new CameraController };
	camera_controller.script->m_entity = camera_entity;
	camera_entity.AddComponent<ScriptComponent>(camera_controller);
	scene->SetMainCamera(camera_entity);
	m_scenes.push_back(scene);
}

void Application::OnUpdate() {
	if (Input::GetKeyPressed(GLFW_KEY_A) || Input::GetKeyPressed(GLFW_KEY_D)) {
		std::cout << "Key pressed." << std::endl;
	}
	if (Input::GetMousePressed(GLFW_MOUSE_BUTTON_LEFT)) {
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