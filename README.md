# Apollo Client (C++)

Apollo client for c++ (unofficial).

# Dependencies

## Libraries

- cpp-libhttp

## Programs

- vcpkg
- ninja
- cmake
- pkg-config

# Build with vcpkg

## Compile

```shell
make release
```

# Find & Link

```cmake
find_package(apollo-client CONFIG REQUIRED)
target_link_libraries(main apollo-client::apollo-client)
```

# Usage

```c++
#include "apollo/api.h"

// Init
apollo::apolloclientoptions options{
    .address = "81.68.181.139:8080",
    .app_id = "000111",
    .cluster = "default",
    .secret_key = "4a46a5e6e6994c1599ddca631f09ecb3"};
apollo::ApolloClient client(options);

// with local cache
auto properties = client.GetProperties("application");
// without local cache
auto properties = client.GetPropertiesDirectly("application");

// Subscribe
client.Subscribe({{"application", "Public"}},
                [](const std::string &nms, apollo::Properties &&p) {
                   spdlog::info("namespace Properties updated: {}", nms);
                 });

```

# Appendices
