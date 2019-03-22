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

add_executable(EventLoopThread_test EventLoopThread_test.cpp)

target_link_libraries(EventLoopThread_test net)

add_executable(EventLoopThreadPool_test EventLoopThreadPool_test.cpp)

target_link_libraries(EventLoopThreadPool_test net)

add_executable(Buffer_test Buffer_test.cpp)

target_link_libraries(Buffer_test net)

add_executable(Tcpserver_test1 Tcpserver_test1.cpp)

target_link_libraries(Tcpserver_test1 net)