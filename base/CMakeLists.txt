
#1.cmake verson，指定cmake版本 
cmake_minimum_required(VERSION 2.8)

#2.project name，指定项目的名称，一般和项目的文件夹名称对应
PROJECT(base)

ADD_DEFINITIONS(-g -W -Wall -Wno-deprecated -DLINUX -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO -DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DTIXML_USE_STL -DHAVE_CXX_STDHEADERS ${CMAKE_CXX_FLAGS} -std=c++11 )

# aux_source_directory(. DIR_SRCS)

# 指定生成目标
# add_executable(timestamp_test ${DIR_SRCS})

#add_executable(LogStream_test LogStream.cpp LogStream_test.cpp )

#add_executable(timestamp_test Timestamp_test.cpp timestamp.cpp )

#add_executable(FileUtil_test FileUtil_test.cpp fileutil.cpp )

#add_executable(Singleton_test Singleton_test.cpp )

add_executable(Logfile_test Logfile_test.cpp logfile.cpp fileutil.cpp logging.cpp timestamp.cpp  LogStream.cpp)

add_executable(Logging_test Logging_test.cpp logging.cpp logfile.cpp timestamp.cpp  LogStream.cpp fileutil.cpp)






