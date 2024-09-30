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

	VkExtent2D getExtent() const { return { static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height) }; }
	bool shouldClose() const;
	bool wasWindowResized() const { return m_FramebufferResized; }
	void resetWindowResizedFlag() { m_FramebufferResized = false; }

	void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

private:
	void initWindow();

private:
	int m_Width;
	int m_Height;
	bool m_FramebufferResized = false;

	std::string m_Name;
	GLFWwindow* m_Window;
};

}