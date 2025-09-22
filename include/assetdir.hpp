#ifndef __HPP_PARADOX_ASSETDIR__
#define __HPP_PARADOX_ASSETDIR__

#include <cassert>
#include <initializer_list>
#include <string>

#include "types.hpp"

namespace pdx {
class AssetDir {
public:
  AssetDir(std::initializer_list<std::string> paths);

  auto GetFile(const char *filename) const -> pdx::path;

private:
  pdx::path m_BaseDir;
};
} // namespace pdx

#endif /*  __HPP_PARADOX_ASSETDIR__ */
