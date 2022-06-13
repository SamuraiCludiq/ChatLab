list (APPEND CLIENT_SRC
    client.cpp
    )

add_library(client)
target_sources(client PRIVATE ${CLIENT_SRC})
install(TARGETS client)
