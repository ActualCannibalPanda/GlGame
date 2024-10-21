cmake_minimum_required(VERSION 3.28)

project(GlCraft)

set(PROGRAM_NAME GlCraft)
set(LIB_NAME glad)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

include_directories(include)
include_directories(external/GLAD/include)

add_subdirectory(external)
add_subdirectory(src)
