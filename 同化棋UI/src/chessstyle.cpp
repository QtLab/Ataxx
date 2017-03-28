#include "chessstyle.h"
#include <QPen>
#include <QGraphicsDropShadowEffect>
#include <QBrush>

ChessStyle::ChessStyle()
{

}

QPen ChessStyle::blackPen = QPen(Qt::black);
QPen ChessStyle::whitePen = QPen(Qt::white);
QBrush ChessStyle::blackBrush = QBrush(Qt::black);
QBrush ChessStyle::whiteBrush = QBrush(Qt::white);
QGraphicsDropShadowEffect* ChessStyle::genShadow() {
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setOffset(0, 0);
    shadow->setBlurRadius(15);
    return shadow;
}
