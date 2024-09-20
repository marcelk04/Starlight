#pragma once

#include "Window.hpp"
#include "Pipeline.hpp"

namespace stl {

class FirstApp {
public:
	void run();

public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

private:
	Window m_Window{ WIDTH, HEIGHT, "Starlight" };
	Pipeline m_Pipeline{ "shaders/SimpleShader.vert.spv", "shaders/SimpleShader.frag.spv" };
};

}