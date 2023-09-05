#pragma once
#include "pch.h"
#include "ComponentArray.h"
#include "Entity.h"
#include "Components.h"

class Scene {
public:
	static const uint32_t MAX_ENTITIES = 10000;
	static const uint8_t MAX_COMPONENT_TYPES = 64;

	template<typename T>
	static ComponentID GetComponentID() {
		static ComponentID s_component_id = s_component_counter++;
		return s_component_id;
	}

	template<typename T>
	void AddComponent(EntityID entity, T component) {
		// Update component arrays.
		uint8_t component_id = GetComponentID<T>();
		if (component_id >= m_component_arrays.size()) {
			m_component_arrays.resize(component_id + 1);
			m_component_arrays[component_id] = new ComponentArray<T>();
		}
		static_cast<ComponentArray<T>*>(m_component_arrays[component_id])->AddComponentToEntity(entity, component);

		// Update entity signatures.
		ComponentMask& entity_signature = m_entity_signatures.GetComponent(entity);
		entity_signature |= GetComponentMask(component_id);

		// Update scene views.
		for (auto& pair : m_scene_views) {
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
		ComponentMask& mask = m_entity_signatures.GetComponent(entity);
		mask &= ~GetComponentMask(component_id);

		// Update scene views.
		for (auto& pair : m_scene_views) {
			auto& view = pair.second;
			view.erase(entity);
		}
	}

	template<typename ...ComponentTypes>
	void RegisterView() {
		ComponentMask mask = 0;
		int component_ids[] = { GetComponentID<ComponentTypes>() ... };
		for (int i = 0; i < (sizeof...(ComponentTypes)); i++) {
			mask |= GetComponentMask(component_ids[i]);
		}
		if (m_scene_views.find(mask) == m_scene_views.end()) {
			m_scene_views[mask] = std::set<EntityID>();
		}
	}

	template<typename ...ComponentTypes>
	const std::set<EntityID>& GetView() {
		ComponentMask mask = 0;
		int component_ids[] = { GetComponentID<ComponentTypes>() ... };
		for (int i = 0; i < (sizeof...(ComponentTypes)); i++) {
			mask |= GetComponentMask(component_ids[i]);
		}
		
		return m_scene_views[mask];
	}

	EntityID CreateEntity() {
		EntityID entity = s_entity_counter++;
		m_entity_signatures.AddComponentToEntity(entity, 0);
		return entity;
	}

	void DestroyEntity(EntityID entity) {
		ComponentMask mask = m_entity_signatures.GetComponent(entity);
		// Loop through all components that the entity has.
		while (mask) {
			ComponentID id = std::countr_zero(mask);
			mask &= ~(1 << id);
			m_component_arrays[id]->DestroyEntity(entity);
		}
		m_entity_signatures.RemoveComponentFromEntity(entity);
	}

	
private:
	static uint64_t s_entity_counter;
	static uint8_t s_component_counter;

	std::vector<IComponentArray*> m_component_arrays;
	std::unordered_map<ComponentMask, std::set<EntityID>> m_scene_views;
	ComponentArray<ComponentMask> m_entity_signatures;

	// Only supports 64 component types for now.
	ComponentMask GetComponentMask(uint8_t component_id) {
		// Prevent
		return static_cast<ComponentMask>(1) << component_id;
	}
};
