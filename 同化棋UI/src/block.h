#ifndef BLOCK_H
#define BLOCK_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <QBrush>
#include "chessstyle.h"

class Block: public QObject, public QGraphicsRectItem, public ChessStyle
{
    Q_OBJECT
public:
    Block(int r = 0, int c = 0);
    static const int width = 52, gapWidth = 8;
private:
    int r, c;
    /* 通过(r,c)计算坐标 */
    static QPoint rc2pos(int r, int c);
signals:
    void blockClick(int r, int c);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
};

#endif // BLOCK_H
