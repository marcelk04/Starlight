#include "GameObject.hpp"

namespace stl {

GameObject GameObject::createGameObject() {
	static id_t currentId = 0;
	return GameObject{ currentId++ };
}

GameObject::GameObject(id_t objectId)
	: m_Id{ objectId } {
}

}