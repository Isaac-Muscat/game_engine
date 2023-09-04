#pragma once
#include "pch.h"
#include "Components.h"

class Scene {
	ComponentArray<TransformComponent> m_transforms;
	ComponentArray<MeshComponent> m_meshes;
	ComponentArray<ScriptComponent> m_scripts;
};

typedef uint64_t Entity;