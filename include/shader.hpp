#ifndef __HPP_GLGAME_SHADER__
#define __HPP_GLGAME_SHADER__

#include <string>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "types.hpp"

namespace glgame {
class Shader {
public:
  Shader(const std::string& vertFile, const std::string& fragFile);

  static void Unbind() { glUseProgram(0); }

  void Use() const;

  void Set1f(const std::string& name, float x) const;
  void Set2f(const std::string& name, float x, float y) const;
  void Set3f(const std::string& name, float x, float y, float z) const;
  void Set4f(const std::string& name, float x, float y, float z, float w) const;
  void Set1i(const std::string& name, int x) const;
  void Set2i(const std::string& name, int x, int y) const;
  void Set3i(const std::string& name, int x, int y, int z) const;
  void Set4i(const std::string& name, int x, int y, int z, int w) const;
  void SetMat4fv(const std::string& name, const glm::mat4x4& value,
                 bool transpose = false) const;
  void Set2fv(const std::string& name, const glm::vec2& value) const;
  void Set3fv(const std::string& name, const glm::vec3& value) const;
  void Set4fv(const std::string& name, const glm::vec4& value) const;

private:
  glgame::program_t m_Program;
};
} // namespace glgame

#endif /*  __HPP_GLGAME_SHADER__ */
