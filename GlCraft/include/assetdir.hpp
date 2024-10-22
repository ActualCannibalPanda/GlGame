#ifndef __HPP_GLCRAFT_ASSETDIR__
#define __HPP_GLCRAFT_ASSETDIR__

#include <cassert>
#include <initializer_list>
#include <string>

#include "types.hpp"

namespace craft {
class AssetDir {
public:
  AssetDir(std::initializer_list<std::string> paths);

  craft::path GetFile(const char *filename, bool checkExists = false) const;

private:
  craft::path m_BaseDir;
};

const AssetDir SHADER_DIR{"data", "shaders"};
} // namespace craft

#endif /*  __HPP_GLCRAFT_ASSETDIR__ */
