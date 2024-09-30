#include "input/Input.hpp"

namespace stl {

bool Input::isKeyPressedImpl(Key key) {
	int state = glfwGetKey(m_Window, static_cast<int>(key));
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::isMouseButtonPressedImpl(MouseButton button) {
	int state = glfwGetMouseButton(m_Window, static_cast<int>(button));
	return state == GLFW_PRESS;
}

std::pair<float, float> Input::getMousePositionImpl() {
	double xpos, ypos;
	glfwGetCursorPos(m_Window, &xpos, &ypos);
	return { static_cast<float>(xpos), static_cast<float>(ypos) };
}

float Input::getMouseXImpl() {
	auto [xpos, ypos] = getMousePositionImpl();
	return xpos;
}

float Input::getMouseYImpl() {
	auto [xpos, ypos] = getMousePositionImpl();
	return ypos;
}

void Input::setWindowPtrImpl(GLFWwindow* window) {
	m_Window = window;
}

Input Input::s_Instance;

}