#include "client.hpp"

namespace chatlab {
    template <class T>
    void Client<T>::WaitForCommand() {
        std::string cmd, msg;
        while (cmd != "quit") {
            std::cout << "Enter command: ";
            std::getline(std::cin, cmd);
            switch(StrToCmd(cmd)) {
                case CmdType::send:
                    std::cout << "Enter message: ";
                    std::getline(std::cin, msg);
                    iface->Send(msg, CmdType::send);
                    break;
                case CmdType::terminate:
                    msg = "";
                    iface->Send(msg, CmdType::terminate);
                    status = ClientStatus::disconnected;
                    break;
                default:
                    break;
            }
        }
    }
}

int main(int argc, char const* argv[])
{
    std::string msg = "my msg!!!";

    chatlab::Client<netiface> client(new Sockiface());
    client.WaitForCommand();

    return 0;
}
