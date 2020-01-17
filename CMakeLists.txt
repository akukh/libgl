cmake_minimum_required(VERSION 3.1)

project(TestOpenGL)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lglfw -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo")

set(MISC_DIR "${CMAKES_CURRENT_SOURCE_DIR}/misc")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LIBS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB SOURCES "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.vert" "${SRC_DIR}/*.frag")
file(GLOB HEADERS "${INCLUDE_DIR}/*.h" "${INCLUDE_DIR}/*.hpp")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")

# glad
set(GLAD_DIR "${LIBS_DIR}/glad")
add_library("GLAD" "${GLAD_DIR}/src/glad.c")
target_include_directories("GLAD" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "GLAD" "${CMAKE_DL_LIBS}")
