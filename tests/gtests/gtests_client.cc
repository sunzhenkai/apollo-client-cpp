#include "apollo/client.h"
#include "apollo/http.h"
#include "apollo/model.h"
#include "gtest/gtest.h"
#include <chrono>
#include <spdlog/spdlog.h>
#include <thread>

using namespace std::chrono_literals;

apollo::ApolloClientOptions client_options{
    .address = "81.68.181.139:8080",
    .app_id = "000111",
    .cluster = "default",
    .secret_key = "4a46a5e6e6994c1599ddca631f09ecb3"};

TEST(Client, Get) {
  apollo::ApolloClient client(client_options);
  auto r = client.GetProperties("application");
  spdlog::info("result_size: {}", r.data.size());

  r = client.GetProperties("Public");
  spdlog::info("result_size: {}", r.data.size());
  ASSERT_TRUE(r.data.size() > 0);
}

TEST(Client, Notify) {
  apollo::ApolloClient client(client_options);
  client.Subscribe({{"application", "Public"}},
                   [](const std::string &nms, apollo::Properties &&p) {
                     spdlog::info("namespace Properties updated: {}", nms);
                   });
  std::this_thread::sleep_for(30s);
}

TEST(Client, GetFromCache) {
  apollo::ApolloClient client(client_options);
  auto r = client.GetPropertiesFromCache("application");
  ASSERT_TRUE(r.data.size() > 0);
}
