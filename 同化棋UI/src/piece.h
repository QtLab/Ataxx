#ifndef PIECE_H
#define PIECE_H

#include <QObject>
#include <QGraphicsItem>
#include <QPen>
#include <QBrush>
#include "chessstyle.h"

enum PieceType { Black = 0, White = 1, None = 2 } ;

class Piece : public QObject, public QGraphicsEllipseItem, public ChessStyle
{
    Q_OBJECT
public:
    static const int width = 40;
    explicit Piece(PieceType pieceType = Black, int r = 0, int c = 0);
    /* 设置棋子类型 */
    void setType(PieceType type);
    /* 提子 */
    void lift();
    /* 放下棋子 */
    void unlift();
signals:
    /* 棋子被按下信号 */
    void pieceClick(int r, int c);
private:
    PieceType pieceType;
    /* 是否被提起 */
    bool lifted;
    QPen pen;
    QBrush brush;
    int r, c;
    QPoint rc2pos(int r, int c);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
};

#endif // PIECE_H
