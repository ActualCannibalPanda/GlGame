#include <Jolt/Jolt.h>

#include <Jolt/Core/Factory.h>
#include <Jolt/Core/IssueReporting.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Math/Quat.h>
#include <Jolt/Math/Real.h>
#include <Jolt/Math/Vec3.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/EActivation.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>

#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <sys/types.h>
#include <thread>

#include "game.hpp"

using namespace JPH::literals;

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

static auto TraceImpl(const char *inFmt, ...) -> void {
  va_list list;
  va_start(list, inFmt);
  char buffer[1024];
  vsnprintf(buffer, sizeof(buffer), inFmt, list);
  va_end(list);

  std::cout << buffer << std::endl;
}

namespace Layers {
static constexpr JPH::ObjectLayer NON_MOVING = 0;
static constexpr JPH::ObjectLayer MOVING = 1;
static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
} // namespace Layers

class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter {
public:
  virtual bool ShouldCollide(JPH::ObjectLayer inObject1,
                             JPH::ObjectLayer inObject2) const override {
    switch (inObject1) {
    case Layers::NON_MOVING:
      return inObject2 == Layers::MOVING;
    case Layers::MOVING:
      return true;
    default:
      JPH_ASSERT(false);
      return false;
    }
  }
};

namespace BroadPhaseLayers {
static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
static constexpr JPH::BroadPhaseLayer MOVING(1);
static constexpr JPH::BroadPhaseLayer NUM_LAYERS(2);
} // namespace BroadPhaseLayers

class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
public:
  BPLayerInterfaceImpl() {
    mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
    mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
  }

  virtual uint GetNumBroadPhaseLayers() const override {
    return BroadPhaseLayers::NUM_LAYERS.GetValue();
  }

  virtual JPH::BroadPhaseLayer
  GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override {
    JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
    return mObjectToBroadPhase[inLayer];
  }

private:
  JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

class ObjectVsBroadPhaseLayerFilterImpl
    : public JPH::ObjectVsBroadPhaseLayerFilter {
public:
  virtual bool ShouldCollide(JPH::ObjectLayer inLayer1,
                             JPH::BroadPhaseLayer inLayer2) const override {
    switch (inLayer1) {
    case Layers::NON_MOVING:
      return inLayer2 == BroadPhaseLayers::MOVING;
    case Layers::MOVING:
      return true;
    default:
      JPH_ASSERT(false);
      return false;
    }
  }
};

// An example contact listener
class MyContactListener : public JPH::ContactListener {
public:
  // See: ContactListener
  virtual JPH::ValidateResult
  OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2,
                    JPH::RVec3Arg inBaseOffset,
                    const JPH::CollideShapeResult& inCollisionResult) override {
    std::cout << "Contact validate callback" << std::endl;

    // Allows you to ignore a contact before it is created (using layers to not
    // make objects collide is cheaper!)
    return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
  }

  virtual void OnContactAdded(const JPH::Body& inBody1,
                              const JPH::Body& inBody2,
                              const JPH::ContactManifold& inManifold,
                              JPH::ContactSettings& ioSettings) override {
    std::cout << "A contact was added" << std::endl;
  }

  virtual void OnContactPersisted(const JPH::Body& inBody1,
                                  const JPH::Body& inBody2,
                                  const JPH::ContactManifold& inManifold,
                                  JPH::ContactSettings& ioSettings) override {
    std::cout << "A contact was persisted" << std::endl;
  }

  virtual void
  OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override {
    std::cout << "A contact was removed" << std::endl;
  }
};

// An example activation listener
class MyBodyActivationListener : public JPH::BodyActivationListener {
public:
  virtual void OnBodyActivated(const JPH::BodyID& inBodyID,
                               JPH::uint64 inBodyUserData) override {
    std::cout << "A body got activated" << std::endl;
  }

  virtual void OnBodyDeactivated(const JPH::BodyID& inBodyID,
                                 JPH::uint64 inBodyUserData) override {
    std::cout << "A body went to sleep" << std::endl;
  }
};

int main(int argc, char **argv) {
  JPH::RegisterDefaultAllocator();
  JPH::Trace = TraceImpl;

  JPH::Factory::sInstance = new JPH::Factory();

  JPH::RegisterTypes();

  JPH::TempAllocatorImpl tempAllocator(10 * 1024 * 1024);

  JPH::JobSystemThreadPool jobSystem(JPH::cMaxPhysicsJobs,
                                     JPH::cMaxPhysicsBarriers,
                                     std::thread::hardware_concurrency() - 1);

  const uint cMaxBodies = 1024;
  const uint cNumBodyMutexes = 0;
  const uint cMaxBodyPairs = 1024;
  const uint cMaxContactConstraints = 1024;

  BPLayerInterfaceImpl broadPhaseLayerInterface;
  ObjectVsBroadPhaseLayerFilterImpl objectVsBroadPhaseLayerFilter;
  ObjectLayerPairFilterImpl objectVsObjectLayerFilter;

  JPH::PhysicsSystem physicsSystem;
  physicsSystem.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs,
                     cMaxContactConstraints, broadPhaseLayerInterface,
                     objectVsBroadPhaseLayerFilter, objectVsObjectLayerFilter);

  MyBodyActivationListener bodyActivationListener;
  physicsSystem.SetBodyActivationListener(&bodyActivationListener);

  MyContactListener contactListener;
  physicsSystem.SetContactListener(&contactListener);

  JPH::BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();

  JPH::BoxShapeSettings floorShapeSettings(JPH::Vec3(100.0f, 1.0f, 100.0f));
  floorShapeSettings.SetEmbedded();

  JPH::ShapeSettings::ShapeResult floorShapeResult =
      floorShapeSettings.Create();
  JPH::ShapeRefC floorShape = floorShapeResult.Get();

  JPH::BodyCreationSettings floorSettings(
      floorShape, JPH::RVec3(0.0_r, -1.0_r, 0.0_r), JPH::Quat::sIdentity(),
      JPH::EMotionType::Static, Layers::NON_MOVING);

  JPH::Body *floor = bodyInterface.CreateBody(floorSettings);

  bodyInterface.AddBody(floor->GetID(), JPH::EActivation::DontActivate);

  JPH::BodyCreationSettings sphereSettings(
      new JPH::SphereShape(0.5f), JPH::RVec3(0.0_r, 2.0_r, 0.0_r),
      JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, Layers::MOVING);
  JPH::BodyID sphereId = bodyInterface.CreateAndAddBody(
      sphereSettings, JPH::EActivation::Activate);

  bodyInterface.SetLinearVelocity(sphereId, JPH::Vec3(0.0f, -5.0f, 0.0f));

  const float cDeltaTime = 1.0f / 60.0f;

  physicsSystem.OptimizeBroadPhase();

  uint step = 0;
  while (bodyInterface.IsActive(sphereId)) {
    ++step;

    JPH::RVec3 position = bodyInterface.GetCenterOfMassPosition(sphereId);
    JPH::Vec3 velocity = bodyInterface.GetLinearVelocity(sphereId);
    std::cout << "Step " << step << ": Position = (" << position.GetX() << ", "
              << position.GetY() << ", " << position.GetZ() << "), Velocity = ("
              << velocity.GetX() << ", " << velocity.GetY() << ", "
              << velocity.GetZ() << ")" << std::endl;

    const int cCollisionSteps = 1;

    physicsSystem.Update(cDeltaTime, cCollisionSteps, &tempAllocator,
                         &jobSystem);
  }

  bodyInterface.RemoveBody(sphereId);
  bodyInterface.DestroyBody(sphereId);
  bodyInterface.RemoveBody(floor->GetID());
  bodyInterface.DestroyBody(floor->GetID());

  JPH::UnregisterTypes();

  delete JPH::Factory::sInstance;
  JPH::Factory::sInstance = nullptr;

  pdx::Game game("Paradox", SCREEN_WIDTH, SCREEN_HEIGHT);
  game.Run();
  return 0;
}
