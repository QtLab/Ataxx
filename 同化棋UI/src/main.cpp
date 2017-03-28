#include <QApplication>
#include "block.h"
#include "piece.h"
#include "board.h"
#include "gamewrapper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GameWrapper game;
    game.show();

    return a.exec();
}
