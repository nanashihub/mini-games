#ifndef BATTLESHIPGAME_H
#define BATTLESHIPGAME_H

#include <QMainWindow>
#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>
#include <vector>
#include <random>

class GridCell : public QPushButton
{
    Q_OBJECT
    
public:
    enum CellState { Empty, Ship, Hit, Miss, Sunk };
    
    GridCell(int row, int col, QWidget* parent = nullptr);
    
    void setState(CellState state);
    CellState getState() const { return state; }
    
    int getRow() const { return row; }
    int getCol() const { return col; }
    
protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    
signals:
    void cellClicked(int row, int col);
    void cellRightClicked(int row, int col);
    
private:
    int row, col;
    CellState state;
};

struct Ship {
    int length;
    int row, col;
    bool horizontal;
    int hits;
    QString name;
    
    Ship(int len, QString n) : length(len), row(-1), col(-1),
         horizontal(true), hits(0), name(n) {}
};

class AIPlayer
{
public:
    AIPlayer();
    std::pair<int, int> makeMove(const std::vector<std::vector<int>>& board);
    void updateResult(int row, int col, bool hit, bool sunk);
    
private:
    enum Mode { Random, Hunt, Target };
    Mode currentMode;
    std::vector<std::pair<int, int>> targetQueue;
    std::pair<int, int> lastHit;
    std::vector<std::pair<int, int>> directions;
    int currentDirection;
    std::mt19937 rng;
    
    void addAdjacentCells(int row, int col);
    bool isValidCell(int row, int col, const std::vector<std::vector<int>>& board);
};

class BattleshipGame : public QMainWindow
{
    Q_OBJECT
    
public:
    BattleshipGame(QWidget* parent = nullptr);
    
private slots:
    void onPlayerCellClicked(int row, int col);
    void onPlayerCellRightClicked(int row, int col);
    void onEnemyCellClicked(int row, int col);
    void restartGame();
    void aiMove();
    
private:
    static const int BOARD_SIZE = 10;
    
    // UI элементы
    QWidget* centralWidget;
    QGridLayout* playerGrid;
    QGridLayout* enemyGrid;
    QLabel* statusLabel;
    QLabel* playerLabel;
    QLabel* enemyLabel;
    QPushButton* restartButton;
    QPushButton* menuButton;
    
    // Игровые поля
    std::vector<std::vector<GridCell*>> playerCells;
    std::vector<std::vector<GridCell*>> enemyCells;
    std::vector<std::vector<int>> playerBoard;  // 0-пусто, 1-корабль, 2-попадание, 3-промах
    std::vector<std::vector<int>> enemyBoard;
    
    // Корабли
    std::vector<Ship> playerShips;
    std::vector<Ship> enemyShips;
    int currentShipIndex;
    bool placementPhase;
    bool gameActive;
    bool playerTurn;
    
    // ИИ
    AIPlayer ai;
    QTimer* aiTimer;
    
    void setupUI();
    void initializeGame();
    void createShips();
    void placeEnemyShips();
    bool canPlaceShip(const std::vector<std::vector<int>>& board, int row, int col,
                      int length, bool horizontal);
    void placeShip(std::vector<std::vector<int>>& board, int row, int col,
                   int length, bool horizontal, int shipId);
    bool isGameOver();
    void checkGameEnd();
    void showGameResult(bool playerWon);
    void updateStatusLabel();
    bool allShipsPlaced();
    void rotateCurrentShip();
    void highlightShipPlacement(int row, int col);
    void clearHighlights();
    bool attackCell(std::vector<std::vector<int>>& board,
                   std::vector<std::vector<GridCell*>>& cells,
                   std::vector<Ship>& ships, int row, int col);
    void markSunkShip(std::vector<std::vector<int>>& board,
                     std::vector<std::vector<GridCell*>>& cells,
                     const Ship& ship);
};

#endif
