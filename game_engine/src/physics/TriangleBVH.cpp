#include "glm/ext/quaternion_geometric.hpp"
#include "pch.h"
#include "TriangleBVH.h"
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include "physics/SphereCollider.h"

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

glm::vec3 ClosestPtPointTriangle(glm::vec3 p, Triangle t) {
    glm::vec3 a = t.v[0];
    glm::vec3 b = t.v[1];
    glm::vec3 c = t.v[2];

    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    glm::vec3 bc = c - b;
    // Compute parametric position s for projection P’ of P on AB,
    // P’ = A + s*AB, s = snom/(snom+sdenom)
    float snom = glm::dot(p - a, ab), sdenom = glm::dot(p - b, a - b);
    // Compute parametric position t for projection P’ of P on AC,
    // P’ = A + t*AC, s = tnom/(tnom+tdenom)
    float tnom = glm::dot(p - a, ac), tdenom = glm::dot(p - c, a - c);
    if (snom <= 0.0f && tnom <= 0.0f) return a; // Vertex region early out
    // Compute parametric position u for projection P’ of P on BC,
    // P’ = B + u*BC, u = unom/(unom+udenom)
    float unom = glm::dot(p - b, bc), udenom = glm::dot(p - c, b - c);
    if (sdenom <= 0.0f && unom <= 0.0f) return b; // Vertex region early out
    if (tdenom <= 0.0f && udenom <= 0.0f) return c; // Vertex region early out
    // P is outside (or on) AB if the triple scalar product [N PA PB] <= 0
    glm::vec3 n = glm::cross(b - a, c - a);
    float vc = glm::dot(n, glm::cross(a - p, b - p));
    // If P outside AB and within feature region of AB,
    // return projection of P onto AB
    if (vc <= 0.0f && snom >= 0.0f && sdenom >= 0.0f)
    return a + snom / (snom + sdenom) * ab;
    // P is outside (or on) BC if the triple scalar product [N PB PC] <= 0
    float va = glm::dot(n, glm::cross(b - p, c - p));
    // If P outside BC and within feature region of BC,
    // return projection of P onto BC
    if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f)
    return b + unom / (unom + udenom) * bc;
    // P is outside (or on) CA if the triple scalar product [N PC PA] <= 0
    float vb = glm::dot(n, glm::cross(c - p, a - p));
    // If P outside CA and within feature region of CA,
    // return projection of P onto CA
    if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f)
    return a + tnom / (tnom + tdenom) * ac;
    // P must project inside face region. Compute Q using barycentric coordinates
    float u = va / (va + vb + vc);
    float v = vb / (va + vb + vc);
    float w = 1.0f - u - v; // = vc / (va + vb + vc)
    return u * a + v * b + w * c;
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

// Taken from Christer Ericson: Real Time Collision Detection
bool TriangleSphereIntersect(Triangle t, SphereCollider sc) {
    // Find point P on triangle ABC closest to sphere center
    glm::vec3 p = ClosestPtPointTriangle(sc.m_pos, t);
    // Sphere and triangle intersect if the (squared) distance from sphere
    // center to point p is less than the (squared) sphere radius
    glm::vec3 v = p - sc.m_pos;
    return glm::dot(v, v) <= sc.m_r * sc.m_r;
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

bool TriangleBVH::Intersect(SphereCollider sc, Triangle& out) {
    // Move bounding box is same as moving triangle
    sc.Translate(-m_position);
    for (auto& tri : m_triangles) {
        if (TriangleSphereIntersect(tri, sc)) {
            out = tri;
            return true;
        }
    }
    return false;
}

bool TriangleBVH::Intersect(SphereCollider sc, glm::vec3& out) {
    bool intersect = false;
    // Move bounding box is same as moving triangle
    sc.Translate(-m_position);
    for (auto& tri : m_triangles) {
        if (TriangleSphereIntersect(tri, sc)) {
            tri.Translate(m_position);
            out = intersect ? glm::max(tri.Max(), out) : tri.Max();
            intersect = true;
        }
    }
    return intersect;
}
