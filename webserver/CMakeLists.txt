#1.cmake verson，指定cmake版本 
cmake_minimum_required(VERSION 2.8)

#2.project name，指定项目的名称，一般和项目的文件夹名称对应
PROJECT(HttpClient)

link_directories(
    ${PROJECT_SOURCE_DIR}/lib
	#/usr/lib64/curl/
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -g -Wall -O0 -Wno-unused-variable -pthread  -lcurl")
set(base_SRCS
	../base/timestamp.cpp
	../base/countdownlatch.cpp
	../base/fileutil.cpp
	../base/logging.cpp
	../base/logfile.cpp
	../base/LogStream.cpp
	../base/asynclogging.cpp
	../base/configfilereader.cpp
	)

set(net_SRCS
	../net/sockets.cpp
	../net/accept.cpp
	../net/channel.cpp
	../net/inetaddress.cpp
	../net/timer.cpp
	../net/timerQueue.cpp
	../net/epollpoller.cpp
	../net/eventloop.cpp
	../net/eventloopthread.cpp
	../net/eventloopthreadpool.cpp
	../net/buffer.cpp
	../net/tcpconnection.cpp
	../net/tcpserver.cpp
	../net/connector.cpp
	../net/tcpclient.cpp
	../net/protocolstream.cpp
	../net/anyone.h
)

set(json_srcs
../jsoncpp-0.5.0/json_reader.cpp
../jsoncpp-0.5.0/json_value.cpp
../jsoncpp-0.5.0/json_writer.cpp
)

set(httpserver_SRCS
  HttpServer.cpp
  HttpResponse.cpp
  HttpContext.cpp
)

set(httpclient_SRCS
  HttpClient.cpp
  )

add_library(HttpClient ${httpclient_SRCS} ${base_SRCS} ${json_srcs} )
target_link_libraries(HttpClient)

add_library(HttpServer ${httpserver_SRCS} ${base_SRCS} ${net_SRCS} )
target_link_libraries(HttpServer)


# 添加 tests 子目录
add_subdirectory(tests)




