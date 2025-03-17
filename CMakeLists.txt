cmake_minimum_required(VERSION 3.14)
project(apollo-client)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

inlcude(GNUInstallDirs) # CMAKE_INSTALL_INCLUDEDIR / CMAKE_INSTALL_LIBDIR / ...

find_package(CURL REQUIRED)
find_package(json-c CONFIG REQUIRED)

include_directories(include)
set(project_sources)
aux_source_directory(src project_sources)
message(STATUS "project sources: ${project_sources}")

add_library(${PROJECT_NAME} ${project_sources})
target_link_libraries(${PROJECT_NAME} CURL::libcurl json-c::json-c)

install(
  TARGETS ${LIBRARY_NAME}
  EXPORT ${LIBRARY_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY include DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(
  EXPORT ${LIBRARY_NAME}
  DESTINATION share/${LIBRARY_NAME}
  FILE ${LIBRARY_NAME}Config.cmake
  NAMESPACE ${LIBRARY_NAME}::)
