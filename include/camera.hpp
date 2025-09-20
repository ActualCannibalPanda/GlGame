#ifndef __HPP_PARADOX_CAMERA__
#define __HPP_PARADOX_CAMERA__

#include <glm/glm.hpp>

namespace pdx {
class Camera {
public:
  Camera(const glm::vec3& position, const glm::vec3& up, float yaw = -90.0f,
         float pitch = 0.0f);

  void Move(const glm::vec3& direction, float delta);
  void Look(float dx, float dy, float delta);

  glm::vec3 Front() const;
  glm::vec3 Position() const;
  glm::mat4 GetViewMatrix() const;
  glm::mat4 GetViewFromCamera(const Camera& other) const;

private:
  void UpdateCameraVectors();

  int m_InvertMouseY = -1;
  int m_InvertMouseX = 1;

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
