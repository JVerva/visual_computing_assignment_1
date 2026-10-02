cmake_minimum_required(VERSION 3.16)
project(MyProject)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
  CACHE STRING "Vcpkg toolchain file")
find_package(OpenCV CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

add_executable(MyProject
    src/triangle_render.cpp
)

target_link_libraries(MyProject PRIVATE
    ${OpenCV_LIBS}
    glfw
    glad::glad
)

add_executable(MyProject main.cpp)
