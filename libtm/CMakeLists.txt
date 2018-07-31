file(GLOB sources "*/*.cpp" "*/*.h")
list(REMOVE_ITEM sources "test/main.cpp")

add_library(tm ${sources})

target_include_directories(tm PRIVATE "${UTILS_INCLUDE_DIR}")
target_link_libraries(tm PUBLIC wing devcore wingcore p2p PRIVATE Boost::system Boost::log Boost::thread)


add_executable(tmtest "test/main.cpp")
target_include_directories(tmtest PRIVATE "${UTILS_INCLUDE_DIR}")
target_link_libraries(tmtest PUBLIC tm wing devcore wingcore p2p PRIVATE Boost::system Boost::log Boost::thread)
