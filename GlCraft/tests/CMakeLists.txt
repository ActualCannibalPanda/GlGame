if (NOT DEFINED EMSCRIPTEN)
    file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS *.cpp)

    add_executable(tests ${TEST_FILES})
    target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

    set_target_properties(tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${tests}
        CXX_STANDARD 20)

    find_package(Catch2 REQUIRED)
    target_include_directories(tests PRIVATE OPENGL_INCLUDE_DIR glm_INCLUDE_DIR)
    target_link_libraries(tests PRIVATE Catch2::Catch2 glm::glm OpenGL::GL ${LIB_NAME} ${PROJECT_LIB}
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>)

    include(CTest)
    include(ParseAndAddCatchTests)
    ParseAndAddCatchTests(tests)
endif()
