#include <string>
#include <vector>

namespace apollo {
struct ConfigMeta {
  std::string app_id;
  std::string cluster{"default"};
  std::string nmspace; // namespace, namespace is a keyword in c++
  std::vector<std::string> keys;
};
} // namespace apollo
