#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class MainMenu : public QMainWindow {
    Q_OBJECT
public:
    MainMenu(QWidget *parent = nullptr);

private slots:
    void startBattleship();
    void startPuzzle();
    void startTicTacToe();

private:
    QPushButton *battleshipButton;
    QPushButton *puzzleButton;
    QPushButton *ticTacToeButton;
};
