#pragma once
#include "pch.h"
#include "Entity.h"

// When adding scripts, inherit from this class.
class ScriptableEntity {
public:
	template<typename T>
	T& GetComponent() {
		return m_entity.GetComponent<T>();
	}
	virtual void OnCreate() {};
	virtual void OnUpdate() {};
	virtual void OnDestroy() {};

private:
	Entity m_entity;
};