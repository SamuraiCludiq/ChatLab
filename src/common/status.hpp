#ifndef STATUS_HPP
#define STATUS_HPP

namespace chatlab {

enum class cl_status {
/* common function return codes */
    SUCCES,
    ERROR,
/* client errors */
    SERVER_CONN_ERROR /* can't connect to server */
/* server errors */
};

}

#endif /* STATUS_HPP */
