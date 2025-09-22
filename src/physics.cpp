#include "physics.hpp"

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
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/EActivation.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/RegisterTypes.h>

#include <cassert>
#include <cstdarg>
#include <iostream>
#include <type_traits>

using namespace pdx;

using namespace JPH::literals;

static auto TraceImpl(const char *inFmt, ...) -> void {
  va_list list;
  va_start(list, inFmt);
  char buffer[1024];
  vsnprintf(buffer, sizeof(buffer), inFmt, list);
  va_end(list);

  std::cout << buffer << std::endl;
}

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

class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
public:
  BPLayerInterfaceImpl() {
    mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
    mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
  }

  virtual unsigned int GetNumBroadPhaseLayers() const override {
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

    // Allows you to ignore a contact before it is created (using layers to
    // not make objects collide is cheaper!)
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

static std::shared_ptr<GamePhysics> _GamePhysics = nullptr;

GamePhysics::~GamePhysics() {
  JPH::UnregisterTypes();

  delete JPH::Factory::sInstance;
  JPH::Factory::sInstance = nullptr;
}

auto GamePhysics::GetPhysicsHandle() -> pdx::PhysicsHandle {
  if (_GamePhysics == nullptr) {
    _GamePhysics = std::make_shared<GamePhysics>();
    _GamePhysics->InitPhysics();
  }
  return _GamePhysics;
}

auto GamePhysics::InitPhysics() -> void {
  if (_GamePhysics != nullptr) {
    return;
  }

  JPH::RegisterDefaultAllocator();
  JPH::Trace = TraceImpl;

  JPH::Factory::sInstance = new JPH::Factory();

  JPH::RegisterTypes();

  JPH::TempAllocatorImpl tempAllocator(10 * 1024 * 1024);

  JPH::JobSystemThreadPool jobSystem(JPH::cMaxPhysicsJobs,
                                     JPH::cMaxPhysicsBarriers,
                                     std::thread::hardware_concurrency() - 1);

  BPLayerInterfaceImpl broadPhaseLayerInterface;
  ObjectVsBroadPhaseLayerFilterImpl objectVsBroadPhaseLayerFilter;
  ObjectLayerPairFilterImpl objectVsObjectLayerFilter;

  m_PhysicsSystem.Init(m_MaxBodies, m_NumBodyMutexes, m_MaxBodyPairs,
                       m_MaxContactConstraints, broadPhaseLayerInterface,
                       objectVsBroadPhaseLayerFilter,
                       objectVsObjectLayerFilter);

  MyBodyActivationListener bodyActivationListener;
  m_PhysicsSystem.SetBodyActivationListener(&bodyActivationListener);

  MyContactListener contactListener;
  m_PhysicsSystem.SetContactListener(&contactListener);

  JPH::BodyInterface& bodyInterface = m_PhysicsSystem.GetBodyInterface();

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
}

auto GamePhysics::GenerateBoxShape(const JPH::BodyCreationSettings& settings,
                                   JPH::EActivation activation) -> JPH::BodyID {
  JPH::BodyInterface& bodyInterface = m_PhysicsSystem.GetBodyInterface();
  return bodyInterface.CreateAndAddBody(settings, activation);
}
