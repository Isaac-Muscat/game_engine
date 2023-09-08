#pragma once
#include "pch.h"
#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Camera {
public:
    static constexpr glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 Position;
    glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up;
    glm::vec3 Right;

    // euler Angles
    float Yaw = 0.0f;
    float Pitch = 0.0f;

    Camera();
    Camera(glm::vec3 position);
    void UpdateCameraVectors();

    glm::mat4 GetViewMatrix() const { 
        return glm::lookAt(Position, Position + Front, Up);
    }
    glm::mat4 GetProjectionMatrix(float aspect_ratio) const {
        return glm::perspective(glm::radians(45.0f), aspect_ratio, 0.1f, 10.0f);
    };
};

