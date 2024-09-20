#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace stl {

class Window {
public:
	Window(int width, int height, const std::string& name);
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	bool shouldClose() const;

	void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

private:
	void initWindow();

private:
	const int m_Width;
	const int m_Height;

	std::string m_Name;
	GLFWwindow* m_Window;
};

}