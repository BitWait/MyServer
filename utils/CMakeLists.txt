
set(utils_SRCS
  DaemonRun.cpp
  MD5.cpp
  StringUtil.cpp
  URLEncodeUtil.cpp
  )

add_library(utils ${utils_SRCS})

# Ìí¼Ó tests ×ÓÄ¿Â¼
#add_subdirectory(tests)


add_executable(urlcode_test tests/urlcode_test.cpp)
target_link_libraries(urlcode_test utils)

