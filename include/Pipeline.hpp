#pragma once

#include <string>
#include <vector>

namespace stl {

class Pipeline {
public:
	Pipeline(const std::string& vsFilepath, const std::string& fsFilepath);

private:
	static std::vector<char> readFile(const std::string& filepath);

	void createGraphicsPipeline(const std::string& vsFilepath, const std::string& fsFilepath);

private:

};

}