/*
Author  Yuxing Zhou(zyx.pulsars@gmail.com)
Date    2016-12-15
*/
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
//#include <iomanip>
#include <ctime>
#include <algorithm>
#include "jsoncpp/json.h" // C++编译时默认包含此库
#include <vector>
using namespace std;

/*
实现用的参数
*/
#define MAX_D 66 //最深层数
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

//方向增量
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
class Game{
public:
    ChessBoard curState; // 当前棋盘
    bool isStart; // 是否进行中
    int MAX_DEPTH; // 当前最大搜索层数
    int st;//开始时间
    /*
        开始
    */
    inline void startGame() {
        isStart = true;
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
    放置失败 返回失败
    放置成功 返回成功
    */
    inline bool placePiece(const int &curUser,const int &f,const int &fx,const int &fy) {
        int x = fx + dx[f];
        int y = fy + dy[f];
        if (curState.placePiece(x, y, curUser)) {
            if (f > 7) {
                if (!curState.removePiece(fx,fy,curUser))
                    return false;
            }
            return true;
        }
        return false;
    }
    /*
    判定是否结束
    type = 1 某一方无法走了
    若结束
    返回 胜者
    */
    inline int isOver(int type) {
        int cnt[3] = {0};
        for (int i = 0;i < BOARD_SIZE;i++)
            for (int j = 0;j < BOARD_SIZE;j++) {
                cnt[curState.chessBoard[i][j].type]++;
                if ((!type) && cnt[0] > 0 && cnt[1] > 0 && cnt[2] > 0)
                    return 0;
            }
        if (cnt[1] == 0)
            return 2;
        if (cnt[2] == 0)
            return 1;
        if ((type || (cnt[0] == 0)) && cnt[1] >= cnt[2])
            return 1;
        if ((type || (cnt[0] == 0)) && cnt[1] < cnt[2])
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
            return AI_WIN - 100 + ans1 - ans2;
        if (ans2 > 24) //对手超过半数棋子
            return -AI_WIN + 100 + ans1 - ans2 ;
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
            if (int ov = isOver(0)) {
                if (ov == curUser)
                    return AI_WIN; // 我方胜
                if (ov != curUser)
                    return -AI_WIN; // 对方胜
            }
            int stc = 0; //计分支数
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
                                stc++;
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
            if (stc == 0){//无路可走
                if (int ov = isOver(1)) {
                    if (ov == curUser)
                        return AI_WIN; // 我方胜
                    if (ov != curUser)
                        return -AI_WIN; // 对方胜
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
        int fd = -1,fx,fy; //最优选择
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
        if (fd != -1)
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

Game G;


int currBotColor; // 我所执子颜色（1为黑，-1为白，棋盘状态亦同）
int dt[7][7][7][7];	//存储每种走法的类型（0~24);
// 从x0, y0提子，在x1, y1处落子，并模拟落子。

int main()
{
	int x0, y0, x1, y1;
	G.startGame();//开始游戏;
	memset(dt,-1,sizeof(dt));
	//初始化向量标号数组（x0,y0)->(x1,y1)是哪种走法
	for (x0 = 0;x0 < 7;x0++)
        for (y0 = 0;y0 < 7;y0++) {
            for (int d = 0;d < 24;d++) {
                x1 = x0 + dx[d];
                y1 = y0 + dy[d];
                if (x1 >= 0 && x1 < 7 && y1 >=0 && y1 < 7)
                    dt[x0][y0][x1][y1] = d;
            }
        }

	// 读入JSON
	string str;
	getline(cin, str);
	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input);

	// 分析自己收到的输入和自己过往的输出，并恢复状态
	int turnID = input["responses"].size();
	currBotColor = input["requests"][(Json::Value::UInt) 0]["x0"].asInt() < 0 ? 1 : -1; // 第一回合收到坐标是-1, -1，说明我是黑方
	int curU = currBotColor == 1? 1 : 2; //由于G和JSON的定义略有差异，需转换用户执棋标号
	for (int i = 0; i < turnID; i++)
	{
		// 根据这些输入输出逐渐恢复状态到当前回合
		x0 = input["requests"][i]["x0"].asInt();
		y0 = input["requests"][i]["y0"].asInt();
		x1 = input["requests"][i]["x1"].asInt();
		y1 = input["requests"][i]["y1"].asInt();
		if (x0 >= 0) {
			G.placePiece(G.nextType(curU),dt[x0][y0][x1][y1],x0,y0);//对手放
		}
		x0 = input["responses"][i]["x0"].asInt();
		y0 = input["responses"][i]["y0"].asInt();
		x1 = input["responses"][i]["x1"].asInt();
		y1 = input["responses"][i]["y1"].asInt();
		if (x0 >= 0) {
			G.placePiece(curU,dt[x0][y0][x1][y1],x0,y0);//自己放
		}
	}
	// 看看自己本回合输入
	x0 = input["requests"][turnID]["x0"].asInt();
	y0 = input["requests"][turnID]["y0"].asInt();
	x1 = input["requests"][turnID]["x1"].asInt();
	y1 = input["requests"][turnID]["y1"].asInt();
	if (x0 >= 0) {
        G.placePiece(G.nextType(curU),dt[x0][y0][x1][y1],x0,y0);
	}
	// 找出合法落子点并做出决策
	int startX, startY, resultX, resultY;
	//进行决策，得出落子坐标，并赋值给上述变量
    Game::choseT cs;
    cs = G.placePieceAI(curU);//用AI得出本回合己方落子
    startX = cs.fx;
    startY = cs.fy;
    resultX = startX + dx[cs.f];
    resultY = startY + dy[cs.f];
	// 决策结束，输出结果

	Json::Value ret;
	ret["response"]["x0"] = startX;
	ret["response"]["y0"] = startY;
	ret["response"]["x1"] = resultX;
	ret["response"]["y1"] = resultY;
	Json::FastWriter writer;
	cout << writer.write(ret) << endl;
	return 0;
}

/*
//本地控制台
int main()
{
    G.startGame();
    int tx,ty,cu = 1,ov;
    Game::choseT cs;
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
    for (;;)
    {
            G.show();
            cin >> tx >> ty >> x >> y;
            G.placePiece(cu, dt[tx][ty][x][y],tx,ty);
            G.show();
            cu = G.nextType(cu);
            cs = G.placePieceAI(cu);
            printf("%c : (%d,%d) -> (%d,%d)\n",cu == 1?'X':'O',cs.fx,cs.fy,cs.fx+dx[cs.f],cs.fy+dy[cs.f]);
            if (ov = G.isOver(0)) {
                printf("%c WIN\n",ov == 1?'X':'O');
                break;
            }
            cu = G.nextType(cu);
            getchar();
    }
    return 0;
}
*/
