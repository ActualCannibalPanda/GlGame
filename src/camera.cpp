#include "camera.hpp"

#include <glm/ext/matrix_transform.hpp>

using namespace pdx;

Camera::Camera(const glm::vec3& position, const glm::vec3& up, float yaw,
               float pitch)
    : m_Position(position), m_Up(up), m_WorldUp(up), m_Yaw(yaw), m_Pitch(pitch),
      m_MovementSpeed(5.0f), m_Sensitivity(0.1f), m_Zoom(1.0f) {
  UpdateCameraVectors();
}

void Camera::Move(const glm::vec3& direction, float delta) {
  m_Position += m_Front * direction.z * m_MovementSpeed * delta;
  m_Position += m_Right * direction.x * m_MovementSpeed * delta;
  m_Position += m_Up * direction.y * m_MovementSpeed * delta;
}

void Camera::Look(float dx, float dy, float delta) {
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

glm::vec3 Camera::Front() const { return m_Front; }

glm::vec3 Camera::Position() const { return m_Position; }

glm::mat4 Camera::GetViewMatrix() const {
  return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

glm::mat4 Camera::GetViewFromCamera(const Camera& other) const {
  return glm::lookAt(m_Position, m_Position + other.m_Front, m_Up);
}

void Camera::UpdateCameraVectors() {
  glm::vec3 front;
  front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
  front.y = sin(glm::radians(m_Pitch));
  front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
  m_Front = glm::normalize(front);
  m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
  m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}
