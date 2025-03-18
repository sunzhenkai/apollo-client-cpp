/**
 * @file model.h
 * @brief the defination of models
 * @author zhenkai.sun
 */
#pragma once
#include <string>
#include <unordered_map>
#include <vector>

namespace apollo {

struct Properties {
  std::unordered_map<std::string, std::string> data;
  long timestamp_ms;

  std::string GetDataStr();
};

struct ApolloClientOptions {
  std::string app_id;
  std::string address; //> apollo config server address with port
  std::string cluster;
  std::string secret_key; //> app level
};

/**
 * @brief Subscribe meta data.
 */
struct SubscribeMeta {
  std::vector<std::string> nmspaces; ///< namespace
};

struct Notifications {
  std::unordered_map<std::string, long> data;

  std::string GetQueryString() const;
};

} // namespace apollo
