#include <sstream>
#include <string>
#include <unordered_map>
// 3rd
#include "apollo/model.h"
#include "fmt/format.h"
#include "httplib.h"
#include "rapidjson/document.h"
#include "spdlog/spdlog.h"
// self
#include "apollo/http.h"
#include "apollo/utils.h"

namespace apollo {
const std::string HMAC_SHA1_SIGN_DELIMITE = "\n";
const char *KEY_CONFIGURATION = "configurations";
const char *URL_NOTIFICATION = "/notifications/v2";

ApolloHttpClient::ApolloHttpClient(const ApolloClientOptions &options)
    : client_(options.address), options_(options) {}

httplib::Headers
ApolloHttpClient::GetAuthHeaders(const std::string &path_with_query) {
  if (options_.secret_key.empty()) {
    return {};
  }
  std::string ts_ms = std::to_string(apollo::CurrentMilliseconds());
  auto key =
      fmt::format("{}{}{}", ts_ms, HMAC_SHA1_SIGN_DELIMITE, path_with_query);
  auto sign = HmacSha1Sign(key, options_.secret_key);
  auto token = fmt::format("Apollo {}:{}", options_.app_id, sign);
  return {{"Timestamp", ts_ms}, {"Authorization", token}};
}

Properties ApolloHttpClient::GetProperties(const std::string &nmspace) {
  std::string url = fmt::format("/configs/{}/{}/{}", options_.app_id,
                                options_.cluster, nmspace);
  auto rsp = client_.Get(url, GetAuthHeaders(url));
  if (rsp == nullptr || rsp->status != 200) {
    throw std::runtime_error(
        fmt::format("get properties from apollo failed. [url={}, status={}]",
                    url, rsp ? rsp->status : -1));
  }
  spdlog::info("url:{}, status:{}, body:{}", url, rsp->status, rsp->body);
  // parse json string
  rapidjson::Document doc;
  if (doc.Parse(rsp->body.c_str()).HasParseError()) {
    throw std::runtime_error(fmt::format(
        "parse properties body failed. [url={}, body={}]", url, rsp->body));
  }

  Properties res;
  auto config_it = doc.FindMember(KEY_CONFIGURATION);
  if (config_it != doc.MemberEnd()) {
    auto cobj = config_it->value.GetObject();
    for (auto it = cobj.MemberBegin(); it != cobj.MemberEnd(); ++it) {
      res[it->name.GetString()] = it->value.GetString();
    }
  }

  return res;
}

Notifications ApolloHttpClient::GetNotifications(const Notifications &meta) {
  Notifications notf;
  httplib::Params params{{"appId", options_.app_id},
                         {"cluster", options_.cluster},
                         {"notifications", ""}};
  return notf;
}

std::string Notifications::GetQueryString() {
  bool is_first = true;
  std::stringstream ss;
  ss << "[";
  for (auto &[k, v] : data) {
    if (is_first) {
      is_first = false;
    } else {
      ss << ",";
    }
    ss << "{\"namespaceName\": \"" << k << "\", \"notificationId\": " << v
       << "}";
  }
  ss << "]";
}
} // namespace apollo
