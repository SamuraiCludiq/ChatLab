#include "client.hpp"

namespace chatlab {
    cl_status Client::Connect() {
        fd = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        if (fd < 0) {
            ERROR_PRINT("can't connect to server\n");
            return cl_status::ERROR;
        }
        DEBUG_PRINT("connected to server\n");
        status = ClientStatus::connected;

        return cl_status::SUCCESS;
    }

    cl_status Client::SockWrite(const void *send_buf, int maxlen) {
        int sent = 0, tmp = 0;

        cl_status status = cl_status::SUCCESS;
        while (1) {
            tmp = write(sock, (char *) send_buf + sent, maxlen - sent);
            if (tmp <= 0) {
                if (errno == EAGAIN) {
                    continue;
                } else if (errno == ECONNRESET) {
                    ERROR_PRINT("write ECONNRESET\n");
                    status = cl_status::ERROR;
                    goto fn_exit;
                }
                status = cl_status::ERROR;
                ERROR_PRINT("write error\n");
                goto fn_exit;
            } else {
                sent += tmp;
            }

            if (sent == maxlen)
                break;
        }

     fn_exit:
        return status;

     fn_fail:
        goto fn_exit;
    }

    cl_status Client::Send(std::string msg) {
        if (msg.size() == 0) {
            // don't panic if message is empty
            DEBUG_PRINT("not sending empty msg\n");
            return cl_status::SUCCESS;
        }

        if (status == ClientStatus::disconnected) {
            ERROR_PRINT("not connected to a server");
        }

        // step 1: sending command
        struct Cmd cmd;

        cmd.msg_size = msg.size();
        cmd.type = CmdType::send;

        if (SockWrite(&cmd, sizeof(cmd)) == cl_status::ERROR) {
            ERROR_PRINT("send failed\n");
            return cl_status::ERROR;
        }
        DEBUG_PRINT("sent cmd to server\n");

        // step 2: sending actual data
        buf = (char *)malloc(sizeof(char) * msg.size());
        strcpy(buf, msg.c_str());
        if (SockWrite(buf, msg.size()) == cl_status::ERROR) {
            free(buf);
            buf = NULL;
            ERROR_PRINT("send failed\n");

            return cl_status::ERROR;
        }
        free(buf);
        buf = NULL;

        return cl_status::SUCCESS;
    }
}

int main(int argc, char const* argv[])
{
    struct sockaddr_in serv_addr;
    std::string msg = "my msg!!!";

    chatlab::Client client;
    client.Connect();
    client.Send(msg);
    client.Disconnect();

    return 0;
}
