find_package(absl REQUIRED)

file(GLOB_RECURSE SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cc)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})

add_executable(installer ${SRC_FILES})

target_include_directories(installer PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(installer PRIVATE messages spdlog absl::base absl::flags absl::flags_parse mysqlcppconn)