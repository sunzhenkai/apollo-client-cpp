/**
 * @file client.h
 * @brief apollo client
 * @author zhenkai.sun
 * */
#pragma once
#include <functional>
#include <shared_mutex>
#include <string>
#include <unordered_map>
// self
#include "apollo/http.h"
#include "apollo/model.h"

namespace apollo {
/**
 * @brief notify function for subscribe
 * */
using NotifyFunction =
    std::function<void(const std::string &nmspace, Properties &&properties)>;

class ApolloClient {
public:
  struct Meta {
    SubscribeMeta meta;
    bool is_running{true};
    std::thread td;
  };
  ApolloClient(const ApolloClientOptions &options); //< Construct
  ~ApolloClient();                                  //< Destructor

  /**
   * @brief Get all properties of the namespace (with cache)
   * @param [in] nmspace namespace
   * @param [in] ttl_s WANR: NOT IMPLEMENT FOR NOW. maximum allowable delay
   * time in seconds, the value 0 means query
   * @return the properties of the namespace
   * immediately
   */
  Properties GetProperties(const std::string &nmspace, int ttl_s = 60);

  Properties GetPropertiesFromCache(const std::string &nmspace);

  /**
   * @brief Get all properties without cache
   * @param [in] nmspace namespace
   * @return the properties of the namespace
   */
  Properties GetPropertiesDirectly(const std::string &nmspace);

  /**
   * @brief subscribe a namespace, and callback function will be called when
   * namespace's properties changed
   * @param [in] subscribe meta data
   * @param [in] callback function. function definaltion see @ref
   * NotifyFunction
   * @return subscribe id
   */
  int Subscribe(SubscribeMeta &&meta, NotifyFunction &&callback);

  /**
   * @brief unscbscribe
   * @param subscribe_id subscribe id
   */
  void Unsubscribe(int subscribe_id);

private:
  ApolloClientOptions options_;
  ApolloHttpClient client_;
  std::shared_mutex subscribe_mtx_{};
  std::vector<Meta *> subscribes{};
  std::shared_mutex properties_mutex_{};
  std::unordered_map<std::string, Properties> properties_{};
};
} // namespace apollo
