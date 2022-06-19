#ifndef SERVER_HPP
#define SERVER_HPP

#include <list>
#include <thread>
#include <mutex>
#include "../common/base.hpp"

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
    std::thread accept_thread;
    std::thread data_thread;
    struct sockaddr_in serv_addr;
    std::list<ServerClient> clients;
    std::mutex clients_mtx;
    int serv_port = CL_DEFAULT_PORT, nclients = 0, serv_socket;
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
    void DataThreadHandler();
};

}

#endif /* !SERVER_HPP */
