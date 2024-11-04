#pragma once
#include "pch.h"
#include "glm/glm.hpp"

class AABB {
public:
    glm::vec3 m_min;
    glm::vec3 m_max;
    AABB() {}
    AABB(glm::vec3 min, glm::vec3 max) : m_min(min), m_max(max) {}
    void Translate(glm::vec3 translation);
    static bool Collision(AABB a1, AABB a2);
};
