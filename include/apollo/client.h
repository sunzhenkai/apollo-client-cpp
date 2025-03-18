/**
 * @file client.h
 * @brief apollo client
 * @author zhenkai.sun
 * */
#include <functional>
#include <string>
#include <unordered_map>
// self
#include "apollo/model.h"

namespace apollo {
/**
 * @brief notify function for subscribe
 * */
using NotifyFunction = std::function<void(Properties &&properties)>;

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

  /**
   * @brief subscribe a namespace, and callback function will be called when
   * namespace's properties changed
   * @param [in] subscribe meta data
   * @param [in] callback function. function definaltion see @ref NotifyFunction
   */
  void Subscribe(SubscribeMeta &&meta, const NotifyFunction &callback);

private:
  ApolloClientOptions options;
};
} // namespace apollo
