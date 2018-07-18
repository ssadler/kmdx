add_executable(aya main.cpp)

target_link_libraries(aya PRIVATE wing ethashseal wingcore)

install(TARGETS aya DESTINATION bin OPTIONAL)
