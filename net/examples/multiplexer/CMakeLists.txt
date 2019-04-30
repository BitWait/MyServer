add_executable(multiplex_server multiplexer.cpp)
target_link_libraries(multiplex_server net)

add_executable(multiplex_server_simple multiplexer_simple.cpp)
target_link_libraries(multiplex_server_simple net)

add_executable(multiplex_demux demux.cpp)
target_link_libraries(multiplex_demux net)

