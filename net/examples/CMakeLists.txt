#add_subdirectory(twisted)

#add_subdirectory(simple)

#add_subdirectory(asio)

#add_subdirectory(filetransfer)

#add_subdirectory(shorturl)

add_subdirectory(socks4a)

add_subdirectory(multiplexer)

if(HIREDIS_INCLUDE_DIR AND HIREDIS_LIBRARY)
  add_subdirectory(hiredis)
else()
  add_subdirectory(hiredis EXCLUDE_FROM_ALL)
endif()