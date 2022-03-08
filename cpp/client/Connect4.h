#ifndef CLIENT_CONNECT4_CPP_H
#define CLIENT_CONNECT4_CPP_H

#include <string>
#include <vector>
using namespace std;

class Connect4 {
public:
    static void displayBoard(const string& board);
    static string getColumnChoice();
    static bool gameOver(int gameStatus);
    static void displayStats(vector<string>& arrayTokens);
};

#endif //CLIENT_CONNECT4_CPP_H
