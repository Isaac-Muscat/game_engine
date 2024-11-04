#pragma once
#include "pch.h"
#include "Entity.h"

// When adding scripts, inherit from this class.
class EntityScript {
public:
	Entity m_entity;
	template<typename T>
	T& GetComponent() {
		return m_entity.GetComponent<T>();
	}
	template<typename T>
	void AddComponent(T component) {
		m_entity.AddComponent<T>(component);
	}
    Entity CreateEntity() {
        return m_entity.GetECS()->CreateEntity();
    }

	Entity FindEntityByTag(const std::string& tag) {
        return m_entity.GetECS()->FindEntityByTag(tag);
    }
	virtual void OnAwake() {};
	virtual void OnCreate() {};
	virtual void OnUpdate() {};
	virtual void OnDestroy() {};

private:
};
