#pragma once

#include "KeyCodes.hpp"
#include "MouseButtonCodes.hpp"

#include <glfw/glfw3.h>

#include <utility>

namespace stl {

class Input {
public:
	Input() = default;

	static bool isKeyPressed(Key key) { return s_Instance.isKeyPressedImpl(key); }

	static bool isMouseButtonPressed(MouseButton button) { return s_Instance.isMouseButtonPressedImpl(button); }
	static std::pair<float, float> getMousePosition() { return s_Instance.getMousePositionImpl(); }
	static float getMouseX() { return s_Instance.getMouseXImpl(); }
	static float getMouseY() { return s_Instance.getMouseYImpl(); }

	static void setWindowPtr(GLFWwindow* window) { s_Instance.setWindowPtrImpl(window); }

private:
	bool isKeyPressedImpl(Key key);

	bool isMouseButtonPressedImpl(MouseButton button);
	std::pair<float, float> getMousePositionImpl();
	float getMouseXImpl();
	float getMouseYImpl();

	void setWindowPtrImpl(GLFWwindow* window);

private:
	static Input s_Instance;

	GLFWwindow* m_Window{ nullptr };
};

}