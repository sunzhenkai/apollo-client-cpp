/**
 * @file http.h
 * @brief http functions
 * @author zhenkai.sun
 */
#pragma once
// 3rd
#include "httplib.h"
// self
#include "apollo/model.h"
#include <string>
#include <unordered_map>

namespace apollo {
class ApolloHttpClient {
public:
  ApolloHttpClient(const ApolloClientOptions &options); //< constructor

  /**
   * @brief Get headers for authorization
   * @param [in] query path
   */
  httplib::Headers GetAuthHeaders(const std::string &path_with_query);

  /**
   * @brief Get the Properties of the namespace
   * @param [in] nmspace namespace
   */
  Properties GetProperties(const std::string &nmspace);

  Notifications GetNotifications(const Notifications &meta);

private:
  httplib::Client client_;
  ApolloClientOptions options_;
  std::unordered_map<std::string, std::string> headers_;
};
} // namespace apollo
