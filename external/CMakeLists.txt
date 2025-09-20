add_subdirectory(GLAD)
add_subdirectory(glm)
add_subdirectory(assimp)

set(TINYGLTF_HEADER_ONLY
    ON
    CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL
    OFF
    CACHE INTERNAL "" FORCE)
add_subdirectory(tinygltf)

if(NOT DEFINED EMSCRIPTEN)
  set(BUILD_SHARED_LIBS
      OFF
      CACHE BOOL "" FORCE)

  set(SDL_LIBC
      ON
      CACHE BOOL "" FORCE)
  set(SDL_TEST
      OFF
      CACHE BOOL "" FORCE)
  add_subdirectory(SDL)
endif()
