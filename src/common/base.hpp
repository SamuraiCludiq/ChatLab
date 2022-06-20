#ifndef BASE_HPP
#define BASE_HPP

#include <iostream>
#include <cstring>

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

enum class cl_status {
/* common function return codes */
    SUCCESS,
    ERROR
};

enum class ClientStatus {
    connected,
    disconnected
};

enum class CmdType {
    send = 0,
    terminate,
    setusername,
    setrights,
    disconnect,
    quit,
    invalid
};

CmdType StrToCmd(std::string cmd) {
    if (cmd == "send") return CmdType::send;
    if (cmd == "terminate") return CmdType::terminate;
    if (cmd == "setusername") return CmdType::setusername;
    if (cmd == "setrights") return CmdType::setrights;
    if (cmd == "disconnect") return CmdType::disconnect;
    return CmdType::invalid;
}

struct Cmd {
    CmdType type;
    unsigned int msg_size;
};

#endif /* BASE_HPP */
