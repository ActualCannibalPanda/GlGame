cmake_minimum_required(VERSION 3.22)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

project(Paradox CXX C)
add_executable(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC "{CMAKE_SOURCE_DIR}/include")

set(CMAKE_CXX_STANDARD 20)

set(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

add_subdirectory(external)

find_package(OpenGL REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(Jolt CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data
          $<TARGET_FILE_DIR:${PROJECT_NAME}>/data)

set(extra_flags "")
if(NOT MSVC)
  set(extra_flags "-mconsole")
endif()

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)
set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
             CXX_STANDARD 20)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC glm::glm-header-only
         OpenGL::GL
         glad
         $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
         $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
         Jolt::Jolt)
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC ${OPENGL_INCLUDE_DIR} ${glm_INCLUDE_DIR} ${SDL2_INCLUDE_DIR}
         ${TINYGLTF_INCLUDE_DIRS} ${JoltPhysics_SOURCE_DIR}/..
         ${CMAKE_SOURCE_DIR}/include/)
