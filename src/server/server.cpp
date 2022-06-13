#include "server.hpp"

namespace chatlab {

cl_status Server::Start() {
    int opt = 1;
    this->serv_addr.sin_addr.s_addr = INADDR_ANY;
    this->serv_addr.sin_port = htons(serv_port);
    this->serv_addr.sin_family = CL_SOCK_TYPE;

    if ((serv_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return cl_status::ERROR;

    if (setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        return cl_status::ERROR;

    if (bind(serv_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        return cl_status::ERROR;

    if(listen(serv_socket, SOMAXCONN) < 0)
        return cl_status::ERROR;

    serv_status = ServerStatus::run;
    DEBUG_PRINT("server is running\n");

    while (serv_status == ServerStatus::run) {
        ServerClient new_client;

        new_client.socket = accept(serv_socket, (struct sockaddr*)&new_client.addr, &new_client.addr_len);
        if (new_client.socket >= 0 && serv_status == ServerStatus::run) {
            DEBUG_PRINT("accepted new connection from %s\n", inet_ntoa(new_client.addr));
            new_client.status = ClientStatus::connected;
            clients.push_back(new_client);
        }
    }

    return cl_status::SUCCES;
}

cl_status Server::Stop() {
    // TODO: Bcast shutdown
    if(DisconnectAll() != cl_status::SUCCES) {
        return cl_status::ERROR;
    };
    close(serv_socket);

    return cl_status::SUCCES;
}

cl_status Server::DisconnectClient(ServerClient &client) {
    return cl_status::SUCCES;
}

cl_status Server::DisconnectAll() {
    return cl_status::SUCCES;
}

cl_status Server::SendTo(ServerClient &client) {
    return cl_status::SUCCES;
}

cl_status Server::Bcast() {
    return cl_status::SUCCES;
}

}

int main(int argc, char const* argv[])
{
    chatlab::Server server;

    server.Start();

    return 0;
}
