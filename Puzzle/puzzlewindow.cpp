#include "puzzlewindow.h"
#include <QDir>
#include <QFileInfoList>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFont>
#include <QCoreApplication>

PuzzleWindow::PuzzleWindow(QWidget *parent)
    : QMainWindow(parent), gridSize(3)
{
    central = new QWidget;
    QVBoxLayout *vbox = new QVBoxLayout;

    QHBoxLayout *topBar = new QHBoxLayout;
    difficultyCombo = new QComboBox;
    difficultyCombo->addItem("Легко (3x3)");
    difficultyCombo->addItem("Средне (4x4)");
    difficultyCombo->addItem("Сложно (5x5)");
    connect(difficultyCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PuzzleWindow::onDifficultyChanged);

    restartBtn = new QPushButton("Рестарт");
    connect(restartBtn, &QPushButton::clicked, this, &PuzzleWindow::shuffleTiles);

    topBar->addWidget(difficultyCombo);
    topBar->addWidget(restartBtn);

    statusLabel = new QLabel("Соберите картину!");
    statusLabel->setAlignment(Qt::AlignCenter);
    QFont statusFont;
    statusFont.setPointSize(14);
    statusLabel->setFont(statusFont);

    boardWidget = new QWidget;
    grid = new QGridLayout(boardWidget);
    grid->setSpacing(2); 
    grid->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *centerLayout = new QHBoxLayout;
    centerLayout->addStretch();
    centerLayout->addWidget(boardWidget);
    centerLayout->addStretch();

    vbox->addLayout(topBar);
    vbox->addWidget(statusLabel);
    vbox->addLayout(centerLayout);

    central->setLayout(vbox);
    setCentralWidget(central);
    setWindowTitle("Собери картину");

    imagesPath = QCoreApplication::applicationDirPath() + "/../images/";

    loadRandomImage();
    splitImage();
    shuffleTiles();
}



void PuzzleWindow::loadRandomImage() {
    QDir dir(imagesPath);
    QFileInfoList images = dir.entryInfoList(QStringList() << "*.jpg" << "*.png" << "*.jpeg", QDir::Files);
    if (images.isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "В папке images нет картинок!");
        exit(1);
    }
    int idx = QRandomGenerator::global()->bounded(images.size());
    currentImage.load(images[idx].absoluteFilePath());
}


void PuzzleWindow::shuffleTiles() {
    do {
        std::shuffle(tileOrder.begin(), tileOrder.end() - 1, gen);
    } while (!isSolvable() || isSolved());
    updateTiles();
    statusLabel->setText("Соберите картину!");
}

void PuzzleWindow::tileClicked() {
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    int clickedIdx = -1, emptyIdx = -1;
    for (int i = 0; i < tileOrder.size(); ++i) {
        if (tiles.value(tileOrder[i]) == btn)
            clickedIdx = i;
        if (tiles.value(tileOrder[i]) == nullptr)
            emptyIdx = i;
    }
    int rowC = clickedIdx / gridSize, colC = clickedIdx % gridSize;
    int rowE = emptyIdx / gridSize, colE = emptyIdx % gridSize;
    if ((abs(rowC - rowE) == 1 && colC == colE) || (abs(colC - colE) == 1 && rowC == rowE)) {
        std::swap(tileOrder[clickedIdx], tileOrder[emptyIdx]);
        updateTiles();
        if (isSolved()) showWinScreen();
    }
}
void PuzzleWindow::updateTiles() {
    while (QLayoutItem *item = grid->takeAt(0)) {
    if (item->widget()) item->widget()->setParent(nullptr);
    delete item;
}
    int idx = 0;
    for (int y = 0; y < gridSize; ++y) {
        for (int x = 0; x < gridSize; ++x, ++idx) {
            QPushButton *btn = tiles.value(tileOrder[idx], nullptr);
            if (btn)
                grid->addWidget(btn, y, x);
        }
    }
}

void PuzzleWindow::splitImage() {
    QLayoutItem *child;
    while ((child = grid->takeAt(0)) != nullptr) {
        if (child->widget()) child->widget()->setParent(nullptr);
        delete child;
    }
    tiles.clear();
    tileOrder.clear();


    int maxTileSize = 110;
    int tileSize = maxTileSize;

    QPixmap scaledImage = currentImage.scaled(tileSize * gridSize, tileSize * gridSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    int w = tileSize;
    int h = tileSize;
    QVector<QPixmap> pieces;
    for (int y = 0; y < gridSize; ++y)
        for (int x = 0; x < gridSize; ++x)
            pieces.append(scaledImage.copy(x * w, y * h, w, h));
    for (int i = 0; i < gridSize * gridSize - 1; ++i) {
        QPushButton *btn = new QPushButton;
        btn->setIcon(QIcon(pieces[i]));
        btn->setIconSize(QSize(w, h));
        btn->setFixedSize(w, h);
        btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        btn->setStyleSheet("border: 2px solid #444; background: #fff;");
        connect(btn, &QPushButton::clicked, this, &PuzzleWindow::tileClicked);
        tiles.append(btn);
        tileOrder.append(i);
    }
    tiles.append(nullptr);
    tileOrder.append(gridSize * gridSize - 1);

    updateTiles();
}



bool PuzzleWindow::isSolvable() {
    int inv = 0;
    for (int i = 0; i < tileOrder.size() - 1; ++i)
        for (int j = i + 1; j < tileOrder.size() - 1; ++j)
            if (tileOrder[i] > tileOrder[j]) inv++;

    if (gridSize % 2 == 1)
        return (inv % 2 == 0);
    else {
        int emptyRow = tileOrder.indexOf(gridSize*gridSize - 1) / gridSize;
        return ((inv + emptyRow) % 2 == 1);
    }
}

void PuzzleWindow::showWinScreen() {
    QMessageBox::information(this, "Победа!", "Вы собрали картину! 🎉");
    loadRandomImage();
    splitImage();
    shuffleTiles();
}

void PuzzleWindow::onDifficultyChanged(int idx) {
    if (idx == 0) gridSize = 3;
    else if (idx == 1) gridSize = 4;
    else gridSize = 5;
    splitImage();
    shuffleTiles();

    int maxTileSize = 110;
    int tileSize = maxTileSize;
    int boardSize = tileSize * gridSize;
    boardWidget->setFixedSize(boardSize, boardSize);
    setFixedSize(boardSize + 60, boardSize + 140);
}

bool PuzzleWindow::isSolved() {
    for (int i = 0; i < tileOrder.size(); ++i)
        if (tileOrder[i] != i)
            return false;
    return true;
}
