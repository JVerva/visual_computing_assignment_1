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

# 3. Create your executable
add_executable(triangle_render src/triangle_render.cpp)
add_executable(engine src/main.cpp)
add_executable(calibration src/calibration.cpp)

target_include_directories(triangle_render PRIVATE ${glm_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
)
target_include_directories(engine PRIVATE ${glm_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/include
)
target_include_directories(calibration PRIVATE ${glm_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(triangle_render PRIVATE
    ${OpenCV_LIBS}
    glfw
    glad
)
target_link_libraries(engine PRIVATE
    ${OpenCV_LIBS}
    glfw
    glad
)
target_link_libraries(calibration PRIVATE
    ${OpenCV_LIBS}
    glfw
    glad
)
