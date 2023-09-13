#pragma once
#include "pch.h"
#include "renderer/vk/VulkanMesh.h"
#include "renderer/vk/VulkanShader.h"
#include "renderer/vk/VulkanMaterial.h"
#include "ecs/EntityTypes.h"
#include "renderer/Camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

struct MeshComponent {
	std::shared_ptr<vk::VulkanMesh> mesh;
};

struct MaterialComponent {
	std::shared_ptr<vk::VulkanMaterial> material;
};

struct TransformComponent {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale = glm::vec3(1.0f);

	glm::mat4 GetModelMatrix() {
		glm::mat4 m = glm::mat4(1.0f);

		m = glm::translate(m, position);
		m = glm::scale(m, scale);
		//m = glm::rotate(m, 0.0f, rotation);
		return m;
	}
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