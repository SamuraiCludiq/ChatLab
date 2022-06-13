
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../common/base.hpp"


namespace chatlab {

class Client {
 public:
    int sock;
    int fd;
    Client() {
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("\n Socket creation error \n");
        }
    }
    ~Client() { close(fd); }
    /* Start client */
    cl_status Start();
    /* Connect to server */
    cl_status Connect();
    /* Disconnect from server */
    cl_status Disconnect();
    /* Broadcast a message to all connected clients */
    cl_status Bcast();
};

}

#endif /* !CLIENT_HPP */
