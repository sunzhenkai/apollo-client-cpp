/**
 * @file model.h
 * @brief the defination of models
 * @author zhenkai.sun
 */
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace apollo {
using Properties = std::unordered_map<std::string, std::string>;

struct ApolloClientOptions {
  std::string app_id;
  std::string address; // apollo config server address with port
  std::string cluster;
};

struct ConfigMeta {
  std::string app_id;
  std::string cluster{"default"};
  std::string nmspace; // namespace, namespace is a keyword in c++
  std::vector<std::string> keys;
};

/**
 * @brief Subscribe meta data.
 */
struct SubscribeMeta {
  std::string nmspace;                  ///< namespace
  std::unordered_set<std::string> keys; ///< keys should be notified
};
} // namespace apollo
