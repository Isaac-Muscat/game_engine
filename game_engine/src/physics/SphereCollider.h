#pragma once
#include "pch.h"
#include "glm/glm.hpp"

class SphereCollider {
public:
    glm::vec3 m_pos;
    float m_r;
    SphereCollider() {}
    SphereCollider(glm::vec3 pos, float radius) : m_pos(pos), m_r(radius) {}
    void Translate(glm::vec3 translation) {
        m_pos += translation;
    }
};
