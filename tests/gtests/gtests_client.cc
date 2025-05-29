#include <spdlog/spdlog.h>

#include <memory>
#include <string>

#include "apollo/client.h"
#include "apollo/model.h"
#include "gtest/gtest.h"

using std::chrono_literals::operator""s;

apollo::ApolloClientOptions client_options{.app_id = "000111",
                                           .address = "81.68.181.139:8080",
                                           .cluster = "default",
                                           .secret_key = "4a46a5e6e6994c1599ddca631f09ecb3"};

TEST(Client, Get) {
  apollo::ApolloClient client(client_options);
  auto r = client.GetProperties("application");
  spdlog::info("result_size: {}", r.data.size());

  r = client.GetProperties("Public");
  spdlog::info("result_size: {}", r.data.size());
  ASSERT_GT(r.data.size(), 0);
}

TEST(Client, Notify) {
  apollo::ApolloClient client(client_options);
  {
    client.Subscribe({{"application", "Public"}}, [](const std::string &nms, apollo::Properties &&p) {
      spdlog::info("namespace Properties updated: {}", nms);
    });
  }
  std::this_thread::sleep_for(3s);
}

TEST(Client, NotifyV2) {
  {
    apollo::ApolloClient client(client_options);
    client.Subscribe({{"application", "Public"}}, [](const std::string &nms, apollo::Properties &&p) {
      spdlog::info("namespace Properties updated: {}", nms);
    });
  }
  {
    std::shared_ptr<apollo::ApolloClient> client;
    client = std::make_shared<apollo::ApolloClient>(client_options);
    client->Subscribe({{"application", "Public"}}, [](const std::string &nms, apollo::Properties &&p) {
      spdlog::info("namespace Properties updated: {}", nms);
    });
  }
}

TEST(Client, GetFromCache) {
  apollo::ApolloClient client(client_options);
  auto r = client.GetPropertiesFromCache("application");
  ASSERT_GT(r.data.size(), 0);
}

TEST(Client, LongPulling) {
  apollo::ApolloClient client(client_options);
  client.Subscribe({{"application", "Public"}}, [](const std::string &nms, apollo::Properties &&p) {
    spdlog::info("namespace Properties updated: {}", nms);
  });
}
