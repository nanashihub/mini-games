#include "mainmenu.h"
#include <QProcess>
#include <QApplication>
#include <QWidget>
#include <QFont>

MainMenu::MainMenu(QWidget *parent) : QMainWindow(parent) {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    QLabel *title = new QLabel("МИНИ ИГРЫ", this);
    QFont font = title->font();
    font.setPointSize(24);
    font.setBold(true);
    title->setFont(font);
    title->setAlignment(Qt::AlignCenter);

    battleshipButton = new QPushButton("Морской бой", this);
    puzzleButton = new QPushButton("Пазлы", this);
    ticTacToeButton = new QPushButton("Крестики-нолики", this);

    layout->addWidget(title);
    layout->addSpacing(30);
    layout->addWidget(battleshipButton);
    layout->addWidget(puzzleButton);
    layout->addWidget(ticTacToeButton);

    setCentralWidget(centralWidget);
    setWindowTitle("Мини игры");
    resize(400, 300);

    connect(battleshipButton, &QPushButton::clicked, this, &MainMenu::startBattleship);
    connect(puzzleButton, &QPushButton::clicked, this, &MainMenu::startPuzzle);
    connect(ticTacToeButton, &QPushButton::clicked, this, &MainMenu::startTicTacToe);
}

void MainMenu::startBattleship() {
    QProcess::startDetached(QCoreApplication::applicationDirPath() + "/battleshipgame");
    this->close();
}

void MainMenu::startPuzzle() {
    QProcess::startDetached(QCoreApplication::applicationDirPath() + "/PuzzleGame");
    this->close();
}

void MainMenu::startTicTacToe() {
    QProcess::startDetached(QCoreApplication::applicationDirPath() + "/MyQtApp");
    this->close();
}