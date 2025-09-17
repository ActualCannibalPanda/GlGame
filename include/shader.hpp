#ifndef __HPP_GLGAME_SHADER__
#define __HPP_GLGAME_SHADER__

#include <string>

#include "types.hpp"

namespace glgame {
class Shader {
public:
  Shader(const std::string& vertFile, const std::string& fragFile);

  void Use() const;

private:
  glgame::program_t m_Program;
};
} // namespace glgame

#endif /*  __HPP_GLGAME_SHADER__ */
