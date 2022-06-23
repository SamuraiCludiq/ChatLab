
#ifndef NETIFACE_HPP
#define NETIFACE_HPP

#include <base.hpp>

enum class NetifaceType { client, server, notinited };

class Netiface {
   public:
    NetifaceType iface_type = NetifaceType::notinited;
    virtual cl_status InitClient() = 0;
    virtual cl_status InitServer() = 0;
    virtual cl_status Send(std::string msg, CmdType cmd_type) = 0;
    virtual cl_status Recv() = 0;
};

#endif /* !NETIFACE_HPP */
