#pragma once
#include "pch.h"
#include "Entity.h"

class IComponentArray {
public:
	virtual void DestroyEntity(EntityID entity) = 0;
};

// TODO: Only allocate if no more room in array. 
// Currently, allocation occurs when an entity is added causing the array to be full.
// TODO: Optimize for no branches and fixed size?
// Maybe could have used vector or fixed array, but good practice.
template<typename T>
class ComponentArray : public IComponentArray {
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

	void AddComponentToEntity(EntityID entity, T component) {
		assert(m_entity_to_index_map.find(entity) == m_entity_to_index_map.end() && "Entity already has component.");
		assert(m_last_index <= m_size && "ComponentArray next index is out of bounds.");

		m_last_index++;
		if (m_last_index == m_size) {
			// Reduces memory allocations (4 allocations to have > 5000 components).
			size_t new_size = m_size + m_size * 8;
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

	void RemoveComponentFromEntity(EntityID entity) {
		assert(m_entity_to_index_map.find(entity) != m_entity_to_index_map.end() && "Entity does not have component.");
		size_t index = m_entity_to_index_map[entity];
		m_components[index] = m_components[m_last_index];

		EntityID entity_to_replace = m_index_to_entity_map[m_last_index];
		m_entity_to_index_map[entity_to_replace] = index;
		m_index_to_entity_map[index] = entity_to_replace;

		m_entity_to_index_map.erase(entity);
		m_index_to_entity_map.erase(m_last_index);
		m_last_index--;
	}	

	T& GetComponent(EntityID entity) {
		assert(m_entity_to_index_map.find(entity) != m_entity_to_index_map.end() && "Entity does not have component.");
		return m_components[m_entity_to_index_map[entity]];
	}

	void DestroyEntity(EntityID entity) override {
		assert(EntityHasComponent(entity) && "Entity does not have component.");
		RemoveComponentFromEntity(entity);
	}

	bool EntityHasComponent(EntityID entity) {
		return m_entity_to_index_map.find(entity) != m_entity_to_index_map.end();
	}
private:
	size_t m_size;
	size_t m_last_index;
	T* m_components;
	std::unordered_map<EntityID, size_t> m_entity_to_index_map;
	std::unordered_map<size_t, EntityID> m_index_to_entity_map;
};