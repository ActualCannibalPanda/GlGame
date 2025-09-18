#ifndef __HPP_GLGAME_GAME__
#define __HPP_GLGAME_GAME__

#include <string>

// clang-format off
#ifdef __EMSCRIPTEN__
# include <glad/egl.h>
# include <glad/gles2.h>
# define SDL_MAIN_HANDLED
# include <SDL.h>
# include <SDL_opengles2.h>

# include <emscripten.h>
# define MAIN_LOOP_BEGIN
# define MAIN_LOOP_END
# define CHECK_SHOULD_QUIT

#else /* !__EMSCRIPTEN__*/

# include <glad/gl.h>

# include <SDL2/SDL.h>
# include <SDL2/SDL_opengl.h>
# define MAIN_LOOP_BEGIN                                                        \
    bool running = true;                                                         \
    do
# define MAIN_LOOP_END while(running)
# define CHECK_SHOULD_QUIT                                                      \
    case SDL_QUIT:                                                               \
      running = false;                                                           \
      break
#endif /* __EMSCRIPTEN__ */
// clang-format on

namespace glgame {
class Game {
public:
  Game();
  Game(const std::string& title, int screenWidth, int screenHeight);
  void Run();

protected:
  int m_WindowWidth, m_WindowHeight;
  SDL_Window *m_Window;
  SDL_GLContext m_Context;
};

#ifdef EMSCRIPTEN
inline void GameRun(void *arg) {
  // simply call run from a cast
  static_cast<Game *>(arg)->Run();
}
#endif
} // namespace glgame

#endif /*  __HPP_GLGAME_GAME__ */
