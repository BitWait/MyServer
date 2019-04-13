add_executable(EventLoop_test EventLoop_test.cpp)

target_link_libraries(EventLoop_test reactor)

add_executable(Reactor_test Reactor_test.cpp)

target_link_libraries(Reactor_test reactor)





