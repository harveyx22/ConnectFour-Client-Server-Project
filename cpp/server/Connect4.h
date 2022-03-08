/**
 * Connect4 contains game logic.
 */

#ifndef SERVER_CONNECT4_H
#define SERVER_CONNECT4_H

using namespace std;

class Connect4 {
public:
    int gamesPlayed;
    int wins;

    Connect4();
    bool clientDrop(int dropChoice);
    int computerDrop();
    bool fullBoard();
    bool checkFour(bool client);
    string boardToString();

private:
    char board[6][7];

    void restart();
};

#endif //SERVER_CONNECT4_H
