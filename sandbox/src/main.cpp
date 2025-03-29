#include <core/Core.h>

#include "CubeMarchTerrain.h"
#include "core/AssetManager.h"
#include "ecs/Components.h"
#include "physics/SphereCollider.h"

Entity point_light;
Entity sun;
Entity sun2;
Entity cube;
Entity trex;

void Application::OnCreate() {
	// Test Workspace

	/* Goal API for ECS
		Mesh m = Mesh("path/to/file.obj");
		player.AddComponent<Mesh>(m);
	*/
	// Create Scenes. Eventually will have an editor with scene serialization and deserialization.
	// For now, manually define scenes in Application::OnCreate function.
	std::shared_ptr<Scene> scene = std::make_shared<Scene>();

    // Add lights
    point_light = scene->m_ecs->CreateEntity();
    LightComponent light1 = {};
    light1.color = glm::vec3(0.8,0.8,0.8);
    light1.ambient = 0.0f;
    light1.diffuse = 1.0f;
    light1.type = LightType::Point;
	point_light.AddComponent<LightComponent>(light1);
	point_light.AddComponent<TransformComponent>({glm::vec3(5.0f,3.0f,0), glm::vec3(0), glm::vec3(1)});

	
	// Make buffers abstract and keep meshes graphics api independant. Same for textures?
    std::shared_ptr<vk::VulkanMesh> cube_mesh = Assets::LoadMesh("assets/models/cube.obj");
    std::shared_ptr<vk::VulkanMesh> sphere_mesh = Assets::LoadMesh("assets/models/sphere.obj");

    std::shared_ptr<vk::VulkanMaterial> white_material = Assets::LoadMaterial("assets/materials/default_white.mat");
    std::shared_ptr<vk::VulkanMaterial> red_pbr_material = Assets::LoadMaterial("assets/materials/red_pbr.mat");
    std::shared_ptr<vk::VulkanMaterial> blue_pbr_material = Assets::LoadMaterial("assets/materials/blue_pbr.mat");
    std::shared_ptr<vk::VulkanMaterial> dark_green_pbr_material = Assets::LoadMaterial("assets/materials/dark_green_pbr.mat");

    // Create terrain manager entity
    Entity terrain_manager = scene->m_ecs->CreateEntity();
    terrain_manager.SetTag("terrain");
	ScriptComponent terrain_manager_script = { std::make_shared<CubeMarchTerrain>() };
	terrain_manager_script.script->m_entity = terrain_manager;
	terrain_manager.AddComponent<ScriptComponent>(terrain_manager_script);


    cube = scene->m_ecs->CreateEntity();
    cube.AddComponent<TransformComponent>({ glm::vec3(10.0f, 3.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f)});
    cube.AddComponent<MeshComponent>({ cube_mesh });
    cube.AddComponent<MaterialComponent>({ red_pbr_material });
	cube.AddComponent<AABBComponent>({AABB(glm::vec3(-1), glm::vec3(1)), false, false});

    // ----------------------------------------------------
    // ----------------------------------------------------
    // ----------------------------------------------------
    Entity cube_x = scene->m_ecs->CreateEntity();
    cube_x.AddComponent<TransformComponent>({ glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0), glm::vec3(2.0f, 0.5f, 0.5f)});
    cube_x.AddComponent<MeshComponent>({ cube_mesh });
    cube_x.AddComponent<MaterialComponent>({ red_pbr_material });

    Entity cube_y = scene->m_ecs->CreateEntity();
    cube_y.AddComponent<TransformComponent>({ glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0), glm::vec3(0.5f, 2.0f, 0.5f)});
    cube_y.AddComponent<MeshComponent>({ cube_mesh });
    cube_y.AddComponent<MaterialComponent>({ dark_green_pbr_material });

    Entity cube_z = scene->m_ecs->CreateEntity();
    cube_z.AddComponent<TransformComponent>({ glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0), glm::vec3(0.5f, 0.5f, 2.0f)});
    cube_z.AddComponent<MeshComponent>({ cube_mesh });
    cube_z.AddComponent<MaterialComponent>({ blue_pbr_material });

    // ----------------------------------------------------
    // ----------------------------------------------------
    // ----------------------------------------------------

    Entity sphere = scene->m_ecs->CreateEntity();
    sphere.AddComponent<TransformComponent>({ glm::vec3(7.0f, 3.0f, -3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f)});
    sphere.AddComponent<MeshComponent>({ sphere_mesh });
    sphere.AddComponent<MaterialComponent>({ red_pbr_material });

    point_light.AddComponent<MeshComponent>({ sphere_mesh });
    point_light.AddComponent<MaterialComponent>({ white_material });

    std::shared_ptr<vk::VulkanMesh> trex_mesh = Assets::LoadMesh("assets/gltf/dino_anim/T-Rex.glb");
    std::shared_ptr<vk::VulkanMaterial> trex_material = Assets::LoadMaterial("assets/gltf/dino_anim/T-Rex.glb");
	trex = scene->m_ecs->CreateEntity();
	trex.AddComponent<TransformComponent>({ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-3.14f/2, 0.0f, 0.0f), glm::vec3(100.0f)});
	trex.AddComponent<MeshComponent>({ trex_mesh });
	trex.AddComponent<MaterialComponent>({ trex_material });

    std::shared_ptr<vk::VulkanMesh> mesh = Assets::LoadMesh("assets/models/viking_room.obj");
    std::shared_ptr<vk::VulkanMaterial> material = Assets::LoadMaterial("assets/materials/viking_room.mat");
    for (int i = -1; i < 1; i++) {
        for (int j = -1; j < 1; j++) {
            float rotation = (rand() % 3600) / 10.0f;
            Entity dungeon = scene->m_ecs->CreateEntity();
            dungeon.AddComponent<TransformComponent>({ glm::vec3(10.0f, i * 3.0f + 3.0f, j * 3.0f), glm::vec3(0.0f, glm::radians(rotation), 0.0f), glm::vec3(2.0f)});
            dungeon.AddComponent<MeshComponent>({ mesh });
            dungeon.AddComponent<MaterialComponent>({ material });
        }
    }


	// Create First Person Camera.
	Entity camera_entity = scene->m_ecs->CreateEntity();
	ScriptComponent camera_controller = { std::make_shared<PlayerController>() };
	camera_controller.script->m_entity = camera_entity;

	camera_entity.AddComponent<SphereColliderComponent>({SphereCollider(glm::vec3(0), 1), false, false});
	camera_entity.AddComponent<CameraComponent>({ Camera() });
	camera_entity.AddComponent<TransformComponent>({ glm::vec3(0, 15, 0), glm::vec3(1), glm::vec3(1) });
	camera_entity.AddComponent<ScriptComponent>(camera_controller);
    camera_entity.SetTag("main_player");

	scene->SetMainCamera(camera_entity);
	m_scenes.push_back(scene);
}

void Application::OnUpdate() {

    static float time = 0;
    time += Time::DeltaTime();
    auto& light = point_light.GetComponent<LightComponent>();
    auto& transform = point_light.GetComponent<TransformComponent>();
    transform.position.z = 5.0f * sin(time);
    transform.position.x = 5 + 5.0f * sin(time);

    auto& transform_cube = cube.GetComponent<TransformComponent>();
    transform_cube.euler_angles.x = time;

    if (Input::GetKeyPressed(GLFW_KEY_ESCAPE)) {
        Exit();
    }

    if (Input::GetKeyPressed(GLFW_KEY_E) && !sun.IsInitialized()) {
        sun = GetCurrentScene()->m_ecs->CreateEntity();
        LightComponent light2 = {};
        light2.color = glm::vec3(1,1,0.7);
        light2.ambient = 0.05f;
        light2.diffuse = 1.0f;
        light2.type = LightType::Directional;
        sun.AddComponent<LightComponent>(light2);
        sun.AddComponent<TransformComponent>({glm::vec3(5.0f,0,0), glm::vec3(1), glm::vec3(1)});

        sun2 = GetCurrentScene()->m_ecs->CreateEntity();
        LightComponent light3 = {};
        light3.color = glm::vec3(1,1,0.7);
        light3.ambient = 0.00f;
        light3.diffuse = 1.0f;
        light3.type = LightType::Directional;
        sun2.AddComponent<LightComponent>(light3);
        sun2.AddComponent<TransformComponent>({glm::vec3(5.0f,0,0), glm::vec3(0.2, 3, 0.2), glm::vec3(1)});
    }
}

void Application::OnDestroy() {}

Application* Application::CreateApplication() {
	ApplicationInfo info{};
	info.platform = Platform::Win32;
	info.graphics_api = PlatformGraphicsAPI::Vulkan;
	info.name = "Game Engine";
	info.screen_width = 1900;
	info.screen_height = 1000;
	return new Application(info);
}
