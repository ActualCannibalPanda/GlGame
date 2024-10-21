#include "assetdir.hpp"

using namespace craft;

AssetDir::AssetDir(std::initializer_list<std::string> paths)
    : m_BaseDir{std::filesystem::current_path()} {
  for (const auto& path : paths) {
    m_BaseDir /= path;
  }
  assert(std::filesystem::exists(m_BaseDir));
}

std::filesystem::path AssetDir::GetFile(const char *filename,
                                        bool checkExists) const {
  auto path = m_BaseDir / filename;
  if (checkExists) {
    assert(std::filesystem::exists(path));
  }
  return path;
}
