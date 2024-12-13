cmake_minimum_required(VERSION 3.10)

project(simulator)

add_compile_options(
    -flto
    -Ofast
    -g
    -Wall
    -Wextra
    -Wpedantic
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(SIMULATOR_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SOURCE_DIR "${SIMULATOR_DIR}/source")
set(INCLUDE_DIR "${SIMULATOR_DIR}/include")

find_package(PkgConfig REQUIRED)
pkg_check_modules(ELF REQUIRED libelf)

include_directories(${ELF_INCLUDE_DIRS})
link_directories(${ELF_LIBRARY_DIRS})

file(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

add_executable(simulator ${SOURCES})
target_include_directories(simulator PRIVATE ${INCLUDE_DIR})

target_link_libraries(simulator ${ELF_LIBRARIES})