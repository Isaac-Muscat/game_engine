#pragma once
#include "pch.h"
#include "renderer/vk/VulkanMesh.h"
#include "renderer/vk/VulkanShader.h"
#include "renderer/vk/VulkanMaterial.h"
#include "ecs/EntityTypes.h"
#include "renderer/Camera.h"
#include "renderer/Light.h"
#include "physics/AABB.h"
#include "physics/TriangleBVH.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

struct MeshComponent {
	std::shared_ptr<vk::VulkanMesh> mesh;
};


struct LightComponent {
    alignas(4) LightType type;
    alignas(4) float ambient;
    alignas(4) float diffuse;
    alignas(4) float specular;
    alignas(16) glm::vec3 color;
};

struct MaterialComponent {
	std::shared_ptr<vk::VulkanMaterial> material;
};

struct TransformComponent {
	glm::vec3 position{0.0f};
	glm::vec3 euler_angles{0.0f};
	glm::vec3 scale = glm::vec3(1.0f);

	glm::mat4 GetModelMatrix() {
		glm::mat4 m = glm::mat4(1.0f);

		m = glm::scale(m, scale);
		m = glm::translate(m, position);
		m = glm::rotate(m, euler_angles.x, glm::vec3(1.0f, 0.0f, 0.0f));
		m = glm::rotate(m, euler_angles.y, glm::vec3(0.0f, 1.0f, 0.0f));
		m = glm::rotate(m, euler_angles.z, glm::vec3(0.0f, 0.0f, 1.0f));
		return m;
	}
    void Print() {
        std::cout << "(" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
    }
};

// TODO: Need to give AABB a position and then copy transformcomponent into aabb
struct AABBComponent {
    AABB aabb;
    bool static_collider;
    bool collided;
};

struct MeshColliderComponent {
    TriangleBVH bvh;
    bool static_collider;
    bool collided;
};

struct NameComponent {
	std::string name;
};

class EntityScript;
struct ScriptComponent {
	std::shared_ptr<EntityScript> script;
};

struct CameraComponent {
	Camera camera;
};
