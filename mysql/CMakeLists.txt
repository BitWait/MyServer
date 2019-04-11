
#1.cmake verson，指定cmake版本 
cmake_minimum_required(VERSION 2.8)

#2.project name，指定项目的名称，一般和项目的文件夹名称对应
PROJECT(mysql)

link_directories(
    ${PROJECT_SOURCE_DIR}/lib
	/usr/lib64/mysql/
)

find_library(MYSQL_LIB libmysqlclient.so /usr/lib64/mysql/)

IF (NOT MYSQL_LIB)
    MESSAGE(FATAL_ERROR "mysqlclient not found. If your mysql is mariadb, please install mariadb-devel otherwise install mysql-devel.")
ENDIF(NOT MYSQL_LIB)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -g -Wall -O0 -Wno-unused-variable -pthread")

set(mysql_SRCS
	../base/timestamp.cpp
	../base/countdownlatch.cpp
	../base/fileutil.cpp
	../base/logging.cpp
	../base/logfile.cpp
	../base/LogStream.cpp
	../base/asynclogging.cpp
	../base/configfilereader.cpp

	../database/databasemysql.cpp
	../database/field.cpp
	../database/queryresult.cpp
	mysqlmanager.cpp
	tasklist.cpp
)

add_library(mysql ${mysql_SRCS})
TARGET_LINK_LIBRARIES(mysql mysqlclient)

# 添加 tests 子目录
add_subdirectory(tests)








