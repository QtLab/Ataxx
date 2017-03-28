#ifndef GAMEWRAPPER_H
#define GAMEWRAPPER_H

#include <QWidget>
#include <QtWidgets>
#include "board.h"
#include "game.h"

class GameWrapper : public QWidget
{
    Q_OBJECT
public:
    explicit GameWrapper(QWidget *parent = 0);
    enum GameMode { Manual, BotFirst, UserFirst };
private:
    /* UI组件 */
    Board board;
    QRadioButton *manualRadio;
    QRadioButton *botFirstRadio;
    QRadioButton *userFirstRadio;
    QPushButton *startBtn;
    QSpinBox *depthSpin;
    /* AI组件 */
    Game ai;
    /* 游戏模式 */
    GameMode mode;
    /* 当前先手 */
    PieceType turn;
    /* 让AI下一步棋 */
    void botPut();
signals:

public slots:
    /* 接受来自board的用户落子操作 */
    void userPut(int r, int c, int nr, int nc);
private slots:
    /* 开始按键按下的操作 */
    void startBtnClick();
};

#endif // GAMEWRAPPER_H
