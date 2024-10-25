#include "assetdir.hpp"

#include <iostream>

using namespace craft;

AssetDir::AssetDir(std::initializer_list<std::string> paths)
#ifdef __EMSCRIPTEN__
    : m_BaseDir{} {
#else /* !__EMSCRIPTEN__ */
    : m_BaseDir{std::filesystem::current_path()} {
#endif /* __EMSCRIPTEN__ */
  for (const auto& path : paths) {
    m_BaseDir /= path;
  }
  std::cout << m_BaseDir.string() << std::endl;
}

std::filesystem::path AssetDir::GetFile(const char *filename) const {
  auto path = m_BaseDir / filename;
  return path;
}
