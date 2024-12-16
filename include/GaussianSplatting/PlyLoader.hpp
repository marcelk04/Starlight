#pragma once

#include "GaussianSplatting/SplatData.hpp"

#include <vector>
#include <memory>
#include <string>

namespace stl {

namespace PlyLoader {
	std::shared_ptr<Gaussians> loadPlyFile(const std::string& path);
}

}