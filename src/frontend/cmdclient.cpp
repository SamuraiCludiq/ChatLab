#include "cmdclient.hpp"

int main(int argc, char const* argv[]) {
    chatlab::CmdClient<Sockiface> client(new Sockiface());
    client.iface->InitClient();
    client.WaitForCommand();

    return 0;
}
