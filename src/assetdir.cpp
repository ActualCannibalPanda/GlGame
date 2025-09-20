#include "assetdir.hpp"

using namespace pdx;

AssetDir::AssetDir(std::initializer_list<std::string> paths)
    : m_BaseDir{std::filesystem::current_path()} {
  for (const auto& path : paths) {
    m_BaseDir /= path;
  }
}

std::filesystem::path AssetDir::GetFile(const char *filename) const {
  auto path = m_BaseDir / filename;
  return path;
}
