#pragma once
#include "pch.h"
#include "ComponentArray.h"
#include "Components.h"

class Entity;
class EntityComponentSystem {
public:
	static const uint32_t MAX_ENTITIES = 10000;
	static const uint8_t MAX_COMPONENT_TYPES = 64;
	template<typename T>
	static ComponentID GetComponentID() {
		static ComponentID s_component_id = s_component_counter++;
		return s_component_id;
	}

	~EntityComponentSystem() {
		for (auto components : m_component_arrays) {
			delete components;
		}
	}

	Entity CreateEntity();

	template<typename T>
	void AddComponent(EntityID entity, T component) {
		// Update component arrays.
		uint32_t component_id = GetComponentID<T>();
		if (component_id >= m_component_arrays.size()) {
			m_component_arrays.resize(component_id + 1);
		}
		if (m_component_ids[component_id] == false) {
			m_component_arrays[component_id] = new ComponentArray<T>();
			m_component_ids[component_id] = true;
		}
		static_cast<ComponentArray<T>*>(m_component_arrays[component_id])->AddComponentToEntity(entity, component);

		// Update entity signatures.
		ComponentMask& entity_signature = m_entity_signatures.GetComponent(entity);
		entity_signature |= GetComponentMask(component_id);

		// Update views.
		for (auto& pair : m_views) {
			auto& mask = pair.first;
			auto& view = pair.second;

			// Entity signature matches system signature - insert into set
			if ((entity_signature & mask) == mask) {
				view.insert(entity);
			}
			// Entity signature does not match system signature - erase from set
			else {
				view.erase(entity);
			}
		}
	}

	template<typename T>
	T& GetComponent(EntityID entity) {
		ComponentID component_id = GetComponentID<T>();
		assert(component_id < m_component_arrays.size() && "Component not added yet");
		return static_cast<ComponentArray<T>*>(m_component_arrays[component_id])->GetComponent(entity);
	}

	template<typename T>
	void DeleteComponent(EntityID entity) {
		// Update component arrays.
		ComponentID component_id = GetComponentID<T>();
		assert(component_id < m_component_arrays.size() && "Component not added yet");
		static_cast<ComponentArray<T>*>(m_component_arrays[component_id])->RemoveComponentFromEntity(entity);

		// Update entity signatures.
		ComponentMask& entity_signature = m_entity_signatures.GetComponent(entity);
		entity_signature &= ~GetComponentMask(component_id);

		// Update views.
		for (auto& pair : m_views) {
			auto& mask = pair.first;
			auto& view = pair.second;

			// Entity signature matches system signature - insert into set
			if ((entity_signature & mask) == mask) {
				view.insert(entity);
			}
			// Entity signature does not match system signature - erase from set
			else {
				view.erase(entity);
			}
		}
	}

	// Returns all entities with matching component.
	// For more complicated queries, use views.
	template<typename T>
	std::vector<EntityID> FindEntitiesWithComponent(T component) {
		std::vector<EntityID> entities;
		auto component_array = static_cast<ComponentArray<T>*>(m_component_arrays[GetComponentID<T>()]);
		for (size_t i = 0; i <= component_array->m_last_index; i++) {
			if (component_array->m_components[i] == component) {
				entities.emplace_back(component_array->m_index_to_entity_map[i]);
			}
		}
		return entities;
	}

	void SetTag(EntityID entity, std::string tag);

	void DeleteTag(EntityID entity) {
		assert(m_entity_tags.EntityHasComponent(entity) && "Attempting to delete entity without tag assigned.");
		m_entity_tags.DestroyEntity(entity);
	}

	// Careful. Not performant.
	// Return invalid EntityID (0) if no entity has tag.
	Entity FindEntityByTag(const std::string& tag);

	template<typename ...ComponentTypes>
	void RegisterView() {
		ComponentMask mask = 0;
		int component_ids[] = { GetComponentID<ComponentTypes>() ... };
		for (int i = 0; i < (sizeof...(ComponentTypes)); i++) {
			mask |= GetComponentMask(component_ids[i]);
		}
		if (m_views.find(mask) == m_views.end()) {
			m_views[mask] = std::set<EntityID>();
		}
	}

	template<typename ...ComponentTypes>
	const std::set<EntityID>& GetView() {
		ComponentMask mask = 0;
		int component_ids[] = { GetComponentID<ComponentTypes>() ... };
		for (int i = 0; i < (sizeof...(ComponentTypes)); i++) {
			mask |= GetComponentMask(component_ids[i]);
		}
		
		return m_views[mask];
	}

	void DestroyEntity(EntityID entity) {
		ComponentMask mask = m_entity_signatures.GetComponent(entity);
		// Loop through all components that the entity has.
		while (mask) {
			ComponentID id = std::countr_zero(mask);
			mask &= ~(1 << id);
			m_component_arrays[id]->DestroyEntity(entity);
		}
		m_entity_signatures.DestroyEntity(entity);
		if (m_entity_tags.EntityHasComponent(entity)) {
			m_entity_tags.DestroyEntity(entity);
		}

		// Update views.
		for (auto& pair : m_views) {
			auto& view = pair.second;
			view.erase(entity);
		}
	}

	
private:
	static uint64_t s_entity_counter;
	static uint8_t s_component_counter;

	std::array<bool, MAX_COMPONENT_TYPES> m_component_ids = {};
	std::vector<IComponentArray*> m_component_arrays;
	std::unordered_map<ComponentMask, std::set<EntityID>> m_views;
	ComponentArray<ComponentMask> m_entity_signatures;
	ComponentArray<std::string> m_entity_tags;

	// Only supports 64 component types for now.
	ComponentMask GetComponentMask(uint8_t component_id) {
		return static_cast<ComponentMask>(1) << component_id;
	}
};
extern EntityComponentSystem g_ecs;