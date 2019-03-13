
#1.cmake verson，指定cmake版本 
cmake_minimum_required(VERSION 2.8)

#2.project name，指定项目的名称，一般和项目的文件夹名称对应
PROJECT(base)

ADD_DEFINITIONS(-g -W -Wall -Wno-deprecated -DLINUX -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO -DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DTIXML_USE_STL -DHAVE_CXX_STDHEADERS ${CMAKE_CXX_FLAGS} -std=c++11 )

set(base_SRCS
	asynclogging.cpp
	countdownlatch.cpp
	logging.cpp
	logfile.cpp
	timestamp.cpp
	LogStream.cpp
	fileutil.cpp
)


add_library(base ${base_SRCS})

file(GLOB HEADERS "*.h")

# 添加 tests 子目录
add_subdirectory(tests)








