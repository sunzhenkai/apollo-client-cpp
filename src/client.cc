#include <spdlog/spdlog.h>

#include <algorithm>
#include <exception>
#include <mutex>
#include <shared_mutex>
#include <stdexcept>
#include <thread>
#include <utility>
// self
#include "apollo/client.h"
#include "apollo/model.h"
#include "apollo/utils.h"

namespace apollo {
ApolloClient::ApolloClient(const ApolloClientOptions &options) : options_(options), client_(options) {}

Properties ApolloClient::GetPropertiesDirectly(const std::string &nmspace) { return client_.GetProperties(nmspace); }

Properties ApolloClient::GetProperties(const std::string &nmspace, int ttl_s) {
  Properties result;
  bool need_fetch = ttl_s <= 0;
  // no need featch if namespace is subscribed
  auto is_nmspace_subscribed = [&](const std::string &nmspace) {
    for (const auto &i : subscribes) {
      if (i == nullptr || !i->is_running) continue;
      for (const auto &j : i->meta.nmspaces) {
        if (j == nmspace) {
          return true;
        }
      }
    }
    return false;
  };

  if (!need_fetch) {
    std::shared_lock lock(properties_mutex_);
    auto it = properties_.find(nmspace);
    if (it != properties_.end()) {
      auto ts_ms = CurrentMilliseconds();
      auto passed_s = (ts_ms - it->second.timestamp_ms) / 1000;
      // need featch if not subscribed && beyond ttl
      need_fetch = !is_nmspace_subscribed(nmspace) && (passed_s > ttl_s);
      if (!need_fetch) {
        result = it->second;
      }
    } else {
      need_fetch = true;
    }
  }
  if (need_fetch) {
    result = client_.GetProperties(nmspace);
    {
      std::unique_lock lock(properties_mutex_);
      properties_[nmspace] = result;
    }
  }
  return result;
}

int ApolloClient::Subscribe(SubscribeMeta &&meta, NotifyFunction &&callback) {
  auto sid = subscribes.size();
  auto nmeta = new ApolloClient::Meta{std::move(meta), true};
  subscribes.emplace_back(nmeta);
  spdlog::info("[{}] add subscribe. [id={}, namespace={}]", __func__, sid, ToString(nmeta->meta.nmspaces));
  // submit backgroud thread
  nmeta->td = std::thread([&, cb = std::move(callback), nmeta = nmeta]() {
    // build NotifyFunction
    Notifications noft;
    for (auto &m : nmeta->meta.nmspaces) {
      noft.data[m] = -1;
    }
    while (nmeta->is_running) {
      spdlog::info("[{}] Query notify. [notify={}]", __func__, noft.GetQueryString());
      Notifications new_notf;
      try {
        new_notf = client_.GetNotifications(noft);
      } catch (const std::exception &e) {
        spdlog::error("[{}] Fetch subscribe notifications failed. [message={}]", __func__, e.what());
      }
      if (!new_notf.data.empty()) {
        spdlog::info("[{}] subscribe updated", __func__, new_notf.GetQueryString());
      }
      for (auto &[k, v] : new_notf.data) {
        // update Notifications id of namespace
        noft.data[k] = v;
        cb(k, GetProperties(k, 0));
      }
    }
  });
  return sid;
}

Properties ApolloClient::GetPropertiesFromCache(const std::string &nmspace) {
  {
    std::shared_lock lock(properties_mutex_);
    auto it = properties_.find(nmspace);
    if (it != properties_.end()) return it->second;
  }
  // no local cache
  return GetProperties(nmspace);
}

void ApolloClient::Unsubscribe(int subscribe_id) {
  if (subscribe_id < 0 || subscribe_id >= subscribes.size()) {
    spdlog::error("unexpected subscribe id. [id={}]", subscribe_id);
    return;
  }
  spdlog::info("[{}] Unsubscribe. [id={}, namespaces={}]", __func__, subscribe_id,
               ToString(subscribes[subscribe_id]->meta.nmspaces));
  subscribes[subscribe_id]->is_running = false;
}

ApolloClient::~ApolloClient() {
  for (auto i = 0; i < subscribes.size(); ++i) Unsubscribe(i);
  for (auto &s : subscribes) {
    s->td.join();
    delete s;
    s = nullptr;
  }
}
}  // namespace apollo
