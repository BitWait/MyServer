cmake_minimum_required(VERSION 2.8)

add_library(protobuf_codec codec.cpp)
target_link_libraries(protobuf_codec protobuf net z)

add_custom_command(OUTPUT query.pb.cc query.pb.h
  COMMAND protoc
  ARGS --cpp_out . ${CMAKE_CURRENT_SOURCE_DIR}/query.proto -I${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS query.proto
  VERBATIM )

ADD_DEFINITIONS(-I/include -std=c++11)

set_source_files_properties(query.pb.cc PROPERTIES COMPILE_FLAGS "-Wno-conversion -Wno-shadow")
include_directories(${PROJECT_BINARY_DIR})

add_library(query_proto query.pb.cc)
target_link_libraries(query_proto protobuf pthread)

add_executable(protobuf_codec_test Codec_test.cpp)
set_target_properties(protobuf_codec_test PROPERTIES COMPILE_FLAGS "-Wno-error=shadow")
target_link_libraries(protobuf_codec_test protobuf_codec query_proto)


