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
	virtual void OnAwake() {};
	virtual void OnCreate() {};
	virtual void OnUpdate() {};
	virtual void OnDestroy() {};

private:
};