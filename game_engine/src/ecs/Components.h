#pragma once
#include "pch.h"
#include "renderer/vk/VulkanMesh.h"
#include "ecs/Entity.h"

#include "glm/glm.hpp"

struct MeshComponent {
	vk::Mesh* mesh;
};

struct TransformComponent {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

class Scriptable {
public:
	virtual void OnCreate() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnDestroy() = 0;
private:
	Entity m_entity;
};

struct ScriptComponent {
	Scriptable* instance;
};

// TODO: Optimize for less branches?
// TODO: Use replace std::vector with custom vector-like class functionality.
// Maybe could have used vector or fixed array, but good practice.
template<typename T>
class ComponentArray {
public:
	ComponentArray() : ComponentArray(1) {}
	ComponentArray(size_t size) 
		: m_size(size), m_last_index(0) {
		assert(size > 0 && "Cannot initialize ComponentArray with size 0.");
		m_components = new T[size];
	}

	~ComponentArray() {
		delete[] m_components;
	}

	void AddComponentToEntity(Entity entity, T component) {
		assert(m_entity_to_index_map.find(entity) == m_entity_to_index_map.end() && "Entity already has component.");
		assert(m_last_index <= m_size && "ComponentArray next index is out of bounds.");

		m_last_index++;
		if (m_last_index == m_size) {
			size_t new_size = m_size + (m_size + 1) / 2;
			T* new_array = new T[new_size];
			for (int i = 0; i < m_size; i++) {
				new_array[i] = m_components[i];
			}
			delete[] m_components;
			m_components = new_array;
			m_size = new_size;
		}

		m_entity_to_index_map[entity] = m_last_index;
		m_index_to_entity_map[m_last_index] = entity;
		m_components[m_last_index] = component;
	}

	void RemoveComponentFromEntity(Entity entity) {
		assert(m_entity_to_index_map.find(entity) != m_entity_to_index_map.end() && "Entity does not have component.");
		size_t index = m_entity_to_index_map[entity];
		m_components[index] = m_components[m_last_index];

		size_t entity_to_replace = m_index_to_entity_map[m_last_index];
		m_entity_to_index_map[entity_to_replace] = index;
		m_index_to_entity_map[index] = entity_to_replace;

		m_entity_to_index_map.erase(entity);
		m_index_to_entity_map.erase(m_last_index);
		m_last_index--;
	}

	T GetComponent(Entity entity) {
		assert(m_entity_to_index_map.find(entity) != m_entity_to_index_map.end() && "Entity does not have component.");
		return m_components[m_entity_to_index_map[entity]];
	}

	void DestroyEntity(Entity entity) {
		if (EntityHasComponent(entity)) {
			RemoveComponentFromEntity(entity);
		}
	}

	bool EntityHasComponent(Entity entity) {
		return m_entity_to_index_map.find(entity) != m_entity_to_index_map.end();
	}
private:
	size_t m_size;
	size_t m_last_index;
	T* m_components;
	std::unordered_map<Entity, size_t> m_entity_to_index_map;
	std::unordered_map<size_t, Entity> m_index_to_entity_map;
};