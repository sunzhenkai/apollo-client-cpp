# Apollo Clien (C++)

Apollo client for cpp (unofficial).

# Dependencies

- json-c
- curl

# Build with vcpkg

## Compile

```shell
make release
```

# Usage

```cmake
find_package(apollo-client CONFIG REQUIRED)
target_link_libraries(main apollo-client::apollo-client)
```

# Appendices

- forked from [lzeqian/apollo](https://github.com/lzeqian/apollo)
