#ifndef __HPP_GLGAME_ASSETDIR__
#define __HPP_GLGAME_ASSETDIR__

#include <cassert>
#include <initializer_list>
#include <string>

#include "types.hpp"

namespace glgame {
class AssetDir {
public:
  AssetDir(std::initializer_list<std::string> paths);

  glgame::path GetFile(const char *filename) const;

private:
  glgame::path m_BaseDir;
};
} // namespace glgame

#endif /*  __HPP_GLGAME_ASSETDIR__ */
