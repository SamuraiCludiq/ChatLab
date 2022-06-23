#ifndef CMDCLIENT_HPP
#define CMDCLIENT_HPP

#include "../client/client.hpp"

namespace chatlab {

template <class T>
class CmdClient : public BaseClient {
   public:
    std::unique_ptr<T> iface;
    CmdClient(T *_iface) : iface(std::unique_ptr<T>(_iface)) {
        this->SetStatus(ClientStatus::connected);
    }
    ~CmdClient() {}
    void WaitForCommand();
};

template <class T>
void CmdClient<T>::WaitForCommand() {
    std::string cmd, msg;
    while (cmd != "quit") {
        std::cout << "Enter command: ";
        std::getline(std::cin, cmd);
        switch (StrToCmd(cmd)) {
            case CmdType::send:
                if (this->GetStatus() == ClientStatus::connected) {
                    std::cout << "Enter message: ";
                    std::getline(std::cin, msg);
                    iface->Send(iface->GetId(), msg, CmdType::send);
                } else {
                    WARNING_PRINT("disconnected from server\n");
                }
                break;
            case CmdType::terminate:
                msg = "";
                iface->Send(iface->GetId(), msg, CmdType::terminate);
                this->SetStatus(ClientStatus::disconnected);
                break;
            default:
                break;
        }
    }
}

}  // namespace chatlab

#endif /* !CMDCLIENT_HPP */