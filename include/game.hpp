#ifndef __HPP_PARADOX_GAME__
#define __HPP_PARADOX_GAME__

#include "camera.hpp"
#include "model.hpp"
#include "portal.hpp"
#include <glad/gl.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <string>
#include <vector>

namespace pdx {
class Game {
public:
  Game();
  Game(const std::string& title, int screenWidth, int screenHeight);
  auto Run() -> void;

private:
  auto DrawPortals(const glm::mat4& view, const glm::mat4& proj,
                   uint32_t recursionLevel) const -> void;
  auto DrawLevel(const glm::mat4& view, const glm::mat4& proj) const -> void;

  std::vector<pdx::Portal> m_Portals;
  std::vector<pdx::Model> m_Models;

  int m_WindowWidth, m_WindowHeight;
  SDL_Window *m_Window;
  SDL_GLContext m_Context;
};
} // namespace pdx

#endif /*  __HPP_PARADOX_GAME__ */
