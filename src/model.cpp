#include "model.hpp"

#include <glad/gl.h>

#include <glm/glm.hpp>

#include <iostream>
#include <optional>
#include <string>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include <tiny_gltf.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace pdx;

Model::Model(tinygltf::Model& model, pdx::vao_t vao, std::map<int, GLuint> ebos,
             std::map<int, GLuint> textures)
    : m_Model(model), m_Vao(vao), m_Ebos(ebos), m_Textures(textures) {}

static bool LoadModel(tinygltf::Model& model, const pdx::path& path) {
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;
  std::string file = path.string();

  bool res = loader.LoadASCIIFromFile(&model, &err, &warn, file.c_str());
  if (!warn.empty()) {
    std::cout << "WARN: " << err << std::endl;
  }

  if (!err.empty()) {
    std::cout << "ERR: " << err << std::endl;
  }

  if (!res) {
    std::cout << "Failed to load glTF: " << file << std::endl;
  } else {
    std::cout << "Loaded glTF: " << file << std::endl;
  }

  return res;
}

static void BindMesh(std::map<int, GLuint>& vbos,
                     std::map<int, GLuint>& textures, tinygltf::Model& model,
                     tinygltf::Mesh& mesh) {
  for (size_t i = 0; i < model.bufferViews.size(); ++i) {
    const tinygltf::BufferView& bufferView = model.bufferViews[i];
    if (bufferView.target == 0) {
      continue;
    }

    const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

    GLuint vbo;
    glGenBuffers(1, &vbo);
    vbos[i] = vbo;
    glBindBuffer(bufferView.target, vbo);

    glBufferData(bufferView.target, bufferView.byteLength,
                 &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
  }

  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
    tinygltf::Primitive primitive = mesh.primitives[i];
    tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

    for (auto& attrib : primitive.attributes) {
      tinygltf::Accessor accessor = model.accessors[attrib.second];
      int byteStride =
          accessor.ByteStride(model.bufferViews[accessor.bufferView]);
      glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

      int size = 1;
      if (accessor.type != TINYGLTF_TYPE_SCALAR) {
        size = accessor.type;
      }

      int vaa = -1;
      if (attrib.first.compare("POSITION") == 0) {
        vaa = 0;
      }
      if (attrib.first.compare("NORMAL") == 0) {
        vaa = 1;
      }
      if (attrib.first.compare("TANGENT") == 0) {
        vaa = 2;
      }
      if (attrib.first.compare("TEXCOORD_0") == 0) {
        vaa = 3;
      }
      if (attrib.first.compare("TEXCOORD_1") == 0) {
        vaa = 4;
      }

      if (vaa > -1) {
        glEnableVertexAttribArray(vaa);
        glVertexAttribPointer(vaa, size, accessor.componentType,
                              accessor.normalized ? GL_TRUE : GL_FALSE,
                              byteStride, BUFFER_OFFSET(accessor.byteOffset));
      } else {
        std::cout << "vaa missing: " << attrib.first << std::endl;
      }
    }

    if (model.textures.size() > 0) {
      for (size_t texIdx = 0; texIdx < model.textures.size(); ++texIdx) {
        tinygltf::Texture& tex = model.textures[texIdx];

        if (tex.source > -1) {
          GLuint texid;
          glGenTextures(1, &texid);
          textures[texIdx] = texid;

          tinygltf::Image& image = model.images[tex.source];

          glBindTexture(GL_TEXTURE_2D, texid);
          glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

          GLenum format = GL_RGBA;
          if (image.component == 1) {
            format = GL_RED;
          } else if (image.component == 2) {
            format = GL_RG;
          } else if (image.component == 3) {
            format = GL_RGB;
          }

          GLenum type = GL_UNSIGNED_BYTE;
          if (image.bits == 16) {
            type = GL_UNSIGNED_SHORT;
          }

          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
                       format, type, &image.image.at(0));
        }
      }
    }
  }
}

static void BindModelNodes(std::map<int, GLuint>& vbos,
                           std::map<int, GLuint>& textures,
                           tinygltf::Model& model, tinygltf::Node& node) {
  if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
    BindMesh(vbos, textures, model, model.meshes[node.mesh]);
  }

  for (size_t i = 0; i < node.children.size(); ++i) {
    assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
    BindModelNodes(vbos, textures, model, model.nodes[node.children[i]]);
  }
}

std::optional<Model> Model::FromGLTF(const pdx::path& path) {
  tinygltf::Model model;

  if (!LoadModel(model, path)) {
    return {};
  }

  std::map<int, GLuint> vbos, textures;
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  const tinygltf::Scene& scene = model.scenes[model.defaultScene];
  for (size_t i = 0; i < scene.nodes.size(); ++i) {
    assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
    BindModelNodes(vbos, textures, model, model.nodes[scene.nodes[i]]);
  }

  glBindVertexArray(0);
  // cleanup vbos but do not delete index buffers yet
  for (auto it = vbos.cbegin(); it != vbos.cend();) {
    tinygltf::BufferView bufferView = model.bufferViews[it->first];
    if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER) {
      glDeleteBuffers(1, &vbos[it->first]);
      vbos.erase(it++);
    } else {
      ++it;
    }
  }
  return std::make_optional(Model(model, vao, vbos, textures));
}

void Model::Draw() const {
  glBindVertexArray(m_Vao);
  for (size_t i = 0; i < m_Textures.size(); ++i) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, m_Textures.at(i));
  }
  const tinygltf::Scene& scene = m_Model.scenes[m_Model.defaultScene];
  for (size_t i = 0; i < scene.nodes.size(); ++i) {
    DrawNodes(m_Model.nodes[scene.nodes[i]]);
  }
  for (size_t i = 0; i < m_Textures.size(); ++i) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  glBindVertexArray(0);
}

void Model::DrawNodes(const tinygltf::Node& node) const {
  if ((node.mesh >= 0) && (node.mesh < m_Model.meshes.size())) {
    DrawMesh(m_Model.meshes[node.mesh]);
  }

  const tinygltf::Scene& scene = m_Model.scenes[m_Model.defaultScene];
  for (size_t i = 0; i < node.children.size(); ++i) {
    DrawNodes(m_Model.nodes[node.children[i]]);
  }
}

void Model::DrawMesh(const tinygltf::Mesh& mesh) const {
  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
    tinygltf::Primitive primitive = mesh.primitives[i];
    tinygltf::Accessor indexAccessor = m_Model.accessors[primitive.indices];

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebos.at(indexAccessor.bufferView));

    glDrawElements(primitive.mode, indexAccessor.count,
                   indexAccessor.componentType,
                   BUFFER_OFFSET(indexAccessor.byteOffset));
  }
}
