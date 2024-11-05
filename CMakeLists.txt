cmake_minimum_required(VERSION 3.27)

project(GlCraft)

set(PROGRAM_NAME GlCraft)
set(PROJECT_LIB ${PROGRAM_NAME}-lib)
set(LIB_NAME glad)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

if (WIN32)
    # Forcing MSVC to use utf-8 encoding
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif(WIN32)

include_directories(external/GLAD/include)
include_directories(include)

add_subdirectory(GlCraft)
add_subdirectory(external)
