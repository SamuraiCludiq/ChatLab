list (APPEND SERVER_SRC
    pollserver.cpp
    )

set(executable "server")
add_executable(${executable} ${SERVER_SRC})
install(TARGETS ${executable} DESTINATION ${CMAKE_BINARY_DIR}/_install)
