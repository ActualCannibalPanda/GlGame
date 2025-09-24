#include "game.hpp"

#include <SDL_events.h>
#include <SDL_mouse.h>
#include <SDL_timer.h>
#include <SDL_video.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>

#include <glm/matrix.hpp>
#include <iostream>

#include "assetdir.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "portal.hpp"
#include "shader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <external/stb_image.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>

using namespace pdx;

// clang-format off
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
// clang-format on

auto GLAPIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id,
                              GLenum severity, GLsizei length,
                              const char *message, const void *userParam)
    -> void {
  // ignore non-significant error/warning codes
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
    return;

  std::cout << "---------------" << std::endl;
  std::cout << "Debug message (" << id << "): " << message << std::endl;

  switch (source) {
  case GL_DEBUG_SOURCE_API:
    std::cout << "Source: API";
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    std::cout << "Source: Window System";
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    std::cout << "Source: Shader Compiler";
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    std::cout << "Source: Third Party";
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    std::cout << "Source: Application";
    break;
  case GL_DEBUG_SOURCE_OTHER:
    std::cout << "Source: Other";
    break;
  }
  std::cout << std::endl;

  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    std::cout << "Type: Error";
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    std::cout << "Type: Deprecated Behaviour";
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    std::cout << "Type: Undefined Behaviour";
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    std::cout << "Type: Portability";
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    std::cout << "Type: Performance";
    break;
  case GL_DEBUG_TYPE_MARKER:
    std::cout << "Type: Marker";
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    std::cout << "Type: Push Group";
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    std::cout << "Type: Pop Group";
    break;
  case GL_DEBUG_TYPE_OTHER:
    std::cout << "Type: Other";
    break;
  }
  std::cout << std::endl;

  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    std::cout << "Severity: high";
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    std::cout << "Severity: medium";
    break;
  case GL_DEBUG_SEVERITY_LOW:
    std::cout << "Severity: low";
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    std::cout << "Severity: notification";
    break;
  }
  std::cout << std::endl;
  std::cout << std::endl;
}

Game::Game() : Game("Game", 800, 600) {}

Game::Game(const std::string& title, int screenWidth, int screenHeight) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "Failed to start SDL: " << SDL_GetError() << std::endl;
  }
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

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

  if (!gladLoaderLoadGL()) {
    std::cout << "Failed to initialize GLAD" << std::endl;
  }

  SDL_SetRelativeMouseMode(SDL_TRUE);
  SDL_GL_SetSwapInterval(1);
  SDL_SetWindowFullscreen(m_Window, SDL_WINDOW_FULLSCREEN_DESKTOP);
  SDL_GetWindowSizeInPixels(m_Window, (int *)&m_WindowWidth,
                            (int *)&m_WindowHeight);

  glViewport(0, 0, m_WindowWidth, m_WindowHeight);
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr,
                          GL_TRUE);
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplSDL2_InitForOpenGL(m_Window, m_Context);
  ImGui_ImplOpenGL3_Init();
}

auto Game::Run() -> void {
  pdx::Shader simpleShader("simple.vert", "simple.frag");
  pdx::Shader singleColorShader("singleColor.vert", "singleColor.frag");
  pdx::Shader lightShader("light.vert", "light.frag");

  glEnable(GL_DEPTH_TEST);

  glm::mat4 projection = glm::perspective(
      glm::radians(45.0f), (float)m_WindowWidth / (float)m_WindowHeight, 0.1f,
      100.0f);
  Camera camera(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, -1.0f), projection);

  pdx::AssetDir cubeDir{"data", "models", "cube"};
  pdx::Model cube = pdx::Model::FromGLTF(cubeDir.GetFile("scene.gltf")).value();

  pdx::AssetDir portalDir{"data", "models", "portal"};
  pdx::Model portal =
      pdx::Model::FromGLTF(portalDir.GetFile("scene.gltf")).value();

  pdx::AssetDir portalFrameDir{"data", "models", "portalFrame"};
  pdx::Model portalFrame =
      pdx::Model::FromGLTF(portalFrameDir.GetFile("scene.gltf")).value();
  pdx::AssetDir floorDir{"data", "models", "floor"};
  pdx::Model floor =
      pdx::Model::FromGLTF(floorDir.GetFile("scene.gltf")).value();

  glm::vec3 cubePosition(0.0f, 0.0f, 0.0f);
  pdx::Portal portalA(pdx::Camera(glm::vec3(0.0f, 0.0f, 4.0f),
                                  glm::vec3(0.0f, 1.0f, 0.0f),
                                  glm::vec3(0.0f, 0.0f, -1.0f), projection));
  portalA.AddAngle(180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
  pdx::Portal portalB(pdx::Camera(glm::vec3(0.0f, 0.0f, -4.0f),
                                  glm::vec3(0.0f, 1.0f, 0.0f),
                                  glm::vec3(0.0f, 0.0f, 1.0f), projection));

  portalA.SetDestination(&portalB);
  portalB.SetDestination(&portalA);

  m_Portals.push_back(portalA);
  m_Portals.push_back(portalB);
  m_Models.push_back(floor);
  m_Models.push_back(cube);

  int now = SDL_GetPerformanceCounter();
  int last = 0;
  double delta;

  bool first = true;

  bool w = false, a = false, s = false, d = false;

  uint64_t frameStart = SDL_GetTicks64();

  bool running = true;
  do {
    last = now;
    now = SDL_GetPerformanceCounter();

    delta = (double)(now - last) / (double)SDL_GetPerformanceFrequency();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
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

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    {
      ImGui::Begin("FPS");
      uint64_t msec = SDL_GetTicks64() - frameStart;
      if (msec > 0.0f) {
        uint64_t fps = 1.0 / (msec / (double)SDL_GetPerformanceFrequency());
        // this is just to stop a warning from showing up on different compilers
#ifdef WIN32
        ImGui::Text("FPS: %llu", fps);
#else
        ImGui::Text("FPS: %lu", fps);
#endif
      }
      frameStart = SDL_GetTicks64();
      ImGui::End();
    }

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    DrawPortals(camera.GetViewMatrix(), camera.GetProjectionMatrix(), 0);

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

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(m_Window);
  } while (running);

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(m_Context);
  SDL_DestroyWindow(m_Window);
  SDL_Quit();
}

constexpr uint32_t MAX_RECURSION_LIMIT = 5;

auto Game::DrawPortals(const glm::mat4& view, const glm::mat4& projection,
                       uint32_t recursionLevel) const -> void {
  pdx::Shader simpleShader("simple.vert", "simple.frag");
  pdx::Shader singleColorShader("singleColor.vert", "singleColor.frag");

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  for (const auto& portal : m_Portals) {
    // disable depth and color masks
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);
    // disable depth test
    glDisable(GL_DEPTH_TEST);
    // enable stencil test
    glEnable(GL_STENCIL_TEST);
    // always fail
    glStencilFuncSeparate(GL_FRONT, GL_NOTEQUAL, recursionLevel, 0xFF);
    // replace passing tests with 1
    glStencilOpSeparate(GL_FRONT, GL_INCR, GL_KEEP, GL_KEEP);
    glStencilMaskSeparate(GL_FRONT, 0xFF);

    portal.DrawPortalPlane(view, projection, singleColorShader);

    glm::mat4 destView =
        view * portal.ModelMatrix() *
        glm::rotate(glm::mat4(1.0f), glm::radians(180.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f) *
                        portal.GetDestination()->Orientation()) *
        glm::inverse(portal.GetDestination()->ModelMatrix());
    if (recursionLevel == MAX_RECURSION_LIMIT) {
      // renenable color and depth mask
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      glDepthMask(GL_TRUE);
      glClear(GL_DEPTH_BUFFER_BIT);
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_STENCIL_TEST);
      glStencilMaskSeparate(GL_FRONT, 0x00);
      glStencilFuncSeparate(GL_FRONT, GL_EQUAL, recursionLevel + 1, 0xFF);

      DrawLevel(destView, portal.ClippedProj(destView, projection));
    } else {
      DrawPortals(destView, portal.ClippedProj(destView, projection),
                  recursionLevel + 1);
    }

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);

    glEnable(GL_STENCIL_TEST);
    glStencilMaskSeparate(GL_FRONT, 0xFF);

    glEnable(GL_DEPTH_TEST);

    glStencilFuncSeparate(GL_FRONT, GL_NOTEQUAL, recursionLevel + 1, 0xFF);
    glStencilOpSeparate(GL_FRONT, GL_DECR, GL_KEEP, GL_KEEP);

    portal.DrawPortalPlane(view, projection, singleColorShader);
  }

  glDisable(GL_STENCIL_TEST);
  glStencilMaskSeparate(GL_FRONT, 0x00);

  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_ALWAYS);
  glDepthMask(GL_TRUE);
  glClear(GL_DEPTH_BUFFER_BIT);

  for (const auto& portal : m_Portals) {
    portal.DrawPortalPlane(view, projection, singleColorShader);
  }

  glDepthFunc(GL_LESS);

  glEnable(GL_STENCIL_TEST);
  glStencilMaskSeparate(GL_FRONT, 0x00);
  glStencilFuncSeparate(GL_FRONT, GL_LEQUAL, recursionLevel, 0xFF);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);

  DrawLevel(view, projection);
}
auto Game::DrawLevel(const glm::mat4& view, const glm::mat4& projection) const
    -> void {
  pdx::Shader shader("simple.vert", "simple.frag");
  for (const auto& portal : m_Portals) {
    portal.DrawPortalFrame(view, projection, shader);
  }
  shader.Use();
  {
    glm::mat4 model =
        glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -3.0, 0.0f)),
                   glm::vec3(10.0f, 1.0f, 10.0f));
    shader.SetMat4fv("model", model);
    shader.SetMat4fv("view", view);
    shader.SetMat4fv("projection", projection);
    m_Models[0].Draw();
  }
  shader.Use();
  {
    glm::mat4 model =
        glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, 0.0, 0.0f)),
                   glm::vec3(1.0f, 1.0f, 1.0f));
    shader.SetMat4fv("model", model);
    shader.SetMat4fv("view", view);
    shader.SetMat4fv("projection", projection);
    m_Models[1].Draw();
  }
}
