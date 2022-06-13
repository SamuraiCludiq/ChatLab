list (APPEND SERVER_SRC
    server.cpp
    )

add_library(server)
target_sources(server PRIVATE ${SERVER_SRC})
install(TARGETS server)
