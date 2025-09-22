#ifndef __HPP_PARADOX_PHYSICS__
#define __HPP_PARADOX_PHYSICS__

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/PhysicsSystem.h>

namespace pdx {
namespace Layers {
static constexpr JPH::ObjectLayer NON_MOVING = 0;
static constexpr JPH::ObjectLayer MOVING = 1;
static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
} // namespace Layers

namespace BroadPhaseLayers {
static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
static constexpr JPH::BroadPhaseLayer MOVING(1);
static constexpr JPH::BroadPhaseLayer NUM_LAYERS(2);
} // namespace BroadPhaseLayers

class GamePhysics;
using PhysicsHandle = std::shared_ptr<GamePhysics>;

class GamePhysics {
public:
  static auto GetPhysicsHandle() -> PhysicsHandle;

  GamePhysics() = default;
  ~GamePhysics();

private:
  auto InitPhysics() -> void;

  JPH::PhysicsSystem m_PhysicsSystem;

  const unsigned int m_MaxBodies = 1024;
  const unsigned int m_NumBodyMutexes = 0;
  const unsigned int m_MaxBodyPairs = 1024;
  const unsigned int m_MaxContactConstraints = 1024;
};
} // namespace pdx

#endif /* __HPP_PARADOX_PHYSICS__ */
