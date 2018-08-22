file(GLOB sources "*/*.cpp" "*/*.h")
get_filename_component(test_main_cpp ${CMAKE_CURRENT_SOURCE_DIR}/test/main.cpp ABSOLUTE)
list(REMOVE_ITEM sources "${test_main_cpp}")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")

add_library(tm ${sources})

target_include_directories(tm PRIVATE "${UTILS_INCLUDE_DIR}" "${Boost_INCLUDE_DIRS}")
target_link_libraries(tm PUBLIC devcore PRIVATE Boost::system Boost::fiber Boost::context Boost::log Boost::thread Boost::date_time)


add_executable(tmtest "test/main.cpp")
target_include_directories(tmtest PRIVATE "${UTILS_INCLUDE_DIR}" "${Boost_INCLUDE_DIRS}")
target_link_libraries(tmtest PUBLIC tm devcore PRIVATE Boost::system Boost::fiber Boost::context Boost::log Boost::thread Boost::date_time)
