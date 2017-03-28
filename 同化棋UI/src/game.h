#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <time.h>
#include <iostream>
using namespace std;
#define MAX_NODE 1
#define MIN_NODE 2
#define BOARD_SIZE 7
#define EMPTY 0
#define USER_1 1
#define USER_2 2
#define AI_EMPTY 0 // 无子
#define AI_MY 1 // 待评价子
#define AI_OP 2 // 对方子
#define AI_CANT 3 //无法落子处
#define INF  106666666
#define ERROR_INDEX -1
#define AI_WIN  100000
const int
    dx[24] = { 0, 0, 1, -1, -1, 1, -1, 1, 2, 2, 2, 2,
        2, -2, -2, -2, -2, -2, 1, 0, -1, 1, 0, -1};
const int
    dy[24] = { 1, -1, 0, 0, 1, -1, -1, 1, 2, -2,1, 0,
        -1, 2,  -2, 1,  0,  -1,  2, 2, 2,  -2, -2,-2};


/*
棋盘格子
*/
class Grid{
public:
    int type; //类型

    Grid() {
        type = EMPTY;
    }

    Grid(int t) {
        type = t;
    }

    void grid(int t = EMPTY) {
        type = t;
    }

    int isEmpty() {
        return type == EMPTY ? true : false;
    }
};

/*
棋盘
*/
class ChessBoard{
public:
    Grid chessBoard[BOARD_SIZE][BOARD_SIZE];
    //默认构造为空棋盘
    ChessBoard() {
        for (int i = 0; i < BOARD_SIZE; ++i)
            for (int j = 0; j < BOARD_SIZE; ++j)
                chessBoard[i][j].grid();
    }
    //复制构造
    ChessBoard(const ChessBoard &othr) {
        for (int i = 0; i < BOARD_SIZE; ++i)
            for (int j = 0; j < BOARD_SIZE; ++j)
                chessBoard[i][j].grid(othr.chessBoard[i][j].type);
    }

    /*
    放置棋子
    返回放置棋子是否成功
    */
    inline bool placePiece(int x, int y, int type) {
        qDebug("ChessBoard::placePiece");
        if (chessBoard[x][y].isEmpty()) {
            chessBoard[x][y].type = type;
            int tx,ty;
            int nt = type == USER_1 ? USER_2 : USER_1;
            for (int d = 0; d < 8;d++) {
                tx = x + dx[d];
                ty = y + dy[d];
                if(tx >= 0 && tx < BOARD_SIZE && ty >= 0 && ty < BOARD_SIZE)
                    if (chessBoard[tx][ty].type == nt)
                        chessBoard[tx][ty].type = type;
            }
            return true;
        }
        return false;
    }

    /*
    拿掉棋子
    返回拿掉棋子是否成功
    */
    inline bool removePiece(int x, int y,int type) {
        if (chessBoard[x][y].isEmpty() || chessBoard[x][y].type == type) {
            chessBoard[x][y].type = EMPTY;
            return true;
        }
        return false;
    }
};

/*
AI
*/
class Game : public QObject {
public:
    ChessBoard curState; // 当前棋盘
    bool isStart; // 是否进行中
    int MAX_DEPTH; // 最大搜索层数
    int MAX_D;
    int st;//开始时间
    int dt[7][7][7][7];
    /*
        开始并设定难度
    */
    Game() {
        int x1,y1,x,y;
        for (int x0 = 0;x0 < 7;x0++)
            for (int y0 = 0;y0 < 7;y0++) {
                for (int d = 0;d < 24;d++) {
                    x1 = x0 + dx[d];
                    y1 = y0 + dy[d];
                    if (x1 >= 0 && x1 < 7 && y1 >=0 && y1 < 7)
                        dt[x0][y0][x1][y1] = d;
                }
            }
    }

    inline void startGame(int nd = 32) {
        MAX_DEPTH = nd;
        isStart = true;
        curState.chessBoard[0][0].type = USER_1;
        curState.chessBoard[6][6].type = USER_1;
        curState.chessBoard[6][0].type = USER_2;
        curState.chessBoard[0][6].type = USER_2;
    }

    inline void reset(int nd = 32) {
        MAX_D = nd;
        isStart = true;
        for(int i = 0; i < 7; i++)
            for(int j = 0; j < 7; j++)
                curState.chessBoard[i][j].type = EMPTY;
        curState.chessBoard[0][0].type = USER_1;
        curState.chessBoard[6][6].type = USER_1;
        curState.chessBoard[6][0].type = USER_2;
        curState.chessBoard[0][6].type = USER_2;
    }

    /*
    给出极大极小搜索树后继节点/棋手的类型
    */
    inline int nextType(int type) {
        return type == MAX_NODE ? MIN_NODE : MAX_NODE;
    }

    /*
    当前行棋人放置棋子
    放置失败返回失败
    放置成功:
    检察游戏是否结束
    转换游戏角色后返回成功
    */
    inline bool placePiece(const int &curUser,const int &f,const int &fx,const int &fy) {
        qDebug("placePiece");
        int x = fx + dx[f];
        int y = fy + dy[f];
        if (curState.placePiece(x, y, curUser)) {
            if (f > 7) {
                if (!curState.removePiece(fx,fy,curUser))
                    return false;
            }
            // 检察行棋人是否胜利
            if (isOver()) {
                isStart = false; // 游戏结束
                return true;
            }
            return true;
        }
        return false;
    }
    /*
    判定是否结束
    若结束
    返回 胜者
    */
    inline int isOver() {
            int cnt[3] = {0};
            for (int i = 0;i < BOARD_SIZE;i++)
                for (int j = 0;j < BOARD_SIZE;j++) {
                    cnt[curState.chessBoard[i][j].type]++;
                    if (cnt[0] > 0 && cnt[1] > 0 && cnt[2] > 0)
                        return 0;
                }
            if (cnt[1] == 0)
                return 2;
            if (cnt[2] == 0)
                return 1;
            if (cnt[0] == 0 && cnt[1] >= cnt[2])
                return 1;
            if (cnt[0] == 0 && cnt[1] < cnt[2])
                return 2;
            return 0;
        }

        /*
        判断X,Y位置是否可以搜
        */
        inline bool canSearch(const ChessBoard &state,const int &x,const int &y) {
            if (state.chessBoard[x][y].type == EMPTY &&
                x >= 0 && x < BOARD_SIZE &&
                y >= 0 && y < BOARD_SIZE
                )
                return true;
            return false;
        }
        /*
        判断X,Y位置是否需要搜(周围相同子+边界<8)  舍弃该剪枝，待改进 备用
        */
        inline bool needSearch(const ChessBoard &state,const int &x,const int &y) {
            int type = state.chessBoard[x][y].type;
            int cnt = 0,tx,ty;
            for (int d = 0; d < 8;d++) {
                tx = x + dx[d];
                ty = y + dy[d];
                if(tx < 0 || tx > BOARD_SIZE || ty < 0 || ty > BOARD_SIZE
                   || state.chessBoard[tx][ty].type == type)
                        cnt++;
            }
            if (cnt < 8)
                return true;
            else
                return false;
        }
        //统计 (x,y) 能同化多少相邻的棋子； 估价函数用 舍弃 备用
        inline int countW(const ChessBoard &state,const int &type,const int &x,const int &y) {
            int nt = nextType(type);
            int tx,ty;
            int ans = 0;
            for (int d = 0; d < 8;d++) {
                tx = x + dx[d];
                ty = y + dy[d];
                if(tx >= 0 && tx < BOARD_SIZE && ty >= 0 && ty < BOARD_SIZE)
                    if (state.chessBoard[tx][ty].type == nt)
                        ans++;
            }
            return ans;
        }

        //计算当前局面的估价=已获得棋子数-对方已获得棋子数
        inline int evaluateState(const ChessBoard &state,const int &type) {
            int nt = nextType(type);
            int ans1 = 0,ans2 = 0;
            for (int i = 0; i < BOARD_SIZE; ++i)
                for (int j = 0; j < BOARD_SIZE; ++j) {
                    if (state.chessBoard[i][j].type == type)
                        ans1++;
                    else
                        if (state.chessBoard[i][j].type == nt)
                            ans2++;
                }
            if (ans1 > 24) //自己超过半数棋子
                return AI_WIN - 50;
            if (ans2 > 24) //对手超过半数棋子
                return -AI_WIN + 50;
            return ans1 - ans2;
        }

        /*
        极大极小法搜索
        state 待转换的状态
        type  当前层的标记：MAX MIN
        depth 当前层深
        f fx fy 父层的选择
        alpha 父层alpha值
        beta  父层beta值
        */
        int miniMax(const ChessBoard &state,const int &curUser,const int &f,const int &fx,const int &fy,const int &type,const int &depth,const int &alpha,const int &beta) {
            ChessBoard newState(state);
            //对父层的选择放子(“next”=父层的执行者)
            int x = fx + dx[f];
            int y = fy + dy[f];
            newState.placePiece(x, y, nextType(type));
            if (f > 7)
                newState.removePiece(fx, fy, nextType(type));
            int weight = 0;
            int max = -INF; // 下层权值上界
            int min = INF; // 下层权值下界

            if (depth < MAX_DEPTH) {//未到深度则搜
                // 已输或已胜则不继续搜索
                if (int ov = isOver()) {
                    if (ov == curUser)
                        return AI_WIN; // 我方胜
                    if (ov != curUser)
                        return -AI_WIN; // 对方胜
                }
                for (int i = 0; i < BOARD_SIZE; ++i) {
                    for (int j = 0; j < BOARD_SIZE; ++j){
                        if (newState.chessBoard[i][j].type == type ) //找出所有备选点
                            for (int d = 0; d < 24;d++){ //枚举走法
                                if(clock() - st > CLOCKS_PER_SEC * 0.8){ //到达限定时间则返回
                                    if (type == curUser) {
                                        if (max == -INF)
                                            return INF;
                                        else
                                            return max;
                                    }else{
                                        if (min == INF)
                                            return -INF;
                                        else
                                            return min;
                                    }
                                }
                                int tx = i + dx[d];
                                int ty = j + dy[d];
                                if (canSearch(newState, tx, ty)) { //可以走则搜索
                                    weight = miniMax(newState, curUser, d, i, j, nextType(type), depth + 1, min, max);//根据选择拓展出下一层
                                    if (weight > max)
                                        max = weight; // 更新下层上界
                                    if (weight < min)
                                        min = weight; // 更新下层下界
                                    // alpha-beta 剪枝
                                    if (type == curUser) { //MAX_NODE
                                        if (max >= alpha)
                                            return max;
                                    }
                                    else {                  //MIN_NODE
                                        if (min <= beta)
                                            return min;
                                    }
                                }
                            }
                    }
                }
                if (type == curUser)
                    return max; // 最大层给出最大值
                else
                    return min; // 最小层给出最小值
            }
            else {
                weight = evaluateState(newState, curUser); // 评估当前方局面
                return weight; // 搜索到限定层后给出估价值
            }
        }

        //保存选择用
        struct choseT{
            int f,fx,fy;//方向 始发点坐标
            choseT(){}
            void s(int a,int b,int c) {
                f = a;
                fx = b;
                fy = c;
            }
        };
        /*
        AI 行棋
        */
        choseT placePieceAI(const int &curUser) {
            int weight;
            int max = -INF; // 本层的权值上界
            int min = INF; // 本层的权值下界
            int fd = -1,fx,fy,fw; //最优选择
            st = clock(); //开始时间
            for (MAX_DEPTH = 1;MAX_DEPTH < MAX_D;MAX_DEPTH++) { //逐步加大搜索层数
                if(clock() - st > CLOCKS_PER_SEC * 0.8) //到达限定时间则退出
                    break;
                for (int i = 0; i < BOARD_SIZE; ++i) {
                    for (int j = 0; j < BOARD_SIZE; ++j)
                        if (curState.chessBoard[i][j].type == curUser) //找出所有备选点
                            for (int d = 0; d < 24;d++){ //枚举走法
                                if(clock() - st > CLOCKS_PER_SEC * 0.8) //到达限定时间则退出
                                    break;
                                int tx = i + dx[d];
                                int ty = j + dy[d];
                                if (canSearch(curState, tx, ty)) { //可以走则搜索
                                    weight = miniMax(curState, curUser,d, i, j, nextType(curUser), 1, min, max);
                                    if (weight > max) {
                                        max = weight; // 更新下层上界
                                        fd = d; //更新最优记录
                                        fx = i;
                                        fy = j;
                                        fw = max;
                                    }
                                    if (weight < min)
                                        min = weight;
                                }
                        }
                }
            }
            choseT ans;
            //记录并执行最优点
            ans.s(fd,fx,fy);
            placePiece(curUser,fd,fx,fy);
            return ans; // AI最优点
        }
        /*
        控制台打印当前棋盘状态
        */
        inline void show() {
            for (int i = 0; i < BOARD_SIZE; ++i) {
                for (int j = 0; j < BOARD_SIZE; ++j) {
                    if (curState.chessBoard[i][j].type == 0)
                        cout << " -";
                    if (curState.chessBoard[i][j].type == 1)
                        cout << " X";
                    if (curState.chessBoard[i][j].type == 2)
                        cout << " O";
                }
                cout << endl;
            }
        }
};



#endif // GAME_H
