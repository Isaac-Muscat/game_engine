#include "glm/ext/quaternion_geometric.hpp"
#include "pch.h"
#include "TriangleBVH.h"
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"

float Max(float a, float b, float c) {
    return glm::max(glm::max(a, b), c);
}

float Min(float a, float b, float c) {
    return glm::min(glm::min(a, b), c);
}

// Taken from Christer Ericson: Real Time Collision Detection
bool PlaneAABBIntersect(Plane p, AABB b) {
    // These two lines not necessary with a (center, extents) AABB representation
    glm::vec3 c = (b.m_max + b.m_min) * 0.5f; // Compute AABB center
    glm::vec3 e = b.m_max - c; // Compute positive extents
    // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
    float r = e[0]*abs(p.n[0]) + e[1]*abs(p.n[1]) + e[2]*abs(p.n[2]);
    // Compute distance of box center from plane
    float s = glm::dot(p.n, c) - p.d;
    // Intersection occurs when distance s falls within [-r,+r] interval
    return abs(s) <= r;
}


bool SATIntersect(Triangle t, glm::vec3 axis, glm::vec3 e) {
    glm::vec3 u0 = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 u1 = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 u2 = glm::vec3(0.0f, 0.0f, 1.0f);

    float p0 = glm::dot(t.v[0], axis);
    float p1 = glm::dot(t.v[1], axis);
    float p2 = glm::dot(t.v[2], axis);
    float r = e.x * abs(glm::dot(u0, axis)) +
        e.y * abs(glm::dot(u1, axis)) +
        e.z * abs(glm::dot(u2, axis));
    if (glm::max(-Max(p0, p1, p2), Min(p0, p1, p2)) > r) return false;
    return true;
}

// Taken from Christer Ericson: Real Time Collision Detection
bool TriangleAABBIntersect(Triangle t, AABB b) {

    glm::vec3 c = (b.m_min + b.m_max) * 0.5f;
    float e0 = (b.m_max.x - b.m_min.x) * 0.5f;
    float e1 = (b.m_max.y - b.m_min.y) * 0.5f;
    float e2 = (b.m_max.z - b.m_min.z) * 0.5f;
    glm::vec3 e = {e0, e1, e2};

    t.v[0] = t.v[0] - c;
    t.v[1] = t.v[1] - c;
    t.v[2] = t.v[2] - c;

    glm::vec3 f0 = t.v[1] - t.v[0], f1 = t.v[2] - t.v[1], f2 = t.v[0] - t.v[2];

    glm::vec3 u0 = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 u1 = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 u2 = glm::vec3(0.0f, 0.0f, 1.0f);

    // Compute the 9 axis
    glm::vec3 axis_u0_f0 = glm::cross(u0, f0);
    glm::vec3 axis_u0_f1 = glm::cross(u0, f1);
    glm::vec3 axis_u0_f2 = glm::cross(u0, f2);

    glm::vec3 axis_u1_f0 = glm::cross(u1, f0);
    glm::vec3 axis_u1_f1 = glm::cross(u1, f1);
    glm::vec3 axis_u1_f2 = glm::cross(u2, f2);

    glm::vec3 axis_u2_f0 = glm::cross(u2, f0);
    glm::vec3 axis_u2_f1 = glm::cross(u2, f1);
    glm::vec3 axis_u2_f2 = glm::cross(u2, f2);

    if (!SATIntersect(t, axis_u0_f0, e)) return false;
    if (!SATIntersect(t, axis_u0_f1, e)) return false;
    if (!SATIntersect(t, axis_u0_f2, e)) return false;
    if (!SATIntersect(t, axis_u1_f0, e)) return false;
    if (!SATIntersect(t, axis_u1_f1, e)) return false;
    if (!SATIntersect(t, axis_u1_f2, e)) return false;
    if (!SATIntersect(t, axis_u2_f0, e)) return false;
    if (!SATIntersect(t, axis_u2_f1, e)) return false;
    if (!SATIntersect(t, axis_u2_f2, e)) return false;



    // Test the three axes corresponding to the face normals of AABB b (category 1).
    // Exit if...
    // ... [-e0, e0] and [min(v0.x,v1.x,v2.x), max(v0.x,v1.x,v2.x)] do not overlap
    if (Max(t.v[0].x, t.v[1].x, t.v[2].x) < -e0 || Min(t.v[0].x, t.v[1].x, t.v[2].x) > e0) return false;
    // ... [-e1, e1] and [min(v0.y,v1.y,v2.y), max(v0.y,v1.y,v2.y)] do not overlap
    if (Max(t.v[0].y, t.v[1].y, t.v[2].y) < -e1 || Min(t.v[0].y, t.v[1].y, t.v[2].y) > e1) return false;
    // ... [-e2, e2] and [min(v0.z,v1.z,v2.z), max(v0.z,v1.z,v2.z)] do not overlap
    if (Max(t.v[0].z, t.v[1].z, t.v[2].z) < -e2 || Min(t.v[0].z, t.v[1].z, t.v[2].z) > e2) return false;


    // Test separating axis corresponding to triangle face normal (category 2)
    Plane p;
    p.n = glm::cross(f0, f1);
    p.d = glm::dot(p.n, t.v[0]);
    AABB b2 = {b.m_min - c, b.m_max - c};
    return PlaneAABBIntersect(p, b2);
}

void TriangleBVH::InsertTriangle(Triangle t) {
    m_triangles.push_back(t);
}

bool TriangleBVH::Intersect(AABB aabb, Triangle& out) {
    // Move bounding box is same as moving triangle
    aabb.Translate(-m_position);
    for (auto& tri : m_triangles) {
        if (TriangleAABBIntersect(tri, aabb)) {
            out = tri;
            return true;
        }
    }
    return false;
}