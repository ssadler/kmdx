cmake_minimum_required(VERSION 3.5.1)

include(cmake/cable/bootstrap.cmake)
include(CableBuildInfo)
include(CableBuildType)
include(CableToolchains)
include(GNUInstallDirs)

cable_configure_toolchain(DEFAULT cxx11)

set(ETH_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake" CACHE PATH "The path to the cmake directory")
list(APPEND CMAKE_MODULE_PATH ${ETH_CMAKE_DIR})

set(CPP_AYA_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH "Path to the root directory for ayeth")

# Map current configuration to configurations of imported targets.
set(CMAKE_MAP_IMPORTED_CONFIG_DEBUG Release)
set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release)

include(HunterGate)
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.21.16.tar.gz"
    SHA1 "aa1faccd24f33dd2c368f85a4e9ca2e5d9c249cb"
    LOCAL
)

project(ayeth)
set(PROJECT_VERSION 0.0.1.dev0)

cable_set_build_type(DEFAULT RelWithDebInfo CONFIGURATION_TYPES Debug;Release;RelWithDebInfo)

cable_add_buildinfo_library(PREFIX eth)

set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY On)

include(EthCcache)
include(EthOptions)
include(EthCompilerSettings)
include(EthExecutableHelper)
include(EthDependencies)
include(EthUtils)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
hunter_add_package(Boost COMPONENTS program_options filesystem system thread context fiber log)
find_package(Boost CONFIG REQUIRED program_options filesystem system thread context fiber log)

hunter_add_package(jsoncpp)
find_package(jsoncpp CONFIG REQUIRED)

hunter_add_package(Snappy)
find_package(Snappy CONFIG REQUIRED)

hunter_add_package(cryptopp)
find_package(cryptopp CONFIG REQUIRED)

hunter_add_package(libjson-rpc-cpp)
find_package(libjson-rpc-cpp CONFIG REQUIRED)

hunter_add_package(libscrypt)
find_package(libscrypt CONFIG REQUIRED)

hunter_add_package(ethash)
find_package(ethash CONFIG REQUIRED)

include(ProjectSecp256k1)
include(ProjectLibFF)

find_package(Threads)

set(UTILS_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/utils")

configure_project()

include_directories("${CMAKE_SOURCE_DIR}")

add_subdirectory(libdevcore)
add_subdirectory(libdevcrypto)
add_subdirectory(libp2p)
add_subdirectory(libevm)

add_subdirectory(libwingcore)
add_subdirectory(libwingvm)
add_subdirectory(libwing)
add_subdirectory(libethashseal)

add_subdirectory(aya)


if (TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

