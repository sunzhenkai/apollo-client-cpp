#include <thread>
#include <utility>
// self
#include "apollo/client.h"

namespace apollo {
ApolloClient::ApolloClient(const ApolloClientOptions &options)
    : options_(options), client_(options) {}

Properties ApolloClient::GetProperties(const std::string &nmspace, int ttl_s) {
  return client_.GetProperties(nmspace);
}

int ApolloClient::Subscribe(SubscribeMeta &&meta,
                            const NotifyFunction &callback) {
  auto sid = subscribes.size();
  auto &nmeta = subscribes.emplace_back(std::move(meta), true);
  // submit backgroud thread
  nmeta.td = std::thread([&]() {
    while (&nmeta.is_running) {
    }
  });
  return sid;
}

void ApolloClient::Unsubscribe(int subscribe_id) {}
ApolloClient::~ApolloClient() {}
} // namespace apollo
