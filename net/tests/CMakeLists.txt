# 指定生成目标 
add_executable(Inetaddress_test Inetaddress_test.cpp)
# 添加链接
target_link_libraries(Inetaddress_test net)
