#pragma once

#include "input/Input.hpp"
#include "GameObject.hpp"

namespace stl {

class KeyboardMovementController {
public:
	struct KeyMappings {
		Key moveLeft = Key::A;
		Key moveRight = Key::D;
		Key moveForward = Key::W;
		Key moveBackward = Key::S;
		Key moveUp = Key::E;
		Key moveDown = Key::Q;

		Key lookLeft = Key::LEFT;
		Key lookRight = Key::RIGHT;
		Key lookUp = Key::UP;
		Key lookDown = Key::DOWN;
	};

public:
	void moveInPlaneXZ(float dt, GameObject& gameObject) const;

public:
	KeyMappings p_Keys{};

	float p_MoveSpeed{ 3.0f };
	float p_LookSpeed{ 1.5f };
};

}