#pragma once
#include "pch.h"
#include "renderer/vk/VulkanMesh.h"
#include "ecs/Entity.h"

#include "glm/glm.hpp"

class Scriptable {
public:
	virtual void OnCreate() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnDestroy() = 0;
private:
	Entity m_entity;
};

struct MeshComponent {
	vk::Mesh* mesh;
};

struct TransformComponent {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

struct ScriptComponent {
	Scriptable* instance;
};