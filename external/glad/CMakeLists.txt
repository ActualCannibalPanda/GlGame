add_library(glad STATIC)

target_sources(glad PRIVATE gl.c)
set_target_properties(glad PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                      ${CMAKE_BINARY_DIR}/glad CXX_STANDARD 20)

set_target_properties(glad PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(glad PUBLIC include)
target_include_directories(${PROJECT_NAME} PUBLIC include)
