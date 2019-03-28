# 指定生成目标 
add_executable(time time/time.cpp time/main.cpp)
# 添加链接
target_link_libraries(time net)
