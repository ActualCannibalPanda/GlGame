#include "game.hpp"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

int main(int argc, char **argv) {
  pdx::Game game("Paradox", SCREEN_WIDTH, SCREEN_HEIGHT);

#ifdef __EMSCRIPTEN__
  int fps = 0;
  emscripten_set_main_loop_arg(glgame::GameRun, static_cast<void *>(&game), fps,
                               true);
#else  /* !__EMSCRIPTEN__ */
  game.Run();
#endif /* __EMSCRIPTEN__ */

  return 0;
}
