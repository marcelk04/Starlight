#pragma once

#include "Window.hpp"
#include "Pipeline.hpp"
#include "Device.hpp"

namespace stl {

class FirstApp {
public:
	void run();

public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

private:
	Window m_Window{ WIDTH, HEIGHT, "Starlight" };
	Device m_Device{ m_Window };
	Pipeline m_Pipeline{ m_Device, "shaders/SimpleShader.vert.spv", "shaders/SimpleShader.frag.spv", Pipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT) };
};

}