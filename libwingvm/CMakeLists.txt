file(GLOB sources "*.cpp" "*.h")

add_library(wingvm ${sources})

target_include_directories(wingvm PRIVATE "${UTILS_INCLUDE_DIR}")
target_link_libraries(wingvm PUBLIC devcore)
