#include <spdlog/spdlog.h>
#include <stdexcept>
#include <thread>
#include <utility>
// self
#include "apollo/client.h"
#include "apollo/model.h"

namespace apollo {
ApolloClient::ApolloClient(const ApolloClientOptions &options)
    : options_(options), client_(options) {}

Properties ApolloClient::GetProperties(const std::string &nmspace, int ttl_s) {
  return client_.GetProperties(nmspace);
}

int ApolloClient::Subscribe(SubscribeMeta &&meta,
                            const NotifyFunction &callback) {
  auto sid = subscribes.size();
  auto &nmeta =
      subscribes.emplace_back(ApolloClient::Meta{std::move(meta), true});
  // submit backgroud thread
  nmeta.td = std::thread([&]() {
    // build NotifyFunction
    Notifications noft;
    for (auto &m : meta.nmspaces) {
      noft.data[m] = -1;
    }
    while (&nmeta.is_running) {
      auto r = client_.GetNotifications(noft);
      if (!r.data.empty()) {
        spdlog::info("[{}] subscribe updated", __func__, r.GetQueryString());
      }
      for (auto &[k, v] : r.data) {
        // update Notifications id of namespace
        noft.data[k] = v;
        callback(k, client_.GetProperties(k));
      }
    }
  });
  return sid;
}

void ApolloClient::Unsubscribe(int subscribe_id) {
  if (subscribe_id < 0 || subscribe_id >= subscribes.size()) {
    spdlog::error("unexpected subscribe id. [id={}]", subscribe_id);
    return;
  }
  subscribes[subscribe_id].is_running = false;
}

ApolloClient::~ApolloClient() {
  for (auto &s : subscribes)
    s.is_running = false;
  for (auto &s : subscribes)
    s.td.join();
}
} // namespace apollo
