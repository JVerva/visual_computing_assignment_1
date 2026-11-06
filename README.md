# Visual Computing - Assignment 1/2

This project contains OpenGL and OpenCV demonstrations developed for the
Visual Computing course:

- `calibration`: calibrates the camera from the bundled chessboard images and
  displays the calibration poses one at a time.
- `feature_tracking`: tracks the bundled target image in the webcam feed and
  estimates its pose to render a cube.
- `triangle_render`: the 1st assignment renderer, which displays a
  triangle over the webcam feed.

## Dependencies

The project uses CMake to find the system-installed OpenCV, GLFW, and GLM
packages. GLAD is downloaded and built automatically by CMake through
`FetchContent`.

### Linux

Install the required tools and development packages:

```bash
sudo apt update
sudo apt install build-essential cmake git libglfw3-dev libglm-dev libopencv-dev
```

### Arch Linux

Install the required tools and development packages:

```bash
sudo pacman -S --needed base-devel cmake git glfw-x11 glm opencv
```

You also need CMake 3.16 or newer, a C++20-compatible compiler, an OpenGL 3.3
compatible graphics driver, and a webcam.

The first CMake configuration requires internet access to download GLAD.

## Build

Clone the repository and configure the project from its root directory:

```bash
git clone https://github.com/JVerva/visual_computing_assignment_1.git
cd visual_computing_assignment_1
cmake -S . -B build
cmake --build build --parallel
```

The executables are created in `build/`:

```text
build/calibration
build/feature_tracking
build/triangle_render
```

## Run

Run any executable with:

```bash
./build/calibration
./build/feature_tracking
./build/triangle_render
```

The repository path is embedded at configure time, so shaders and bundled
images are found even when the executables are launched from another
directory.

 Sorry for putting
definitions in header files, I don't love having to change 100 files; this will be corrected in the
future.

## Controls

### `calibration`

| Key/input | Action |
| --- | --- |
| `C` | Run camera calibration |
| `Enter` in the terminal/console | Advance to the next calibration scenario |
| `Esc` | Exit |

Calibration pauses after each scenario and waits for an Enter key press on the
console where the executable was launched.

### `feature_tracking`

| Key | Action |
| --- | --- |
| `0` | Feature-matching view |
| `1` | Pose-estimation view |
| `Esc` | Exit |

The camera is calibrated automatically at startup using the images in
`assets/images/calibration`, and the target image is
`assets/images/feature_matching/target.png`.

### `triangle_render`

| Key | Action |
| --- | --- |
| `W` | Move the camera forward |
| `S` | Move the camera backward |
| `A` | Move the camera left |
| `D` | Move the camera right |
| `0` | Disable the image filter |
| `1` | Apply grayscale |
| `2` | Apply blur |
| `3` | Apply Sobel |
| `4` | Apply sharpen |
| `Esc` | Exit |
