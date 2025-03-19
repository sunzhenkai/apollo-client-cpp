#include <algorithm>
#include <mutex>
#include <shared_mutex>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <thread>
#include <utility>
// self
#include "apollo/client.h"
#include "apollo/model.h"
#include "apollo/utils.h"

namespace apollo {
ApolloClient::ApolloClient(const ApolloClientOptions &options)
    : options_(options), client_(options) {}

Properties ApolloClient::GetPropertiesDirectly(const std::string &nmspace) {
  return client_.GetProperties(nmspace);
}

Properties ApolloClient::GetProperties(const std::string &nmspace, int ttl_s) {
  Properties result;
  bool need_featch = ttl_s <= 0;
  // no need featch if namespace is subscribed
  auto is_nmspace_subscribed = [&](const std::string &nmspace) {
    for (const auto &i : subscribes) {
      if (!i.is_running)
        continue;
      for (const auto &j : i.meta.nmspaces) {
        if (j == nmspace) {
          return true;
        }
      }
    }
    return false;
  };

  if (!need_featch) {
    std::shared_lock lock(properties_mutex_);
    auto it = properties_.find(nmspace);
    if (it != properties_.end()) {
      auto ts_ms = CurrentMilliseconds();
      auto passed_s = (ts_ms - it->second.timestamp_ms) / 1000;
      // need featch if not subscribed && beyond ttl
      need_featch = !is_nmspace_subscribed(nmspace) && (passed_s > ttl_s);
      if (!need_featch) {
        result = it->second;
      }
    } else {
      need_featch = true;
    }
  }
  if (need_featch) {
    result = client_.GetProperties(nmspace);
    {
      std::unique_lock lock(properties_mutex_);
      properties_[nmspace] = result;
    }
  }
  return result;
}

int ApolloClient::Subscribe(SubscribeMeta &&meta,
                            const NotifyFunction &callback) {
  auto sid = subscribes.size();
  auto &nmeta =
      subscribes.emplace_back(ApolloClient::Meta{std::move(meta), true});
  spdlog::info("[{}] add subscribe. [id={}, namespace={}]", __func__, sid,
               ToString(nmeta.meta.nmspaces));
  // submit backgroud thread
  nmeta.td = std::thread([&]() {
    // build NotifyFunction
    Notifications noft;
    for (auto &m : nmeta.meta.nmspaces) {
      noft.data[m] = -1;
    }
    while (&nmeta.is_running) {
      spdlog::info("[{}] Query notify. [notify={}]", __func__,
                   noft.GetQueryString());
      auto r = client_.GetNotifications(noft);
      if (!r.data.empty()) {
        spdlog::info("[{}] subscribe updated", __func__, r.GetQueryString());
      }
      for (auto &[k, v] : r.data) {
        // update Notifications id of namespace
        noft.data[k] = v;
        callback(k, GetProperties(k, 0));
      }
    }
  });
  return sid;
}

Properties ApolloClient::GetPropertiesFromCache(const std::string &nmspace) {
  {
    std::shared_lock lock(properties_mutex_);
    auto it = properties_.find(nmspace);
    if (it != properties_.end())
      return it->second;
  }
  // no local cache
  return GetProperties(nmspace);
}

void ApolloClient::Unsubscribe(int subscribe_id) {
  if (subscribe_id < 0 || subscribe_id >= subscribes.size()) {
    spdlog::error("unexpected subscribe id. [id={}]", subscribe_id);
    return;
  }
  spdlog::info("[{}] Unsubscribe. [id={}, namespaces={}]", __func__,
               subscribe_id, ToString(subscribes[subscribe_id].meta.nmspaces));
  subscribes[subscribe_id].is_running = false;
}

ApolloClient::~ApolloClient() {
  for (auto i = 0; i < subscribes.size(); ++i)
    Unsubscribe(i);
  for (auto &s : subscribes)
    s.td.join();
}
} // namespace apollo
