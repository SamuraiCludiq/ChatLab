#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <memory>
#include <string>

#include "../common/base.hpp"
#include "../network/sockiface.hpp"

namespace chatlab {

class BaseClient {
    std::string username = "anonymous";
    ClientStatus status = ClientStatus::disconnected;

   public:
    ClientStatus GetStatus() { return status; };
    cl_status SetStatus(ClientStatus status) {
        this->status = status;
        return cl_status::SUCCESS;
    };
    std::string GetUsername() { return username; }
    cl_status SetUsername(std::string username) {
        this->username = username;
        return cl_status::SUCCESS;
    }
};

}  // namespace chatlab

#endif /* !CLIENT_HPP */
