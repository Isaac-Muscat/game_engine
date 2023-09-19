#pragma once
#include "pch.h"
#include "ecs/EntityScript.h"
#include "hid/Input.h"
#include "core/Time.h"

class CameraController : public EntityScript {
public:
    // camera options
    float MovementSpeed = 5.0f;
    float MouseSensitivity = 0.1f;

	void OnUpdate() override {
        Camera& camera = GetComponent<CameraComponent>().camera;
        float velocity = MovementSpeed * Time::DeltaTime(); //* deltaTime;
        if (Input::GetKeyPressed(GLFW_KEY_W))
            camera.Position += glm::normalize(glm::vec3(camera.Front.x, 0, camera.Front.z)) * velocity;
        if (Input::GetKeyPressed(GLFW_KEY_S))
            camera.Position -= glm::normalize(glm::vec3(camera.Front.x, 0, camera.Front.z)) * velocity;
        if (Input::GetKeyPressed(GLFW_KEY_A))
            camera.Position -= glm::normalize(glm::vec3(camera.Right.x, 0, camera.Right.z)) * velocity;
        if (Input::GetKeyPressed(GLFW_KEY_D))
            camera.Position += glm::normalize(glm::vec3(camera.Right.x, 0, camera.Right.z)) * velocity;
        if (Input::GetKeyPressed(GLFW_KEY_SPACE))
            camera.Position += camera.WorldUp * velocity;
        if (Input::GetKeyPressed(GLFW_KEY_LEFT_SHIFT))
            camera.Position -= camera.WorldUp * velocity;

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

        camera.UpdateCameraVectors();
	};
};