#pragma once
#include <QMainWindow>
#include <QGridLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QVector>
#include <QPixmap>
#include <random>

class PuzzleWindow : public QMainWindow {
    Q_OBJECT
public:
    PuzzleWindow(QWidget *parent = nullptr);
private slots:
    void shuffleTiles();
    void tileClicked();
    void onDifficultyChanged(int idx);
private:
    void loadRandomImage();
    void splitImage();
    void updateTiles();
    bool isSolved();
    void showWinScreen();
    bool isSolvable();
    std::mt19937 gen;

    QWidget *central;
    QGridLayout *grid;
    QComboBox *difficultyCombo;
    QPushButton *restartBtn;
    QLabel *statusLabel;
    QWidget* boardWidget;


    QVector<QPushButton*> tiles;
    QVector<int> tileOrder;
    QPixmap currentImage;
    int gridSize;
    QString imagesPath;
};
