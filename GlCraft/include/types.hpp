#ifndef __HPP_GLCRAFT_TYPES__
#define __HPP_GLCRAFT_TYPES__

#include <cstdint>

#include <filesystem>

namespace craft {
typedef uint32_t vao_t;
typedef uint32_t vbo_t;
typedef uint32_t program_t;
typedef uint32_t shader_t;
typedef std::filesystem::path path;
} // namespace craft

#endif /*  __HPP_GLCRAFT_TYPES__ */
