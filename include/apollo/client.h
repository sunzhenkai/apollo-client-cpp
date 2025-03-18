/**
 * @file client.h
 * @brief apollo client
 * @author zhenkai.sun
 * */
#pragma once
#include <functional>
#include <shared_mutex>
#include <string>
// self
#include "apollo/http.h"
#include "apollo/model.h"

namespace apollo {
/**
 * @brief notify function for subscribe
 * */
using NotifyFunction = std::function<void(Properties &&properties)>;

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

  /**
   * @brief subscribe a namespace, and callback function will be called when
   * namespace's properties changed
   * @param [in] subscribe meta data
   * @param [in] callback function. function definaltion see @ref
   * NotifyFunction
   * @return subscribe id
   */
  int Subscribe(SubscribeMeta &&meta, const NotifyFunction &callback);

  /**
   * @brief unscbscribe
   * @param subscribe_id subscribe id
   */
  void Unsubscribe(int subscribe_id);

private:
  ApolloClientOptions options_;
  ApolloHttpClient client_;
  std::shared_mutex subscribe_mtx_;
  std::vector<Meta> subscribes{};
};
} // namespace apollo
