#include "pch.h"
#include "EntityComponentSystem.h"
#include "Entity.h"

// If entity has 0 ID then it is invalid
uint64_t EntityComponentSystem::s_entity_counter = 1;
uint8_t EntityComponentSystem::s_component_counter = 0;

Entity EntityComponentSystem::CreateEntity() {
	EntityID entity_id = s_entity_counter++;
	m_entity_signatures.AddComponentToEntity(entity_id, 0);
	Entity entity(entity_id, this);
	return entity;
}

void EntityComponentSystem::SetTag(EntityID entity, std::string tag) {
	if (m_entity_tags.EntityHasComponent(entity)) {
		assert(FindEntityByTag(tag).m_id == entity && "Tag already exists. Make sure each entity tag is unique.");
		auto& tag_ref = m_entity_tags.GetComponent(entity);
		tag_ref = tag;
	}
	else {
		assert(FindEntityByTag(tag).m_id == 0 && "Tag already exists. Make sure each entity tag is unique.");
		m_entity_tags.AddComponentToEntity(entity, tag);
	}
}

Entity EntityComponentSystem::FindEntityByTag(const std::string& tag) {
	for (size_t i = 0; i < m_entity_tags.End(); i++) {
		if (m_entity_tags[i] == tag) {
			return Entity(m_entity_tags.m_index_to_entity_map[i], this);
		}
	}
	return Entity(0, this);
}