#include "client.hpp"


int main(int argc, char const* argv[])
{
    chatlab::Client client;
    struct sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(CL_DEFAULT_PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if ((client.fd
         = connect(client.sock, (struct sockaddr*)&serv_addr,
                   sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    return 0;
}
