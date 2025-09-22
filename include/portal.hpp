#ifndef __HPP_PARADOX_PORTAL__
#define __HPP_PARADOX_PORTAL__

#include "camera.hpp"

namespace pdx {
class Portal {
public:
  Portal(const glm::vec3& position, const glm::vec3& up,
         const glm::vec3& front);

private:
  pdx::Camera m_Viewpoint;
  glm::mat4 m_ModelMatrix;
};
} // namespace pdx

#endif /* __HPP_PARADOX_PORTAL */
