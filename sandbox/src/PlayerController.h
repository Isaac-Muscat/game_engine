#include "ecs/EntityScript.h"
#include "hid/Input.h"

class PlayerController : public EntityScript {
public:
	Entity player1;
	void OnCreate() override {
		player1 = m_entity.GetECS()->FindEntityByTag("Player1");
		std::string tag = player1.GetComponent<NameComponent>().name;
		std::cout << "Found " << tag << ". Created player controller." << std::endl;
	}

	void OnUpdate() override {
		TransformComponent& t = GetComponent<TransformComponent>();
		if (Input::GetKeyPressed(GLFW_KEY_A)) {
			t.position.x += 1;
			//Print(t);
		} else if (Input::GetKeyPressed(GLFW_KEY_D)) {
			t.position.x -= 1;
			//Print(t);
		}
	}

	void Print(TransformComponent& t) {
		std::cout << t.position.x << std::endl;
		/*std::cout << t.position.y << std::endl;
		std::cout << t.position.z << std::endl;*/
	}
};