#include <cstdio>
#include <string.h>

#include <glad/gl.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>

#include "assetdir.hpp"
#include "shader.hpp"
#include "types.hpp"

using namespace pdx;

static const AssetDir SHADER_DIR{"data", "shaders"};

Shader::Shader(const std::string& vertFile, const std::string& fragFile) {
  auto vFile = SHADER_DIR.GetFile(vertFile.c_str());
  auto fFile = SHADER_DIR.GetFile(fragFile.c_str());
  std::string vertexCode;
  std::string fragCode;
  std::ifstream vertfs;
  std::ifstream fragfs;
  vertfs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fragfs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    vertfs.open(vFile);
    fragfs.open(fFile);
    std::stringstream vertSS, fragSS;
    vertSS << vertfs.rdbuf();
    fragSS << fragfs.rdbuf();
    vertexCode = vertSS.str();
    fragCode = fragSS.str();
  } catch (std::ifstream::failure e) {
    std::cout << "Failed to read file" << std::endl;
  }
  const char *vcode = vertexCode.c_str();
  const char *fcode = fragCode.c_str();

  pdx::shader_t vert = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert, 1, &vcode, nullptr);
  glCompileShader(vert);
  // check for shader compile errors
  {
    int success;
    char infoLog[512];
    glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(vert, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                << infoLog << std::endl;
    }
  }
  pdx::shader_t frag = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(frag, 1, &fcode, nullptr);
  glCompileShader(frag);
  // check for shader compile errors
  {
    int success;
    char infoLog[512];
    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(frag, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::FRAG::COMPILATION_FAILED\n"
                << infoLog << std::endl;
    }
  }

  m_Program = glCreateProgram();
  glAttachShader(m_Program, vert);
  glAttachShader(m_Program, frag);
  glLinkProgram(m_Program);
  glDeleteShader(vert);
  glDeleteShader(frag);
}

auto Shader::Use() const -> void { glUseProgram(m_Program); }

auto Shader::Set1f(const std::string& name, float x) const -> void {
  glUniform1f(glGetUniformLocation(m_Program, name.c_str()), x);
}
auto Shader::Set2f(const std::string& name, float x, float y) const -> void {
  glUniform2f(glGetUniformLocation(m_Program, name.c_str()), x, y);
}
auto Shader::Set3f(const std::string& name, float x, float y, float z) const
    -> void {

  glUniform3f(glGetUniformLocation(m_Program, name.c_str()), x, y, z);
}
auto Shader::Set4f(const std::string& name, float x, float y, float z,
                   float w) const -> void {
  glUniform4f(glGetUniformLocation(m_Program, name.c_str()), x, y, z, w);
}
auto Shader::Set1i(const std::string& name, int x) const -> void {
  glUniform1i(glGetUniformLocation(m_Program, name.c_str()), x);
}
auto Shader::Set2i(const std::string& name, int x, int y) const -> void {
  glUniform2i(glGetUniformLocation(m_Program, name.c_str()), x, y);
}
auto Shader::Set3i(const std::string& name, int x, int y, int z) const -> void {
  glUniform3f(glGetUniformLocation(m_Program, name.c_str()), x, y, z);
}
auto Shader::Set4i(const std::string& name, int x, int y, int z, int w) const
    -> void {
  glUniform4f(glGetUniformLocation(m_Program, name.c_str()), x, y, z, w);
}
auto Shader::SetMat4fv(const std::string& name, const glm::mat4x4& value,
                       bool transpose) const -> void {
  glUniformMatrix4fv(glGetUniformLocation(m_Program, name.c_str()), 1,
                     transpose, glm::value_ptr(value));
}
auto Shader::Set2fv(const std::string& name, const glm::vec2& value) const
    -> void {
  glUniform2fv(glGetUniformLocation(m_Program, name.c_str()), 1,
               glm::value_ptr(value));
}
auto Shader::Set3fv(const std::string& name, const glm::vec3& value) const
    -> void {
  glUniform3fv(glGetUniformLocation(m_Program, name.c_str()), 1,
               glm::value_ptr(value));
}
auto Shader::Set4fv(const std::string& name, const glm::vec4& value) const
    -> void {
  glUniform4fv(glGetUniformLocation(m_Program, name.c_str()), 1,
               glm::value_ptr(value));
}
