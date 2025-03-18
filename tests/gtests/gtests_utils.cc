#include "apollo/utils.h"
#include "gtest/gtest.h"
#include <spdlog/spdlog.h>
#include <string>
#include <unordered_map>

TEST(ToString, Map) {
  std::unordered_map<std::string, std::string> m = {{"foo", "bar"},
                                                    {"hello", "world"}};
  spdlog::info("map: {}", apollo::ToString(m));
}
