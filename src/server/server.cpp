#include "server.hpp"

namespace chatlab {

cl_status Server::Start() {
    int opt = 1;
    this->serv_port = CL_DEFAULT_PORT;
    this->serv_addr.sin_addr.s_addr = inet_addr(CL_DEFAULT_SERV_ADDR);
    this->serv_addr.sin_port = htons(serv_port);
    this->serv_addr.sin_family = CL_SOCK_TYPE;

    if ((serv_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        ERROR_PRINT("can't create socket\n");
        return cl_status::ERROR;
    }

    if (setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        ERROR_PRINT("can't set socket options\n");
        return cl_status::ERROR;
    }

    if (bind(serv_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <
        0) {
        ERROR_PRINT("can't bind socket\n");
        return cl_status::ERROR;
    }

    if (listen(serv_socket, SOMAXCONN) < 0) {
        ERROR_PRINT("can't listen\n");
        return cl_status::ERROR;
    }

    serv_status = ServerStatus::run;
    DEBUG_PRINT("server addres: %s, server_port: %d\n",
                inet_ntoa(this->serv_addr.sin_addr),
                htons(this->serv_addr.sin_port));
    DEBUG_PRINT("server is running\n");

    AddToPoll(serv_socket,
              POLLIN);  // adding serv_socket to accept incoming connections

    while (serv_status == ServerStatus::run) {
        int poll_cnt = poll(pfds, nclients, -1);
        if (poll_cnt == -1) {
            ERROR_PRINT("failed to poll\n");
            return cl_status::ERROR;
        }
        DEBUG_PRINT("poll event occured\n");

        for (int i = 0; i < nclients; i++) {
            if (pfds[i].revents & POLLIN) {
                if (pfds[i].fd == serv_socket) {  // incoming connection
                    AcceptConnection();
                } else {  // client
                    Cmd cmd;

                    // reading command, we are expecting that alignement is even
                    // on all machines
                    int nbytes = read(pfds[i].fd, &cmd, sizeof(cmd));
                    if (nbytes <= 0) {
                        if (nbytes == 0) {
                            ERROR_PRINT("socket %d hung up\n", pfds[i].fd);
                        } else {
                            ERROR_PRINT("reading cmd from fd=%d failed\n",
                                        pfds[i].fd);
                        }
                        close(pfds[i].fd);
                        RmFromPoll(i);
                    }

                    switch (cmd.type) {
                        case CmdType::send:
                            if (cmd.msg_size > 0) {
                                DEBUG_PRINT("waiting for msg_size=%d\n",
                                            cmd.msg_size);
                                msg_buffer =
                                    (char *)malloc(cmd.msg_size * sizeof(char));
                                memset(msg_buffer, 0,
                                       cmd.msg_size * sizeof(char));

                                nbytes =
                                    read(pfds[i].fd, msg_buffer, cmd.msg_size);
                                if (nbytes <= 0) {
                                    if (nbytes == 0) {
                                        ERROR_PRINT("socket %d hung up\n",
                                                    pfds[i].fd);
                                    } else {
                                        ERROR_PRINT("recv from fd=%d failed\n",
                                                    pfds[i].fd);
                                    }
                                    close(pfds[i].fd);
                                    RmFromPoll(i);
                                }
                                std::cout << msg_buffer << std::endl;
                                free(msg_buffer);
                                msg_buffer = NULL;
                            }
                            break;
                        case CmdType::terminate:
                            if (true) {  // only admin should terminate server
                                DEBUG_PRINT("server recieved terminate cmd\n");
                                serv_status = ServerStatus::stop;
                            }
                            break;
                        default:
                            break;
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
        pfds = (struct pollfd *)realloc(pfds, sizeof *pfds * max_nclients);
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
    new_client.addr_len = sizeof((struct sockaddr *)&new_client.addr);
    new_client.socket = accept(serv_socket, (struct sockaddr *)&new_client.addr,
                               &new_client.addr_len);
    if (new_client.socket >= 0 && serv_status == ServerStatus::run) {
        DEBUG_PRINT("accepted new connection from %s\n",
                    inet_ntoa(new_client.addr.sin_addr));
        new_client.status = ClientStatus::connected;
        clients.push_back(new_client);
        AddToPoll(new_client.socket, POLLIN);
        return cl_status::SUCCESS;
    } else {
        ERROR_PRINT("connection with %s failed\n",
                    inet_ntoa(new_client.addr.sin_addr));
        switch (errno) {
            case EBADF:
                ERROR_PRINT("EBADF\n");
                break;
            case ENOTSOCK:
                ERROR_PRINT("ENOTSOCK\n");
                break;
            case EOPNOTSUPP:
                ERROR_PRINT("EOPNOTSUPP\n");
                break;
            case ECONNABORTED:
                ERROR_PRINT("ECONNABORTED\n");
                break;
            case EINVAL:
                ERROR_PRINT("EINVAL\n");
                break;
            case EMFILE:
                ERROR_PRINT("EMFILE\n");
                break;
            case ENFILE:
                ERROR_PRINT("ENFILE\n");
                break;
            default:
                break;
        }
        return cl_status::ERROR;
    }
}

cl_status Server::Stop() {
    // TODO: Bcast shutdown
    if (DisconnectAll() != cl_status::SUCCESS) {
        return cl_status::ERROR;
    };
    close(serv_socket);

    WARNING_PRINT("server is shutting down\n");

    return cl_status::SUCCESS;
}

cl_status Server::DisconnectClient(ServerClient &client) {
    return cl_status::SUCCESS;
}

cl_status Server::DisconnectAll() { return cl_status::SUCCESS; }

cl_status Server::SendTo(ServerClient &client) { return cl_status::SUCCESS; }

cl_status Server::Bcast() { return cl_status::SUCCESS; }

}  // namespace chatlab

int main(int argc, char const *argv[]) {
    chatlab::Server server;

    server.Start();
    server.Stop();

    return 0;
}
