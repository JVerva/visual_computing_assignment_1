cmake_minimum_required(VERSION 3.16)
project(AURenderer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3 REQUIRED)
find_package(OpenCV REQUIRED)

include(FetchContent)

FetchContent_Declare(glad GIT_REPOSITORY https://github.com/Dav1dde/glad.git)
FetchContent_MakeAvailable(glad)

add_executable(engine src/main.cpp src/computer_vision.cpp src/stb_image.cpp)

target_compile_definitions(engine PUBLIC PROJECT_DIR="${CMAKE_SOURCE_DIR}")

target_include_directories(engine PRIVATE ${glm_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/include
  glad
)

target_link_libraries(engine PRIVATE
    ${OpenCV_LIBS}
    glfw
    glad
)

add_executable(calibration src/calibration.cpp src/computer_vision.cpp src/stb_image.cpp)

target_compile_definitions(calibration PUBLIC PROJECT_DIR="${CMAKE_SOURCE_DIR}")

target_include_directories(calibration PRIVATE ${glm_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/include
  glad
)

target_link_libraries(calibration PRIVATE
    ${OpenCV_LIBS}
    glfw
    glad
)


add_executable(feature_tracking src/feature_tracking.cpp src/computer_vision.cpp src/stb_image.cpp)

target_compile_definitions(feature_tracking PUBLIC PROJECT_DIR="${CMAKE_SOURCE_DIR}")

target_include_directories(feature_tracking PRIVATE ${glm_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/include
  glad
)

target_link_libraries(feature_tracking PRIVATE
    ${OpenCV_LIBS}
    glfw
    glad
)
