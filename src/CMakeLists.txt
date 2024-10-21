add_executable(${PROGRAM_NAME})

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

set_target_properties(${PROGRAM_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROGRAM_NAME}
    CXX_STANDARD 20)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS *.cpp)

target_sources(${PROGRAM_NAME} PRIVATE ${SOURCE_FILES})

if ("${PLATFORM}" STREQUAL "Web")
   add_custom_command(
       TARGET ${PROGRAM_NAME} PRE_BUILD
       COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:${PROGRAM_NAME}>/../data
   )
else ()
   add_custom_command(
       TARGET ${PROGRAM_NAME} POST_BUILD
       COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:${PROGRAM_NAME}>/data
   )
endif ()

target_include_directories(${PROGRAM_NAME} PRIVATE OPENGL_INCLUDE_DIR glm_INCLUDE_DIR glfw3_INCLUDE_DIR include/)

target_link_libraries(${PROGRAM_NAME} PRIVATE glm::glm glfw OpenGL::GL ${LIB_NAME})

# Web Configurations
if ("${PLATFORM}" STREQUAL "Web")
   set_target_properties(${PROGRAM_NAME} PROPERTIES SUFFIX ".html")
   target_link_options(${PROGRAM_NAME} PUBLIC -sUSE_GLFW=3 PUBLIC --preload-file data)
endif ()
