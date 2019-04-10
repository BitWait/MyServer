
add_executable(httpserver_test tests/HttpServer_test.cpp)
target_link_libraries(httpserver_test http)

add_executable(httprequest_unittest tests/HttpRequest_test.cpp)
target_link_libraries(httprequest_unittest http)


