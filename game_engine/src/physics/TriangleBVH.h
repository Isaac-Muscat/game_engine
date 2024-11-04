#pragma once
#include "pch.h"
#include "glm/glm.hpp"

#include "AABB.h"


struct Plane {
    glm::vec3 n;
    float d;
};

struct Triangle {
    glm::vec3 v[3];
    glm::vec3 GetNormal() {
        return glm::cross(v[1], v[0]);
    }
};

bool PlaneAABBIntersect(Plane p, AABB b);
bool TriangleAABBIntersect(Triangle t, AABB b);
bool SATIntersect(Triangle t, glm::vec3 axis, glm::vec3 e);

class TriangleBVH {
private:
    std::vector<Triangle> m_triangles;

public:
    glm::vec3 m_position;
    TriangleBVH() {}
    void InsertTriangle(Triangle t);
    bool Intersect(AABB aabb, Triangle& out);
};
