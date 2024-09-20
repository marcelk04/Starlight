#include "Pipeline.hpp"

#include <stdexcept>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace stl {

Pipeline::Pipeline(const std::string& vsFilepath, const std::string& fsFilepath) {
	createGraphicsPipeline(vsFilepath, fsFilepath);
}

std::vector<char> Pipeline::readFile(const std::string& filepath) {
	std::ifstream file(std::filesystem::absolute(filepath), std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file: " + filepath);
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

void Pipeline::createGraphicsPipeline(const std::string& vsFilepath, const std::string& fsFilepath) {
	auto vertCode = readFile(vsFilepath);
	auto fragCode = readFile(fsFilepath);

	std::cout << "Vertex shader code size: " << vertCode.size() << '\n';
	std::cout << "Fragment shader code size: " << fragCode.size() << '\n';
}

}