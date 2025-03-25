cmake_minimum_required(VERSION 3.10)
project(OpenGLPicasa)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
    ${JPEG_INCLUDE_DIRS}
)

add_compile_options(-Wall -Wextra)

file(GLOB SOURCES "src/*.cpp")

add_executable(picasa ${SOURCES})

target_link_libraries(picasa
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    glfw
    ${PNG_LIBRARIES}
    ${JPEG_LIBRARIES}
    dl
    pthread
)

file(GLOB SHADER_FILES "shaders/*")
file(COPY ${SHADER_FILES} DESTINATION ${CMAKE_BINARY_DIR}/shaders)
