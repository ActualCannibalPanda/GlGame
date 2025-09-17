cmake_minimum_required(VERSION 3.22)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

project(GlGame)
add_executable(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC "{CMAKE_SOURCE_DIR}/include")
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

set(LIB_NAME glad)

add_subdirectory(external)

if (WIN32)
    # Forcing MSVC to use utf-8 encoding
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif(WIN32)

if(NOT DEFINED EMSCRIPTEN)
    find_package(OpenGL REQUIRED)
endif()

if ("${PLATFORM}" STREQUAL "Web")
   add_custom_command(
       TARGET ${PROJECT_NAME} PRE_BUILD
       COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:${PROJECT_NAME}>/../data
   )
else ()
   add_custom_command(
       TARGET ${PROJECT_NAME} POST_BUILD
       COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
   )
endif ()

if (DEFINED EMSCRIPTEN)
    add_compile_definitions("__EMSCRIPTEN__")
    set(USE_FLAGS "-sUSE_SDL=2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_FLAGS}")
    target_link_options(${PROJECT_NAME} PUBLIC
        "-sMODULARIZE=1"
        "-sEXPORT_NAME='Main'"
        "-sINVOKE_RUN=0"
        "-sDISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=0"
        "-sEXPORTED_FUNCTIONS=['_main']"
        "-sEXPORTED_RUNTIME_METHODS=['callMain', 'ccall', 'cwrap']"
        "-sFORCE_FILESYSTEM=1"
        "-sALLOW_MEMORY_GROWTH=1" 
        "-sASYNCIFY"
        "-sFETCH"
        "-sMIN_WEBGL_VERSION=2"
        "--preload-file ${CMAKE_SOURCE_DIR}/data"
        "-O2"
    )
    file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)

    target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${LIB_NAME})

    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/index.html
                ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/index.html
        )
else()
    set(extra_flags "")
    if (NOT MSVC)
        set(extra_flags "-mconsole")
    endif()

    file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
        CXX_STANDARD 20)

    target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})
    
    target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm OpenGL::GL ${LIB_NAME}
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>)
    target_include_directories(${PROJECT_NAME} PRIVATE OPENGL_INCLUDE_DIR glm_INCLUDE_DIR SDL2_INCLUDE_DIR include/)
endif()
