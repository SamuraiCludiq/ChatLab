
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include "../common/base.hpp"


namespace chatlab {

class Client {
 private:
    int sock;
    int fd;
    char *buf = NULL;
    struct sockaddr_in serv_addr;
    std::string username = "anonymous";
    ClientStatus status = ClientStatus::disconnected;
 public:
    Client(char *addr = NULL, int port = 8080, sa_family_t sin_family = AF_INET) {
        serv_addr.sin_family = sin_family;
        serv_addr.sin_port = htons(port);
        serv_addr.sin_addr.s_addr = inet_addr(addr != NULL ? addr : CL_DEFAULT_SERV_ADDR);

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("\n Socket creation error \n");
        }
        DEBUG_PRINT("created socket\n");
    }
    ~Client() {
        close(fd);
        if(buf) {
            free(buf);
        }
    }
    cl_status Connect();
    /* Disconnect from server */
    cl_status Disconnect() {
        DEBUG_PRINT("disconnected from server\n");
        close(fd);
        return cl_status::SUCCESS;
    };
    cl_status SockWrite(const void *send_buf, int maxlen);
    cl_status Send(std::string msg, CmdType cmd_type = CmdType::send);
    /* Broadcast a message to all connected clients */
    cl_status Bcast();
    void WaitForCommand();
    cl_status SetUsername(std::string name) {
        this->username = name;
        return cl_status::SUCCESS;
    }
    std::string GetName() { return username; }
};

}

#endif /* !CLIENT_HPP */
