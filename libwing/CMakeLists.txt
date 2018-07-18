file(GLOB sources "*.cpp" "*.h")

add_library(wing ${sources})

target_include_directories(wing PRIVATE "${UTILS_INCLUDE_DIR}")
target_link_libraries(wing PUBLIC evm wingcore wingvm p2p devcrypto devcore PRIVATE jsoncpp_lib_static Snappy::snappy Boost::fiber Boost::context)
