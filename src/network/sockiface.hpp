#ifndef SOCKIFACE_H
#define SOCKIFACE_H

#include "netiface.h"

class Sockiface : public Netiface<int> {
   private:
    // id represents socket
    int port = 0;

    cl_status SockWrite(int sock, const void *send_buf, int maxlen);
    cl_status SockRead(int sock, const void *send_buf, int maxlen);

   public:
    struct sockaddr_in serv_addr;

    Sockiface(char *addr = NULL, int port = CL_DEFAULT_PORT,
              sa_family_t sin_family = AF_INET);
    ~Sockiface();
    cl_status InitClient();
    cl_status InitServer();
    cl_status Send(int sock, std::string msg, CmdType cmd_type);
    cl_status Recv(int sock, std::string *msg, CmdType *cmd);
};

Sockiface::Sockiface(char *addr, int port, sa_family_t sin_family) {
    this->id = -1;
    this->port = port;
    serv_addr.sin_family = sin_family;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr =
        inet_addr(addr != NULL ? addr : CL_DEFAULT_SERV_ADDR);

    if ((id = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
    }
    DEBUG_PRINT("created socket\n");
}

cl_status Sockiface::InitClient() {
    if (iface_type != NetifaceType::notinited) {
        ERROR_PRINT("iface already initialised\n");
        return cl_status::ERROR;
    }

    if (connect(id, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
        ERROR_PRINT("can't connect to server\n");
        switch (errno) {
            case EBADF:
                ERROR_PRINT("EBADF\n");
                break;
            case EFAULT:
                ERROR_PRINT("EFAULT\n");
                break;
            case ENOTSOCK:
                ERROR_PRINT("ENOTSOCK\n");
                break;
            case EISCONN:
                ERROR_PRINT("EISCONN\n");
                break;
            case ECONNREFUSED:
                ERROR_PRINT("ECONNREFUSED\n");
                break;
            default:
                break;
        }
        return cl_status::ERROR;
    }
    DEBUG_PRINT("connected to server\n");

    iface_type = NetifaceType::client;
    return cl_status::SUCCESS;
}

cl_status Sockiface::InitServer() {
    int opt = 1;

    if (iface_type != NetifaceType::notinited) {
        ERROR_PRINT("iface already initialised\n");
        return cl_status::ERROR;
    }

    if (setsockopt(id, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)) != 0) {
        ERROR_PRINT("can't set socket options\n");
        return cl_status::ERROR;
    }

    if (bind(id, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
        ERROR_PRINT("can't bind socket\n");
        return cl_status::ERROR;
    }

    if (listen(id, SOMAXCONN) != 0) {
        ERROR_PRINT("can't listen\n");
        return cl_status::ERROR;
    }

    DEBUG_PRINT("server addres: %s, server_port: %d\n",
                inet_ntoa(this->serv_addr.sin_addr),
                htons(this->serv_addr.sin_port));
    DEBUG_PRINT("server is running\n");

    return cl_status::SUCCESS;
}

Sockiface::~Sockiface() {
    if (id) {
        close(id);
    }
}

cl_status Sockiface::SockWrite(int sock, const void *send_buf, int maxlen) {
    int sent = 0, tmp = 0;

    cl_status status = cl_status::SUCCESS;
    while (1) {
        tmp = write(sock, (char *)send_buf + sent, maxlen - sent);
        if (tmp <= 0) {
            if (errno == EAGAIN) {
                continue;
            } else if (errno == ECONNRESET) {
                ERROR_PRINT("ECONNRESET\n");
                status = cl_status::ERROR;
                goto fn_exit;
            }
            status = cl_status::ERROR;
            ERROR_PRINT("write error\n");
            goto fn_exit;
        } else {
            sent += tmp;
        }

        if (sent == maxlen) break;
    }

fn_exit:
    return status;

fn_fail:
    goto fn_exit;
}

cl_status Sockiface::SockRead(int sock, const void *recv_buf, int maxlen) {
    int recvd = 0, tmp = 0;

    cl_status status = cl_status::SUCCESS;
    while (1) {
        do {
            tmp = read(sock, (char *)recv_buf + recvd, maxlen - recvd);
            printf("tmp=%d, errno=%d\n", tmp, errno);
            if (tmp < 0) {
                if (errno == ECONNRESET) {
                    ERROR_PRINT("ECONNRESET\n");
                    status = cl_status::ERROR;
                    goto fn_exit;
                }
            }
        } while (tmp < 0 && errno == EINTR);

        if (tmp < 0) {
            status = cl_status::ERROR;
            ERROR_PRINT("read error\n");
            goto fn_exit;
        } else if (tmp == 0) {
            goto fn_exit;
        } else {
            recvd += tmp;
        }

        if (recvd == maxlen) break;
    }

fn_exit:
    return status;

fn_fail:
    goto fn_exit;
}

cl_status Sockiface::Send(int sock, std::string msg, CmdType cmd_type) {
    // step 1: sending command
    struct Cmd cmd;
    char *buf = NULL;

    cmd.msg_size = msg.size();
    cmd.type = cmd_type;

    if (SockWrite(sock, &cmd, sizeof(cmd)) == cl_status::ERROR) {
        ERROR_PRINT("send failed\n");
        return cl_status::ERROR;
    }
    DEBUG_PRINT("sent cmd to server\n");

    if (msg.size() > 0) {
        // step 2: sending actual data
        buf = (char *)malloc(sizeof(char) * msg.size());
        strcpy(buf, msg.c_str());
        if (SockWrite(sock, buf, msg.size()) == cl_status::ERROR) {
            free(buf);
            ERROR_PRINT("send failed\n");

            return cl_status::ERROR;
        }
        free(buf);

        DEBUG_PRINT("sent data to server\n");
    }

    return cl_status::SUCCESS;
}

cl_status Sockiface::Recv(int sock, std::string *msg, CmdType *type) {
    // step 1: recv command
    Cmd cmd;
    char *buf = NULL;

    if (SockRead(sock, &cmd, sizeof(cmd)) == cl_status::ERROR) {
        ERROR_PRINT("recv cmd failed\n");
        return cl_status::ERROR;
    }

    *type = cmd.type;

    // step 2: recv msg

    if (cmd.msg_size > 0) {
        DEBUG_PRINT("waiting for msg_size=%d\n", cmd.msg_size);
        buf = (char *)malloc(cmd.msg_size * sizeof(char));
        memset(buf, 0, cmd.msg_size * sizeof(char));

        if (SockRead(sock, buf, cmd.msg_size) == cl_status::ERROR) {
            ERROR_PRINT("recv msg failed\n");
            return cl_status::ERROR;
        }
        *msg = std::string(buf);
        free(buf);
        buf = NULL;
    }

    return cl_status::SUCCESS;
}

#endif /* !SOCKIFACE_H */
