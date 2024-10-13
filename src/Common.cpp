#include "Core/Common.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace stl {

std::vector<char> Common::readFile(const std::string& filepath) {
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

}