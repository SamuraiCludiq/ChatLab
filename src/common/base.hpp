#ifndef BASE_HPP
#define BASE_HPP

#include <iostream>
#include <cstring>

#include "status.hpp"

#define CL_SOCK_TYPE AF_INET
#define CL_DEFAULT_PORT 8080
#define CL_DEFAULT_SERV_ADDR "127.0.0.1"
#define CL_SERV_INITIAL_CLIENTS 10

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#ifdef _WIN32
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif


#ifdef DEBUG
#define DEBUG_PRINT(fmt, args...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, \
                                          __FILE__, __LINE__, __func__, ##args)
#else
#define DEBUG_PRINT(fmt, ...)
#endif

#define ERROR_PRINT(fmt, args...) fprintf(stderr, "ERROR: %s:%d:%s(): " fmt, \
                                          __FILE__, __LINE__, __func__, ##args)

#define WARNING_PRINT(fmt, args...) fprintf(stderr, "WARNING: " fmt, ##args)

#define PRINT(fmt, args...) fprintf(stderr, fmt, ##args)

enum class ClientStatus {
    connected,
    disconnected
};

enum class CmdType {
    send,
    terminate,
    setusername,
    setrights
};

struct Cmd {
    CmdType type;
    unsigned int msg_size;
};

#endif /* BASE_HPP */
