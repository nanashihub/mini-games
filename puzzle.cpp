#include <QApplication>
#include "puzzlewindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    PuzzleWindow w;
    w.show();
    return app.exec();
}
