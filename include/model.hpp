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

  static auto FromGLTF(const pdx::path& file) -> std::optional<Model>;

private:
  Model(tinygltf::Model& model, pdx::vao_t vao, std::map<int, GLuint> vbos,
        std::map<int, GLuint> textures);

  auto DrawMesh(const tinygltf::Mesh& mesh) const -> void;
  auto DrawNodes(const tinygltf::Node& node) const -> void;

  tinygltf::Model m_Model;
  pdx::vao_t m_Vao;
  std::map<int, pdx::ebo_t> m_Ebos;
  std::map<int, GLuint> m_Textures;
};
} // namespace pdx

#endif /* __HPP_PARADOX_MODEL__ */
