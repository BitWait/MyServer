#1.cmake verson，指定cmake版本 
cmake_minimum_required(VERSION 2.8)

#2.project name，指定项目的名称，一般和项目的文件夹名称对应
PROJECT(reactor)

link_directories(
    ${PROJECT_SOURCE_DIR}/lib
)

#ADD_DEFINITIONS(-g -W -Wall -Wno-deprecated -DLINUX -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO -DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DTIXML_USE_STL -DHAVE_CXX_STDHEADERS ${CMAKE_CXX_FLAGS} -std=c++11 )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -g -Wall -O0 -Wno-unused-variable -pthread")
set(reactor_SRCS
	../base/timestamp.cpp
	../base/countdownlatch.cpp
	../base/fileutil.cpp
	../base/logging.cpp
	../base/logfile.cpp
	../base/LogStream.cpp
	../base/asynclogging.cpp
	../base/configfilereader.cpp

	eventloop.cpp
	channel.cpp
	epollpoller.cpp
)


add_library(reactor ${reactor_SRCS})


# 添加 tests 子目录
add_subdirectory(tests)

