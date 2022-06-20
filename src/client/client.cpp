#include "client.hpp"

namespace chatlab {
    cl_status Client::Connect() {
        fd = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        if (fd != 0) {
            ERROR_PRINT("can't connect to server\n");
            switch (errno)
            {
            case EBADF:
                ERROR_PRINT("EBADF\n");
                break;
            case EFAULT:
                ERROR_PRINT("ERROR_PRINT\n");
                break;
            case ENOTSOCK:
                ERROR_PRINT("ERROR_PRINT\n");
                break;
            case EISCONN:
                ERROR_PRINT("ERROR_PRINT\n");
                break;
            case ECONNREFUSED:
                ERROR_PRINT("ECONNREFUSED\n");
                break;
            default:
                break;
            }
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

    cl_status Client::Send(std::string msg, CmdType cmd_type) {
        if (status == ClientStatus::disconnected) {
            ERROR_PRINT("not connected to a server\n");
        }

        // step 1: sending command
        struct Cmd cmd;

        cmd.msg_size = msg.size();
        cmd.type = cmd_type;

        if (SockWrite(&cmd, sizeof(cmd)) == cl_status::ERROR) {
            ERROR_PRINT("send failed\n");
            return cl_status::ERROR;
        }
        DEBUG_PRINT("sent cmd to server\n");

        if (msg.size() > 0) {
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
        }

        return cl_status::SUCCESS;
    }

    void Client::WaitForCommand() {
        std::string cmd, msg;
        while (cmd != "quit") {
            std::cout << "Enter command: ";
            std::getline(std::cin, cmd);
            switch(StrToCmd(cmd)) {
                case CmdType::send:
                    std::cout << "Enter message: ";
                    std::getline(std::cin, msg);
                    Send(msg);
                    break;
                case CmdType::terminate:
                    msg = "";
                    Send(msg, CmdType::terminate);
                    break;
                default:
                    break;
            }
        }
    }
}

int main(int argc, char const* argv[])
{
    struct sockaddr_in serv_addr;
    std::string msg = "my msg!!!";

    chatlab::Client client;
    client.Connect();
    client.WaitForCommand();
    client.Disconnect();

    return 0;
}
