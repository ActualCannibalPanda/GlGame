#include <Jolt/Jolt.h>

#include <Jolt/Core/Memory.h>

#include "game.hpp"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

int main(int argc, char **argv) {
  JPH::RegisterDefaultAllocator();
  pdx::Game game("Paradox", SCREEN_WIDTH, SCREEN_HEIGHT);
  game.Run();
  return 0;
}
