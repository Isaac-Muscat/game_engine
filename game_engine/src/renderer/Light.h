#pragma once
#include "pch.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

enum class LightType {
    Directional,
    Point,
    Spot,
    Area // Going to take some time to implement this one
};

struct Light {
    alignas(4) LightType type;
    alignas(4) float ambient;
    alignas(4) float diffuse;
    alignas(4) float specular;
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 direction;
    alignas(16) glm::vec3 color;
};

//struct Light {
//    alignas(16) glm::vec3 position;
//    alignas(16) glm::vec3 direction;
//    alignas(16) glm::vec3 color;
//    alignas(4) LightType type;
//    alignas(4) float ambient;
//    alignas(4) float diffuse;
//    alignas(4) float specular;
//};
