#ifndef POLLSERVER_HPP
#define POLLSERVER_HPP

#include <poll.h>

#include <list>

#include "../client/client.hpp"

namespace chatlab {

class SockServerClient : public BaseClient {
   public:
    int socket;
    socklen_t addr_len;
    struct sockaddr_in addr;

    SockServerClient() {}
    ~SockServerClient() {}
};

enum class ServerStatus { run, stop };

class SockServer : public Sockiface {
   private:
    int nclients, max_nclients;
    std::list<SockServerClient> clients;
    struct pollfd *pfds;
    ServerStatus serv_status = ServerStatus::stop;

    cl_status AcceptConnection();
    cl_status AddToPoll(int fd, short int events);
    cl_status RmFromPoll(int pos);

   public:
    SockServer() : nclients(0), max_nclients(CL_SERV_INITIAL_CLIENTS) {
        pfds = (struct pollfd *)malloc(sizeof *pfds * max_nclients);
    }
    ~SockServer() {
        if (pfds) {
            free(pfds);
        }
    }
    cl_status Start();
    cl_status Stop();
};

cl_status SockServer::Start() {
    char *msg_buffer = NULL;

    this->InitServer();

    serv_status = ServerStatus::run;
    DEBUG_PRINT("server addres: %s, server_port: %d\n",
                inet_ntoa(this->serv_addr.sin_addr),
                htons(this->serv_addr.sin_port));
    DEBUG_PRINT("server is running\n");

    AddToPoll(this->GetId(),
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
                if (pfds[i].fd == this->GetId()) {  // incoming connection
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

cl_status SockServer::AcceptConnection() {
    SockServerClient new_client;
    new_client.addr_len = sizeof((struct sockaddr *)&new_client.addr);
    new_client.socket =
        accept(this->GetId(), (struct sockaddr *)&new_client.addr,
               &new_client.addr_len);
    if (new_client.socket >= 0 && serv_status == ServerStatus::run) {
        DEBUG_PRINT("accepted new connection from %s\n",
                    inet_ntoa(new_client.addr.sin_addr));
        new_client.SetStatus(ClientStatus::connected);
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

cl_status SockServer::AddToPoll(int fd, short int events) {
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

cl_status SockServer::RmFromPoll(int pos) {
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

cl_status SockServer::Stop() {
    // TODO: Bcast shutdown
    close(this->GetId());

    WARNING_PRINT("server is shutting down\n");

    return cl_status::SUCCESS;
}

}  // namespace chatlab

#endif /* !POLLSERVER_HPP */
