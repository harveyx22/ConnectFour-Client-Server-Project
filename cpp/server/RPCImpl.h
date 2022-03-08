#pragma once
#include <string>
#include <vector>
#include "Connect4.h"

using namespace std;

class RPCImpl {
public:
    explicit RPCImpl(int socket);
    ~RPCImpl();
    void processRPC();
private:
    int m_socket;

    static void parseTokens(char* buffer, vector<string>& a);
    void sendResponse(char* buffer) const;

    bool processConnectRPC(vector<string>& arrayTokens) const;
    Connect4* playConnect4RPC(vector<string>& arrayTokens);
    void playPieceRPC(Connect4* game, vector<string>& arrayTokens) const;
    void checkStatsRPC() const;
    void processDisconnectRPC() const;

};