#include "Application.hpp"

#include "Core/Logger.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <filesystem>

int main(int argc, char** argv) {
    std::filesystem::current_path(PROJ_DIR);

    std::string modelPath = "";

    if (argc < 2) {
        SWARN("No path to gaussians passed. Loading default model.");
        modelPath = "assets/point_clouds/17_12_resolution_4x.ply";
    } else {
        modelPath = argv[1];
    }

    stl::Application app(modelPath);

    try {
        app.run();
    } catch (const std::exception& e) {
        SFATAL(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}