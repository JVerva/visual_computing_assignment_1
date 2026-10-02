# About The Project
Assignment 1 for Visual Computing course.\
This project renders a triangle (at origin coordinates) in front of a camera (that always looks at origin coordinates) and a background displaying a live webcam capture, mutiple filters can be applied on top of it.\
The rendering is done with OpenGL and the webcam capture and image filtering with OpenCV.\
This lays the groundwork for a future augmented reality application.
# Dependencies
## vcpkg

This project uses [vcpkg](https://github.com/microsoft/vcpkg) to provide its C++ dependencies. Install vcpkg and set `VCPKG_PATH` to the vcpkg installation directory before configuring CMake.

The vcpkg installation commands differ slightly between operating systems.

### Linux and macOS
Install the required Linux build tools and development libraries:

```bash
sudo apt-get update
sudo apt-get install bison
sudo apt-get install autoconf autoconf-archive automake libtool
sudo apt-get install libdbus-1-dev libxi-dev libxtst-dev
```
Clone vcpkg and run its bootstrap script:
```bash
git clone https://github.com/microsoft/vcpkg.git /path/to/your/vcpkg
cd /path/to/your/vcpkg
./bootstrap-vcpkg.sh
```
### Windows
Clone vcpkg and run its bootstrap script:
```powershell
git clone https://github.com/microsoft/vcpkg.git /path/to/your/vcpkg
cd /path/to/your/vcpkg
./bootstrap-vcpkg.bat
```

The following vcpkg packages are required:

- `opencv`: reads the bundled video and applies the image-processing filters.
- `glad`: loads the OpenGL functions used by the renderer.
- `glm`: provides the vector and matrix types used for the camera and OpenGL transformations.
- `glfw3`: creates the application window, OpenGL context, and keyboard input handling.

You also need:

- CMake 3.16 or newer
- A C++17-compatible compiler
- An OpenGL 3.3-compatible graphics driver

## Install vcpkg packages

Add the vcpkg installation directory to your `PATH` before running the install command:

### Linux and macOS
```bash
export PATH="/path/to/your/vcpkg:$PATH"
```

### Windows

```powershell
$env:Path = "C:\path\to\your\vcpkg;$env:Path"
```

Then install the required packages using vcpkg:

```bash
vcpkg install opencv glad glm glfw3
```


# Project Setup

## Clone the repository

Run these commands from the directory where you want to clone the repository.

```bash
git clone https://github.com/JVerva/visual_computing_assignment_1.git
cd visual_computing_assignment_1
```

## Configure and build

Run the following commands from the repository directory:

## Linux and macOS

```bash
export VCPKG_PATH="/path/to/your/vcpkg"
cmake -S . -B build
cmake --build build
```

## Windows

```powershell
$env:VCPKG_PATH = "/path/to/your/vcpkg"
cmake -S . -B build -G "Ninja"
cmake --build build
```

The executable is created at `build/triangle_render`.

## Run project

Run the executable from the project root so that the relative shader paths resolve correctly:

### On Windows
```bash
./build/triangle_render
```

### On Windows
```powershell
.\build\triangle_render.exe
```

## Controls

| Key | Action |
| --- | --- |
|Camera|
| `W` | Move the camera forward |
| `S` | Move the camera backward |
| `A` | Move the camera left |
| `D` | Move the camera right |
|Filters|
| `0` | Disable the image filter |
| `1` | Apply grayscale |
| `2` | Apply blur |
| `3` | Apply Sobel |
| `4` | Apply sharpen |
|Window|
| `Esc` | Exit the application |
