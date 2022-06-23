
#ifndef NETIFACE_HPP
#define NETIFACE_HPP

#include <base.hpp>

enum class NetifaceType { client, server, notinited };

template <class T>
class Netiface {
   public:
    T id;
    NetifaceType iface_type = NetifaceType::notinited;
    virtual cl_status InitClient() = 0;
    virtual cl_status InitServer() = 0;
    virtual cl_status Send(T, std::string, CmdType) = 0;
    virtual cl_status Recv(T) = 0;
    T GetId() { return id; }
    cl_status SetId(T id) {
        this->id = id;
        return cl_status::SUCCESS;
    }
};

#endif /* !NETIFACE_HPP */
