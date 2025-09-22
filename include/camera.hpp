#ifndef __HPP_PARADOX_CAMERA__
#define __HPP_PARADOX_CAMERA__

#include <glm/glm.hpp>

namespace pdx {
class Camera {
public:
  Camera(const glm::vec3& position, const glm::vec3& up, float yaw = -90.0f,
         float pitch = 0.0f);
  Camera(const glm::vec3& position, const glm::vec3& up,
         const glm::vec3& forward);

  auto Move(const glm::vec3& direction, float delta) -> void;
  auto Look(float dx, float dy, float delta) -> void;

  auto Front() const -> glm::vec3;
  auto Position() const -> glm::vec3;

  auto GetViewMatrix() const -> glm::mat4;
  auto GetViewFromCamera(const Camera& other) const -> glm::mat4;
  auto GetRotationMatrix() const -> glm::mat4;

  auto SetSpeed(float value) -> void;
  auto SetSensitivity(float value) -> void;
  auto SetZoom(float value) -> void;

private:
  auto UpdateCameraVectors(bool calcFront = true) -> void;
  auto UpdateYawAndPitch() -> void;

  int m_InvertMouseY = -1;
  int m_InvertMouseX = -1;

  glm::vec3 m_Position;
  glm::vec3 m_Front;
  glm::vec3 m_Up;
  glm::vec3 m_WorldUp;
  glm::vec3 m_Right;
  float m_Yaw;
  float m_Pitch;
  float m_MovementSpeed;
  float m_Sensitivity;
  float m_Zoom;
};
} // namespace pdx

#endif /* __HPP_PARADOX_CAMERA__ */
