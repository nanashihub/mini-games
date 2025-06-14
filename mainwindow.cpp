#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <cstdlib>
#include <ctime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      board(3, QVector<Player>(3, Player::None)),
      buttons(3, QVector<QPushButton*>(3, nullptr)),
      currentPlayer(Player::X),
      vsAI(true),
      startingPlayer(Player::X)

{
    QWidget *central = new QWidget;
    QVBoxLayout *vbox = new QVBoxLayout;
    QHBoxLayout *topBar = new QHBoxLayout;

    modeCombo = new QComboBox;
    modeCombo->addItem("Игрок vs AI");
    modeCombo->addItem("Игрок vs Игрок");
    connect(modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onModeChanged);

    difficultyCombo = new QComboBox;
    difficultyCombo->addItem("Легко");
    difficultyCombo->addItem("Сложно");
    connect(difficultyCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onDifficultyChanged);

    aiDifficulty = difficultyCombo->currentIndex();

    restartBtn = new QPushButton("Рестарт");
    connect(restartBtn, &QPushButton::clicked, this, &MainWindow::restartGame);

    topBar->addWidget(modeCombo);
    topBar->addWidget(difficultyCombo);
    topBar->addWidget(restartBtn);

    QGridLayout *grid = new QGridLayout;
    QFont btnFont;
    btnFont.setPointSize(32);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j) {
            QPushButton *btn = new QPushButton;
            btn->setFixedSize(100, 100);
            btn->setFont(btnFont);
            grid->addWidget(btn, i, j);
            buttons[i][j] = btn;
            connect(btn, &QPushButton::clicked, [=]{ handleButton(i, j); });
        }

    statusLabel = new QLabel("Ваш ход (X)");
    statusLabel->setAlignment(Qt::AlignCenter);
    QFont statusFont;
    statusFont.setPointSize(16);
    statusLabel->setFont(statusFont);

    vbox->addLayout(topBar);
    vbox->addLayout(grid);
    vbox->addWidget(statusLabel);

    central->setLayout(vbox);
    setCentralWidget(central);
    setWindowTitle("Крестики-нолики");

    srand(time(0));
    restartGame();
}

void MainWindow::handleButton(int row, int col) {
    if (board[row][col] != Player::None) return;
    if (vsAI && currentPlayer == Player::O) return;

    board[row][col] = currentPlayer;
    buttons[row][col]->setText(currentPlayer == Player::X ? "X" : "O");

    if (checkGameOver()) return;

    if (vsAI && currentPlayer == Player::O) {
        aiMove();
    }
}

void MainWindow::aiMove() {
    if (isBoardFull() || checkWin(Player::X) || checkWin(Player::O)) return;

    if (aiDifficulty == 0)
        makeAIMoveRandom();
    else
        makeAIMoveMinimax();

    if (checkGameOver()) return;
}

bool MainWindow::checkGameOver() {
    QVector<QPair<int,int>> winLine;
    if (checkWin(Player::X, &winLine)) {
        for (auto cell : winLine)
            buttons[cell.first][cell.second]->setStyleSheet("background-color: yellow");
        showEndScreen("Вы выиграли! 🎉");
        return true;
    } else if (checkWin(Player::O, &winLine)) {
        for (auto cell : winLine)
            buttons[cell.first][cell.second]->setStyleSheet("background-color: red");
        showEndScreen("Вы проиграли 😢");
        return true;
    } else if (isBoardFull()) {
        showEndScreen("Ничья 🤝");
        return true;
    }
    currentPlayer = (currentPlayer == Player::X) ? Player::O : Player::X;
    updateStatus();
    return false;
}

bool MainWindow::checkWin(Player p, QVector<QPair<int,int>>* winLine) {
    for (int i=0; i<3; ++i) {
        if (board[i][0]==p && board[i][1]==p && board[i][2]==p) {
            if (winLine) *winLine = {{i,0},{i,1},{i,2}};
            return true;
        }
        if (board[0][i]==p && board[1][i]==p && board[2][i]==p) {
            if (winLine) *winLine = {{0,i},{1,i},{2,i}};
            return true;
        }
    }
    if (board[0][0]==p && board[1][1]==p && board[2][2]==p) {
        if (winLine) *winLine = {{0,0},{1,1},{2,2}};
        return true;
    }
    if (board[0][2]==p && board[1][1]==p && board[2][0]==p) {
        if (winLine) *winLine = {{0,2},{1,1},{2,0}};
        return true;
    }
    return false;
}

bool MainWindow::isBoardFull() {
    for (int i=0; i<3; ++i)
        for (int j=0; j<3; ++j)
            if (board[i][j] == Player::None)
                return false;
    return true;
}

void MainWindow::updateStatus() {
    QString msg;
    if (vsAI) {
        msg = (currentPlayer == Player::X) ? "Ваш ход (X)" : "Ходит AI (O)";
    } else {
        msg = (currentPlayer == Player::X) ? "Ходит игрок X" : "Ходит игрок O";
    }
    statusLabel->setText(msg);
}

void MainWindow::showEndScreen(QString message) {
    QMessageBox::information(this, "Игра окончена", message + "\nХотите сыграть ещё?");
    restartGame();
}

void MainWindow::restartGame() {
    board = QVector<QVector<Player>>(3, QVector<Player>(3, Player::None));
    for (int i=0; i<3; ++i)
        for (int j=0; j<3; ++j) {
            buttons[i][j]->setText("");
            buttons[i][j]->setStyleSheet("");
        }
    currentPlayer = startingPlayer;
    updateStatus();

    startingPlayer = (startingPlayer == Player::X) ? Player::O : Player::X;

    if (vsAI && currentPlayer == Player::O) {
        aiMove();
    }
}


void MainWindow::onModeChanged(int idx) {
    vsAI = (idx == 0);
    restartGame();
}

void MainWindow::onDifficultyChanged(int idx) {
    aiDifficulty = idx;
    restartGame();
}

void MainWindow::makeAIMoveRandom() {
    QVector<QPair<int,int>> freeCells;
    for (int i=0; i<3; ++i)
        for (int j=0; j<3; ++j)
            if (board[i][j] == Player::None)
                freeCells.append({i,j});
    if (!freeCells.isEmpty()) {
        auto move = freeCells[rand() % freeCells.size()];
        board[move.first][move.second] = Player::O;
        buttons[move.first][move.second]->setText("O");
    }
}

void MainWindow::makeAIMoveMinimax() {
    bool empty = true;
    for (int i=0; i<3 && empty; ++i)
        for (int j=0; j<3 && empty; ++j)
            if (board[i][j] != Player::None)
                empty = false;
    if (empty) {
        QVector<QPair<int,int>> bestStarts = {{0,0},{0,2},{2,0},{2,2},{1,1}};
        auto move = bestStarts[rand() % bestStarts.size()];
        board[move.first][move.second] = Player::O;
        buttons[move.first][move.second]->setText("O");
        return;
    }
    int bestRow = -1, bestCol = -1;
    minimax(Player::O, 0, bestRow, bestCol);
    if (bestRow >= 0 && bestCol >= 0) {
        board[bestRow][bestCol] = Player::O;
        buttons[bestRow][bestCol]->setText("O");
    }
}


int MainWindow::minimax(Player current, int depth, int &bestRow, int &bestCol) {
    if (checkWin(Player::O)) return 10 - depth;
    if (checkWin(Player::X)) return depth - 10;
    if (isBoardFull()) return 0;

    int bestScore = (current == Player::O) ? -1000 : 1000;
    for (int i=0; i<3; ++i)
        for (int j=0; j<3; ++j)
            if (board[i][j] == Player::None) {
                board[i][j] = current;
                int r, c;
                int score = minimax((current == Player::O) ? Player::X : Player::O, depth+1, r, c);
                board[i][j] = Player::None;
                if (current == Player::O) {
                    if (score > bestScore) {
                        bestScore = score;
                        if (depth == 0) { bestRow = i; bestCol = j; }
                    }
                } else {
                    if (score < bestScore) {
                        bestScore = score;
                        if (depth == 0) { bestRow = i; bestCol = j; }
                    }
                }
            }
    return bestScore;
}
