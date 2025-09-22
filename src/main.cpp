#include "game.hpp"
#include "physics.hpp"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

int main(int argc, char **argv) {
  pdx::PhysicsHandle handle = pdx::GamePhysics::GetPhysicsHandle();
  pdx::Game game("Paradox", SCREEN_WIDTH, SCREEN_HEIGHT);
  game.Run();
  return 0;
}
