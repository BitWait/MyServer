#add_executable(EventLoop_test EventLoop_test.cpp)

#target_link_libraries(EventLoop_test reactor)

#add_executable(Reactor_test Reactor_test.cpp)

#target_link_libraries(Reactor_test reactor)

add_executable(timerqueue_test timerqueue_test.cpp)

target_link_libraries(timerqueue_test reactor)

add_executable(PendingFunc_test PendingFunc_test.cpp)

target_link_libraries(PendingFunc_test reactor)

add_executable(EventloopThread_test EventloopThread_test.cpp)

target_link_libraries(EventloopThread_test reactor)




