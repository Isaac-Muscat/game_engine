#pragma once
#include "pch.h"
#include "ecs/EntityComponentSystem.h"
#include "ecs/Entity.h"

// A scene is a collection of entities.
class Scene {
public:
	std::unique_ptr<EntityComponentSystem> m_ecs;
	Entity m_main_camera;
	Scene();

	Camera GetMainCamera() const {
		return m_main_camera.GetComponent<CameraComponent>().camera;
	}

	void SetMainCamera(Entity camera_entity) {
		m_main_camera = camera_entity;
	}

	void Init();
	void OnUpdate();
	void RenderScene();
	void Destroy();
private:
};