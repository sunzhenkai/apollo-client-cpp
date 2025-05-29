cmake_minimum_required(VERSION 3.14)
project(apollo-client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(BUILD_TESTING "build testing" OFF)

find_package(absl CONFIG REQUIRED)
find_package(httplib CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)

set(THIRD_LIBRARIES
    absl::status
    httplib::httplib
    fmt::fmt
    spdlog::spdlog
    OpenSSL::SSL
    OpenSSL::Crypto
    RapidJSON
    rapidjson
    pthread)

include_directories(include)
aux_source_directory(src SRCS)

message(STATUS "SRCS=${SRCS}")

add_library(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} ${THIRD_LIBRARIES})

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

# install
include(GNUInstallDirs) # CMAKE_INSTALL_INCLUDEDIR / CMAKE_INSTALL_LIBDIR / ...
install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(
  EXPORT ${PROJECT_NAME}
  DESTINATION share/${PROJECT_NAME}
  FILE ${PROJECT_NAME}Config.cmake
  NAMESPACE ${PROJECT_NAME}::)
