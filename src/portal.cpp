#include "portal.hpp"
#include "assetdir.hpp"
#include "model.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

using namespace pdx;

static AssetDir portalPlaneDir{"data", "models", "portal"};
static AssetDir portalFrameDir{"data", "models", "portalFrame"};
static std::optional<pdx::Model> portalPlane = {};
static std::optional<pdx::Model> portalFrame = {};

Portal::Portal(const pdx::Camera& viewpoint, float rotation)
    : m_Viewpoint(viewpoint) {
  if (!portalPlane.has_value()) {
    portalPlane = pdx::Model::FromGLTF(portalPlaneDir.GetFile("scene.gltf"));
  }
  if (!portalFrame.has_value()) {
    portalFrame = pdx::Model::FromGLTF(portalFrameDir.GetFile("scene.gltf"));
  }
  m_ModelMatrix = glm::mat4(1.0);
  m_ModelMatrix = glm::translate(m_ModelMatrix, m_Viewpoint.Position());
  m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(90.0f),
                              glm::vec3(1.0f, 0.0f, 0.0f));
  m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(rotation),
                              glm::vec3(0.0f, 0.0f, 1.0f));
}

auto Portal::DrawPortalFrame(const glm::mat4& view, const glm::mat4& proj,
                             const pdx::Shader& shader) const -> void {
  if (portalFrame.has_value()) {
    shader.Use();
    shader.SetMat4fv("model", m_ModelMatrix);
    shader.SetMat4fv("view", view);
    shader.SetMat4fv("projection", proj);
    portalFrame->Draw();
  }
}

auto Portal::DrawPortalPlane(const glm::mat4& view, const glm::mat4& proj,
                             const pdx::Shader& shader) const -> void {
  if (portalPlane.has_value()) {
    shader.Use();
    shader.SetMat4fv("model",
                     glm::scale(m_ModelMatrix, glm::vec3(2.0f, 2.0f, 2.0f)));
    shader.SetMat4fv("view", view);
    shader.SetMat4fv("projection", proj);
    portalPlane->Draw();
  }
}

auto Portal::Position() const -> glm::vec3 { return m_Viewpoint.Position(); }
auto Portal::Front() const -> glm::vec3 { return m_Viewpoint.Front(); }
auto Portal::ModelMatrix() const -> glm::mat4 { return m_ModelMatrix; }
auto Portal::ViewMatrix() const -> glm::mat4 {
  return m_Viewpoint.GetViewMatrix();
}
auto Portal::GetCamera() const -> pdx::Camera { return m_Viewpoint; }

auto Portal::ClippedProj(const glm::mat4& view, const glm::mat4& proj) const
    -> glm::mat4 {
  float dist = glm::length(Position());
  glm::vec4 clipPlane(Front(), dist);
  clipPlane = glm::inverse(glm::transpose(view)) * clipPlane;

  if (clipPlane.w > 0.0f)
    return proj;

  glm::vec4 q =
      glm::inverse(proj) *
      glm::vec4(glm::sign(clipPlane.x), glm::sign(clipPlane.y), 1.0f, 1.0f);

  glm::vec4 c = clipPlane * (2.0f / (glm::dot(clipPlane, q)));

  glm::mat4 newProj = proj;
  // third row = clip plane - fourth row
  newProj = glm::row(newProj, 2, c - glm::row(newProj, 3));

  return newProj;
}

auto Portal::SetDestination(pdx::Portal *portal) -> void {
  m_Destination = portal;
}

auto Portal::GetDestination() const -> pdx::Portal * { return m_Destination; }
