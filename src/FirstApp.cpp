#include "FirstApp.hpp"

namespace stl {

void FirstApp::run() {
	while (!m_Window.shouldClose()) {
		glfwPollEvents();
	}
}

}