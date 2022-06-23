#include "client.hpp"

namespace chatlab {
template <class T>
void Client<T>::WaitForCommand() {
    std::string cmd, msg;
    while (cmd != "quit") {
        std::cout << "Enter command: ";
        std::getline(std::cin, cmd);
        switch (StrToCmd(cmd)) {
            case CmdType::send:
                if (status == ClientStatus::connected) {
                    std::cout << "Enter message: ";
                    std::getline(std::cin, msg);
                    iface->Send(msg, CmdType::send);
                } else {
                    WARNING_PRINT("disconnected from server\n");
                }
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
}  // namespace chatlab

int main(int argc, char const* argv[]) {
    chatlab::Client<Netiface> client(new Sockiface());
    client.iface->InitClient();
    client.WaitForCommand();

    return 0;
}
