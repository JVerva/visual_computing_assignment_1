# About The Project
Assignment 1 for Visual Computing course.\
This project renders a triangle (at origin coordinates) in front of a camera (that always looks at origin coordinates) and a background displaying a live webcam capture, mutiple filters can be applied on top of it.\
The rendering is done with OpenGL and the webcam capture and image filtering with OpenCV.\
This lays the groundwork for a future Augmented reality application.
# Dependencies
## vcpkg

This project uses [vcpkg](https://github.com/microsoft/vcpkg) to provide its C++ dependencies. Install vcpkg and set `VCPKG_PATH` to the vcpkg installation directory before configuring CMake.

### Install vcpkg

The vcpkg installation commands differ slightly between operating systems.


Clone vcpkg and run its bootstrap script:

```bash
git clone https://github.com/microsoft/vcpkg.git /path/to/your/vcpkg
cd /path/to/your/vcpkg
./bootstrap-vcpkg.sh
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

Install the required packages using vcpkg

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
cmake -S . -B build
cmake --build build
```

The executable is created at `build/triangle_render`.

## Run project

Run the executable from the project root so that the relative shader paths resolve correctly:

```bash
./build/triangle_render
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
