file(GLOB_RECURSE LIB_FILES CONFIGURE_DEPENDS *.cpp)

add_library(${PROJECT_LIB} STATIC ${LIB_FILES})
target_include_directories(${PROJECT_LIB} PRIVATE OPENGL_INCLUDE_DIR glm_INCLUDE_DIR glfw3_INCLUDE_DIR)
target_link_libraries(${PROJECT_LIB} PRIVATE glm::glm glfw OpenGL::GL ${LIB_NAME})
