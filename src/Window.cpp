#include "renderer/wrapper/Window.hpp"

#include "input/Input.hpp"

#include <stdexcept>
#include <utility>

namespace stl {

Window::Window(int width, int height, const std::string& name)
	: m_Width(width), m_Height(height), m_Name(name) {
	initWindow();

	Input::setWindowPtr(m_Window);
}

Window::Window(Window&& other) noexcept 
	: m_Width{ std::exchange(other.m_Width, 0) },
	m_Height{ std::exchange(other.m_Height, 0) },
	m_FramebufferResized{ std::exchange(other.m_FramebufferResized, false) },
	m_Name{ std::move(other.m_Name) },
	m_Window{ std::exchange(other.m_Window, nullptr) } {
}

Window::~Window() {
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

Window& Window::operator=(Window&& other) noexcept {
	Window& copy(other);

	std::swap(m_Width, copy.m_Width);
	std::swap(m_Height, copy.m_Height);
	std::swap(m_FramebufferResized, copy.m_FramebufferResized);
	std::swap(m_Name, copy.m_Name);
	std::swap(m_Window, copy.m_Window);

	return *this;
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