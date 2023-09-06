#pragma once
#include "pch.h"
#include "ecs/EntityComponentSystem.h"

// A scene is a collection of entities.
class Scene {
public:
	std::unique_ptr<EntityComponentSystem> m_ecs;
	Scene();

	void Init();
	void OnUpdate();
	void RenderScene();
	void Destroy();
private:
};