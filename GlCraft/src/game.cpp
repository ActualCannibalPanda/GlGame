#include "game.hpp"

#include <iostream>

using namespace glgame;

Game::Game() : Game("Game", 800, 600) {}

Game::Game(const std::string& title, int screenWidth, int screenHeight) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "Failed to start SDL: " << SDL_GetError() << std::endl;
  }
#ifndef __EMSCRIPTEN__
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else  /* __EMSCRIPTEN__*/
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif /* !__EMSCRIPTEN__ */

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetSwapInterval(1);
  m_Window = SDL_CreateWindow(
      title.c_str(), 100, 100, screenWidth, screenHeight,
      SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
  if (!m_Window) {
    std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
  }
  m_Context = SDL_GL_CreateContext(m_Window);

#ifndef __EMSCRIPTEN__
  if (!gladLoaderLoadGL()) {
    std::cout << "Failed to initialize GLAD" << std::endl;
  }
#else  /* __EMSCRIPTEN__ */
  gladLoaderLoadGLES2();
#endif /* !__EMSCRIPTEN__ */

  SDL_GL_SetSwapInterval(1);

  glViewport(0, 0, screenWidth, screenHeight);
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

void Game::Run() {
  MAIN_LOOP_BEGIN {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) { CHECK_SHOULD_QUIT; }
    }
    glClearColor(0.5, 0.2, 0.7, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    SDL_GL_SwapWindow(m_Window);
  }
  MAIN_LOOP_END;
}
