#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QMessageBox>
#include <QVector>

enum class Player { None, X, O };

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
private slots:
    void handleButton(int row, int col);
    void restartGame();
    void updateStatus();
    void showEndScreen(QString message);
    void aiMove();
    void onModeChanged(int);
    void onDifficultyChanged(int);

private:
    bool checkGameOver();
    bool isBoardFull();
    bool checkWin(Player p, QVector<QPair<int,int>>* winLine = nullptr);
    void makeAIMoveRandom();
    void makeAIMoveMinimax();
    int minimax(Player current, int depth, int &bestRow, int &bestCol);
    Player startingPlayer;
    QVector<QVector<Player>> board;
    QVector<QVector<QPushButton*>> buttons;
    QLabel *statusLabel;
    QComboBox *modeCombo;
    QComboBox *difficultyCombo;
    QPushButton *restartBtn;
    Player currentPlayer;
    bool vsAI;
    int aiDifficulty; // 0 - Easy, 1 - Hard
};
