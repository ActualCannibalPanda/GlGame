#include "game.hpp"

#include <SDL_events.h>
#include <SDL_mouse.h>
#include <SDL_timer.h>
#include <SDL_video.h>

#include <glm/ext/matrix_transform.hpp>

#include <iostream>

#include "assetdir.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "shader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <external/stb_image.h>

using namespace pdx;

// clang-format off
static float cubeVertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f), 
    glm::vec3( 2.0f,  5.0f, -15.0f), 
    glm::vec3(-1.5f, -2.2f, -2.5f),  
    glm::vec3(-3.8f, -2.0f, -12.3f),  
    glm::vec3( 2.4f, -0.4f, -3.5f),  
    glm::vec3(-1.7f,  3.0f, -7.5f),  
    glm::vec3( 1.3f, -2.0f, -2.5f),  
    glm::vec3( 1.5f,  2.0f, -2.5f), 
    glm::vec3( 1.5f,  0.2f, -1.5f), 
    glm::vec3(-1.3f,  1.0f, -1.5f)  
};

float planeVertices[] = {
   0.5f,  0.5f, 0.0f,  // top right
   0.5f, -0.5f, 0.0f,  // bottom right
  -0.5f, -0.5f, 0.0f,  // bottom left
  -0.5f,  0.5f, 0.0f   // top left
};
unsigned int planeIndices[] = {  // note that we start from 0!
  0, 1, 3,  // first Triangle
  1, 2, 3   // second Triangle
};
// clang-format on

Game::Game() : Game("Game", 800, 600) {}

Game::Game(const std::string& title, int screenWidth, int screenHeight) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "Failed to start SDL: " << SDL_GetError() << std::endl;
  }
#ifndef __EMSCRIPTEN__
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);
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

  SDL_SetRelativeMouseMode(SDL_TRUE);
  SDL_GL_SetSwapInterval(1);

  m_WindowWidth = screenWidth;
  m_WindowHeight = screenHeight;

  glViewport(0, 0, screenWidth, screenHeight);
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void Game::Run() {
  pdx::Shader simpleShader("simple.vert", "simple.frag");
  pdx::Shader singleColorShader("singleColor.vert", "singleColor.frag");
  pdx::Shader lightShader("light.vert", "light.frag");
  /*
    pdx::vao_t VAOCube;
    glGenVertexArrays(1, &VAOCube);
    glBindVertexArray(VAOCube);

    pdx::vbo_t VBOCube;
    glGenBuffers(1, &VBOCube);
    glBindBuffer(GL_ARRAY_BUFFER, VBOCube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void
    *)0); glEnableVertexAttribArray(0); glVertexAttribPointer(1, 2, GL_FLOAT,
    GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    pdx::vao_t VAOPortal;
    pdx::vbo_t VBOPortal;
    pdx::ebo_t EBOPortal;
    glGenVertexArrays(1, &VAOPortal);
    glGenBuffers(1, &VBOPortal);
    glGenBuffers(1, &EBOPortal);
    glBindVertexArray(VAOPortal);

    glBindBuffer(GL_ARRAY_BUFFER, VBOPortal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices,
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOPortal);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), planeIndices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void
    *)0); glEnableVertexAttribArray(0); glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    AssetDir dir = AssetDir{"data", "textures"};
    std::string file = dir.GetFile("container.jpg").string();
    stbi_set_flip_vertically_on_load(true);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    unsigned char *data =
        stbi_load(file.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                   GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);
  */
  glEnable(GL_DEPTH_TEST);

  Camera camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  Camera virtCam(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f));

  pdx::AssetDir assetDir{"data"};
  pdx::Model assetModel =
      pdx::Model::FromGLTF(assetDir.GetFile("scene.gltf")).value();

  int now = SDL_GetPerformanceCounter();
  int last = 0;
  double delta;

  bool first = true;

  bool w = false, a = false, s = false, d = false;

  MAIN_LOOP_BEGIN {
    last = now;
    now = SDL_GetPerformanceCounter();

    delta = (double)(now - last) / (double)SDL_GetPerformanceFrequency();

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 model(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), (float)m_WindowWidth / (float)m_WindowHeight, 0.1f,
        100.0f);

    lightShader.Use();
    glm::mat4 mvp = projection * view * model;
    lightShader.SetMat4fv("MVP", mvp);

    assetModel.Draw();

    /*
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClearStencil(0);
    glStencilMask(0xFF);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // disable depth and color masks
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);
    // disable depth test
    glDisable(GL_DEPTH_TEST);
    // enable stencil test
    glEnable(GL_STENCIL_TEST);
    // always fail
    glStencilFunc(GL_NEVER, 1, 0xFF);
    // replace passing tests with 1
    glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);
    glStencilMask(0xFF);
    glClear(GL_STENCIL_BUFFER_BIT);

    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), (float)m_WindowWidth / (float)m_WindowHeight, 0.1f,
        100.0f);

    // draw "portal" from regular viewspace
    singleColorShader.Use();
    {
      glm::mat4 model = glm::scale(glm::mat4(1.0), glm::vec3(1.0f, 1.0f, 1.0f));
      singleColorShader.SetMat4fv("model", model);
      singleColorShader.SetMat4fv("view", view);
      singleColorShader.SetMat4fv("projection", projection);
      glBindVertexArray(VAOPortal);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    // renenable color and depth mask
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glStencilMask(0x00);
    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    // draw scene from pov of other camera
    simpleShader.Use();
    simpleShader.SetMat4fv("view", virtCam.GetViewFromCamera(camera));
    simpleShader.SetMat4fv("projection", projection);

    glBindVertexArray(VAOCube);
    for (unsigned int i = 0; i < 10; ++i) {
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, cubePositions[i]);
      if (i == 0 || (i + 1) % 3 == 0) {
        model = glm::rotate(model, (float)SDL_GetTicks64() / 1000.0f,
                            glm::vec3(0.5f, 1.0f, 0.0f));
      } else {
        model = glm::rotate(model, glm::radians(20.0f * i),
                            glm::vec3(1.0f, 0.3f, 0.5f));
      }
      simpleShader.SetMat4fv("model", model);
      glDrawArrays(GL_TRIANGLES, 0, sizeof(cubeVertices) / sizeof(float) / 5);
    }

    // clear depth buffer
    glDisable(GL_STENCIL_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    // draw "portal" to depth buffer so it doesn't get overridden
    singleColorShader.Use();
    {
      glm::mat4 model = glm::scale(glm::mat4(1.0), glm::vec3(1.0f, 1.0f, 1.0f));
      singleColorShader.SetMat4fv("model", model);
      singleColorShader.SetMat4fv("view", view);
      singleColorShader.SetMat4fv("projection", projection);
      glBindVertexArray(VAOPortal);
    }

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    */
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        running = false;
        continue;
      case SDL_MOUSEMOTION:
        camera.Look((float)event.motion.xrel, (float)event.motion.yrel, delta);
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_w:
          w = true;
          break;
        case SDLK_s:
          s = true;
          break;
        case SDLK_a:
          a = true;
          break;
        case SDLK_d:
          d = true;
          break;
        case SDLK_ESCAPE:
          running = false;
          continue;
        }
        break;
      case SDL_KEYUP:
        switch (event.key.keysym.sym) {
        case SDLK_w:
          w = false;
          break;
        case SDLK_s:
          s = false;
          break;
        case SDLK_a:
          a = false;
          break;
        case SDLK_d:
          d = false;
          break;
        }
        break;
      }
    }

    glm::vec3 moveDirection(0.0f);
    if (w) {
      moveDirection.z += 1.0f;
    }
    if (s) {
      moveDirection.z -= 1.0f;
    }
    if (a) {
      moveDirection.x -= 1.0f;
    }
    if (d) {
      moveDirection.x += 1.0f;
    }
    camera.Move(moveDirection, delta);
    SDL_GL_SwapWindow(m_Window);
  }
  MAIN_LOOP_END;
}
