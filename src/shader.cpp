#include "shader.hpp"

#include <fstream>
#include <glad/glad.h>
#include <iostream>
#include <sstream>

#include "types.hpp"

using namespace craft;

Shader::Shader(const craft::path& vertFile, const craft::path& fragFile) {
  std::string vertexCode;
  std::string fragCode;
  std::ifstream vertfs;
  std::ifstream fragfs;
  vertfs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fragfs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    vertfs.open(vertFile);
    fragfs.open(fragFile);
    std::stringstream vertSS, fragSS;
    vertSS << vertfs.rdbuf();
    fragSS << fragfs.rdbuf();
    vertexCode = vertSS.str();
    fragCode = fragSS.str();
  } catch (std::ifstream::failure e) {
    std::cerr << "Failed to read file" << std::endl;
  }

  const char *vcode = vertexCode.c_str();
  const char *fcode = fragCode.c_str();

  craft::shader_t vert = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert, 1, &vcode, nullptr);
  glCompileShader(vert);
  craft::shader_t frag = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag, 1, &fcode, nullptr);
  glCompileShader(frag);

  m_Program = glCreateProgram();
  glAttachShader(m_Program, vert);
  glAttachShader(m_Program, frag);
  glLinkProgram(m_Program);
  glDeleteShader(vert);
  glDeleteProgram(frag);
}

void Shader::Use() const { glUseProgram(m_Program); }
