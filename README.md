# Starlight Engine

Starlight Engine is a 3D game engine written in C++ using the Vulkan API. It uses GLFW for window management and glm for math. It is a work in progress and is not yet ready for use.

## Installing the Dependencies

Clone the repository recursively to get the submodules:

```bash
git clone --recurse-submodules https://github.com/marcelk04/Starlight.git
```

### Windows

Make sure you have the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home) installed.

### Linux Debian

Install the Vulkan dependencies using:

```bash
sudo apt install vulkan-tools libvulkan-dev vulkan-validationlayers glslc
```

You will also need to install the GLFW dependencies:

```bash
sudo apt install libwayland-dev libxkbcommon-dev xorg-dev
```

### Linux Fedora

Install the Vulkan dependencies using:

```bash
sudo dnf install vulkan-tools vulkan-loader-devel vulkan-validation-layers-devel glslc
```

You will also need to install the GLFW dependencies:

```bash
sudo dnf install wayland-devel libxkbcommon-devel libXcursor-devel libXi-devel libXinerama-devel libXrandr-devel
```

### Other

For more information, checkout [Vulkan Tutorial](https://vulkan-tutorial.com/Development_environment) and the [GLFW documentation](https://www.glfw.org/docs/latest/compile.html#compile_deps).

## Building

**Note:** If the CMake script does not find glslc, you will need to compile the shaders manually. To do so, call the according script from your project directory, e.g.:

```bash
./scripts/linux_compile.sh
```

### CMake in the Command Line

Navigate to the project directory and follow these steps:

```bash
cmake -B build
cmake --build build
```

You can the execute the example in the build directory, for example on Linux use:

```bash
./build/Main
```

### Visual Studio Code

If you have the CMake Tools extension installed, you can open the project in Visual Studio Code and build it from there. See the [CMake Tools documentation](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) for more information.

### Visual Studio

Open the folder in Visual Studio and build the project from there using CMake. See the [Visual Studio documentation](https://learn.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=msvc-170) for more information.
