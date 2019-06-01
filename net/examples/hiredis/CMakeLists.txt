add_executable(mrediscli Hiredis.cpp mrediscli.cpp)
target_link_libraries(mrediscli net hiredis)
