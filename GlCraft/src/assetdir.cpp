#include "assetdir.hpp"

using namespace glgame;

AssetDir::AssetDir(std::initializer_list<std::string> paths)
#ifdef __EMSCRIPTEN__
    : m_BaseDir{} {
#else  /* !__EMSCRIPTEN__ */
    : m_BaseDir{std::filesystem::current_path()} {
#endif /* __EMSCRIPTEN__ */
  for (const auto& path : paths) {
    m_BaseDir /= path;
  }
}

std::filesystem::path AssetDir::GetFile(const char *filename) const {
  auto path = m_BaseDir / filename;
  return path;
}
