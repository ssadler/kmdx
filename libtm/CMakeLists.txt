file(GLOB sources "*/*.cpp" "*/*.h")

add_library(tm ${sources})

target_include_directories(tm PRIVATE "${UTILS_INCLUDE_DIR}")
target_link_libraries(tm PUBLIC PRIVATE Boost::system Boost::log Boost::thread)
