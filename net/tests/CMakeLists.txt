# 指定生成目标 
add_executable(Inetaddress_test Inetaddress_test.cpp)
# 添加链接
target_link_libraries(Inetaddress_test net)

add_executable(Sockets_test Sockets_test.cpp)

target_link_libraries(Sockets_test net)

add_executable(EventLoop_test EventLoop_test.cpp)

target_link_libraries(EventLoop_test net)

add_executable(TimerQueue_test TimerQueue_test.cpp)

target_link_libraries(TimerQueue_test net)

add_executable(Channel_test Channel_test.cpp)

target_link_libraries(Channel_test net)