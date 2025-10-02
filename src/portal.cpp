#include "portal.hpp"
#include "assetdir.hpp"
#include "model.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace pdx;

static AssetDir portalDir{"data", "models", "portal"};
static std::optional<pdx::Model> portal = {};

Portal::Portal(const pdx::Camera& viewpoint) : m_Viewpoint(viewpoint) {
  if (!portal.has_value()) {
    portal = pdx::Model::FromGLTF(portalDir.GetFile("scene.gltf"));
  }
  m_Orientation = glm::fquat(1.0f, 0.0f, 0.0f, 0.0f);
  m_ModelMatrix = glm::mat4(1.0);
  m_ModelMatrix = glm::translate(m_ModelMatrix, m_Viewpoint.Position()) *
                  glm::mat4_cast(m_Orientation);
}

auto Portal::DrawPortalFrame(const glm::mat4& view, const glm::mat4& proj,
                             const pdx::Shader& shader) const -> void {
  if (portal.has_value()) {
    shader.Use();
    shader.SetMat4fv("model", m_ModelMatrix);
    shader.SetMat4fv("view", view);
    shader.SetMat4fv("projection", proj);
    portal->Draw("Frame");
  }
}

auto Portal::DrawPortalPlane(const glm::mat4& view, const glm::mat4& proj,
                             const pdx::Shader& shader) const -> void {
  if (portal.has_value()) {
    shader.Use();
    shader.SetMat4fv("model", m_ModelMatrix);
    shader.SetMat4fv("view", view);
    shader.SetMat4fv("projection", proj);
    portal->Draw("Portal");
  }
}

auto Portal::Position() const -> glm::vec3 { return m_Viewpoint.Position(); }
auto Portal::Front() const -> glm::vec3 { return m_Viewpoint.Front(); }
auto Portal::Right() const -> glm::vec3 { return m_Viewpoint.Right(); }
auto Portal::ModelMatrix() const -> glm::mat4 { return m_ModelMatrix; }
auto Portal::ViewMatrix() const -> glm::mat4 {
  return m_Viewpoint.GetViewMatrix();
}

auto Portal::ClippedProj(const glm::mat4& view, const glm::mat4& proj) const
    -> glm::mat4 {
  float d = glm::length(Position());
  // Calculate the clip plane with a normal and distance to the origin
  glm::vec4 newClipPlane(m_Orientation * glm::vec3(0.0f, 0.0f, -1.0f), d);
  newClipPlane = glm::inverse(glm::transpose(view)) * newClipPlane;
  // If the new clip plane's fourth component (w) is greater than 0, indicating
  // that it is facing away from the camera,
  if (newClipPlane.w > 0.0f)
    return proj;
  glm::vec4 q =
      glm::inverse(proj) * glm::vec4(glm::sign(newClipPlane.x),
                                     glm::sign(newClipPlane.y), 1.0f, 1.0f);
  glm::vec4 c = newClipPlane * (2.0f / (glm::dot(newClipPlane, q)));
  glm::mat4 newProjMat = proj;
  // third row = new clip plane - fourth row of projection matrix
  newProjMat = glm::row(newProjMat, 2, c - glm::row(newProjMat, 3));
  return newProjMat;
}

auto Portal::SetDestination(pdx::Portal *portal) -> void {
  m_Destination = portal;
}

auto Portal::GetDestination() const -> pdx::Portal * { return m_Destination; }
auto Portal::Orientation() const -> glm::fquat { return m_Orientation; }

auto Portal::AddAngle(float angle, const glm::vec3& axis) -> void {
  m_Orientation *= glm::angleAxis(glm::radians(angle), axis);
  m_ModelMatrix = glm::translate(glm::mat4(1.0f), Position()) *
                  glm::mat4_cast(m_Orientation);
}
