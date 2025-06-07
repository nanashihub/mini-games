#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QInputDialog>
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtCore/QTimer>
#include <vector>
#include <iostream>
#include <ctime>
#include <cstdlib>

const int CELL_SIZE = 40;
const int GRID_SIZE = 10;
const int FIELD_MARGIN = 20;

enum CellState { EMPTY, SHIP, HIT, MISS };
enum AIStrategy { EASY, HARD };
enum GameState { PLACING, PLAYING, GAME_OVER };

struct Ship {
    std::vector<QPoint> positions;

    bool isSunk(const std::vector<std::vector<CellState>>& field) const {
        for (const auto& pos : positions)
            if (field[pos.y()][pos.x()] != HIT)
                return false;
        return true;
    }

    bool contains(int x, int y) const {
        for (const auto& p : positions)
            if (p.x() == x && p.y() == y)
                return true;
        return false;
    }
};

struct Player {
    std::vector<std::vector<CellState>> field;
    std::vector<std::vector<bool>> revealed;
    std::vector<Ship> ships;
    int shipsLeft;

    Player() {
        field = std::vector<std::vector<CellState>>(GRID_SIZE, std::vector<CellState>(GRID_SIZE, EMPTY));
        revealed = std::vector<std::vector<bool>>(GRID_SIZE, std::vector<bool>(GRID_SIZE, false));
        shipsLeft = 0;
    }

    void countShipsLeft() {
        shipsLeft = ships.size();
    }
};

class BattleshipWidget : public QWidget {
    Q_OBJECT

public:
    BattleshipWidget(QWidget* parent = nullptr) : QWidget(parent) {
        srand(time(nullptr));
        
        bool ok;
        int aiLevel = QInputDialog::getInt(this, "Настройка ИИ",
                                         "Выберите сложность ИИ (0 - легкий, 1 - сложный):",
                                         0, 0, 1, 1, &ok);
        if (!ok) aiLevel = 0;
        
        aiMode = (aiLevel == 1) ? HARD : EASY;
        
        randomPlaceFleet(computer);
        
        gameState = PLACING;
        horizontal = true;
        shipQueue = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};
        currentShipSize = shipQueue.front();
        playerTurn = true;
        
        setFixedSize(900, 500);
        setMouseTracking(true);
        
        aiTimer = new QTimer(this);
        connect(aiTimer, &QTimer::timeout, this, &BattleshipWidget::performAITurn);
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        QPainter painter(this);
        painter.fillRect(rect(), Qt::white);
        
        drawField(painter, human, false, FIELD_MARGIN);
        drawField(painter, computer, true, 500);
        
        if (gameState == PLACING) {
            drawShipPreview(painter);
        }
        
        if (gameState == GAME_OVER) {
            drawGameResult(painter);
        }
    }

    void mousePressEvent(QMouseEvent* event) override {
        if (gameState == PLACING) {
            handlePlacingClick(event);
        } else if (gameState == PLAYING && playerTurn) {
            handleGameClick(event);
        }
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        if (gameState == PLACING) {
            mousePos = event->pos();
            update();
        }
    }

private slots:
    void performAITurn() {
        aiTurn(computer, human, aiMode);
        if (human.shipsLeft == 0) {
            gameState = GAME_OVER;
            gameResult = "Поражение!";
            gameResultColor = Qt::red;
        }
        playerTurn = true;
        update();
    }

private:
    Player human, computer;
    AIStrategy aiMode;
    GameState gameState;
    bool horizontal;
    std::vector<int> shipQueue;
    int currentShipSize;
    bool playerTurn;
    QPoint mousePos;
    QTimer* aiTimer;
    QString gameResult;
    QColor gameResultColor;

    bool canPlaceShip(const Player& player, int x, int y, int size, bool horizontal) {
        for (int i = 0; i < size; ++i) {
            int xi = x + (horizontal ? i : 0);
            int yi = y + (horizontal ? 0 : i);
            if (xi < 0 || xi >= GRID_SIZE || yi < 0 || yi >= GRID_SIZE)
                return false;
            if (player.field[yi][xi] != EMPTY)
                return false;
        }
        return true;
    }

    bool isAreaFree(const Player& player, int x, int y, int size, bool horizontal) {
        for (int i = -1; i <= size; ++i) {
            for (int j = -1; j <= 1; ++j) {
                int xi = x + (horizontal ? i : 0) + (horizontal ? 0 : j);
                int yi = y + (horizontal ? j : 0) + (horizontal ? j : i);
                if (xi >= 0 && yi >= 0 && xi < GRID_SIZE && yi < GRID_SIZE) {
                    if (player.field[yi][xi] != EMPTY)
                        return false;
                }
            }
        }
        return true;
    }

    void placeShip(Player& player, int x, int y, int size, bool horizontal) {
        Ship ship;
        for (int i = 0; i < size; ++i) {
            int xi = x + (horizontal ? i : 0);
            int yi = y + (horizontal ? 0 : i);
            player.field[yi][xi] = SHIP;
            ship.positions.push_back(QPoint(xi, yi));
        }
        player.ships.push_back(ship);
    }

    void randomPlaceFleet(Player& player) {
        auto place = [&](int count, int size) {
            for (int i = 0; i < count; ++i) {
                while (true) {
                    int x = rand() % GRID_SIZE;
                    int y = rand() % GRID_SIZE;
                    bool horizontal = rand() % 2;
                    if (canPlaceShip(player, x, y, size, horizontal) &&
                        isAreaFree(player, x, y, size, horizontal)) {
                        placeShip(player, x, y, size, horizontal);
                        break;
                    }
                }
            }
        };
        place(1, 4);
        place(2, 3);
        place(3, 2);
        place(4, 1);
        player.countShipsLeft();
    }

    void drawField(QPainter& painter, const Player& player, bool hideShips, int offsetX) {
        for (int y = 0; y < GRID_SIZE; ++y) {
            for (int x = 0; x < GRID_SIZE; ++x) {
                QRect cellRect(offsetX + x * CELL_SIZE, FIELD_MARGIN + y * CELL_SIZE,
                              CELL_SIZE - 2, CELL_SIZE - 2);
                
                QColor cellColor;
                if (player.revealed[y][x]) {
                    cellColor = (player.field[y][x] == SHIP) ? Qt::red : QColor(200, 200, 200);
                } else {
                    if (!hideShips && player.field[y][x] == SHIP)
                        cellColor = Qt::green;
                    else
                        cellColor = Qt::blue;
                }
                
                painter.fillRect(cellRect, cellColor);
                painter.setPen(Qt::black);
                painter.drawRect(cellRect);
            }
        }
    }

    void drawShipPreview(QPainter& painter) {
        int x = (mousePos.x() - FIELD_MARGIN) / CELL_SIZE;
        int y = (mousePos.y() - FIELD_MARGIN) / CELL_SIZE;
        
        if (canPlaceShip(human, x, y, currentShipSize, horizontal) &&
            isAreaFree(human, x, y, currentShipSize, horizontal)) {
            
            painter.fillRect(0, 0, 0, 0); // Dummy call to ensure painter is ready
            QColor previewColor(100, 255, 100, 150);
            
            for (int i = 0; i < currentShipSize; ++i) {
                int xi = x + (horizontal ? i : 0);
                int yi = y + (horizontal ? 0 : i);
                QRect cellRect(FIELD_MARGIN + xi * CELL_SIZE, FIELD_MARGIN + yi * CELL_SIZE,
                              CELL_SIZE - 2, CELL_SIZE - 2);
                painter.fillRect(cellRect, previewColor);
            }
        }
    }

    void drawGameResult(QPainter& painter) {
        painter.setPen(gameResultColor);
        QFont font = painter.font();
        font.setPointSize(24);
        painter.setFont(font);
        painter.drawText(QRect(300, 200, 300, 100), Qt::AlignCenter, gameResult);
    }

    bool handleClick(Player& shooter, Player& enemy, QPoint mousePos, int offsetX) {
        int x = (mousePos.x() - offsetX) / CELL_SIZE;
        int y = (mousePos.y() - FIELD_MARGIN) / CELL_SIZE;
        if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE || shooter.revealed[y][x])
            return false;

        shooter.revealed[y][x] = true;
        if (enemy.field[y][x] == SHIP) {
            enemy.field[y][x] = HIT;
            for (auto& ship : enemy.ships)
                if (ship.contains(x, y) && ship.isSunk(enemy.field))
                    enemy.shipsLeft--;
            return true;
        } else {
            enemy.field[y][x] = MISS;
            return false;
        }
    }

    void aiTurn(Player& ai, Player& human, AIStrategy mode) {
        auto valid = [&](int x, int y) {
            return x >= 0 && y >= 0 && x < GRID_SIZE && y < GRID_SIZE && !ai.revealed[y][x];
        };

        if (mode == HARD) {
            for (int y = 0; y < GRID_SIZE; ++y) {
                for (int x = 0; x < GRID_SIZE; ++x) {
                    if (human.field[y][x] == HIT) {
                        const int dx[] = {1, -1, 0, 0};
                        const int dy[] = {0, 0, 1, -1};
                        for (int d = 0; d < 4; ++d) {
                            int nx = x + dx[d], ny = y + dy[d];
                            if (valid(nx, ny)) {
                                ai.revealed[ny][nx] = true;
                                if (human.field[ny][nx] == SHIP) {
                                    human.field[ny][nx] = HIT;
                                    for (auto& ship : human.ships)
                                        if (ship.contains(nx, ny) && ship.isSunk(human.field))
                                            human.shipsLeft--;
                                    aiTurn(ai, human, mode);
                                } else {
                                    human.field[ny][nx] = MISS;
                                }
                                return;
                            }
                        }
                    }
                }
            }
        }

        while (true) {
            int x = rand() % GRID_SIZE;
            int y = rand() % GRID_SIZE;
            if (!ai.revealed[y][x]) {
                ai.revealed[y][x] = true;
                if (human.field[y][x] == SHIP) {
                    human.field[y][x] = HIT;
                    for (auto& ship : human.ships)
                        if (ship.contains(x, y) && ship.isSunk(human.field))
                            human.shipsLeft--;
                    continue;
                } else {
                    human.field[y][x] = MISS;
                    break;
                }
            }
        }
    }

    void handlePlacingClick(QMouseEvent* event) {
        int x = (event->pos().x() - FIELD_MARGIN) / CELL_SIZE;
        int y = (event->pos().y() - FIELD_MARGIN) / CELL_SIZE;

        if (event->button() == Qt::RightButton) {
            horizontal = !horizontal;
            update();
        } else if (event->button() == Qt::LeftButton) {
            if (canPlaceShip(human, x, y, currentShipSize, horizontal) &&
                isAreaFree(human, x, y, currentShipSize, horizontal)) {
                placeShip(human, x, y, currentShipSize, horizontal);
                shipQueue.erase(shipQueue.begin());
                if (!shipQueue.empty()) {
                    currentShipSize = shipQueue.front();
                } else {
                    human.countShipsLeft();
                    gameState = PLAYING;
                }
                update();
            }
        }
    }

    void handleGameClick(QMouseEvent* event) {
        if (event->button() == Qt::LeftButton) {
            if (!handleClick(human, computer, event->pos(), 500)) {
                playerTurn = false;
                aiTimer->start(1000); // Задержка 1 секунда для хода ИИ
            } else if (computer.shipsLeft == 0) {
                gameResult = "Победа!";
                gameResultColor = Qt::green;
                gameState = GAME_OVER;
            }
            update();
        }
    }
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Морской бой");
        setCentralWidget(new BattleshipWidget(this));
        setFixedSize(920, 540);
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
