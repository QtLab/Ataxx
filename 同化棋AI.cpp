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
#include "jsoncpp/json.h" // C++����ʱĬ�ϰ����˿�
#include <vector>
using namespace std;

/*
ʵ���õĲ���
*/
#define MAX_D 66 //�������
#define MAX_NODE 1
#define MIN_NODE 2
#define BOARD_SIZE 7
#define EMPTY 0
#define USER_1 1
#define USER_2 2
#define AI_EMPTY 0 // ����
#define AI_MY 1 // ��������
#define AI_OP 2 // �Է���
#define AI_CANT 3 //�޷����Ӵ�
#define INF  106666666
#define ERROR_INDEX -1
#define AI_WIN  100000

//��������
const int
    dx[24] = { 0, 0, 1, -1, -1, 1, -1, 1, 2, 2, 2, 2,
        2, -2, -2, -2, -2, -2, 1, 0, -1, 1, 0, -1};
const int
    dy[24] = { 1, -1, 0, 0, 1, -1, -1, 1, 2, -2,1, 0,
        -1, 2,  -2, 1,  0,  -1,  2, 2, 2,  -2, -2,-2};

/*
���̸���
*/
class Grid{
public:
    int type; //����
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
����
*/
class ChessBoard{
public:
    Grid chessBoard[BOARD_SIZE][BOARD_SIZE];
    //Ĭ�Ϲ���Ϊ������
    ChessBoard() {
        for (int i = 0; i < BOARD_SIZE; ++i)
            for (int j = 0; j < BOARD_SIZE; ++j)
                chessBoard[i][j].grid();
    }
    //���ƹ���
    ChessBoard(const ChessBoard &othr) {
        for (int i = 0; i < BOARD_SIZE; ++i)
            for (int j = 0; j < BOARD_SIZE; ++j)
                chessBoard[i][j].grid(othr.chessBoard[i][j].type);
    }

    /*
    ��������
    ���ط��������Ƿ�ɹ�
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
    �õ�����
    �����õ������Ƿ�ɹ�
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
    ChessBoard curState; // ��ǰ����
    bool isStart; // �Ƿ������
    int MAX_DEPTH; // ��ǰ�����������
    int st;//��ʼʱ��
    /*
        ��ʼ
    */
    inline void startGame() {
        isStart = true;
        curState.chessBoard[0][0].type = USER_1;
        curState.chessBoard[6][6].type = USER_1;
        curState.chessBoard[6][0].type = USER_2;
        curState.chessBoard[0][6].type = USER_2;
    }
    /*
    ��������С��������̽ڵ�/���ֵ�����
    */
    inline int nextType(int type) {
        return type == MAX_NODE ? MIN_NODE : MAX_NODE;
    }
    /*
    ��ǰ�����˷�������
    ����ʧ�� ����ʧ��
    ���óɹ� ���سɹ�
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
    �ж��Ƿ����
    type = 1 ĳһ���޷�����
    ������
    ���� ʤ��
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
    �ж�X,Yλ���Ƿ������
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
    �ж�X,Yλ���Ƿ���Ҫ��(��Χ��ͬ��+�߽�<8)  �����ü�֦�����Ľ� ����
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
    //ͳ�� (x,y) ��ͬ���������ڵ����ӣ� ���ۺ����� ���� ����
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

    //���㵱ǰ����Ĺ���=�ѻ��������-�Է��ѻ��������
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
        if (ans1 > 24) //�Լ�������������
            return AI_WIN - 100 + ans1 - ans2;
        if (ans2 > 24) //���ֳ�����������
            return -AI_WIN + 100 + ans1 - ans2 ;
        return ans1 - ans2;
    }

    /*
    ����С������
    state ��ת����״̬
    type  ��ǰ��ı�ǣ�MAX MIN
    depth ��ǰ����
    f fx fy �����ѡ��
    alpha ����alphaֵ
    beta  ����betaֵ
    */
    int miniMax(const ChessBoard &state,const int &curUser,const int &f,const int &fx,const int &fy,const int &type,const int &depth,const int &alpha,const int &beta) {
        ChessBoard newState(state);
        //�Ը����ѡ�����(��next��=�����ִ����)
        int x = fx + dx[f];
        int y = fy + dy[f];
        newState.placePiece(x, y, nextType(type));
        if (f > 7)
            newState.removePiece(fx, fy, nextType(type));
        int weight = 0;
        int max = -INF; // �²�Ȩֵ�Ͻ�
        int min = INF; // �²�Ȩֵ�½�

        if (depth < MAX_DEPTH) {//δ���������
            // �������ʤ�򲻼�������
            if (int ov = isOver(0)) {
                if (ov == curUser)
                    return AI_WIN; // �ҷ�ʤ
                if (ov != curUser)
                    return -AI_WIN; // �Է�ʤ
            }
            int stc = 0; //�Ʒ�֧��
            for (int i = 0; i < BOARD_SIZE; ++i) {
                for (int j = 0; j < BOARD_SIZE; ++j){
                    if (newState.chessBoard[i][j].type == type ) //�ҳ����б�ѡ��
                        for (int d = 0; d < 24;d++){ //ö���߷�
                            if(clock() - st > CLOCKS_PER_SEC * 0.8){ //�����޶�ʱ���򷵻�
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
                            if (canSearch(newState, tx, ty)) { //������������
                                stc++;
                                weight = miniMax(newState, curUser, d, i, j, nextType(type), depth + 1, min, max);//����ѡ����չ����һ��
                                if (weight > max)
                                    max = weight; // �����²��Ͻ�
                                if (weight < min)
                                    min = weight; // �����²��½�
                                // alpha-beta ��֦
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
            if (stc == 0){//��·����
                if (int ov = isOver(1)) {
                    if (ov == curUser)
                        return AI_WIN; // �ҷ�ʤ
                    if (ov != curUser)
                        return -AI_WIN; // �Է�ʤ
                }
            }
            if (type == curUser)
                return max; // ����������ֵ
            else
                return min; // ��С�������Сֵ
        }
        else {
            weight = evaluateState(newState, curUser); // ������ǰ������
            return weight; // �������޶�����������ֵ
        }
    }

    //����ѡ����
    struct choseT{
        int f,fx,fy;//���� ʼ��������
        choseT(){}
        void s(int a,int b,int c) {
            f = a;
            fx = b;
            fy = c;
        }
    };
    /*
    AI ����
    */
    choseT placePieceAI(const int &curUser) {
        int weight;
        int max = -INF; // �����Ȩֵ�Ͻ�
        int min = INF; // �����Ȩֵ�½�
        int fd = -1,fx,fy; //����ѡ��
        st = clock(); //��ʼʱ��
        for (MAX_DEPTH = 1;MAX_DEPTH < MAX_D;MAX_DEPTH++) { //�𲽼Ӵ���������
            if(clock() - st > CLOCKS_PER_SEC * 0.8) //�����޶�ʱ�����˳�
                break;
            for (int i = 0; i < BOARD_SIZE; ++i) {
                for (int j = 0; j < BOARD_SIZE; ++j)
                    if (curState.chessBoard[i][j].type == curUser) //�ҳ����б�ѡ��
                        for (int d = 0; d < 24;d++){ //ö���߷�
                            if(clock() - st > CLOCKS_PER_SEC * 0.8) //�����޶�ʱ�����˳�
                                break;
                            int tx = i + dx[d];
                            int ty = j + dy[d];
                            if (canSearch(curState, tx, ty)) { //������������
                                weight = miniMax(curState, curUser,d, i, j, nextType(curUser), 1, min, max);
                                if (weight > max) {
                                    max = weight; // �����²��Ͻ�
                                    fd = d; //�������ż�¼
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
        //��¼��ִ�����ŵ�
        ans.s(fd,fx,fy);
        if (fd != -1)
            placePiece(curUser,fd,fx,fy);
        return ans; // AI���ŵ�
    }
    /*
    ����̨��ӡ��ǰ����״̬
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


int currBotColor; // ����ִ����ɫ��1Ϊ�ڣ�-1Ϊ�ף�����״̬��ͬ��
int dt[7][7][7][7];	//�洢ÿ���߷������ͣ�0~24);
// ��x0, y0���ӣ���x1, y1�����ӣ���ģ�����ӡ�

int main()
{
	int x0, y0, x1, y1;
	G.startGame();//��ʼ��Ϸ;
	memset(dt,-1,sizeof(dt));
	//��ʼ������������飨x0,y0)->(x1,y1)�������߷�
	for (x0 = 0;x0 < 7;x0++)
        for (y0 = 0;y0 < 7;y0++) {
            for (int d = 0;d < 24;d++) {
                x1 = x0 + dx[d];
                y1 = y0 + dy[d];
                if (x1 >= 0 && x1 < 7 && y1 >=0 && y1 < 7)
                    dt[x0][y0][x1][y1] = d;
            }
        }

	// ����JSON
	string str;
	getline(cin, str);
	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input);

	// �����Լ��յ���������Լ���������������ָ�״̬
	int turnID = input["responses"].size();
	currBotColor = input["requests"][(Json::Value::UInt) 0]["x0"].asInt() < 0 ? 1 : -1; // ��һ�غ��յ�������-1, -1��˵�����Ǻڷ�
	int curU = currBotColor == 1? 1 : 2; //����G��JSON�Ķ������в��죬��ת���û�ִ����
	for (int i = 0; i < turnID; i++)
	{
		// ������Щ��������𽥻ָ�״̬����ǰ�غ�
		x0 = input["requests"][i]["x0"].asInt();
		y0 = input["requests"][i]["y0"].asInt();
		x1 = input["requests"][i]["x1"].asInt();
		y1 = input["requests"][i]["y1"].asInt();
		if (x0 >= 0) {
			G.placePiece(G.nextType(curU),dt[x0][y0][x1][y1],x0,y0);//���ַ�
		}
		x0 = input["responses"][i]["x0"].asInt();
		y0 = input["responses"][i]["y0"].asInt();
		x1 = input["responses"][i]["x1"].asInt();
		y1 = input["responses"][i]["y1"].asInt();
		if (x0 >= 0) {
			G.placePiece(curU,dt[x0][y0][x1][y1],x0,y0);//�Լ���
		}
	}
	// �����Լ����غ�����
	x0 = input["requests"][turnID]["x0"].asInt();
	y0 = input["requests"][turnID]["y0"].asInt();
	x1 = input["requests"][turnID]["x1"].asInt();
	y1 = input["requests"][turnID]["y1"].asInt();
	if (x0 >= 0) {
        G.placePiece(G.nextType(curU),dt[x0][y0][x1][y1],x0,y0);
	}
	// �ҳ��Ϸ����ӵ㲢��������
	int startX, startY, resultX, resultY;
	//���о��ߣ��ó��������꣬����ֵ����������
    Game::choseT cs;
    cs = G.placePieceAI(curU);//��AI�ó����غϼ�������
    startX = cs.fx;
    startY = cs.fy;
    resultX = startX + dx[cs.f];
    resultY = startY + dy[cs.f];
	// ���߽�����������

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
//���ؿ���̨
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
