#include "apollo/api.h"
#include "apollo/http.h"
#include "apollo/model.h"
#include "gtest/gtest.h"
#include <spdlog/spdlog.h>

apollo::ApolloClientOptions options{.address = "81.68.181.139:8080",
                                    .app_id = "000111",
                                    .cluster = "default",
                                    .secret_key =
                                        "4a46a5e6e6994c1599ddca631f09ecb3"};

TEST(Http, Get) {
  apollo::ApolloHttpClient client(options);
  auto r = client.GetProperties("application");
  spdlog::info("result_size: {}", r.size());
}

TEST(Client, Get) {
  apollo::ApolloClient client(options);
  auto r = client.GetProperties("application");
  spdlog::info("result_size: {}", r.size());

  r = client.GetProperties("Public");
  spdlog::info("result_size: {}", r.size());
}

TEST(Http, Notify) {
  apollo::Notifications notf;
  notf.data["app1"] = 100;
  notf.data["app2"] = 200;
  spdlog::info("Notifications: {}", notf.GetQueryString());
}

TEST(Http, GetNotf) {
  apollo::ApolloHttpClient client(options);
  apollo::Notifications notf = {.data = {{"application", -1}}};
  auto r = client.GetNotifications(notf);
  spdlog::info("updated Notifications: {}", r.GetQueryString());
}
