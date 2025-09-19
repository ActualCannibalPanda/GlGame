#ifndef __HPP_PARADOX_TYPES__
#define __HPP_PARADOX_TYPES__

#include <cstdint>

#include <filesystem>

namespace pdx {
typedef uint32_t vao_t;
typedef uint32_t vbo_t;
typedef uint32_t ebo_t;
typedef uint32_t program_t;
typedef uint32_t shader_t;
typedef std::filesystem::path path;
} // namespace pdx

#endif /*  __HPP_PARADOX_TYPES__ */
