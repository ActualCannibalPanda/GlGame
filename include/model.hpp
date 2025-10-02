#ifndef __HPP_PARADOX_MODEL__
#define __HPP_PARADOX_MODEL__

#include <glad/gl.h>

#include <map>
#include <optional>

#include <tiny_gltf.h>

#include "types.hpp"

namespace pdx {
class Model {
public:
  auto Draw() const -> void;
  auto Draw(const std::string& name) const -> void;

  static auto FromGLTF(const std::filesystem::path& file)
      -> std::optional<Model>;

private:
  Model(tinygltf::Model& model, std::map<std::string, pdx::vao_t> vaos,
        std::map<int, GLuint> vbos, std::map<int, GLuint> textures);

  auto DrawMesh(const tinygltf::Mesh& mesh) const -> void;
  auto DrawNodes(const tinygltf::Node& node) const -> void;

  tinygltf::Model m_Model;
  std::map<std::string, pdx::vao_t> m_Vaos;
  std::map<int, pdx::ebo_t> m_Ebos;
  std::map<int, GLuint> m_Textures;
};
} // namespace pdx

#endif /* __HPP_PARADOX_MODEL__ */
