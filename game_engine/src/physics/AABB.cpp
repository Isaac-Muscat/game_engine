#include "pch.h"
#include "AABB.h"
#include "glm/glm.hpp"

void AABB::Translate(glm::vec3 translation) {
    m_min += translation;
    m_max += translation;
}
bool AABB::Collision(AABB a1, AABB a2) {
    if (a1.m_max.x < a2.m_min.x || a1.m_min.x > a2.m_max.x) return false;
    if (a1.m_max.y < a2.m_min.y || a1.m_min.y > a2.m_max.y) return false;
    if (a1.m_max.z < a2.m_min.z || a1.m_min.z > a2.m_max.z) return false;
    return true;
}
