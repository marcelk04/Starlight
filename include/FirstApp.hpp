#pragma once

#include "Window.hpp"
#include "Pipeline.hpp"
#include "Device.hpp"
#include "Model.hpp"
#include "GameObject.hpp"
#include "Renderer.hpp"
#include "SimpleRenderSystem.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <vector>
#include <memory>

namespace stl {

class FirstApp {
public:
	FirstApp();
	~FirstApp();

	FirstApp(const FirstApp&) = delete;
	FirstApp& operator=(const FirstApp&) = delete;

	void run();

private:
	void loadGameObjects();

public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

private:
	Window m_Window{ WIDTH, HEIGHT, "Starlight" };
	Device m_Device{ m_Window };
	Renderer m_Renderer{ m_Window, m_Device };

	std::vector<GameObject> m_GameObjects;
};

}