
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -g -Wall -O0 -Wno-unused-variable -liconv")
set(utils_SRCS
  DaemonRun.cpp
  MD5.cpp
  StringUtil.cpp
  URLEncodeUtil.cpp
  Adapter.cpp
  )

add_library(utils ${utils_SRCS})

# Ìí¼Ó tests ×ÓÄ¿Â¼
#add_subdirectory(tests)


add_executable(urlcode_test tests/urlcode_test.cpp)
target_link_libraries(urlcode_test utils)

add_executable(Adapter_test tests/Adapter_test.cpp)
target_link_libraries(Adapter_test utils)
