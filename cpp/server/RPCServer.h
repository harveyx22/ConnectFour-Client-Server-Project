/**
 * Class containing code to start the server, listen for client, and
 * initialize threads.
 */

#pragma once
#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <cstring>
#include <vector>
#include <iterator>
using namespace std;


class RPCServer {
public:
    RPCServer(const char *serverIP, int port);
    ~RPCServer();
    void StartServer();
    void ListenForClient();

private:
    int m_server_fd{};
    int m_socket{};
    char* m_serverIP;
    int m_port;
    struct sockaddr_in m_address{};

};