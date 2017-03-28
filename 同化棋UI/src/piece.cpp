#include "piece.h"
#include <QGraphicsDropShadowEffect>
#include "block.h"

Piece::Piece(PieceType pieceType, int r, int c) :pieceType(pieceType), r(r), c(c)
{
    lifted = false;
    setAcceptHoverEvents(true);
    this->setGraphicsEffect(genShadow());
    this->setRect(rc2pos(r,c).x(), rc2pos(r,c).y(), width, width);
    this->setVisible(pieceType != None);
    this->setPen(pieceType == White ? whitePen : blackPen);
    this->setBrush(pieceType == White ? blackBrush : whiteBrush);
}

void Piece::setType(PieceType pieceType) {
    if(pieceType == None) {
        this->setVisible(false);
        //this->disconnect();
    }
    else {
        this->setVisible(true);
        this->setBrush(pieceType == White ? whiteBrush : blackBrush);
        this->setPen(pieceType == White ? blackPen : Qt::NoPen);
    }
}

QPoint Piece::rc2pos(int r, int c) {
    return QPoint(c * (Block::width + Block::gapWidth) + (Block::width - width) / 2,
                  r * (Block::width + Block::gapWidth) + (Block::width - width) / 2);
}

void Piece::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    emit pieceClick(r, c);
}

void Piece::lift() {
    if(!lifted) {
        lifted = true;
        this->setPen(QPen(Qt::blue));
        this->setPos(this->pos() - QPoint(3, 3));
    }
}

void Piece::unlift() {
    if(lifted) {
        lifted = false;
        this->setPen(blackPen);
        this->setPos(this->pos() + QPoint(3, 3));
    }
}
