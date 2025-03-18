#include "apollo/http.h"
#include "apollo/model.h"
#include "gtest/gtest.h"
#include <spdlog/spdlog.h>

apollo::ApolloClientOptions client_options{
    .address = "81.68.181.139:8080",
    .app_id = "000111",
    .cluster = "default",
    .secret_key = "4a46a5e6e6994c1599ddca631f09ecb3"};

TEST(Client, Notify) {}
