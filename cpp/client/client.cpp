#include <cstdio>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <iterator>
#include <iostream>
#include <random>
#include "Connect4.h"

using namespace std;

bool ConnectToServer(const char *serverAddress, int port, int &sock);
void sendRPC(const string &RPC, const int &sock, vector<string> &arrayTokens);
void ParseTokens(char *buffer, vector<string> &a);

/**
 * TODO
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char const *argv[]) {
    // Takes first command line argument: IP address of the server
    const char *serverAddress = argv[1];
    // Takes second command line argument: port the server is listening on
    const int port = atoi(argv[2]);

    int sock = 0;
    vector<string> arrayTokens;

    bool bConnect = ConnectToServer(serverAddress, port, sock);

    bool validLogin = false;
    while (bConnect && !validLogin) {
        string username,
                password,
                connectRPC;

        // Collect username and password
        cout << "\nEnter your username: ";
        cin >> username;
        cout << "Enter your password: ";
        cin >> password;

        // Create string to send to server. Ex.: connect;USERNAME;PASSWORD1234;"
        connectRPC.append("connect;").append(username).append(";");
        connectRPC.append(password).append(";");

        sendRPC(connectRPC, sock, arrayTokens);

        // Authenticate login
        if (stoi(arrayTokens[0]) == 1) {
            validLogin = true;
            printf("Login successful.\n\nWelcome to Connect Four!\n");
        } else
            printf("Invalid username and/or password.\n");
    }

    // PlayConnect4RPC Section
    bool continuePlaying = true;
    while (continuePlaying && bConnect) {
        auto *game = new Connect4();

        int turnChoice;
        do {
            cout << "\nEnter 1 to take the first turn, or enter 2 for the "
                    "computer to take the first turn: ";
            cin >> turnChoice;
        } while (turnChoice != 1 && turnChoice != 2);

        // Create string to send to server. Ex.: playconnectfour;"
        string playConnect4RPC;
        playConnect4RPC.append("playconnect4;").append(to_string(turnChoice));

        sendRPC(playConnect4RPC, sock, arrayTokens);

        // PlayPieceRPC Section
        // gameStatus will be what is returned from the server after the RPC call.
        // 1-7: The computer's column choice returned from the RPC call
        // 8: Player has selected a column that is full
        // 9: Player has won
        // 10: Computer has won
        // 11: Board is full
        int gameStatus;
        do {
            Connect4::displayBoard(arrayTokens[0]);
            string columnChoice = Connect4::getColumnChoice();

            // Create string to send to server.
            string playPieceRPC;
            playPieceRPC.append("playpiece;").append(columnChoice).append(";");

            sendRPC(playPieceRPC, sock, arrayTokens);

            gameStatus = stoi(arrayTokens[1]);

            if (gameStatus == 8)
                printf("You selected a column that is full. Please try again");
            else if (gameStatus >= 1 && gameStatus <= 7)
                cout << "The computer has selected column " << gameStatus << ".\n";

        } while (gameStatus >= 1 && gameStatus <= 8);

        Connect4::displayBoard(arrayTokens[0]);

        continuePlaying = Connect4::gameOver(gameStatus);
    }

    // checkStatsRPC section
    // currently it displays the total games from all clients
    // still need work to display the total games by that particular client
    int gamesPlayed;
    string checkStatsRPC;
    checkStatsRPC.append("checkstats;");
    sendRPC(checkStatsRPC, sock, arrayTokens);
    gamesPlayed = stoi(arrayTokens[0]);
    cout << "Total number games played: " << gamesPlayed << endl;

    // Do a disconnect Message
    if (bConnect) {

        string exit;
        while (exit != "EXIT") {
            cout << "Type 'EXIT' to disconnect" << endl;
            cin >> exit;
        }

        const char *disconnectRPC = "disconnect;";
        sendRPC(disconnectRPC, sock, arrayTokens);

    } else {
        printf("Exit without calling RPC");
    }

    // Terminate connection
    close(sock);

    return 0;
}

/**
 * ConnectToServer will connect to the Server based on command line.
 * @param serverAddress: The IP address of the server.
 * @param port: The port the server is listening on.
 * @param sock (passed by reference): A variable that will be assigned the socket.
 * @return Returns true if a connection to the server is successful, false otherwise.
*/
bool ConnectToServer(const char *serverAddress, int port, int &sock) {
    struct sockaddr_in serv_addr{};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return false;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, serverAddress, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return false;
    }

    // If connection failed, it might be the server is not running
    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed! Check if the server is running. \n");
        return false;
    }
    return true;
}

/**
 * Sends RPC to server.
 * @param RPC: string RPC.
 * @param sock: socket to send message on.
 * @param arrayTokens (passed by reference): token array to write buffer to.
 */
void sendRPC(const string &RPC, const int &sock, vector<string> &arrayTokens) {
    // Array of characters created as buffer, which will be passed to server
    char buffer[1024] = {0};
    int nlen;

    // Copies the characters from connectRPC to the buffer array
    strcpy(buffer, RPC.c_str());
    nlen = (int) strlen(buffer);
    // Puts the null terminator at the end of the connectRPC characters
    buffer[nlen] = 0;

    // Sends the contents of the buffer through the created socket
    send(sock, buffer, strlen(buffer) + 1, 0);

    // Assigns the server response to the buffer
    read(sock, buffer, 1024);

    arrayTokens.clear();
    ParseTokens(buffer, arrayTokens);
}

/**
 * ParseTokens splits a string by semicolons and adds all strings to the input vector.
 * @param buffer: The string to be split.
 * @param a: The vector to have strings added to.
 */
void ParseTokens(char *buffer, vector<string> &a) {
    char *token;
    char *rest = (char *) buffer;

    while ((token = strtok_r(rest, ";", &rest))) {
        a.emplace_back(token);
    }
}