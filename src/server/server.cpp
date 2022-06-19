#include "server.hpp"

namespace chatlab {

cl_status Server::Start() {
    int opt = 1;
    this->serv_port = CL_DEFAULT_PORT;
    this->serv_addr.sin_addr.s_addr = inet_addr(CL_DEFAULT_SERV_ADDR);
    this->serv_addr.sin_port = htons(serv_port);
    this->serv_addr.sin_family = CL_SOCK_TYPE;

    if ((serv_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        ERROR_PRINT("can't create socket");
        return cl_status::ERROR;
    }

    if (setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        ERROR_PRINT("can't set socket options");
        return cl_status::ERROR;
    }

    if (bind(serv_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        ERROR_PRINT("can't bind socket");
        return cl_status::ERROR;
    }

    if(listen(serv_socket, SOMAXCONN) < 0) {
        ERROR_PRINT("can't listen");
        return cl_status::ERROR;
    }

    serv_status = ServerStatus::run;
    DEBUG_PRINT("server addres: %s, server_port: %d\n",
                inet_ntoa(this->serv_addr.sin_addr), htons(this->serv_addr.sin_port));
    DEBUG_PRINT("server is running\n");

    AddToPoll(serv_socket, POLLIN); // adding serv_socket to accept incoming connections

    char buf[100];
    while(true) {
        int poll_cnt = poll(pfds, nclients, -1);
        if (poll_cnt == -1) {
            ERROR_PRINT("failed to poll\n");
            return cl_status::ERROR;
        }

        DEBUG_PRINT("poll get event\n");

        for(int i = 0; i < nclients; i++) {
            if (pfds[i].revents & POLLIN) {
                if (pfds[i].fd == serv_socket) { // incoming connection
                    AcceptConnection();
                } else { // client
                    int nbytes = recv(pfds[i].fd, buf, sizeof buf, 0);

                    if (nbytes <= 0) {
                        if (nbytes == 0) {
                            ERROR_PRINT("socket %d hung up\n", pfds[i].fd);
                        } else {
                            ERROR_PRINT("recv from fd=%d failed\n", pfds[i].fd);
                        }
                        close(pfds[i].fd);
                        RmFromPoll(i);
                    } else {
                        std::cout << buf << std::endl;
                    }
                }
            }
        }
    }

    return cl_status::SUCCESS;
}

cl_status Server::AddToPoll(int fd, short int events) {
    if (nclients == max_nclients) {
        max_nclients *= 2;
        pfds = (struct pollfd*)realloc(pfds, sizeof *pfds * max_nclients);
        DEBUG_PRINT("increasing pfds to %d\n", max_nclients);
    }
    DEBUG_PRINT("adding fd=%d to pfds\n", fd);

    pfds[nclients].fd = fd;
    pfds[nclients].events = events;

    nclients++;
    return cl_status::SUCCESS;
}

cl_status Server::RmFromPoll(int pos) {
    if (nclients > 0) {
        DEBUG_PRINT("removing fd=%d to pfds\n", pfds[pos].fd);

        pfds[pos].fd = pfds[nclients - 1].fd;
        pfds[nclients].events = pfds[nclients - 1].events;

        nclients--;
    } else {
        ERROR_PRINT("can't remove fd from pfds\n");
        return cl_status::ERROR;
    }

    return cl_status::SUCCESS;
}

cl_status Server::AcceptConnection() {
    ServerClient new_client;
    new_client.socket = accept(serv_socket, (struct sockaddr*)&new_client.addr, &new_client.addr_len);
    if (new_client.socket >= 0 && serv_status == ServerStatus::run) {
        DEBUG_PRINT("accepted new connection from %s\n", inet_ntoa(new_client.addr.sin_addr));
        new_client.status = ClientStatus::connected;
        clients.push_back(new_client);
        AddToPoll(new_client.socket, POLLIN);
        return cl_status::SUCCESS;
    } else {
        return cl_status::ERROR;
    }
}

cl_status Server::Stop() {
    // TODO: Bcast shutdown
    if(DisconnectAll() != cl_status::SUCCESS) {
        return cl_status::ERROR;
    };
    close(serv_socket);

    WARNING_PRINT("server is shutting down\n");

    return cl_status::SUCCESS;
}

cl_status Server::DisconnectClient(ServerClient &client) {
    return cl_status::SUCCESS;
}

cl_status Server::DisconnectAll() {
    return cl_status::SUCCESS;
}

cl_status Server::SendTo(ServerClient &client) {
    return cl_status::SUCCESS;
}

cl_status Server::Bcast() {
    return cl_status::SUCCESS;
}

}

int main(int argc, char const* argv[])
{
    chatlab::Server server;

    server.Start();
    server.Stop();

    return 0;
}
