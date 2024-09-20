#include "FirstApp.hpp"

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
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}