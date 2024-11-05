#include <catch2/catch_test_macros.hpp>

#include <assetdir.hpp>
#include <filesystem>

TEST_CASE("Factorials are computed", "[factorial]") {
  REQUIRE(std::filesystem::exists(
      glgame::AssetDir({"./data/shaders/"}).GetFile("simple.frag")));
}
