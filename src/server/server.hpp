#ifndef SERVER_HPP
#define SERVER_HPP

#include "../common/base.hpp"
#include <list>

namespace chatlab {

enum class ServerStatus {
    stop,
    run
};

enum class ClientStatus {
    connected,
    disconnected
};

struct ServerClient {
    struct sockaddr_in addr;
    socklen_t addr_len;
    ClientStatus status = ClientStatus::disconnected;
    int socket;
};

class Server {
    struct sockaddr_in serv_addr;
    std::list<ServerClient> clients;
    int serv_port, serv_socket;
    ServerStatus serv_status = ServerStatus::stop;
 public:
    Server() {}
    ~Server() {}
    cl_status Start();
    cl_status Stop();
    cl_status DisconnectClient(ServerClient &client);
    cl_status DisconnectAll();
    cl_status SendTo(ServerClient &client);
    cl_status Bcast();
    void AcceptThreadHandler();
};

}

#endif /* !SERVER_HPP */
