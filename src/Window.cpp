#include "Window.hpp"

namespace stl {

Window::Window(int width, int height, const std::string& name)
	: m_Width(width), m_Height(height), m_Name(name) {
	initWindow();
}

Window::~Window() {
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

bool Window::shouldClose() const {
	return glfwWindowShouldClose(m_Window);
}

void Window::initWindow() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_Window = glfwCreateWindow(m_Width, m_Height, m_Name.c_str(), nullptr, nullptr);
}

}