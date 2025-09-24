#include "camera.hpp"

#include <glm/ext/matrix_transform.hpp>

using namespace pdx;

Camera::Camera(const glm::vec3& position, const glm::vec3& up,
               const glm::mat4& projection, float yaw, float pitch)
    : m_Position(position), m_Up(up), m_WorldUp(up), m_Yaw(yaw), m_Pitch(pitch),
      m_Projection(projection), m_MovementSpeed(5.0f), m_Sensitivity(0.1f),
      m_Zoom(1.0f) {
  UpdateCameraVectors();
}
Camera::Camera(const glm::vec3& position, const glm::vec3& up,
               const glm::vec3& front, const glm::mat4& projection)
    : m_Position(position), m_Up(up), m_Front(front), m_WorldUp(up),
      m_Projection(projection), m_Yaw(0.0f), m_Pitch(0.0f),
      m_MovementSpeed(5.0f), m_Sensitivity(0.1f), m_Zoom(1.0f) {
  UpdateYawAndPitch();
}

auto Camera::Move(const glm::vec3& direction, float delta) -> void {
  m_Position += m_Front * direction.z * m_MovementSpeed * delta;
  m_Position += m_Right * direction.x * m_MovementSpeed * delta;
  m_Position += m_Up * direction.y * m_MovementSpeed * delta;
}

auto Camera::Look(float dx, float dy, float delta) -> void {
  m_Yaw += dx * m_Sensitivity * m_InvertMouseX;
  m_Pitch += dy * m_Sensitivity * m_InvertMouseY;

  if (m_Pitch > 89.0f) {
    m_Pitch = 89.0f;
  }
  if (m_Pitch < -89.0f) {
    m_Pitch = -89.0f;
  }

  UpdateCameraVectors();
}

auto Camera::Front() const -> glm::vec3 { return m_Front; }

auto Camera::Position() const -> glm::vec3 { return m_Position; }

auto Camera::GetViewMatrix() const -> glm::mat4 {
  return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

auto Camera::GetRotationMatrix() const -> glm::mat4 {
  float alpha = m_Yaw;
  float beta = m_Pitch;

  float cosAlpha = cos(alpha);
  float sinAlpha = sin(alpha);
  float cosBeta = cos(beta);
  float sinBeta = sin(beta);

  glm::mat4 R_z = glm::mat4(
      // clang-format off
    cosAlpha, -sinAlpha, 0, 0, 
    sinAlpha, cosAlpha,  0, 0,
    0,        0,         1, 0, 
    0,        0,         0, 1
    //clang-format on
  );
  glm::mat4 R_y = glm::mat4(
    // clang-format off
    cosBeta,  0, sinBeta, 0,
    0,        1, 0,       0,
    -sinBeta, 0, cosBeta, 0,
    0,        0, 0,       1
    //
  );
  return R_z * R_y;
}

auto Camera::GetProjectionMatrix() const -> glm::mat4 {
  return m_Projection;
}

auto Camera::GetViewFromCamera(const Camera& other) const -> glm:: mat4 {
  return glm::lookAt(other.m_Position, other.m_Position + other.m_Front, other.m_Up);
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
  glm::mat4 view = GetViewMatrix();
  m_Yaw = glm::degrees(atan2(vec.x, vec.z));
  m_Pitch = glm::degrees(asin(vec.y));
  UpdateCameraVectors(false);
}
