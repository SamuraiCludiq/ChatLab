#ifndef SOCKIFACE_H
#define SOCKIFACE_H

#include "netiface.h"

class Sockiface : public Netiface {
   private:
    int sock = 0;
    struct sockaddr_in serv_addr;

    cl_status SockWrite(const void *send_buf, int maxlen);
    cl_status SockRead(const void *send_buf, int maxlen);

   public:
    Sockiface(char *addr = NULL, int port = 8080,
              sa_family_t sin_family = AF_INET);
    ~Sockiface();
    cl_status InitClient();
    cl_status InitServer();
    cl_status Send(std::string msg, CmdType cmd_type);
    cl_status Recv();
};

Sockiface::Sockiface(char *addr, int port, sa_family_t sin_family) {
    serv_addr.sin_family = sin_family;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr =
        inet_addr(addr != NULL ? addr : CL_DEFAULT_SERV_ADDR);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
    }
    DEBUG_PRINT("created socket\n");
}

cl_status Sockiface::InitClient() {
    if (iface_type != NetifaceType::notinited) {
        ERROR_PRINT("iface already initialised\n");
        return cl_status::ERROR;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
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

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        ERROR_PRINT("can't set socket options\n");
        return cl_status::ERROR;
    }

    if (bind(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        ERROR_PRINT("can't bind socket\n");
        return cl_status::ERROR;
    }

    if (listen(sock, SOMAXCONN) < 0) {
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
    if (sock) {
        close(sock);
    }
}

cl_status Sockiface::SockWrite(const void *send_buf, int maxlen) {
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

cl_status Sockiface::Send(std::string msg, CmdType cmd_type) {
    // step 1: sending command
    struct Cmd cmd;
    char *buf = NULL;

    cmd.msg_size = msg.size();
    cmd.type = cmd_type;

    if (SockWrite(&cmd, sizeof(cmd)) == cl_status::ERROR) {
        ERROR_PRINT("send failed\n");
        return cl_status::ERROR;
    }
    DEBUG_PRINT("sent cmd to server\n");

    if (msg.size() > 0) {
        // step 2: sending actual data
        buf = (char *)malloc(sizeof(char) * msg.size());
        strcpy(buf, msg.c_str());
        if (SockWrite(buf, msg.size()) == cl_status::ERROR) {
            free(buf);
            ERROR_PRINT("send failed\n");

            return cl_status::ERROR;
        }
        free(buf);

        DEBUG_PRINT("sent data to server\n");
    }

    return cl_status::SUCCESS;
}

cl_status Sockiface::Recv() { return cl_status::SUCCESS; }

#endif /* !SOCKIFACE_H */
