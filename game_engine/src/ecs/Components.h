#pragma once
#include "pch.h"
#include "renderer/vk/VulkanMesh.h"
#include "ecs/EntityTypes.h"
#include "renderer/Camera.h"

#include "glm/glm.hpp"

struct MeshComponent {
	vk::Mesh* mesh;
};

struct TransformComponent {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

struct NameComponent {
	std::string name;
};

class EntityScript;
struct ScriptComponent {
	EntityScript* script;
};

struct CameraComponent {
	Camera camera;
};