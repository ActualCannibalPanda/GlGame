#include "game.hpp"
#include "shader.hpp"
#include "types.hpp"

float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

int main(int argc, char **argv) {
  glgame::Game game("GlGame", SCREEN_WIDTH, SCREEN_HEIGHT);

  glgame::Shader shader("simple.vert", "simple.frag");
  shader.Use();

  glgame::vao_t VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glgame::vbo_t VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

#ifdef __EMSCRIPTEN__
  int fps = 0;
  emscripten_set_main_loop_arg(glgame::GameRun, static_cast<void *>(&game), fps,
                               true);
#else  /* !__EMSCRIPTEN__ */
  game.Run();
#endif /* __EMSCRIPTEN__ */

  return 0;
}
