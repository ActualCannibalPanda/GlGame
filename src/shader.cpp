#include <cstdio>
#include <string.h>

// clang-format off
#ifdef __EMSCRIPTEN__
#  include <glad/egl.h>
#  include <glad/gles2.h>

#  include <emscripten.h>
#  include <emscripten/fetch.h>
#else /* !__EMSCRIPTEN__ */
# include <glad/gl.h>

# include <SDL2/SDL.h>
# include <SDL2/SDL_opengl.h>
#endif /* __EMSCRIPTEN__ */
// clang-format on

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>

#include "assetdir.hpp"
#include "shader.hpp"
#include "types.hpp"

using namespace glgame;

#ifdef __EMSCRIPTEN__
static const AssetDir SHADER_DIR{"/", "data", "shaders", "gles"};
#else  /* !__EMSCRIPTEN__ */
static const AssetDir SHADER_DIR{"data", "shaders"};
#endif /* __EMSCRIPTEN__ */

#ifdef __EMSCRIPTEN__
void onSuccess(emscripten_fetch_t *fetch);
void onError(emscripten_fetch_t *fetch);

class Fetch {
public:
  Fetch(const std::string& thing) : m_Thing(thing) {}

  void sendRequest() const {
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.attributes =
        EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_PERSIST_FILE;
    attr.onsuccess = onSuccess;
    attr.onerror = onError;
    attr.userData = static_cast<void *>(const_cast<Fetch *>(this));
    emscripten_fetch(&attr, m_Thing.c_str());
  }

  void __set_job_done(bool success, const char *data) {
    m_Success = success;
    m_JobDone = true;
    if (success) {
      size_t len = strlen(data);
      m_Data = new char[len];
      strcpy(m_Data, data);
    }
  }

  bool isDone() const { return m_JobDone; }
  bool isValid() const { return m_Success; }

  const char *getData() { return m_Data; }

private:
  volatile bool m_JobDone = false;
  bool m_Success = false;
  char *m_Data = nullptr;
  std::string m_Thing;
};

void onSuccess(emscripten_fetch_t *fetch) {
  Fetch *f = static_cast<Fetch *>(fetch->userData);
  f->__set_job_done(true, fetch->data);
  emscripten_fetch_close(fetch);
}
void onError(emscripten_fetch_t *fetch) {
  Fetch *f = static_cast<Fetch *>(fetch->userData);
  f->__set_job_done(false, nullptr);
  emscripten_fetch_close(fetch);
}
#endif /* __EMSCRIPTEN__ */

Shader::Shader(const std::string& vertFile, const std::string& fragFile) {
  auto vFile = SHADER_DIR.GetFile(vertFile.c_str());
  auto fFile = SHADER_DIR.GetFile(fragFile.c_str());
#ifdef __EMSCRIPTEN__
  std::string vname = vFile.string();
  std::string fname = fFile.string();
  Fetch vFetch(vFile.c_str());
  Fetch fFetch(fFile.c_str());
  vFetch.sendRequest();
  fFetch.sendRequest();
  while (!fFetch.isDone() || !vFetch.isDone()) {
    emscripten_sleep(100);
  }
  const char *vcode = vFetch.getData();
  const char *fcode = fFetch.getData();
#else  /* !__EMSCRIPTEN__ */
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
#endif /* __EMSCRIPTEN__ */

  glgame::shader_t vert = glCreateShader(GL_VERTEX_SHADER);
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
  glgame::shader_t frag = glCreateShader(GL_FRAGMENT_SHADER);
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
  glDeleteProgram(frag);
}

void Shader::Use() const { glUseProgram(m_Program); }

void Shader::Set1f(const std::string& name, float x) const {
  glUniform1f(glGetUniformLocation(m_Program, name.c_str()), x);
}
void Shader::Set2f(const std::string& name, float x, float y) const {
  glUniform2f(glGetUniformLocation(m_Program, name.c_str()), x, y);
}
void Shader::Set3f(const std::string& name, float x, float y, float z) const {

  glUniform3f(glGetUniformLocation(m_Program, name.c_str()), x, y, z);
}
void Shader::Set4f(const std::string& name, float x, float y, float z,
                   float w) const {
  glUniform4f(glGetUniformLocation(m_Program, name.c_str()), x, y, z, w);
}
void Shader::Set1i(const std::string& name, int x) const {
  glUniform1i(glGetUniformLocation(m_Program, name.c_str()), x);
}
void Shader::Set2i(const std::string& name, int x, int y) const {
  glUniform2i(glGetUniformLocation(m_Program, name.c_str()), x, y);
}
void Shader::Set3i(const std::string& name, int x, int y, int z) const {

  glUniform3f(glGetUniformLocation(m_Program, name.c_str()), x, y, z);
}
void Shader::Set4i(const std::string& name, int x, int y, int z, int w) const {
  glUniform4f(glGetUniformLocation(m_Program, name.c_str()), x, y, z, w);
}
void Shader::SetMat4fv(const std::string& name, const glm::mat4x4& value,
                       bool transpose) const {

  glUniformMatrix4fv(glGetUniformLocation(m_Program, name.c_str()), 1,
                     transpose, glm::value_ptr(value));
}
void Shader::Set2fv(const std::string& name, const glm::vec2& value) const {
  glUniform2fv(glGetUniformLocation(m_Program, name.c_str()), 1,
               glm::value_ptr(value));
}
void Shader::Set3fv(const std::string& name, const glm::vec3& value) const {
  glUniform3fv(glGetUniformLocation(m_Program, name.c_str()), 1,
               glm::value_ptr(value));
}
void Shader::Set4fv(const std::string& name, const glm::vec4& value) const {
  glUniform4fv(glGetUniformLocation(m_Program, name.c_str()), 1,
               glm::value_ptr(value));
}
