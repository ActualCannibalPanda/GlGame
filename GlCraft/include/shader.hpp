#ifndef __HPP_GLCRAFT_SHADER__
#define __HPP_GLCRAFT_SHADER__

#include <string>

#include "types.hpp"

namespace craft {
class Shader {
public:
  Shader(const std::string& vertFile, const std::string& fragFile);

  void Use() const;

private:
  craft::program_t m_Program;
};
} // namespace craft

#endif /*  __HPP_GLCRAFT_SHADER__ */
