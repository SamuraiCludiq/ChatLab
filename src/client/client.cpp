#include "client.hpp"


int main(int argc, char const* argv[])
{
    chatlab::Client client;
    struct sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(CL_DEFAULT_PORT);
    serv_addr.sin_addr.s_addr = inet_addr(CL_DEFAULT_SERV_ADDR);

    PRINT("Startup: server addres: %s, server_port: %d \n",
          inet_ntoa(serv_addr.sin_addr), htons(serv_addr.sin_port));

    if ((client.fd
         = connect(client.sock, (struct sockaddr*)&serv_addr,
                   sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    return 0;
}
