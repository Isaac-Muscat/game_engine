#pragma once
#include "pch.h"
#include "renderer/vk/VulkanMesh.h"
#include "ecs/EntityTypes.h"

#include "glm/glm.hpp"

struct MeshComponent {
	vk::Mesh* mesh;
};

struct TransformComponent {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

typedef std::string NameComponent;

class ScriptableEntity;
struct ScriptComponent {
	ScriptableEntity* instance;
};