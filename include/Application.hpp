#pragma once

#include "GaussianSplatting/PointCloud.hpp"
#include "GaussianSplatting/Ellipsoids.hpp"
#include "renderer/rendersystems/SimpleRenderSystem.hpp"
#include "renderer/rendersystems/PointLightSystem.hpp"
#include "renderer/rendersystems/GaussianSystem.hpp"
#include "renderer/wrapper/Window.hpp"
#include "renderer/wrapper/Pipeline.hpp"
#include "renderer/wrapper/Device.hpp"
#include "renderer/wrapper/Descriptors.hpp"
#include "renderer/Model.hpp"
#include "renderer/Renderer.hpp"
#include "GameObject.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <vector>
#include <memory>
#include <string>

namespace stl {

class Application {
public:
	Application(const std::string& gaussiansPath);
	~Application();

	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	void run();

private:
	void loadGameObjects();

public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

private:
	std::string m_GaussiansPath;

	Window m_Window{ WIDTH, HEIGHT, "Starlight" };
	Device m_Device{ m_Window };
	Renderer m_Renderer{ m_Window, m_Device };

	std::unique_ptr<DescriptorPool> m_GlobalPool{};

	GameObject::Map m_GameObjects;
	std::shared_ptr<Ellipsoids> m_Ellipsoids;
};

}