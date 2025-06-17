#include "battleshipgame.h"
#include <QApplication>
#include <algorithm>


GridCell::GridCell(int row, int col, QWidget* parent)
    : QPushButton(parent), row(row), col(col), state(Empty)
{
    setFixedSize(30, 30);
    setStyleSheet("QPushButton { border: 1px solid black; background-color: lightblue; }");
}

void GridCell::setState(CellState newState)
{
    state = newState;
    update();
}

void GridCell::paintEvent(QPaintEvent* event)
{
    QPushButton::paintEvent(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    switch (state) {
        case Ship:
            painter.fillRect(rect(), QColor(100, 100, 100));
            break;
        case Hit:
            painter.fillRect(rect(), QColor(255, 0, 0));
            painter.setPen(QPen(Qt::white, 2));
            painter.drawLine(5, 5, 25, 25);
            painter.drawLine(25, 5, 5, 25);
            break;
        case Miss:
            painter.fillRect(rect(), QColor(0, 0, 255, 100));
            painter.setPen(QPen(Qt::blue, 3));
            painter.drawEllipse(10, 10, 10, 10);
            break;
        case Sunk:
            painter.fillRect(rect(), QColor(139, 0, 0));
            break;
        default:
            break;
    }
}

void GridCell::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit cellClicked(row, col);
    } else if (event->button() == Qt::RightButton) {
        emit cellRightClicked(row, col);
    }
    QPushButton::mousePressEvent(event);
}

AIPlayer::AIPlayer() : currentMode(Random), currentDirection(0), rng(std::random_device{}())
{
    directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
}

std::pair<int, int> AIPlayer::makeMove(const std::vector<std::vector<int>>& board)
{
    while (!targetQueue.empty()) {
        auto move = targetQueue.back();
        targetQueue.pop_back();
        if (isValidCell(move.first, move.second, board)) {
            return move;
        }
    }
    
    if (currentMode == Target && lastHit.first != -1) {
        for (auto& dir : directions) {
            int newRow = lastHit.first + dir.first;
            int newCol = lastHit.second + dir.second;
            if (isValidCell(newRow, newCol, board)) {
                return {newRow, newCol};
            }
        }

        currentMode = Random;
    }
    
    std::vector<std::pair<int, int>> availableMoves;
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            if (board[i][j] == 0) {
                availableMoves.push_back({i, j});
            }
        }
    }
    
    if (availableMoves.empty()) {
        return {-1, -1};
    }
    
    std::vector<std::pair<int, int>> priorityMoves;
    for (auto& move : availableMoves) {
        if ((move.first + move.second) % 2 == 0) {
            priorityMoves.push_back(move);
        }
    }
    
    auto& movesToChoose = priorityMoves.empty() ? availableMoves : priorityMoves;
    std::uniform_int_distribution<> dis(0, movesToChoose.size() - 1);
    return movesToChoose[dis(rng)];
}


bool AIPlayer::isValidCell(int row, int col, const std::vector<std::vector<int>>& board)
{
    return row >= 0 && row < 10 && col >= 0 && col < 10 &&
           board[row][col] != 2 && board[row][col] != 3;
}


void AIPlayer::updateResult(int row, int col, bool hit, bool sunk)
{
    if (hit && !sunk) {
        currentMode = Target;
        lastHit = {row, col};
        
        for (auto& dir : directions) {
            int newRow = row + dir.first;
            int newCol = col + dir.second;
            if (newRow >= 0 && newRow < 10 && newCol >= 0 && newCol < 10) {
                targetQueue.push_back({newRow, newCol});
            }
        }
    } else if (sunk) {
        currentMode = Random;
        targetQueue.clear();
        lastHit = {-1, -1};
    }
}


void AIPlayer::addAdjacentCells(int row, int col)
{
    for (auto& dir : directions) {
        int newRow = row + dir.first;
        int newCol = col + dir.second;
        if (newRow >= 0 && newRow < 10 && newCol >= 0 && newCol < 10) {
            targetQueue.push_back({newRow, newCol});
        }
    }
}


BattleshipGame::BattleshipGame(QWidget* parent)
    : QMainWindow(parent), currentShipIndex(0), placementPhase(true),
      gameActive(false), playerTurn(true)
{
    setupUI();
    initializeGame();
    
    aiTimer = new QTimer(this);
    aiTimer->setSingleShot(true);
    connect(aiTimer, &QTimer::timeout, this, &BattleshipGame::aiMove);
}

void BattleshipGame::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    auto* mainLayout = new QVBoxLayout(centralWidget);
    
    statusLabel = new QLabel("Расставьте корабли. ПКМ - поворот корабля", this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("QLabel { font-size: 14px; font-weight: bold; }");
    mainLayout->addWidget(statusLabel);
    
   
    auto* boardsLayout = new QHBoxLayout();
    
 
    auto* playerSection = new QVBoxLayout();
    playerLabel = new QLabel("Ваше поле", this);
    playerLabel->setAlignment(Qt::AlignCenter);
    playerSection->addWidget(playerLabel);
    
    auto* playerWidget = new QWidget();
    playerGrid = new QGridLayout(playerWidget);
    playerGrid->setSpacing(1);
    playerSection->addWidget(playerWidget);
    
    
    auto* enemySection = new QVBoxLayout();
    enemyLabel = new QLabel("Поле противника", this);
    enemyLabel->setAlignment(Qt::AlignCenter);
    enemySection->addWidget(enemyLabel);
    
    auto* enemyWidget = new QWidget();
    enemyGrid = new QGridLayout(enemyWidget);
    enemyGrid->setSpacing(1);
    enemySection->addWidget(enemyWidget);
    
    boardsLayout->addLayout(playerSection);
    boardsLayout->addSpacing(50);
    boardsLayout->addLayout(enemySection);
    
    mainLayout->addLayout(boardsLayout);
    
    
    restartButton = new QPushButton("Новая игра", this);
    restartButton->setStyleSheet("QPushButton { font-size: 12px; padding: 10px; }");
    connect(restartButton, &QPushButton::clicked, this, &BattleshipGame::restartGame);
    mainLayout->addWidget(restartButton);
    
    
    playerCells.resize(BOARD_SIZE, std::vector<GridCell*>(BOARD_SIZE));
    enemyCells.resize(BOARD_SIZE, std::vector<GridCell*>(BOARD_SIZE));
    
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            
            playerCells[i][j] = new GridCell(i, j, this);
            connect(playerCells[i][j], &GridCell::cellClicked,
                    this, &BattleshipGame::onPlayerCellClicked);
            connect(playerCells[i][j], &GridCell::cellRightClicked,
                    this, &BattleshipGame::onPlayerCellRightClicked);
            playerGrid->addWidget(playerCells[i][j], i, j);
            
            
            enemyCells[i][j] = new GridCell(i, j, this);
            connect(enemyCells[i][j], &GridCell::cellClicked,
                    this, &BattleshipGame::onEnemyCellClicked);
            enemyGrid->addWidget(enemyCells[i][j], i, j);
        }
    }
    
    setWindowTitle("Морской бой");
    resize(800, 600);
}

void BattleshipGame::initializeGame()
{
    playerBoard.assign(BOARD_SIZE, std::vector<int>(BOARD_SIZE, 0));
    enemyBoard.assign(BOARD_SIZE, std::vector<int>(BOARD_SIZE, 0));
    
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            playerCells[i][j]->setState(GridCell::Empty);
            enemyCells[i][j]->setState(GridCell::Empty);
        }
    }
    
    createShips();
    placeEnemyShips();
    
    currentShipIndex = 0;
    placementPhase = true;
    gameActive = false;
    playerTurn = true;
    
    updateStatusLabel();
}

void BattleshipGame::createShips()
{
    playerShips.clear();
    enemyShips.clear();
    
    std::vector<std::pair<int, QString>> shipTypes = {
        {5, "Авианосец"},
        {4, "Линкор"},
        {3, "Крейсер"},
        {3, "Подводная лодка"},
        {2, "Эсминец"}
    };
    
    for (auto& shipType : shipTypes) {
        playerShips.emplace_back(shipType.first, shipType.second);
        enemyShips.emplace_back(shipType.first, shipType.second);
    }
}

void BattleshipGame::placeEnemyShips()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    
    for (int shipIdx = 0; shipIdx < enemyShips.size(); ++shipIdx) {
        Ship& ship = enemyShips[shipIdx];
        bool placed = false;
        int attempts = 0;
        
        while (!placed && attempts < 1000) {
            std::uniform_int_distribution<> rowDis(0, BOARD_SIZE - 1);
            std::uniform_int_distribution<> colDis(0, BOARD_SIZE - 1);
            std::uniform_int_distribution<> orientDis(0, 1);
            
            int row = rowDis(gen);
            int col = colDis(gen);
            bool horizontal = orientDis(gen);
            
            if (canPlaceShip(enemyBoard, row, col, ship.length, horizontal)) {
                placeShip(enemyBoard, row, col, ship.length, horizontal, shipIdx + 1);
                ship.row = row;
                ship.col = col;
                ship.horizontal = horizontal;
                placed = true;
            }
            attempts++;
        }
    }
}

bool BattleshipGame::canPlaceShip(const std::vector<std::vector<int>>& board,
                                  int row, int col, int length, bool horizontal)
{
    if (horizontal) {
        if (col + length > BOARD_SIZE) return false;
    } else {
        if (row + length > BOARD_SIZE) return false;
    }
    
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            for (int k = 0; k < length; ++k) {
                int checkRow = row + (horizontal ? i : i + k);
                int checkCol = col + (horizontal ? j + k : j);
                
                if (checkRow >= 0 && checkRow < BOARD_SIZE &&
                    checkCol >= 0 && checkCol < BOARD_SIZE) {
                    if (board[checkRow][checkCol] != 0) {
                        return false;
                    }
                }
            }
        }
    }
    
    return true;
}

void BattleshipGame::placeShip(std::vector<std::vector<int>>& board, int row, int col,
                               int length, bool horizontal, int shipId)
{
    for (int i = 0; i < length; ++i) {
        if (horizontal) {
            board[row][col + i] = shipId;
        } else {
            board[row + i][col] = shipId;
        }
    }
}

void BattleshipGame::onPlayerCellClicked(int row, int col)
{
    if (!placementPhase) return;
    
    if (currentShipIndex >= playerShips.size()) return;
    
    Ship& ship = playerShips[currentShipIndex];
    
    if (canPlaceShip(playerBoard, row, col, ship.length, ship.horizontal)) {
        placeShip(playerBoard, row, col, ship.length, ship.horizontal, currentShipIndex + 1);
        ship.row = row;
        ship.col = col;
        
        for (int i = 0; i < ship.length; ++i) {
            int shipRow = ship.horizontal ? row : row + i;
            int shipCol = ship.horizontal ? col + i : col;
            playerCells[shipRow][shipCol]->setState(GridCell::Ship);
        }
        
        currentShipIndex++;
        
        if (allShipsPlaced()) {
            placementPhase = false;
            gameActive = true;
            updateStatusLabel();
        } else {
            updateStatusLabel();
        }
    }
}

void BattleshipGame::onPlayerCellRightClicked(int row, int col)
{
    if (placementPhase && currentShipIndex < playerShips.size()) {
        rotateCurrentShip();
    }
}

void BattleshipGame::rotateCurrentShip()
{
    if (currentShipIndex < playerShips.size()) {
        playerShips[currentShipIndex].horizontal = !playerShips[currentShipIndex].horizontal;
        updateStatusLabel();
    }
}

bool BattleshipGame::allShipsPlaced()
{
    return currentShipIndex >= playerShips.size();
}

void BattleshipGame::onEnemyCellClicked(int row, int col)
{
    if (!gameActive || !playerTurn) {
        return;
    }

    GridCell::CellState currentState = enemyCells[row][col]->getState();
    if (currentState != GridCell::Empty) {
        return;
    }
    
    bool hit = attackCell(enemyBoard, enemyCells, enemyShips, row, col);
    
    if (hit) {
        updateStatusLabel();
        if (isGameOver()) {
            showGameResult(true);
            return;
        }
    } else {
        playerTurn = false;
        updateStatusLabel();
        aiTimer->start(1000);
    }
}


void BattleshipGame::aiMove()
{
    if (!gameActive || playerTurn) return;
    
    auto move = ai.makeMove(playerBoard);
    
    if (move.first == -1) {
        if (isGameOver()) {
            showGameResult(true);
        }
        return;
    }
    
    int row = move.first;
    int col = move.second;
    
    if (playerBoard[row][col] == 2 || playerBoard[row][col] == 3) {
        aiTimer->start(100);
        return;
    }
    
    bool hit = attackCell(playerBoard, playerCells, playerShips, row, col);
    bool sunk = false;
    
    if (hit && playerBoard[row][col] == 2) {
        for (auto& ship : playerShips) {
            if (ship.hits >= ship.length && ship.hits > 0) {
                bool isThisShip = false;
                for (int i = 0; i < ship.length; ++i) {
                    int shipRow = ship.horizontal ? ship.row : ship.row + i;
                    int shipCol = ship.horizontal ? ship.col + i : ship.col;
                    if (shipRow == row && shipCol == col) {
                        isThisShip = true;
                        break;
                    }
                }
                if (isThisShip) {
                    sunk = true;
                    break;
                }
            }
        }
    }
    
    ai.updateResult(row, col, hit, sunk);
    
    if (hit) {
        updateStatusLabel();
        if (isGameOver()) {
            showGameResult(false);
            return;
        }
        aiTimer->start(1000);
    } else {
        playerTurn = true;
        updateStatusLabel();
    }
}



bool BattleshipGame::attackCell(std::vector<std::vector<int>>& board,
                               std::vector<std::vector<GridCell*>>& cells,
                               std::vector<Ship>& ships, int row, int col)
{
    if (board[row][col] == 2 || board[row][col] == 3) {
        return false;
    }

    if (board[row][col] > 0) {
        int shipId = board[row][col];
        board[row][col] = 2;
        cells[row][col]->setState(GridCell::Hit);
        
        if (shipId >= 1 && shipId <= static_cast<int>(ships.size())) {
            Ship& hitShip = ships[shipId - 1];
            hitShip.hits++;
            
            if (hitShip.hits >= hitShip.length) {
                markSunkShip(board, cells, hitShip);
                return true;
            }
        }
        return true;
    } else {
        board[row][col] = 3;
        cells[row][col]->setState(GridCell::Miss);
        return false;
    }
}


void BattleshipGame::markSunkShip(std::vector<std::vector<int>>& board,
                                 std::vector<std::vector<GridCell*>>& cells,
                                 const Ship& ship)
{
    for (int i = 0; i < ship.length; ++i) {
        int shipRow = ship.horizontal ? ship.row : ship.row + i;
        int shipCol = ship.horizontal ? ship.col + i : ship.col;
        
        if (shipRow >= 0 && shipRow < BOARD_SIZE &&
            shipCol >= 0 && shipCol < BOARD_SIZE) {
            cells[shipRow][shipCol]->setState(GridCell::Sunk);
        }
    }
}


bool BattleshipGame::isGameOver()
{
    bool playerDefeated = true;
    for (auto& ship : playerShips) {
        if (ship.hits < ship.length) {
            playerDefeated = false;
            break;
        }
    }
    
    bool enemyDefeated = true;
    for (auto& ship : enemyShips) {
        if (ship.hits < ship.length) {
            enemyDefeated = false;
            break;
        }
    }
    
    return playerDefeated || enemyDefeated;
}

void BattleshipGame::showGameResult(bool playerWon)
{
    gameActive = false;
    
    QMessageBox msgBox;
    msgBox.setWindowTitle("Игра окончена");
    
    if (playerWon) {
        msgBox.setText("Поздравляем! Вы победили!");
        msgBox.setIcon(QMessageBox::Information);
    } else {
        msgBox.setText("Вы проиграли. Попробуйте еще раз!");
        msgBox.setIcon(QMessageBox::Warning);
    }
    
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void BattleshipGame::updateStatusLabel()
{
    if (placementPhase) {
        if (currentShipIndex < playerShips.size()) {
            Ship& ship = playerShips[currentShipIndex];
            QString orientation = ship.horizontal ? "горизонтально" : "вертикально";
            statusLabel->setText(QString("Разместите %1 (длина: %2, %3). ПКМ - поворот")
                               .arg(ship.name).arg(ship.length).arg(orientation));
        }
    } else if (gameActive) {
        if (playerTurn) {
            statusLabel->setText("Ваш ход - выберите клетку на поле противника");
        } else {
            statusLabel->setText("Ход противника...");
        }
    }
}

void BattleshipGame::restartGame()
{
    initializeGame();
}

#include "battleshipgame.moc"
