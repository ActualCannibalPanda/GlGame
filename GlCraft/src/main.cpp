#include "assetdir.hpp"
#include "shader.hpp"
#include "types.hpp"
#include <SDL2/SDL_video.h>
#include <iostream>

#include <glad/glad.h>
// Keeo glad above this
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

int main(int argc, char **argv) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "Failed to start SDL: " << SDL_GetError() << std::endl;
  }

  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  SDL_Window *window = SDL_CreateWindow("GlCraft", 100, 100, SCREEN_WIDTH,
                                        SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
  if (!window) {
    std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
    return -1;
  }
  SDL_GLContext context = SDL_GL_CreateContext(window);
  // GLFWwindow *window = glfwCreateWindow(800, 600, "GlCraft", nullptr,
  // nullptr); if (window == nullptr) {
  //   std::cout << "Failed to create GLFW window" << std::endl;
  //   glfwTerminate();
  //   return -1;
  // }
  // glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader(
          reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  SDL_GL_SetSwapInterval(1);

  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

  craft::Shader shader(craft::SHADER_DIR.GetFile("simple.vert"),
                       craft::SHADER_DIR.GetFile("simple.frag"));
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
  glBindVertexArray(0);

  bool running = true;
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
    }
    glClearColor(0.5, 0.2, 0.7, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    shader.Use();
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    SDL_GL_SwapWindow(window);
  }

  return 0;
}
