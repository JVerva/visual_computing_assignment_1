set(CMAKE_POLICY_VERSION_MINIMUM "3.5")
cmake_minimum_required(VERSION 3.14)
project(TRIANGLE_RENDER)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
set(OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/opencv/build")
find_package(OpenCV CONFIG REQUIRED)
set(CMAKE_GENERATOR_PLATFORM x64)
set(OpenCV_BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/opencv/build/x64/vc16/bin")

add_executable(triangle_render
    src/triangle_render.cpp
)

include(FetchContent)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG latest
)
FetchContent_MakeAvailable(glfw)

# glm (header-only)
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG master
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG master
)
FetchContent_MakeAvailable(glad)

# Link GLFW + OpenGL
target_link_libraries(triangle_render
    PRIVATE
        glad
        glfw
        OpenGL::GL
        ${OpenCV_LIBS}
)

# glm is header-only → only include directory
target_include_directories(triangle_render PRIVATE
    ${glm_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
)

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
target_compile_options(triangle_render PRIVATE /EHsc)

file(GLOB OpenCV_RUNTIME_DLLS "${OpenCV_BIN_DIR}/*.dll")
foreach(OpenCV_RUNTIME_DLL ${OpenCV_RUNTIME_DLLS})
  add_custom_command(TARGET triangle_render POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${OpenCV_RUNTIME_DLL}"
      "$<TARGET_FILE_DIR:triangle_render>"
  )
endforeach()
