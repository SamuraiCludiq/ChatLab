
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <memory>
#include <string>

#include "../common/base.hpp"
#include "../network/sockiface.hpp"

namespace chatlab {

template <class T>
class Client {
   private:
    std::string username = "anonymous";
    ClientStatus status = ClientStatus::disconnected;

   public:
    std::unique_ptr<T> iface;
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

}  // namespace chatlab

#endif /* !CLIENT_HPP */
