#include "TerrainManager.h"
#include "ecs/Components.h"
#include "ecs/EntityScript.h"
#include "core/Time.h"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/constants.hpp"
#include "hid/Input.h"
#include "physics/SphereCollider.h"
#include "physics/TriangleBVH.h"
#include <memory>

class PlayerController : public EntityScript {
public:
    std::shared_ptr<TerrainManager> terrain_manager;
    // camera options
    float MovementSpeed = 5.0f;
    float MouseSensitivity = 0.1f;

    glm::vec3 acceleration = glm::vec3(0.0f, -9.8f, 0.0f);
    glm::vec3 velocity = glm::vec3(0.0f);
    float max_speed = 10.0f;
    glm::vec3 prev_pos{};
    bool grounded = false;

    void OnAwake() override {
        terrain_manager = FindEntityByTag("terrain_manager").GetScriptComponent<TerrainManager>();
    }

	void OnUpdate() override {
        TransformComponent& transform = GetComponent<TransformComponent>();
        Camera& camera = GetComponent<CameraComponent>().camera;
        TriangleBVH bvh = terrain_manager->GetClosestChunk(transform.position).GetComponent<MeshColliderComponent>().bvh;
        SphereCollider s = GetComponent<SphereColliderComponent>().collider;

        prev_pos = transform.position;
        float distance = MovementSpeed * Time::DeltaTime(); //* deltaTime;


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
        transform.position += velocity * Time::DeltaTime();

        if (!grounded) {
            glm::vec3 delta_velocity = acceleration * Time::DeltaTime();
            float velocity_mag = glm::length(velocity + delta_velocity);
            if (velocity_mag < max_speed) {
                velocity += delta_velocity;
            } else {
                velocity = velocity / velocity_mag * max_speed;
            }
        }
        if (Input::GetKeyPressed(GLFW_KEY_SPACE)) {
            velocity = glm::vec3(0.0f);
        }

        s.Translate(transform.position);
        // Physics!!?! ... sort of 
        Triangle out;
        if (bvh.Intersect(s, out)) {
            transform.position.y -= glm::dot((transform.position - prev_pos) , glm::normalize(out.GetNormal()));
            velocity = glm::vec3(0);
            if (glm::dot(out.GetNormal(), glm::vec3(0, 1, 0)) > 0) {
                grounded = true;
            }
        } 
        // TODO need to add sphere collider in collision physics loop
        /*
        if (aabb.collided) {
            transform.position = prev_pos;
            velocity = glm::vec3(0);
            grounded = true;
        }
        */

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
