file(GLOB SOURCES *.c)
add_library(${LIB_NAME} STATIC)

target_sources(${LIB_NAME} PRIVATE ${SOURCES})

set_target_properties(${LIB_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${LIB_NAME}
    CXX_STANDARD 20)

set_target_properties(${LIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${LIB_NAME} PRIVATE include)

