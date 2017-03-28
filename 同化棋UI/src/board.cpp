#include "board.h"
#include <QGraphicsView>
#include "block.h"
#include <algorithm>
#include <QDebug>
using namespace std;

Board::Board(QObject *parent) : QObject(parent)
{
    view.setScene(&scene);
    turn = Black;
    lifted = false;
    enabled = true;

    /* 抗锯齿 */
    view.setRenderHints(QPainter::Antialiasing);

    for(int i = 0; i < N; i++)
        for(int j = 0; j < M; j++)
            g[i][j] = None;

    /* 放好所有棋，只有4颗可见 */
    for(int i = 0; i < N; i++)
        for(int j = 0; j < M; j++) {
            blocks[i][j] = new Block(i, j);
            scene.addItem(blocks[i][j]);
            connect(blocks[i][j], SIGNAL(blockClick(int,int)), this, SLOT(blockClick(int,int)));

            pieces[i][j] = new Piece(None, i, j);
            scene.addItem(pieces[i][j]);
            connect(pieces[i][j], SIGNAL(pieceClick(int,int)), this, SLOT(blockClick(int,int)));
        }

    /* 边角的4颗棋 */
    g[0][0] = g[6][6] = Black;
    g[0][6] = g[6][0] = White;

    pieces[0][0]->setType(Black);
    pieces[0][6]->setType(White);
    pieces[6][0]->setType(White);
    pieces[6][6]->setType(Black);

}

QGraphicsView* Board::View() {
    return &view;
}

void Board::blockClick(int r, int c) {
    if(!enabled) return;
    /* 有一颗棋子已经被提起 */
    if(lifted) {
        /* 想落在r,c处 */
        if(g[r][c] == None) {
            int d = max(abs(r-liftR), abs(c-liftC));
            /* 可以落子 */
            if(d > 0 && d <= 2) {
                put(liftR, liftC, r, c);
                emit userPut(liftR, liftC, r, c);
            }
            /* 把提起来的子放回去 */
            lifted = false;
            pieces[liftR][liftC]->unlift();
        }
        else {
            /* 提起另一颗子 */
            pieces[liftR][liftC]->unlift();
            if(!(r == liftR && c == liftC) && g[r][c] == turn) {
                liftR = r; liftC = c;
                pieces[r][c]->lift();
            }
            else lifted = false;
        }
    }
    else {
        /* 提起一颗子，必须是当前先手棋子 */
        if(g[r][c] == turn) {
            liftR = r; liftC = c;
            lifted = true;
            pieces[r][c]->lift();
        }
    }
}

/* 将(r,c)上的棋放到(nr, nc)上, flipTurn == true时反转turn */
void Board::put(int r, int c, int nr, int nc, bool flipTurn) {
    /* 判断非法情况 */
    if(!(r >= 0 && r < N && c >= 0 && c <= M))
        throw;

    if(g[r][c] != turn)
        throw;

    if(g[nr][nc] != None)
        throw;


    int dr = abs(nr - r), dc = abs(nc - c), d = max(dr, dc);
    if(d > 2 || d == 0) throw; /* 距离太远 */

    g[nr][nc] = turn;
    if(d == 2) {
        g[r][c] = None;
    }

    /* 感染(nr, nc)周围的棋子 */
    for(dr = -1; dr <= 1; dr++)
        for(dc = -1; dc <= 1; dc++) {
            r = nr + dr; c = nc + dc;
            if(!(r >= 0 && r < N && c >= 0 && c < M)
                || g[r][c] == None
                || (dr == 0 && dc == 0)) continue;
            g[r][c] = turn;
        }

    if(flipTurn)
        turn = turn == White ? Black : White;
    updateView();
}

void Board::put(PieceType f[N][M]) {
    memcpy(g, f, sizeof(g));
    updateView();
}

/* 根据g[][]的值更新视图 */
void Board::updateView() {
    for(int i = 0; i < N; i++)
        for(int j = 0; j < M; j++) {
            pieces[i][j]->setType(g[i][j]);
        }
}

void Board::reset() {
    for(int i = 0; i < N; i++)
        for(int j = 0; j < M; j++)
            g[i][j] = None;
    g[0][0] = g[6][6] = Black;
    g[0][6] = g[6][0] = White;
    turn = Black;
    updateView();
}

void Board::enable() {
    enabled = true;
}

void Board::disable() {
    enabled = false;
}
