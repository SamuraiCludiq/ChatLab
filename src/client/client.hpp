
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <memory>
#include "../common/base.hpp"
#include "../network/sockiface.hpp"


namespace chatlab {

template <class T>
class Client {
 private:
    std::string username = "anonymous";
    ClientStatus status = ClientStatus::disconnected;
    std::unique_ptr<T> iface;
 public:
    Client(T *_iface) : iface(std::unique_ptr<T>(_iface)) {
        status = ClientStatus::connected;
    }
    ~Client() {}
    void WaitForCommand();
    cl_status SetUsername(std::string name) {
        this->username = name;
        return cl_status::SUCCESS;
    }
    std::string GetUsername() { return username; }
};

}

#endif /* !CLIENT_HPP */
