/**
 * This class implements the various RPC calls.
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
//#include <bits/stdc++.h>
#include <cmath>
#include "RPCImpl.h"
#include "Connect4.h"
using namespace std;

typedef struct GlobalContext {
    double highestScore = 0;
} GlobalContext;

GlobalContext globalObj;

//typedef struct LocalContext{
//    double wins;
//    double gamesPlayed;
//} LocalContext;
//
//LocalContext localObj;

extern pthread_mutex_t myMutex;

/**
 * Constructor,
 * @param socket: To connect on.
 */
RPCImpl::RPCImpl(int socket) {
    m_socket = socket;

    this->game = new Connect4();
}

/**
 * Default destructor.
 */
RPCImpl::~RPCImpl() = default;

 /**
  * ParseTokens splits a string by semicolons and adds all strings to the input
  * vector.
  * @param buffer: The string to be split.
  * @param a: The vector to have strings added to.
  */
void RPCImpl::parseTokens(char* buffer, vector<string>& a) {
    char* token;
    char* rest = (char*)buffer;

    while ((token = strtok_r(rest, ";", &rest))) {
        a.emplace_back(token);
    }
}

/**
 * Sends response to client.
 * @param buffer: String response to send.
 */
void RPCImpl::sendResponse(char* buffer) const {
    int nlen = (int) strlen(buffer);
    buffer[nlen] = 0;
    send(this->m_socket, buffer, (int) strlen(buffer) + 1, 0);
}

/**
 * ProcessRPC will examine buffer and will essentially control the server
 * all processes.
 */
void RPCImpl::processRPC() {
    char buffer[1024] = { 0 };  // Holds messages.
    vector<string> arrayTokens; // Array of message tokens
    const int RPC_TOKEN = 0;    // RPC token is always first.
    int valread;
    bool bConnected = false;
    bool bContinue = true;
    bool playingGame = false;


    while (bContinue) {
        // Should be blocked when a new RPC has not called us yet
        if ((valread = (int) read(this->m_socket, buffer, sizeof(buffer))) <= 0) {
            break;
        }

        arrayTokens.clear();
        this->parseTokens(buffer, arrayTokens);

        // Enumerate through the tokens. The first token is always the RPC
        for (const auto& x : arrayTokens) {
            char* token = (char*)malloc(x.size() + 1);
            memcpy(token, x.c_str(), x.size() + 1);
            printf("Token received = %s\n", token);
            free(token);
        }

        string strRPC = arrayTokens[RPC_TOKEN];

        // Check for RPC.
        if (!bConnected && (strRPC == "connect")) {
            bConnected = processConnectRPC(arrayTokens);
        } else if (bConnected && strRPC == "playconnect4") {
            //game =
                    playConnect4RPC(arrayTokens);
            playingGame = true;
        } else if (bConnected && playingGame && strRPC == "playpiece") {
            playPieceRPC(game, arrayTokens);
        } else if (bConnected && strRPC == "checkstats") {
            checkStatsRPC(game);
        } else if (bConnected && (strRPC == "disconnect")) {
            processDisconnectRPC();
            //cout << "Total games played = " << totalGamesPlayed << endl;
            printf("Disconnected from client.\n\n");
            bContinue = false; // We are going to leave this loop
        } else {
            printf("Error: not an RPC.\n");
        }
        printf("Waiting.\n");
    }
}

/**
 * Processes the "connect" RPC.
 * @param arrayTokens: tokens received from client (Ex: "connect," "USERNAME,"
 * and "PASSWORD1234."
 * @return True if login is valid.
 */
bool RPCImpl::processConnectRPC(vector<string>& arrayTokens) const {
    const int USERNAME_TOKEN = 1;
    const int PASSWORD_TOKEN = 2;
    bool validLogin;

    // Strip out tokens 1 and 2 (username, password)
    string usernameString = arrayTokens[USERNAME_TOKEN];
    string passwordString = arrayTokens[PASSWORD_TOKEN];
    char szBuffer[80];

    // Our authentication logic.
    if ((usernameString == "USERNAME") && (passwordString == "PASSWORD1234")) {
        strcpy(szBuffer, "1;"); // Connected
        validLogin = true;
        printf("Successful login.\n\n");
    } else {
        strcpy(szBuffer, "0;"); // Not connected
        validLogin = false;
        printf("Failed login.\n\n");
    }

    // Send Response back on our socket
    int nlen = (int) strlen(szBuffer);
    szBuffer[nlen] = 0;
    send(this->m_socket, szBuffer, (int) strlen(szBuffer) + 1, 0);

    if (validLogin) return true;
    else return false;
}

/**
 * Processes playConnect4RPC.
 * @param arrayTokens: Tokens received from client (Ex: "1" or "2" for who,
 * client or computer, should take first turn).
 * @return New Connect4 game object.
 */
void RPCImpl::playConnect4RPC(vector<string>& arrayTokens)  {
    // Get first turn token
    const int TURN_TOKEN = 1;
    int firstTurn = stoi(arrayTokens[TURN_TOKEN]);

//    auto* game = new Connect4();    // Initialize new game.
    game->restart();
    // Check if computer goes first.
    if (firstTurn == 2)
        game->computerDrop();

    // Convert board array to string to send back on socket.
    string strBoard = game->boardToString().append(";");

    char szBuffer[50];
    strcpy(szBuffer, strBoard.c_str());

    // Send response back on our socket.
    sendResponse(szBuffer);

    //pthread_mutex_lock(&myMutex);
    (game->localGamesPlayed)++;
    //pthread_mutex_unlock(&myMutex);

    //return game;
}

/**
 * Processes playPieceRPC.
 * @param game: Game object to use.
 * @param arrayTokens: Tokens received from client (Ex. column choice (1-7)).
 */
void RPCImpl::playPieceRPC(Connect4* game, vector<string>& arrayTokens) const {
    // Get column choice token
    const int COLUMN_TOKEN = 1;
    int columnChoice = stoi(arrayTokens[COLUMN_TOKEN]);

    int response;

    // If column is full.
    if (!game->clientDrop(columnChoice-1)) {
        response = 8;
    }

    else {
        // Check if client wins.
        if (game->checkFour(true)){
            // If client wins
            response = 9;
            //pthread_mutex_lock(&myMutex);
            game->localWins++;
            //pthread_mutex_unlock(&myMutex);
        }
        else if (game->fullBoard())
            // If board is full.
            response = 11;
        else {
            // Computer moves. Save move to response.
            response = game->computerDrop();
            // Check if computer wins.
            if (game->checkFour(false)){
                response = 10;
            }
        }
    }

    // Convert board array to string to send back on socket.
    string strBoard = game->boardToString().append(";");

    // Append response to board.
    strBoard.append(to_string(response)).append(";");

    char szBuffer[50];
    strcpy(szBuffer, strBoard.c_str());

    // Send response back on our socket
    sendResponse(szBuffer);
}

/**
 * Processes checkStatsRPC.
 */
void RPCImpl::checkStatsRPC(Connect4* game) const {
    // Lock code while in use.
    pthread_mutex_lock(&myMutex);
    if ((game->localWins/game->localGamesPlayed) > (globalObj.highestScore))
        globalObj.highestScore = (game->localWins/game->localGamesPlayed) * 100;

    // Unlock.
    pthread_mutex_unlock(&myMutex);

    //string totalGames = to_string(totalGamesPlayed).append(";");
    string stats = to_string(game->localGamesPlayed).append(";");
    stats.append(to_string(game->localWins)).append(";");
    double winRate = round( (double)(globalObj.highestScore));
    stats.append(to_string((winRate)).append(";"));

    char szBuffer[50];
    strcpy(szBuffer, stats.c_str());

    // Send response back on our socket
    sendResponse(szBuffer);



    // Reset stats
    //game->localGamesPlayed = 0;
    //game->wins = 0;

}

/**
 * Processes disconnectRPC.
 */
void RPCImpl::processDisconnectRPC() const {
    char szBuffer[16];
    strcpy(szBuffer, "1;");

    // Send response back on our socket
    sendResponse(szBuffer);
}