#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
// 3rd
#include "apollo/model.h"
#include "fmt/format.h"
#include "httplib.h"
#include "rapidjson/document.h"
// self
#include <absl/status/status.h>

#include "apollo/http.h"
#include "apollo/utils.h"

namespace apollo {
const char *HMAC_SHA1_SIGN_DELIMITE = "\n";
const char *KEY_CONFIGURATION = "configurations";
const char *URL_NOTIFICATION = "/notifications/v2";
const char *KEY_NAMESPACE = "namespaceName";
const char *KEY_NOTIFICATION_ID = "notificationId";

ApolloHttpClient::ApolloHttpClient(const ApolloClientOptions &options) : client_(options.address), options_(options) {
  client_.set_read_timeout(80, 0);  // 80 seconds, for long pull request
}

httplib::Headers ApolloHttpClient::GetAuthHeaders(const std::string &path_with_query) {
  if (options_.secret_key.empty()) {
    return {};
  }
  std::string ts_ms = std::to_string(apollo::CurrentMilliseconds());
  auto key = fmt::format("{}{}{}", ts_ms, HMAC_SHA1_SIGN_DELIMITE, path_with_query);
  auto sign = HmacSha1Sign(key, options_.secret_key);
  auto token = fmt::format("Apollo {}:{}", options_.app_id, sign);
  return {{"Timestamp", ts_ms}, {"Authorization", token}};
}

absl::Status ApolloHttpClient::GetProperties(Properties &res, const std::string &nmspace) {
  std::string url = fmt::format("/configs/{}/{}/{}", options_.app_id, options_.cluster, nmspace);
  auto rsp = client_.Get(url, GetAuthHeaders(url));
  if (rsp == nullptr || rsp->status != 200) {
    auto msg = fmt::format("get properties from apollo failed. [url={}, status={}]", url, rsp ? rsp->status : -1);
    return absl::InternalError(msg);
  }

  // parse json string
  rapidjson::Document doc;
  if (doc.Parse(rsp->body.c_str()).HasParseError()) {
    auto msg = fmt::format("parse properties body failed. [url={}, body={}]", url, rsp->body);
    return absl::InternalError(msg);
  }

  auto config_it = doc.FindMember(KEY_CONFIGURATION);
  if (config_it != doc.MemberEnd()) {
    auto cobj = config_it->value.GetObject();
    for (auto it = cobj.MemberBegin(); it != cobj.MemberEnd(); ++it) {
      res.data[it->name.GetString()] = it->value.GetString();
    }
  }
  res.timestamp_ms = CurrentMilliseconds();
  return absl::OkStatus();
}

Notifications ApolloHttpClient::GetNotifications(const Notifications &meta) {
  Notifications notf;
  httplib::Params params{
      {"appId", options_.app_id}, {"cluster", options_.cluster}, {"notifications", meta.GetQueryString()}};
  auto url = httplib::append_query_params(URL_NOTIFICATION, params);
  auto rsp = client_.Get(url, GetAuthHeaders(url));
  // ignore 304 or other error
  // status == 200 means data has changed
  if (rsp->status == 200) {
    rapidjson::Document doc;
    auto err = [&]() {
      return std::runtime_error(fmt::format("parse notifivations body failed. [url={}, body={}]", url, rsp->body));
    };
    if (doc.Parse(rsp->body.c_str()).HasParseError() || !doc.IsArray()) {
      throw err();
    }
    for (auto &item : doc.GetArray()) {
      auto obj = item.GetObject();
      auto nmspace = obj.FindMember(KEY_NAMESPACE);
      auto notfid = obj.FindMember(KEY_NOTIFICATION_ID);
      if (nmspace != obj.MemberEnd() && notfid != obj.MemberEnd()) {
        notf.data[nmspace->value.GetString()] = notfid->value.GetInt();
      }
    }
  }
  return notf;
}

std::string Notifications::GetQueryString() const {
  bool is_first = true;
  std::stringstream ss;
  ss << "[";
  for (auto &[k, v] : data) {
    if (is_first) {
      is_first = false;
    } else {
      ss << ",";
    }
    ss << "{\"namespaceName\":\"" << k << "\",\"notificationId\":" << v << "}";
  }
  ss << "]";
  return ss.str();
}
}  // namespace apollo
