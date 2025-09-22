#ifndef __HPP_PARADOX_SHADER__
#define __HPP_PARADOX_SHADER__

#include <string>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "types.hpp"

namespace pdx {
class Shader {
public:
  Shader(const std::string& vertFile, const std::string& fragFile);

  auto Use() const -> void;

  auto Set1f(const std::string& name, float x) const -> void;
  auto Set2f(const std::string& name, float x, float y) const -> void;
  auto Set3f(const std::string& name, float x, float y, float z) const -> void;
  auto Set4f(const std::string& name, float x, float y, float z, float w) const
      -> void;
  auto Set1i(const std::string& name, int x) const -> void;
  auto Set2i(const std::string& name, int x, int y) const -> void;
  auto Set3i(const std::string& name, int x, int y, int z) const -> void;
  auto Set4i(const std::string& name, int x, int y, int z, int w) const -> void;
  auto SetMat4fv(const std::string& name, const glm::mat4x4& value,
                 bool transpose = false) const -> void;
  auto Set2fv(const std::string& name, const glm::vec2& value) const -> void;
  auto Set3fv(const std::string& name, const glm::vec3& value) const -> void;
  auto Set4fv(const std::string& name, const glm::vec4& value) const -> void;

private:
  pdx::program_t m_Program;
};
} // namespace pdx

#endif /*  __HPP_PARADOX_SHADER__ */
