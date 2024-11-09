#include "TerrainManager.h"
#include "ecs/Components.h"
#include "ecs/EntityScript.h"
#include "core/Time.h"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/geometric.hpp"
#include "hid/Input.h"
#include "physics/TriangleBVH.h"
#include <memory>

class PlayerController : public EntityScript {
public:
    std::shared_ptr<TerrainManager> terrain_manager;
    // camera options
    float MovementSpeed = 5.0f;
    float MouseSensitivity = 0.1f;

    glm::vec3 accleration = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 prev_pos{};
    bool grounded = false;

    void OnAwake() override {
        terrain_manager = FindEntityByTag("terrain_manager").GetScriptComponent<TerrainManager>();
    }

	void OnUpdate() override {
        TransformComponent& transform = GetComponent<TransformComponent>();
        Camera& camera = GetComponent<CameraComponent>().camera;
        TriangleBVH bvh = terrain_manager->GetClosestChunk(transform.position).GetComponent<MeshColliderComponent>().bvh;
        AABBComponent aabb = GetComponent<AABBComponent>();
        aabb.aabb.Translate(transform.position);
        Triangle out;

        float distance = MovementSpeed * Time::DeltaTime(); //* deltaTime;

        if (!grounded) {
            velocity += accleration * Time::DeltaTime();
        }
        if (Input::GetKeyPressed(GLFW_KEY_SPACE)) {
            velocity = glm::vec3(0.0f);
        }
        // Physics!!?! ... sort of 
        if (bvh.Intersect(aabb.aabb, out)) {
            transform.position += distance * glm::normalize(out.GetNormal());
            velocity = glm::vec3(0);
            if (glm::dot(out.GetNormal(), glm::vec3(0, 1, 0)) > 0) {
                grounded = true;
            }
        } 
        if (aabb.collided) {
            transform.position = prev_pos;
            velocity = glm::vec3(0);
            grounded = true;
        }

        prev_pos = transform.position;
        transform.position += velocity * Time::DeltaTime();

        if (Input::GetKeyPressed(GLFW_KEY_W)) {
            transform.position += glm::normalize(glm::vec3(camera.Front.x, 0, camera.Front.z)) * distance;
            grounded = false;
        }
        if (Input::GetKeyPressed(GLFW_KEY_S)) {
            transform.position -= glm::normalize(glm::vec3(camera.Front.x, 0, camera.Front.z)) * distance;
            grounded = false;
        }
        if (Input::GetKeyPressed(GLFW_KEY_A)) {
            transform.position -= glm::normalize(glm::vec3(camera.Right.x, 0, camera.Right.z)) * distance;
            grounded = false;
        }
        if (Input::GetKeyPressed(GLFW_KEY_D)) {
            transform.position += glm::normalize(glm::vec3(camera.Right.x, 0, camera.Right.z)) * distance;
            grounded = false;
        }
        if (Input::GetKeyPressed(GLFW_KEY_SPACE)) {
            transform.position += Camera::WorldUp * distance;
            grounded = false;
        }
        if (Input::GetKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
            transform.position -= Camera::WorldUp * distance;
            grounded = false;
        }

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
