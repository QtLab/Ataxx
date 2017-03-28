#ifndef CHESSSTYLE_H
#define CHESSSTYLE_H
#include <QPen>
#include <QGraphicsDropShadowEffect>
#include <QBrush>

class ChessStyle
{
public:
    ChessStyle();
    static QBrush blackBrush;
    static QBrush whiteBrush;
    static QPen whitePen;
    static QPen blackPen;
    static QGraphicsDropShadowEffect* genShadow();
};

#endif // CHESSSTYLE_H
