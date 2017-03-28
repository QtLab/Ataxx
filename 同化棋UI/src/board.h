#ifndef BOARD_H
#define BOARD_H

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "piece.h"
#include "board.h"
#include "block.h"

class Board : public QObject
{
    Q_OBJECT
public:
    explicit Board(QObject *parent = 0);
    static const int N = 7, M = 7;
    /* 返回用于渲染的QGraphicsView* */
    QGraphicsView* View();
    /* 走一回合，两个点分别为(r,c)和(nr,nc). 若flipTurn为真，下完这步棋后会转换先后手 */
    void put(int r, int c, int nr, int nc, bool flipTurn = true); /* put alternatively with black first */
    /* 重置棋盘，以f[][]为状态 */
    void put(PieceType f[N][M]);
    /* 重置为开局状态 */
    void reset();
    /* 允许用户落子 */
    void enable();
    /* 不允许用户落子 */
    void disable();
private:
    /* 渲染用的容器 */
    QGraphicsScene scene;
    QGraphicsView view;

    /* 棋盘状态，当前先手 */
    PieceType g[N][M], turn;
    /* 棋盘格子 */
    Block* blocks[N][M];
    /* 棋子 */
    Piece* pieces[N][M];
    bool lifted; /* 是否拿起一颗子 */
    int liftR, liftC; /* 拿起棋子坐标 */
    /* 根据g[][]更新pieces[][]颜色 */
    void updateView(); /* update view according to g[][] */
    /* 是否允许用户落子 */
    bool enabled;
signals:
    /* 用户落子消息 */
    void userPut(int r, int c, int nr, int nc); /* signal passed to AI */
private slots:
    /* 处理鼠标点击的slot */
    void blockClick(int r, int c);
};

#endif // BOARD_H
