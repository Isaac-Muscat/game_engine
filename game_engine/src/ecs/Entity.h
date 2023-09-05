#pragma once
#include "pch.h"

typedef uint64_t EntityID;
typedef uint64_t ComponentMask;
typedef uint8_t ComponentID;

class Scene;
class Entity {
	Scene* m_scene;
	EntityID m_entity;
};