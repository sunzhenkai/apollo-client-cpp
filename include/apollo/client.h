/**
 * @file client.h
 * @brief apollo client
 * @author zhenkai.sun
 * */
#include <string>
#include <unordered_map>
// self
#include "apollo/model.h"

namespace apollo {
class ApolloClient {
public:
  ApolloClient(ApolloClientOptions &&options); //< Construct
  ~ApolloClient();                             //< Destructor

  /**
   * @brief Get all properties of the namespace (with cache)
   * @param [in] nmspace namespace
   * @param [in] ttl_s maximum allowable delay time in seconds, the value 0
   * means query
   * @return the properties of the namespace
   * immediately
   */
  Properties GetProperties(const std::string &nmspace, int ttl_s = 60);

  void Subscribe(SubscribeMeta &&meta);

private:
  ApolloClientOptions options;
};
} // namespace apollo
