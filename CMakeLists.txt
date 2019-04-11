cmake_minimum_required(VERSION 2.8)

project (MYSERVER)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -g -Wall -O0 -Wno-unused-variable -pthread")

link_directories(
    ${PROJECT_SOURCE_DIR}/lib
	/usr/lib64/mysql/
)

find_library(MYSQL_LIB libmysqlclient.so /usr/lib64/mysql/)

IF (NOT MYSQL_LIB)
    MESSAGE(FATAL_ERROR "mysqlclient not found. If your mysql is mariadb, please install mariadb-devel otherwise install mysql-devel.")
ENDIF(NOT MYSQL_LIB)


set(net_srcs 
base/timestamp.cpp
base/countdownlatch.cpp
base/fileutil.cpp
base/logging.cpp
base/logfile.cpp
base/LogStream.cpp
base/asynclogging.cpp
base/configfilereader.cpp

net/accept.cpp
net/buffer.cpp
net/channel.cpp
net/connector.cpp
net/epollpoller.cpp
net/eventloop.cpp
net/inetaddress.cpp
net/sockets.cpp
net/tcpclient.cpp
net/tcpconnection.cpp
net/tcpserver.cpp
net/eventloopthread.cpp
net/eventloopthreadpool.cpp
net/protocolstream.cpp
net/timer.cpp
net/timerQueue.cpp
)

set(database_srcs
database/databasemysql.cpp
database/field.cpp
database/queryresult.cpp
)

set(mysql_srcs
mysql/mysqlmanager.cpp
mysql/tasklist.cpp
)

set(json_srcs
jsoncpp-0.5.0/json_reader.cpp
jsoncpp-0.5.0/json_value.cpp
jsoncpp-0.5.0/json_writer.cpp
)

set(zlib_srcs
zlib1.2.11/zutil.c
zlib1.2.11/uncompr.c
zlib1.2.11/trees.c
zlib1.2.11/inftrees.c
zlib1.2.11/inflate.c
zlib1.2.11/inffast.c
zlib1.2.11/infback.c
zlib1.2.11/gzwrite.c
zlib1.2.11/gzread.c
zlib1.2.11/gzlib.c
zlib1.2.11/gzclose.c
zlib1.2.11/deflate.c
zlib1.2.11/crc32.c
zlib1.2.11/compress.c
zlib1.2.11/adler32.c
zlib1.2.11/ZlibUtil.cpp
)

set(utils_srcs
utils/StringUtil.cpp
utils/URLEncodeUtil.cpp
utils/MD5.cpp
utils/DaemonRun.cpp
)

set(mychatserver_srcs
chatserversrc/main.cpp
chatserversrc/IMServer.cpp
chatserversrc/chatSession.cpp
chatserversrc/tcpSession.cpp)


add_executable(mychatserver ${net_srcs} ${mychatserver_srcs} ${zlib_srcs} ${utils_srcs} ${mysql_srcs} ${database_srcs})
#光包含库目录是没用的，还必须使用TARGET_LINK_LIBRARIES链接该库
TARGET_LINK_LIBRARIES(mychatserver mysqlclient)