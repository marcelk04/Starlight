#include "FirstApp.hpp"

#include "Core/Logger.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <filesystem>

int main() {
    std::filesystem::current_path(PROJ_DIR);

    stl::FirstApp app;

    try {
        app.run();
    } catch (const std::exception& e) {
        SFATAL(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}