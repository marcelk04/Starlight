#include "Window.hpp"

#include "Input.hpp"

#include <stdexcept>

namespace stl {

Window::Window(int width, int height, const std::string& name)
	: m_Width(width), m_Height(height), m_Name(name) {
	initWindow();

	Input::setWindowPtr(m_Window);
}

Window::~Window() {
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

bool Window::shouldClose() const {
	return glfwWindowShouldClose(m_Window);
}

void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
	if (glfwCreateWindowSurface(instance, m_Window, nullptr, surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create window surface!");
	}
}

void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	Window* stlWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

	stlWindow->m_FramebufferResized = true;
	stlWindow->m_Width = width;
	stlWindow->m_Height = height;
}

void Window::initWindow() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	m_Window = glfwCreateWindow(m_Width, m_Height, m_Name.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(m_Window, this);
	glfwSetFramebufferSizeCallback(m_Window, framebufferResizeCallback);
}

}