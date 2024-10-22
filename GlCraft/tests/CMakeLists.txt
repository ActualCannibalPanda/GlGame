file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS *.cpp)

add_executable(tests ${TEST_FILES})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

find_package(Catch2 REQUIRED)
target_include_directories(tests PRIVATE OPENGL_INCLUDE_DIR glm_INCLUDE_DIR glfw3_INCLUDE_DIR)
target_link_libraries(tests PRIVATE Catch2::Catch2 glm::glm glfw OpenGL::GL ${LIB_NAME} ${PROJECT_LIB})

include(CTest)
include(ParseAndAddCatchTests)
ParseAndAddCatchTests(tests)
