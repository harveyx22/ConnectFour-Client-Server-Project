#include <iostream>
#include "Connect4.h"

using namespace std;

/**
 * Prints the board.
 * @param board: String of board to print.
 */
void Connect4::displayBoard(const string &board) {
    int rows = 6, columns = 7;

    cout << endl;
    cout << "Connect Four Game Board" << endl;

    for (int i = rows - 1; i >= 0; i--) {
        cout << "\t|";
        cout << board.substr((i * columns), columns);
        cout << "|" << endl;
    }
    cout << endl;
}

/**
 * Gets column choice from client.
 * @return String of column choice.
 */
string Connect4::getColumnChoice() {
    int columnChoice;
    do {
        cout << "Please enter which column you'd like to play your piece (must "
                "be between 1 and 7): ";
        cin >> columnChoice;
    } while (columnChoice < 1 || columnChoice > 7);

    return to_string(columnChoice);
}

/**
 * Checks for game over scenario.
 * @param gameStatus: Determines game over scenario.
 * @return True to play again, false otherwise.
 */
bool Connect4::gameOver(int gameStatus) {
    switch (gameStatus) {
        case 9:
            printf("You win!\n");
            break;
        case 10:
            printf("The computer won!\n");
            break;
        case 11:
            printf("The game board is full!\n");
            break;
        default:
            printf("There was an error in the game.\n");
            break;
    }

    string input;
    printf("Enter \"y\" to play again. Enter anything else to exit. ");
    cin >> input;
    if (input == "y" || input == "Y")
        return true;
    else return false;
}
