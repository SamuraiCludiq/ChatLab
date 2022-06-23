#include "pollserver.hpp"

int main(int argc, char const *argv[]) {
    chatlab::SockServer server;

    server.Start();
    server.Stop();

    return 0;
}
