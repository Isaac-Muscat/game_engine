#pragma once
#include "pch.h"
#include "EntityTypes.h"
#include "EntityComponentSystem.h"

// Main interface object that wraps the ecs.
class Entity {
public:
	friend class EntityComponentSystem;
	Entity(EntityID id, EntityComponentSystem* ecs) : m_id(id), m_ecs(ecs) {}
	Entity() { m_id = 0; m_ecs = nullptr; };
	template<typename T>
	void AddComponent(T component) {
		m_ecs->AddComponent<T>(m_id, component);
	}

	template<typename T>
	T& GetComponent() {
		return m_ecs->GetComponent<T>(m_id);
	}

	template<typename T>
	void DeleteComponent() {
		m_ecs->DeleteComponent<T>(m_id);
	}

	// Careful. Not performant.
	void SetTag(std::string tag) {
		m_ecs->SetTag(m_id, tag);
	}

	// Careful. Not performant.
	void DeleteTag() {
		return m_ecs->DeleteTag(m_id);
	}
private:
	EntityComponentSystem* m_ecs;
	EntityID m_id;
};