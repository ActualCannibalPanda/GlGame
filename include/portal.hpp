#ifndef __HPP_PARADOX_PORTAL__
#define __HPP_PARADOX_PORTAL__

#include "camera.hpp"
#include "shader.hpp"
#include <memory>

namespace pdx {
class Portal {
public:
  Portal(const pdx::Camera& viewPoint, float rotation);

  auto Position() const -> glm::vec3;
  auto Front() const -> glm::vec3;
  auto ModelMatrix() const -> glm::mat4;
  auto ViewMatrix() const -> glm::mat4;
  auto GetCamera() const -> pdx::Camera;

  auto DrawPortalFrame(const glm::mat4& view, const glm::mat4& proj,
                       const pdx::Shader& shader) const -> void;
  auto DrawPortalPlane(const glm::mat4& view, const glm::mat4& proj,
                       const pdx::Shader& shader) const -> void;

  auto ClippedProj(const glm::mat4& view, const glm::mat4& proj) const
      -> glm::mat4;

  auto SetDestination(pdx::Portal *portal) -> void;
  auto GetDestination() const -> pdx::Portal *;

private:
  pdx::Camera m_Viewpoint;
  pdx::Portal *m_Destination;
  glm::mat4 m_ModelMatrix;
};
} // namespace pdx

#endif /* __HPP_PARADOX_PORTAL */
