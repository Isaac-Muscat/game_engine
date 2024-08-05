#include "GLFW/glfw3.h"
#include "core/EntryPoint.h"
#include "core/Time.h"
#include "ecs/Components.h"
#include "ecs/EntityComponentSystem.h"
#include "ecs/Entity.h"
#include "hid/Input.h"
#include "renderer/Light.h"
#include "renderer/Renderer.h"
#include "renderer/vk/VulkanMesh.h"
#include "renderer/vk/VulkanRenderer.h"
#include "renderer/vk/VulkanMaterial.h"
#include "core/SimplexNoise.h"

#include "PlayerController.h"
#include "scripts/CameraController.h"
#include <algorithm>

Entity point_light;
Entity sun;

std::shared_ptr<vk::VulkanMesh> make_terrain() {

    std::vector<vk::Vertex> vertices;
    std::vector<uint32_t> indices;
    /*
    vk::Vertex v;

    v = {
        glm::vec3(-1.0f, 0.0f, 1.0f),
        glm::vec3(1),
        glm::vec2(0),
        glm::vec3(0, 1, 0)
    };
    vertices.push_back(v);
    v = {
        glm::vec3(1.0f, 0.0f, 1.0f),
        glm::vec3(1),
        glm::vec2(0),
        glm::vec3(0, 1, 0)
    };
    vertices.push_back(v);
    v = {
        glm::vec3(-1.0f, 0.0f, -1.0f),
        glm::vec3(1),
        glm::vec2(0),
        glm::vec3(0, 1, 0)
    };
    vertices.push_back(v);
    v = {
        glm::vec3(1.0f, 0.0f, -1.0f),
        glm::vec3(1),
        glm::vec2(0),
        glm::vec3(0, 1, 0)
    };
    vertices.push_back(v);

    indices = {0, 1, 2, 2, 1, 3};
    */

    // Create all vertices (there are duplicates to fix normals)
    // Each inner loop creates two triangles to make a quad (6 vertices each)
    SimplexNoise noise;
    int size_x = 5;
    int size_z = 5;
    float step = 1.0f;
    int index = 0;
    for (int i_x = -size_x; i_x <= size_x; i_x++) {
        for (int i_z = -size_z; i_z <= size_z; i_z++) {
            float x = i_x * step;
            float z = i_z * step;

            vk::Vertex v1, v2, v3;
            v1 = { glm::vec3(x,         noise.noise(x, z),               z       ), glm::vec3(1), glm::vec2(0), glm::vec3(0, 1, 0) };
            v2 = { glm::vec3(x,         noise.noise(x, z + step),        z + step), glm::vec3(1), glm::vec2(0), glm::vec3(0, 1, 0) };
            v3 = { glm::vec3(x + step,  noise.noise(x + step, z + step), z + step), glm::vec3(1), glm::vec2(0), glm::vec3(0, 1, 0) };
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
            indices.push_back(index);
            indices.push_back(index + 1);
            indices.push_back(index + 2);
            
            index += 3;

            vk::Vertex v4, v5, v6;
            v4 = { glm::vec3(x,         noise.noise(x, z),               z       ), glm::vec3(1), glm::vec2(0), glm::vec3(0, 1, 0) };
            v5 = { glm::vec3(x + step,  noise.noise(x + step, z + step), z + step), glm::vec3(1), glm::vec2(0), glm::vec3(0, 1, 0) };
            v6 = { glm::vec3(x + step,  noise.noise(x + step, z),        z       ), glm::vec3(1), glm::vec2(0), glm::vec3(0, 1, 0) };
            vertices.push_back(v4);
            vertices.push_back(v5);
            vertices.push_back(v6);
            indices.push_back(index);
            indices.push_back(index + 1);
            indices.push_back(index + 2);
            
            index += 3;
        }
    }
    return std::make_shared<vk::VulkanMesh>(vertices, indices);
}


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
    light1.color = glm::vec3(0.6,0.5,0.4);
    light1.ambient = 0.0f;
    light1.diffuse = 1.0f;
    light1.type = LightType::Point;
	point_light.AddComponent<LightComponent>(light1);
	point_light.AddComponent<TransformComponent>({glm::vec3(5.0f,3.0f,0), glm::vec3(1), glm::vec3(1)});

	
	// Make buffers abstract and keep meshes graphics api independant. Same for textures?
    std::shared_ptr<vk::VulkanMesh> cube_mesh = std::make_shared<vk::VulkanMesh>("assets/models/cube.obj");
    std::shared_ptr<vk::VulkanMesh> sphere_mesh = std::make_shared<vk::VulkanMesh>("assets/models/sphere.obj");
    std::shared_ptr<vk::VulkanMesh> plane_mesh = make_terrain();

    std::shared_ptr<vk::VulkanTexture> white_texture = std::make_shared<vk::VulkanTexture>("assets/textures/white.png");
    std::shared_ptr<vk::VulkanMaterial> white_material = std::make_shared<vk::VulkanMaterial>(white_texture);

    Entity plane = scene->m_ecs->CreateEntity();
    plane.AddComponent<TransformComponent>({ glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1)});
    plane.AddComponent<MeshComponent>({ plane_mesh });
    plane.AddComponent<MaterialComponent>({ white_material });

    Entity cube = scene->m_ecs->CreateEntity();
    cube.AddComponent<TransformComponent>({ glm::vec3(7.0f, 3.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f)});
    cube.AddComponent<MeshComponent>({ cube_mesh });
    cube.AddComponent<MaterialComponent>({ white_material });

    Entity sphere = scene->m_ecs->CreateEntity();
    sphere.AddComponent<TransformComponent>({ glm::vec3(7.0f, 3.0f, -3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f)});
    sphere.AddComponent<MeshComponent>({ sphere_mesh });
    sphere.AddComponent<MaterialComponent>({ white_material });

    point_light.AddComponent<MeshComponent>({ sphere_mesh });
    point_light.AddComponent<MaterialComponent>({ white_material });

    std::shared_ptr<vk::VulkanMesh> mesh = std::make_shared<vk::VulkanMesh>("assets/models/viking_room.obj");
    std::shared_ptr<vk::VulkanTexture> texture = std::make_shared<vk::VulkanTexture>("assets/textures/viking_room.png");
    std::shared_ptr<vk::VulkanMaterial> material = std::make_shared<vk::VulkanMaterial>(texture);

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            float rotation = (rand() % 3600) / 10.0f;
            Entity dungeon = scene->m_ecs->CreateEntity();
            dungeon.AddComponent<TransformComponent>({ glm::vec3(10.0f, i * 3.0f + 3.0f, j * 3.0f), glm::vec3(0.0f, glm::radians(rotation), 0.0f), glm::vec3(2.0f)});
            dungeon.AddComponent<MeshComponent>({ mesh });
            dungeon.AddComponent<MaterialComponent>({ material });
        }
    }

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
    static float time = 0;
    time += Time::DeltaTime();
    auto& light = point_light.GetComponent<LightComponent>();
    auto& transform = point_light.GetComponent<TransformComponent>();
    transform.position.z = 5.0f * sin(time);
    transform.position.x = 5 + 5.0f * sin(time);

    if (Input::GetKeyPressed(GLFW_KEY_ESCAPE)) {
        Exit();
    }

    if (Input::GetKeyPressed(GLFW_KEY_E) && !sun.IsInitialized()) {
        sun = GetCurrentScene()->m_ecs->CreateEntity();
        LightComponent light2 = {};
        light2.color = glm::vec3(0.9,0.8,0.7);
        light2.ambient = 0.05f;
        light2.diffuse = 1.0f;
        light2.type = LightType::Directional;
        sun.AddComponent<LightComponent>(light2);
        sun.AddComponent<TransformComponent>({glm::vec3(5.0f,0,0), glm::vec3(1), glm::vec3(1)});
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
