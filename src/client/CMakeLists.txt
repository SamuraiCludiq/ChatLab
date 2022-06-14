list (APPEND CLIENT_SRC
    client.cpp
    )

set(executable "client")
add_executable(${executable} ${CLIENT_SRC})
install(TARGETS ${executable} DESTINATION ${CMAKE_INSTALL_PREFIX})
