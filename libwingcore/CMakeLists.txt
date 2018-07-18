file(GLOB sources "*.cpp" "*.h")

add_library(wingcore ${sources})

target_include_directories(wingcore PRIVATE "${UTILS_INCLUDE_DIR}")
target_link_libraries(wingcore PUBLIC devcrypto devcore)
