#pragma once
#include "pch.h"
#include "ecs/Components.h"
#include "ecs/EntityScript.h"
#include "hid/Input.h"
#include "core/Time.h"

class CameraController : public EntityScript {
public:
    // camera options
    float MovementSpeed = 5.0f;
    float MouseSensitivity = 0.1f;

	void OnUpdate() override {
        TransformComponent& transform = GetComponent<TransformComponent>();
        Camera& camera = GetComponent<CameraComponent>().camera;
        float velocity = MovementSpeed * Time::DeltaTime(); //* deltaTime;
        if (Input::GetKeyPressed(GLFW_KEY_W))
            transform.position += glm::normalize(glm::vec3(camera.Front.x, 0, camera.Front.z)) * velocity;
        if (Input::GetKeyPressed(GLFW_KEY_S))
            transform.position -= glm::normalize(glm::vec3(camera.Front.x, 0, camera.Front.z)) * velocity;
        if (Input::GetKeyPressed(GLFW_KEY_A))
            transform.position -= glm::normalize(glm::vec3(camera.Right.x, 0, camera.Right.z)) * velocity;
        if (Input::GetKeyPressed(GLFW_KEY_D))
            transform.position += glm::normalize(glm::vec3(camera.Right.x, 0, camera.Right.z)) * velocity;
        if (Input::GetKeyPressed(GLFW_KEY_SPACE))
            transform.position += Camera::WorldUp * velocity;
        if (Input::GetKeyPressed(GLFW_KEY_LEFT_SHIFT))
            transform.position -= Camera::WorldUp * velocity;

        static glm::vec2 last_mouse_pos = Input::GetMousePosition();
        static float first_move = true;

        if (first_move && last_mouse_pos != Input::GetMousePosition()) {
            last_mouse_pos = Input::GetMousePosition();
            first_move = false;
        }

        glm::vec2 mouse_pos = Input::GetMousePosition();
        camera.Yaw += (mouse_pos.x - last_mouse_pos.x) * MouseSensitivity;
        camera.Pitch -= (mouse_pos.y - last_mouse_pos.y) * MouseSensitivity;

        last_mouse_pos = mouse_pos;

        if (camera.Pitch > 89.0f)
            camera.Pitch = 89.0f;
        if (camera.Pitch < -89.0f)
            camera.Pitch = -89.0f;

        camera.Position = transform.position;
        camera.UpdateCameraVectors();
	};
};
