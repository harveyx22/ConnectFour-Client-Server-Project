/* This is a very simple example of a server that will listen to a client
 * server, then run the various RPCs that between the server and client.
 *
 * Version 1 will have the server handle one client at a time. The server will:
 *      - Wait for connection from client
 *      - Process the Connect API once connected
 *      - Process all RPC requests until the client sends a disconnect RPC
 *      - This initial server will handle 2 RPC's:
 *          - Connect
 *          - Disconnect
*/

#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <vector>
#include <iterator>

#include "RPCServer.h"
#include "RPCImpl.cpp"

using namespace std;

// A normal C function that is executed as a thread
// when its name is specified in pthread_create()
void* initThread(void* vargp) {
    sleep(1);

    int socket = *(int *) vargp;
    auto *rpcImplObj = new RPCImpl(socket);
    rpcImplObj->processRPC();   // This will go until client disconnects;
    printf("Done with thread");

    return nullptr;
}

/*
 * Constructor for RPCServer
 * Input:
 *      serverIP: The IP of the server
 *      port: The port the server will be listening on
 */
RPCServer::RPCServer(const char* serverIP, int port) {
    int j = 0;
    m_serverIP = (char*)serverIP;
    m_port = port;
}

/*
 * Destructor
 */
RPCServer::~RPCServer() = default;

/*
 * StartServer will create a server on a port that was passed in, and create a
 * socket
 */
void RPCServer::StartServer() {
    int opt = 1;
    const int BACKLOG = 10;

    // Creating socket file descriptor
    if ((m_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port
    if (setsockopt(m_server_fd, SOL_SOCKET, SO_REUSEADDR,
        &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    m_address.sin_family = AF_INET;
    m_address.sin_addr.s_addr = INADDR_ANY;
    m_address.sin_port = htons(m_port);

    // Forcefully attaching socket to the port
    if (::bind(m_server_fd, (struct sockaddr*)&m_address,
        sizeof(m_address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(m_server_fd, BACKLOG) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}

/*
 * Will accept a new connection by listening on its address
 */
void RPCServer::ListenForClient() {
    int addrlen = sizeof(m_address);

    for (;;) {
        if ((m_socket = accept(m_server_fd, (struct sockaddr *) &m_address,
                               (socklen_t *) &addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        pthread_t thread_id;
        printf("Launching thread.\n");
        int socket = m_socket;
        pthread_create(&thread_id, nullptr, initThread, (void*)&socket);
        // TODO Probably should save thread_id into some type of array
        // TODO Figure out how to close thread when client disconnects
    }
}