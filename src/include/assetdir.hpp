#ifndef __HPP_GLCRAFT_ASSETDIR__
#define __HPP_GLCRAFT_ASSETDIR__

#include <cassert>
#include <filesystem>
#include <initializer_list>
#include <string>

namespace craft {
class AssetDir {
public:
  AssetDir(std::initializer_list<std::string> paths);

  std::filesystem::path GetFile(const char *filename,
                                bool checkExists = false) const;

private:
  std::filesystem::path m_BaseDir;
};

const AssetDir SHADER_DIR{"data", "shaders"};
} // namespace craft

#endif /*  __HPP_GLCRAFT_ASSETDIR__ */
