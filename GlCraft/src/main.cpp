#include <iostream>

// clang-format: off
#ifdef __EMSCRIPTEN__
# include <glad/egl.h>
# include <glad/gles2.h>
# define SDL_MAIN_HANDLED
# include <SDL.h>
# include <SDL_opengles2.h>

# include <emscripten.h>
#else /* !__EMSCRIPTEN__*/
# include <glad/gl.h>

# include <SDL2/SDL.h>
# include <SDL2/SDL_opengl.h>
#endif /* __EMSCRIPTEN__ */

#include "shader.hpp"
#include "types.hpp"

#ifndef __EMSCRIPTEN__
# define MAIN_LOOP_BEGIN                                                        \
    bool running = true;                                                         \
    do
# define MAIN_LOOP_END while(running)
# define CHECK_SHOULD_QUIT                                                      \
    case SDL_QUIT:                                                               \
      running = false;                                                           \
      break
#else /* !__EMSCRIPTEN__*/
# define MAIN_LOOP_BEGIN
# define MAIN_LOOP_END
# define CHECK_SHOULD_QUIT
#endif /* __EMSCRIPTEN__ */
// clang-format: on

SDL_Window *window;

void mainLoop() {
  MAIN_LOOP_BEGIN {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) { CHECK_SHOULD_QUIT; }
    }
    glClearColor(0.5, 0.2, 0.7, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    SDL_GL_SwapWindow(window);
  } MAIN_LOOP_END;
}

float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

int main(int argc, char **argv) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "Failed to start SDL: " << SDL_GetError() << std::endl;
  }
#ifndef __EMSCRIPTEN__
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else /* __EMSCRIPTEN__*/
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif /* !__EMSCRIPTEN__ */

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetSwapInterval(1);
  window = SDL_CreateWindow("GlCraft", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT,
                            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                                SDL_WINDOW_SHOWN);
  if (!window) {
    std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
    return -1;
  }
  SDL_GLContext context = SDL_GL_CreateContext(window);

#ifndef __EMSCRIPTEN__
  if (!gladLoaderLoadGL()) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
#else /* __EMSCRIPTEN__ */
  gladLoaderLoadGLES2();
#endif /* !__EMSCRIPTEN__ */

  SDL_GL_SetSwapInterval(1);

  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

  craft::Shader shader("simple.vert", "simple.frag");
  shader.Use();

  craft::vao_t VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  craft::vbo_t VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

#ifdef __EMSCRIPTEN__
  int fps = 0;
  emscripten_set_main_loop(mainLoop, fps, true);
#else /* !__EMSCRIPTEN__ */
  mainLoop();
#endif /* __EMSCRIPTEN__ */

  return 0;
}
