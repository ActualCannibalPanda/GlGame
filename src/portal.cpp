#include "portal.hpp"
#include "assetdir.hpp"
#include "model.hpp"

#include <glm/ext/matrix_transform.hpp>

using namespace pdx;

static AssetDir portalPlaneDir{"data", "models", "portal"};
static AssetDir portalFrameDir{"data", "models", "portalFrame"};
static std::optional<pdx::Model> portalPlane = {};
static std::optional<pdx::Model> portalFrame = {};

Portal::Portal(const glm::vec3& position, const glm::vec3& up,
               const glm::vec3& front)
    : m_Viewpoint(Camera(position, up, front)) {
  if (!portalPlane.has_value()) {
    portalPlane = pdx::Model::FromGLTF(portalPlaneDir.GetFile("scene.gltf"));
  }
  if (!portalFrame.has_value()) {
    portalFrame = pdx::Model::FromGLTF(portalFrameDir.GetFile("scene.gltf"));
  }
  m_ModelMatrix = m_Viewpoint.GetRotationMatrix();
  m_ModelMatrix = glm::translate(m_ModelMatrix, position);
}
