file(GLOB sources "*.cpp" "*.h")

add_executable(kmdx ${sources})
target_link_libraries(
    kmdx
    PRIVATE wing ethashseal evm web3jsonrpc webthree devcore Boost::program_options
)

target_include_directories(kmdx PRIVATE ../utils)

if(MINIUPNPC)
    target_compile_definitions(kmdx PRIVATE ETH_MINIUPNPC)
endif()

install(TARGETS kmdx DESTINATION bin)
