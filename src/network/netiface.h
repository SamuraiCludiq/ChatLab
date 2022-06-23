#include <base.hpp>

class netiface {
 public:
    virtual cl_status Send(std::string msg, CmdType cmd_type) = 0;
    virtual cl_status Recv() = 0;
};
