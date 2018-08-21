file(GLOB sources "*/*.cpp" "*/*.h")
list(REMOVE_ITEM sources "/home/thpn/dev/kmdx/kmdx/libtm/test/main.cpp")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")

add_library(tm ${sources})

find_package(Boost 1.60.0 COMPONENTS date_time fiber)

target_include_directories(tm PRIVATE "${UTILS_INCLUDE_DIR}" "${Boost_INCLUDE_DIRS}")
target_link_libraries(tm PUBLIC
        #        wing
        devcore
        #        wingcore
        #        p2p
        PRIVATE Boost::system Boost::fiber Boost::context Boost::log Boost::thread ${Boost_DATE_TIME_LIBRARY})


#add_executable(tmtest "test/main.cpp")
#[[
target_include_directories(tmtest PRIVATE "${UTILS_INCLUDE_DIR}" "${Boost_INCLUDE_DIRS}")
target_link_libraries(tmtest PUBLIC tm
#        wing
        devcore
#        wingcore
#        p2p
        PRIVATE Boost::system Boost::fiber Boost::context Boost::log Boost::thread ${Boost_DATE_TIME_LIBRARY})]]
