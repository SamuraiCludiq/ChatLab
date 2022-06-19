#ifndef SERVER_HPP
#define SERVER_HPP

#include <list>
#include <thread>
#include <mutex>
#include <poll.h>

#include "../common/base.hpp"

namespace chatlab {

enum class ServerStatus {
    stop,
    run
};

struct ServerClient {
    struct sockaddr_in addr;
    socklen_t addr_len;
    ClientStatus status = ClientStatus::disconnected;
    int socket;
};

class Server {
    int serv_port = CL_DEFAULT_PORT, nclients, max_nclients, serv_socket;
    char *msg_buffer = NULL;
    struct sockaddr_in serv_addr;
    struct pollfd *pfds;
    std::list<ServerClient> clients;
    ServerStatus serv_status = ServerStatus::stop;

    cl_status AcceptConnection();
    cl_status AddToPoll(int fd, short int events);
    cl_status RmFromPoll(int pos);
    cl_status DisconnectClient(ServerClient &client);
    cl_status DisconnectAll();
 public:
    Server() : nclients(0), max_nclients(CL_SERV_INITIAL_CLIENTS) {
        pfds = (struct pollfd*) malloc(sizeof *pfds * max_nclients);
    }
    ~Server() {
        if (pfds) {
            free(pfds);
        }
        if (msg_buffer) {
            free(msg_buffer);
        }
    }
    cl_status Start();
    cl_status Stop();
    cl_status SendTo(ServerClient &client);
    cl_status Bcast();
};

}

#endif /* !SERVER_HPP */
