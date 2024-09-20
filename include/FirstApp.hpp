#pragma once

#include "Window.hpp"

namespace stl {

class FirstApp {
public:
	void run();

public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

private:
	Window m_Window{ WIDTH, HEIGHT, "Starlight" };
};

}