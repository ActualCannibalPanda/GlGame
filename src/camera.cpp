#include "camera.hpp"

#include <glm/ext/matrix_transform.hpp>

using namespace pdx;

constexpr float SENSITIVITY = 0.2f;
constexpr float MOVEMENT_SPEED = 5.0f;
constexpr float ZOOM = 1.0f;

Camera::Camera(const glm::vec3& position, const glm::vec3& up,
               const glm::vec3& front)
    : m_Position(position), m_Up(up), m_Front(front), m_WorldUp(up),
      m_MovementSpeed(MOVEMENT_SPEED), m_Sensitivity(SENSITIVITY),
      m_Zoom(ZOOM) {
  UpdateYawAndPitch();
}

auto Camera::Update() -> void {
  m_Yaw += (m_TargetYaw - m_Yaw) * 0.5f;
  m_Pitch += (m_TargetPitch - m_Pitch) * 0.5f;
  UpdateCameraVectors();
}

auto Camera::Move(const glm::vec3& direction, float delta) -> void {
  m_Position += m_Front * direction.z * m_MovementSpeed * delta;
  m_Position += m_Right * direction.x * m_MovementSpeed * delta;
  m_Position += m_Up * direction.y * m_MovementSpeed * delta;
}

auto Camera::Look(float dx, float dy) -> void {
  m_TargetYaw += dx * m_Sensitivity * m_InvertMouseX;
  m_TargetPitch += dy * m_Sensitivity * m_InvertMouseY;

  if (m_TargetPitch > 89.0f) {
    m_TargetPitch = 89.0f;
  }
  if (m_TargetPitch < -89.0f) {
    m_TargetPitch = -89.0f;
  }

  UpdateCameraVectors();
}

auto Camera::Front() const -> glm::vec3 { return m_Front; }

auto Camera::Position() const -> glm::vec3 { return m_Position; }

auto Camera::Right() const -> glm::vec3 { return m_Right; }

auto Camera::SetPosition(const glm::vec3& position) -> void {
  m_Position = position;
}

auto Camera::SetFront(const glm::vec3& front) -> void {
  m_Front = front;
  UpdateYawAndPitch();
}

auto Camera::GetViewMatrix() const -> glm::mat4 {
  return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

auto Camera::SetSpeed(float value) -> void {
  if (value < 0.0) {
    return;
  }
  m_MovementSpeed = value;
}
auto Camera::SetSensitivity(float value) -> void {
  if (value < 0.0) {
    return;
  }
  m_Sensitivity = value;
}
auto Camera::SetZoom(float value) -> void {
  if (value < 0.0) {
    return;
  }
  m_Zoom = value;
}

auto Camera::UpdateCameraVectors(bool calcFront) -> void {
  if (calcFront) {
    glm::vec3 front;
    front.x = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_Front = glm::normalize(front);
  }
  m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
  m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}

auto Camera::UpdateYawAndPitch() -> void {
  glm::vec3 vec = m_Front;
  m_TargetYaw = m_Yaw = glm::degrees(atan2(vec.x, vec.z));
  m_TargetPitch = m_Pitch = glm::degrees(asin(vec.y));
  UpdateCameraVectors(false);
}
