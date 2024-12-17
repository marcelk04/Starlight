#include "GaussianSplatting/PlyLoader.hpp"

#include "Core/Asserts.hpp"
#include "Core/Logger.hpp"

#include <fstream>
#include <sstream>
#include <filesystem>

namespace stl {

std::shared_ptr<Gaussians> PlyLoader::loadPlyFile(const std::string& path) {
	std::ifstream file(std::filesystem::absolute(path), std::ios::binary);
	SASSERT_MSG(file.is_open(), "Failed to open .ply file located at ", std::filesystem::absolute(path));
	
	std::string line;

	// File has to start with "ply".
	std::getline(file, line);
	SASSERT_MSG(line == "ply", "Invalid .ply file.");

	// Format should be binary and little endian. Other formats are not supported and should not be outputted by 3DGS.
	std::getline(file, line);
	SASSERT_MSG(line == "format binary_little_endian 1.0", "Invalid .ply file.");

	// Read in the number of gaussians. The line should look like this: "element vertex <num_gaussians>".
	std::getline(file, line);
	std::stringstream ss(line);
	std::string dummy;
	size_t numSplats = 0;
	ss >> dummy >> dummy >> numSplats;
	SASSERT_MSG(numSplats > 0, "Invalid .ply file or file does not contain any gaussians.");

	SINFO("'", path, "' contains ", numSplats, " gaussians.");

	// Skip to the end of the header.
	while (std::getline(file, line)) {
		if (line == "end_header") {
			break;
		}
	}

	std::shared_ptr<Gaussians> gaussians = std::make_unique<std::vector<RichPoint>>(numSplats);

	file.read((char*)gaussians->data(), numSplats * sizeof(RichPoint));

	// End of file is triggered when file.read() reached the end of the file before reading the specified number of characters.
	if (file.eof()) {
		SWARN("Could only read ", (file.gcount() / sizeof(RichPoint)), " of ", numSplats, " gaussians. The read data could still be valid.");
		gaussians->shrink_to_fit();
	}
	
	file.close();

	return gaussians;
}

}