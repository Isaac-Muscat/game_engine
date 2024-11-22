#pragma once
#include "pch.h"
#include "glm/glm.hpp"

#include "AABB.h"
#include "physics/SphereCollider.h"


struct Plane {
    glm::vec3 n;
    float d;
};

struct Triangle {
    glm::vec3 v[3];
    glm::vec3 GetNormal() {
        glm::vec3 l1 = v[0] - v[1];
        glm::vec3 l2 = v[0] - v[2];
        return glm::cross(l1, l2);
    }
    glm::vec3 Max() {
        return glm::max(glm::max(v[0], v[1]), v[2]);
    }
    void Translate(glm::vec3 translation){
        v[0] += translation;
        v[1] += translation;
        v[2] += translation;
    }
};

bool PlaneAABBIntersect(Plane p, AABB b);
bool TriangleAABBIntersect(Triangle t, AABB b);
bool TriangleSphereIntersect(Triangle t, SphereCollider sc);
bool SATIntersect(Triangle t, glm::vec3 axis, glm::vec3 e);

class TriangleBVH {
private:
    std::vector<Triangle> m_triangles;

public:
    glm::vec3 m_position;
    TriangleBVH() {}
    void InsertTriangle(Triangle t);
    bool Intersect(AABB aabb, Triangle& out);
    bool Intersect(SphereCollider sc, Triangle& out);
    bool Intersect(SphereCollider sc, glm::vec3& out);
};
