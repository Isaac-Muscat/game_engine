#include "core/EntryPoint.h"
#include "ecs/EntityComponentSystem.h"
#include "ecs/Entity.h"
#include "hid/Input.h"
#include "renderer/Renderer.h"
#include "renderer/vk/VulkanRenderer.h"
#include "renderer/vk/VulkanMaterial.h"

#include "PlayerController.h"
#include "scripts/CameraController.h"

void Application::OnCreate() {
	// Test Workspace

	/* Goal API for ECS
		Mesh m = Mesh("path/to/file.obj");
		player.AddComponent<Mesh>(m);
	*/
	// Create Scenes. Eventually will have an editor with scene serialization and deserialization.
	// For now, manually define scenes in Application::OnCreate function.
	std::shared_ptr<Scene> scene = std::make_shared<Scene>();
	
	// Make buffers abstract and keep meshes graphics api independant. Same for textures?
	Entity dungeon = scene->m_ecs->CreateEntity();
	std::shared_ptr<vk::VulkanMesh> mesh = std::make_shared<vk::VulkanMesh>("assets/models/viking_room.obj");
	std::shared_ptr<vk::VulkanTexture> texture = std::make_shared<vk::VulkanTexture>("assets/textures/viking_room.png");
	std::shared_ptr<vk::VulkanMaterial> material = std::make_shared<vk::VulkanMaterial>(texture);
	dungeon.AddComponent<TransformComponent>({ glm::vec3(2.0f, 0.0f, 0.0f) });
	dungeon.AddComponent<MeshComponent>({ mesh });
	dungeon.AddComponent<MaterialComponent>({ material });

	Entity dungeon2 = scene->m_ecs->CreateEntity();
	dungeon2.AddComponent<TransformComponent>({ glm::vec3(4.0f, 0.0f, 0.0f) });
	dungeon2.AddComponent<MeshComponent>({ mesh });
	dungeon2.AddComponent<MaterialComponent>({ material });

	// Create First Person Camera.
	Entity camera_entity = scene->m_ecs->CreateEntity();
	camera_entity.AddComponent<CameraComponent>({ Camera(glm::vec3(0.0f)) });
	ScriptComponent camera_controller = { std::make_shared<CameraController>() };
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