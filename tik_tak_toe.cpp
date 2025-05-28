#include <iostream>
#include <algorithm>y
#include <vector>
#include <ctime>
using namespace std;

char board[3][3] = {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}};

void printBoard() {
    cout << "  0 1 2\n";
    for (int i = 0; i < 3; i++) {
        cout << i << " ";
        for (int j = 0; j < 3; j++) {
            cout << board[i][j];
            if (j < 2) {
                cout << "|";
            }
        }
        cout << "\n";
        if (i < 3) {
            cout << "  -----\n";
        }
    }
}

bool isMoveValid(int row, int col) {
    return (row >= 0 && row < 3 && col >= 0 && col < 3 && board[row][col] == ' ');
}

bool checkWin(char player) {
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == player && board[i][1] == player && board[i][2] == player) {
            return true;
        }
        if (board[0][i] == player && board[1][i] == player && board[2][i] == player) {
            return true;
        }
    }
    
    if (board[0][0] == player && board[1][1] == player && board[2][2] == player) {
        return true;
    }
    if (board[0][2] == player && board[1][1] == player && board[2][0] == player) {
        return true;
    }
    return false;
}

bool isBoardFull() {
    for (int i; i < 3; i++) {
        for (int j; j < 3; j++) {
            if (board[i][j] == ' '){
                return false;
            }
        }
    }
    return true;
}

void playerMove() {
    int row, col;
    cout << "Your move(row and column, for example: 1 2): ";
    cin >> row >> col;
    while (!isMoveValid(row, col)) {
        cout << "Incorrect move! Try again: ";
        cin >> row >> col;
    }
    board[row][col] = 'X';
}

void aiMove() {
    srand(time(0));
    int row, col;
    do {
        row = rand() % 3;
        col = rand() % 3;
    } while (!isMoveValid(row, col));
    board[row][col] = 'O';
    cout << "AI's move: " << row << " " << col << "\n";
}

int main() {
    cout << "Welcome to Tic-Tac Toe game!" << endl;
    printBoard();

    while (true) {
        playerMove();
        printBoard();
        if (checkWin('X')) {
            cout << "You won" << endl;
            break;
        }
        if (isBoardFull()) {
            cout << "Draw" << endl;
            break;
        }

        aiMove();
        printBoard();
        
        if (checkWin('O')) {
            cout << "You lost" << endl;
                break;
        }
        if (isBoardFull()) {
            cout << "Draw" << endl;
            break;
        }

        return 0;
    }
}