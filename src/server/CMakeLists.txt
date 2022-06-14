list (APPEND SERVER_SRC
    server.cpp
    )

set(executable "server")
add_executable(server ${SERVER_SRC})
install(TARGETS server DESTINATION ${CMAKE_BINARY_DIR}/_install)
