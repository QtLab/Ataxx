#include "gamewrapper.h"

GameWrapper::GameWrapper(QWidget *parent) : QWidget(parent)
{
    mode = Manual;
    ai.reset();

    /* 控制区 */
    QHBoxLayout *loTop = new QHBoxLayout;
    manualRadio = new QRadioButton(QStringLiteral("人人对弈"));
    botFirstRadio = new QRadioButton(QStringLiteral("AI先手"));
    userFirstRadio = new QRadioButton(QStringLiteral("AI后手"));
    manualRadio->setChecked(true);
    QLabel *label = new QLabel(QStringLiteral("搜索深度"));
    depthSpin = new QSpinBox;
    depthSpin->setRange(3, 64);
    label->setBuddy(depthSpin);
    startBtn = new QPushButton(QStringLiteral("开始"));
    loTop->addWidget(manualRadio);
    loTop->addWidget(botFirstRadio);
    loTop->addWidget(userFirstRadio);
    loTop->addWidget(label);
    loTop->addWidget(depthSpin);
    loTop->addWidget(startBtn);

    /* 链接开始按钮和用户落子信号 */
    connect(startBtn, SIGNAL(clicked(bool)), this, SLOT(startBtnClick()));
    connect(&board, SIGNAL(userPut(int,int,int,int)), this, SLOT(userPut(int,int,int,int)));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(loTop);
    layout->addWidget(board.View());
    setLayout(layout);
    setFixedSize(sizeHint());
}

void GameWrapper::startBtnClick() {
    int depth = depthSpin->value();
    ai.reset(depth);
    board.reset();
    turn = Black;

    if(botFirstRadio->isChecked()) {
        mode = BotFirst;
        botPut();
    }
    else if(userFirstRadio->isChecked()) {
        mode = UserFirst;
    }
    else {
        mode = Manual;
    }
}

void GameWrapper::botPut() {
    board.disable();
    int cu = turn == Black ? 1 : ai.nextType(1);
    Game::choseT cs = ai.placePieceAI(cu);
    board.put(cs.fx,cs.fy,cs.fx+dx[cs.f],cs.fy+dy[cs.f]);
    turn = turn == Black ? White : Black;
    board.enable();
}

void GameWrapper::userPut(int r, int c, int nr, int nc) {
    qDebug("UserPut %d,%d,%d,%d", r, c, nr, nc);
    int cu = turn == Black ? 1 : ai.nextType(1);
    ai.placePiece(cu, ai.dt[r][c][nr][nc], r, c);

    int winner;
    static const QString winner_s[] = { QStringLiteral("黑棋"), QStringLiteral("白棋") };
    if((winner = ai.isOver()) != 0) {
        QMessageBox msgBox;
        msgBox.setText(QStringLiteral("游戏结束!") + winner_s[winner-1] + QStringLiteral("胜"));
        msgBox.exec();
        return;
    }

    turn = turn == Black ? White : Black;
    if(mode != Manual) {
        botPut();
        if((winner = ai.isOver()) != 0) {
            QMessageBox msgBox;
            msgBox.setText(QStringLiteral("游戏结束!") + winner_s[winner-1] + QStringLiteral("胜"));
            msgBox.exec();
        }
    }
}
