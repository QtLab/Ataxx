#include "block.h"
#include <QGraphicsDropShadowEffect>
#include <QPen>
#include <QBrush>

Block::Block(int r, int c): r(r), c(c)
{
    setAcceptHoverEvents(true);
    /* 求得棋盘格坐标 */
    QPoint pos = rc2pos(r, c);
    /* 在指定位置绘制正方形 */
    this->setRect(pos.x(), pos.y(), width, width);
    /* 加阴影 */
    this->setGraphicsEffect(genShadow());
    this->setBrush(whiteBrush);
    this->setPen(Qt::NoPen);
}

void Block::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    emit blockClick(r, c);
}

QPoint Block::rc2pos(int r, int c) {
    return QPoint(c * (width + gapWidth), r * (width + gapWidth));
}

