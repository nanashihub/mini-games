#include <QApplication>
#include "mainmenu.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainMenu w;
    w.show();
    return app.exec();
}
