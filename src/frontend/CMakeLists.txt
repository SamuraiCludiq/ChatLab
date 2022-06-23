list (APPEND CLIENT_SRC
    cmdclient.cpp
    )

set(executable "client")
add_executable(${executable} ${CLIENT_SRC})
install(TARGETS ${executable} DESTINATION ${CMAKE_BINARY_DIR}/_install)
