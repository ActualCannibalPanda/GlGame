#ifndef __HPP_PARADOX_GAME__
#define __HPP_PARADOX_GAME__

#include <string>

#include <glad/gl.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

namespace pdx {
class Game {
public:
  Game();
  Game(const std::string& title, int screenWidth, int screenHeight);
  auto Run() -> void;

protected:
  int m_WindowWidth, m_WindowHeight;
  SDL_Window *m_Window;
  SDL_GLContext m_Context;
};
} // namespace pdx

#endif /*  __HPP_PARADOX_GAME__ */
