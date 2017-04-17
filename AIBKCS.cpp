#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <queue>
#include <vector>
#include <time.h>

#define  For(i, a, b) for (int i = a; i < b; i++)
#define  FOR(i, a, b) for (int i = a; i >= b; i--)
#define  maxN 20
#define  maxM 30
#define  oo 100000000
#define  maxTurn 1001
#define maxDepth  3

using namespace std;

typedef pair<int, int> PR;

int dx[4] = {0,-1,0,1};
int dy[4] = {-1,0,1,0};
string directions[4] = {"LEFT","UP","RIGHT","DOWN"};
// chỉ số id của mình
int myId;
// Số lượng người chơi
int numPlayer;
// Số lượng ô stable kết thúc được chọn
int numChoosenStable;
// Số lượng ô không là stable kết thúc được chọn
int numFinishUnstable;
// chỉ số  hướng đi của lượt chơi trước
int preDirection;
// chỉ cố hướng đi của lượt chơi tiếp theo
int nextDirection;
// trạng thái board hiện tại
int state[maxN + 1][maxM + 1];
// (posX,posY) vị trí hiện tại của người chơi
int posX[maxN], posY[maxN];
// khảng cách ngắn nhất tới ô (i,j)
int distances[maxN + 1][maxM + 1];
// truy vết đường đi ngắn nhất đến ô (i,j)
int trace[maxN + 1][maxM + 1][2];
// đánh dấu các ô stable ở ngoài cùng
//
int currentTurn;

void DFS(int, int, int, int , int &, int &, int, int, int, bool[maxN + 1][maxM + 1], int[maxN + 1][maxM + 1]);

bool checkSatisfyUnstable(int, int, int, int, int, bool[maxN + 1][maxM + 1], int[maxN + 1][maxM + 1]);

// các ô stable kết thúc được chọn
PR choosenStable[maxN * maxM + 1];
// số lượng ô stable có thể tạo ra nếu chọn ô stable i làm ô kết thúc
int choosenValue[maxN * maxM + 1];
// các ô ko là stable kết thúc được chọn
PR finishUnstable[maxN * maxM + 1];
// Ô mục tiêu của bước trước
PR preFinish;

/*-----------------Cập nhật lại trạng thái bàn cờ sau một lượt chơi---------------------------------------------*/
void    updateState() {
    char c;
    For(i, 0, maxN) {
        For(j, 0, maxM) {
            cin >> c;
            state[i][j] = c - '0';
        }
    }
    For(i, 0, numPlayer)
        cin >> posX[i] >> posY[i];
}
/*-----------------Trả về True nếu ô (i, j)là ô stable của mình --------------------------------------------------*/
bool    isStable(int id, int i, int j) {
    //cout << i << " " << j << " " << id << " " <<state[i][j] << endl;
    return state[i][j] == 2 * id - 1;
}
/*-----------------Trả về True nếu ô (i, j)là ô unstable của mình -------------------------------------------------*/
bool    isUnStable(int id, int i, int j) {
    return state[i][j] == 2 * id;
}
/*-----------------Trả về True nếu ô (i, j)là ô nằm trong bàn chơi ------------------------------------------------*/
bool    isInBoard(int i, int j) {
    return i >= 0 && i < maxN && j >= 0 && j < maxM;
}
/*-----------------Khoảng cách ngắn nhất tới các người choi còn lại ---------------------------------------------*/
int     distanceToOtherPlayer(int x, int y) {
    int minV = oo;
    For(i, 0, numPlayer) {
        if (i == myId - 1) continue;
        minV = min(minV, abs(posX[i] - x) + abs(posY[i] - y));
    }
    return minV;
}

/*--------------- Đánh dấu các ô stable bao ngoài cùng bởi mảng đánh dấu isEdgeStable[]-------------------------*/
void    findEdgeStable(int id, bool isEdgeStable[maxN + 1][maxM + 1]) {
    For(i, 0, maxN)
        For(j, 0, maxM) {
            isEdgeStable[i][j] = false;
            if (!isStable(id, i, j)) continue;
           // cout << id << " "<< j << " " <<state[i][j] << endl;
            For(k, 0, 4) {
                int x = i + dx[k];
                int y = j + dy[k];
                if (!isInBoard(x, y)) continue;
                if (!isStable(id, x, y)) {
                    isEdgeStable[i][j] = true;
                }
            }
        }

}
/* */
void    makeTmpState(PR finish, PR start, int tmpState[maxN + 1][maxM + 1], int id) {
    int finishX = finish.first;
    int finishY = finish.second;
    while (finishX != start.first || finishY != start.second) {
        int tmp = finishX;
        finishX = trace[finishX][finishY][0];
        finishY = trace[tmp][finishY][1];
        tmpState[finishX][finishY] = 2 * id - 1;
    }
}
/*----------------Chọn các ô stable thích hợp trong các ô edgeStable---------------------------------------------*/
void    chooseStable(int startX, int startY, int dis, int id,
                     bool isEdgeStable[maxN + 1][maxM + 1], int minDisBlock[maxN + 1][maxM + 1]) {
    bool isStart = false;
    if (startX == posX[id - 1] && startY == posY[id - 1]) isStart = true;
    For(i, 0, maxN)
        For(j, 0, maxM) {
            if (!isEdgeStable[i][j]) continue;
            int finishX = i;
            int finishY = j;
            if (distances[finishX][finishY] == oo) continue;
            if (distances[finishX][finishY] > minDisBlock[finishX][finishY])
                continue;
           // makeTmpState(make_pair(finishX, finishY), make_pair(startX, startY));
            bool satisfy = true;
            while (finishX != startX || finishY != startY) {
                int tmp = finishX;
                finishX = trace[finishX][finishY][0];
                finishY = trace[tmp][finishY][1];
                int dist = distances[i][j] - 1;//Khoang cach tu o stable bien den o xuat phat
                int distToOther = distanceToOtherPlayer(finishX, finishY);
                if (!isStart) {
                    dist += dis;
                }
                if (dist >= distToOther) {
                    satisfy = false;
                    break;
                }
            }

            For(u, 0, maxN) {
                if (!satisfy) break;
                For(v, 0, maxM) {
                    if (!isUnStable(id,u, v)) continue;
                    int dist = distances[i][j] - 1;
                    int distToOther = distanceToOtherPlayer(u, v);
                    if (!isStart) {
                        dist += dis;
                    }
                    if (dist >= distToOther) {
                        satisfy = false;
                        break;
                    }
                }
            }

            if (satisfy) {
                choosenStable[numChoosenStable] = make_pair(i, j);
                numChoosenStable++;
            }
        }
}
/*----------------Trả về True nếu (x,y) thuộc danh sách các ô stable được chọn -----------------------------------*/
bool    isInChoosenStable(int x, int y) {
    For(i, 0, numChoosenStable)
        if (x == choosenStable[i].first && y == choosenStable[i].second) return true;
    return false;
}
/*----------------Trả về true nếu (x, y) là ô hiện tại của đối thủ------------------------------------------------*/
bool    isPosOfOtherPlayer(int x, int y) {
    For(i, 0, numPlayer) {
        if (i == myId - 1) continue;
        if (posX[i] == x && posY[i] == y) return true;
    }
    return false;
}

/*-----------------Trả về true nếu (x, y) là ô kề cạnh với ô đối thủ-----------------------------------------------*/
bool    isBesideOtherPlayer(int x, int y) {
    For(i, 0, numPlayer) {
        if (i == myId - 1) continue;
        int enemyPosX = posX[i];
        int enemyPosY = posY[i];
        For(k, 0, 4) {
            int sideX = enemyPosX + dx[k];
            int sideY = enemyPosY + dy[k];
            if (!isInBoard(sideX, sideY)) continue;
            if (state[sideX][sideY] == 2*(i+1)) continue;
            if (sideX == x && sideY == y) return true;
        }
    }
    return false;
}
/*------------------Đếm số lượng unstable của trạng thái a -------------------------------------------------------*/
int     countNumberUnStable(int a[maxN + 1][maxM + 1], int id) {
    int counter = 0;
    For(i, 0, maxN)
        For(j, 0, maxM) {
            if (!isUnStable(id, i, j)) continue;
            counter ++;
        }
    return counter;
}
/*--------------BFS từ vị trí Stable hiện tại tới tất cả các ô trong bàn chơi-----------------------------------
- distance[x][y] là khảng cách ngắn nhất từ ô stable hiện tại đến ô (x, y)
- trace[x][y][0] là chỉ số hàng của ô liền trước (x,y) trên đường đi ngắn nhất đến ô (x,y)
- trace[x][y][1] là chỉ số cột của ô liền trước (x,y) trên đường đi ngắn nhất đến ô (x,y)
-----------------------------------------------------------------------------------------------------------------*/
void    BfsFromMyCurrentStable(int id) {
    queue<PR> Q;
    int startX = posX[myId - 1];
    int startY = posY[myId - 1];
    bool isStart;
    For(i, 0, maxN)
        For(j, 0, maxM)
            distances[i][j] = oo;
    distances[startX][startY] = 1;
    Q.push(make_pair(startX, startY));

    while (!Q.empty()) {
        PR currentPos = Q.front();
        Q.pop();
        if (currentPos.first == startX && currentPos.second == startY) isStart = true;
        else isStart = false;
        if (!isStable(id, currentPos.first, currentPos.second)) continue;
        For(i, 0, 4) {
            //if (preDirection != -1 && isStart && abs(preDirection - i) == 2) continue;
            PR nextPos = make_pair(currentPos.first + dx[i], currentPos.second + dy[i]);
            int nextX = nextPos.first;
            int nextY = nextPos.second;
            if (!isInBoard(nextX, nextY)) continue;
            if (isStart && isBesideOtherPlayer(nextX, nextY)) continue;
            if (!isUnStable(id, nextX, nextY) && distances[nextX][nextY] == oo) {
                distances[nextX][nextY] = distances[currentPos.first][currentPos.second] + 1;
                Q.push(make_pair(nextX, nextY));
                trace[nextX][nextY][0] = currentPos.first;
                trace[nextX][nextY][1] = currentPos.second;
            }
        }

    }
}
/*--------------BFS từ vị trí Unstable hiện tại-tới ô EdgeStable chỉ đi qua các ô ko phải stable--------------------
- distance[x][y] là khảng cách ngắn nhất từ ô Unstable hiện tại đến ô (x, y)
- trace[x][y][0] là chỉ số hàng của ô liền trước (x,y) trên đường đi ngắn nhất đến ô (x,y)
- trace[x][y][1] là chỉ số cột của ô liền trước (x,y) trên đường đi ngắn nhất đến ô (x,y)
-----------------------------------------------------------------------------------------------------------------*/
void    BfsFromMyCurrentUnStable(int startX, int startY, int preDirec, int id,
                                 bool isEdgeStable[maxN + 1][maxM + 1]) {
     queue<PR> Q;
    bool isStart;
    PR finish;
    For(i, 0, maxN)
        For(j, 0, maxM)
            distances[i][j] = oo;
    distances[startX][startY] = 1;
    Q.push(make_pair(startX, startY));
    while (!Q.empty()) {
        PR currentPos = Q.front();
        Q.pop();
        if (currentPos.first == startX && currentPos.second == startY) isStart = true;
        if (isEdgeStable[currentPos.first][currentPos.second]) continue;
        else isStart = false;
        For(i, 0, 4) {
            if (preDirec != -1 && isStart && abs(preDirec - i) == 2) continue;
            PR nextPos = make_pair(currentPos.first + dx[i], currentPos.second + dy[i]);
            int nextX = nextPos.first;
            int nextY = nextPos.second;
            if (!isInBoard(nextX, nextY)) continue;
            if (isStart && isBesideOtherPlayer(nextX, nextY)) continue;
            if (!isUnStable(id,nextX, nextY) && distances[nextX][nextY] == oo) {

                distances[nextX][nextY] = distances[currentPos.first][currentPos.second] + 1;
                cout <<"queue:" <<nextX << " " << nextY << " " << distances[nextX][nextY]<<endl;
                Q.push(make_pair(nextX, nextY));
                trace[nextX][nextY][0] = currentPos.first;
                trace[nextX][nextY][1] = currentPos.second;
            }
        }
    }
}
/*---------------------Khoảng cách ngắn nhất từ (x,y)tới ô stable của các đối thủ --------------------------------------*/
int     distanceToStableOtherPlayer(int x, int y) {
    int minV = oo;
    For(k, 0, numPlayer) {
        if (k == myId - 1) continue;
        For(i, 0, maxN)
            For(j, 0, maxM)
                if (state[i][j] == 2*(k+1) - 1) minV = min(minV, abs(x - i) + abs(y - j));
    }
    return minV;
}
/*-------------------Trả về True nếu p thuộc arr -----------------------------------------------------------------------*/
bool    checkBelongTo(vector<PR> arr, PR p) {
    For(i, 0, arr.size())
        if (p.first == arr[i].first && p.second == arr[i].second) return true;
    return false;
}

/*-------------------Khởi tạo tmpState = state hiện tại ----------------------------------------------------------------*/
void    initTmpState(int tmpState[maxN + 1][maxM + 1], int id) {
    For(i, 0, maxN)
        For(j, 0, maxM) {
            tmpState[i][j] = state[i][j];
            if (isUnStable(id, i, j)) tmpState[i][j] = 2 * myId - 1;
        }
}
/*-------------------Tính số lượng ô stable có thể chiếm được -----------------------------------------------------------*/
int     countStableCapture(int tmpState[maxN + 1][maxM + 1], int idStable)
{
    int res = 0;
    int queueX[maxN * maxM + 1], queueY[maxN * maxM + 1];
    int v = 0, r = 1;
    bool boo[maxN + 1][maxM + 1];
    For(i, 0, maxN)
        For(j, 0, maxM)
            boo[i][j] = true;
    For(i, 0, maxN)
        For(j, 0, maxM)
            if (tmpState[i][j] == idStable){
                res++;
                boo[i][j] = false;
            }
   // cout << res << endl;
    For(i, 0, maxN)
        For(j, 0, maxM)
        if (boo[i][j]) {
            v = 1;
            r = 1;
            int tmp = 0;
            int isResult = true;
            queueX[v] = i;
            queueY[v] = j;
            boo[i][j] = false;
            while (v >= r)
            {
                int xx = queueX[r];
                int yy = queueY[r];
                r++;
                tmp++;
                if (xx == 0 || xx == maxN - 1 || yy == 0 || yy == maxM - 1)
                    isResult = false;
                for (int k = 0; k < 4; k++)
                {
                    int uu = xx + dx[k];
                    int vv = yy + dy[k];
                    if (isInBoard(uu, vv) && boo[uu][vv])
                    {
                        boo[uu][vv] = false;
                        v++;
                        queueX[v] = uu;
                        queueY[v] = vv;
                    }
                }
            }
           // cout << i << " " << j << " " << " " << isResult << " " << tmp << endl;
            if (isResult)
            {
                for (int k = 1; k <= v; k++)
                    tmpState[queueX[k]][queueY[k]] = idStable;
                res = res + tmp;
                break;
            }
        }

    return res;
}
/**/

/*-------------------Tính số lượng ô stable có thể chiếm được -----------------------------------------------------------*/
int     countStableCaptureEnemy(int tmpState[maxN + 1][maxM + 1], int idStable,
                                int minDisBlock[maxN + 1][maxM + 1], int dis)
{
    int res = 0;
    int queueX[maxN * maxM + 1], queueY[maxN * maxM + 1];
    int v = 0, r = 1;
    bool boo[maxN + 1][maxM + 1];
    For(i, 0, maxN)
        For(j, 0, maxM)
            boo[i][j] = true;
    For(i, 0, maxN)
        For(j, 0, maxM)
            if (tmpState[i][j] == idStable){
                res++;
                boo[i][j] = false;
            }
   // cout << res << endl;
    For(i, 0, maxN)
        For(j, 0, maxM)
        if (boo[i][j]) {
            v = 1;
            r = 1;
            int tmp = 0;
            int isResult = true;
            queueX[v] = i;
            queueY[v] = j;
            boo[i][j] = false;
            while (v >= r)
            {
                int xx = queueX[r];
                int yy = queueY[r];
                r++;
                tmp++;
                if (xx == 0 || xx == maxN - 1 || yy == 0 || yy == maxM - 1)
                    isResult = false;
                for (int k = 0; k < 4; k++)
                {
                    int uu = xx + dx[k];
                    int vv = yy + dy[k];
                    if (isInBoard(uu, vv) && boo[uu][vv])
                    {
                        boo[uu][vv] = false;
                        v++;
                        queueX[v] = uu;
                        queueY[v] = vv;
                    }
                }
            }
           // cout << i << " " << j << " " << " " << isResult << " " << tmp << endl;
            if (isResult)
            {
                for (int k = 1; k <= v; k++)
                    minDisBlock[queueX[k]][queueY[k]] = min(minDisBlock[queueX[k]][queueY[k]], k);
                res = res + tmp;
                break;
            }
        }

    return res;
}

void SignTheInSecureStable(int startXE, int startYE, int preDirectionE,
                           int minDisBlock[maxN + 1][maxM + 1])
{
    int idEnemy = 3 - myId;
    bool isEdgeStableEnemy[maxN + 1][maxM + 1];
    findEdgeStable(idEnemy, isEdgeStableEnemy);
    BfsFromMyCurrentUnStable(startXE, startYE, preDirectionE, idEnemy, isEdgeStableEnemy);
    For(i, 0 , maxN)
        For(j, 0, maxM)
            minDisBlock[i][j] = oo;
    For(i, 0, maxN)
        For(j, 0, maxM)
        if (isEdgeStableEnemy[i][j]){
        int dis = distances[i][j];
        cout << i << " " << j << " " << dis << endl;
        if (dis != oo)
        {
            int tmpState[maxN + 1][maxM + 1];
            initTmpState(tmpState, idEnemy);
            makeTmpState(make_pair(i, j), make_pair(startXE, startYE), tmpState, idEnemy);
            countStableCaptureEnemy(tmpState, 2 * idEnemy - 1, minDisBlock, dis);
        }
    }
}

int findFirstDirectEnemy()
{
    int idEnemy = 1 - myId;
    for (int k = 0; k < 4; k++)
    {
        int u = posX[idEnemy] - dx[k];
        int v = posY[idEnemy] - dy[k];
        if (isInBoard(u, v))
        {
            if (isUnStable(idEnemy, u, v))
                return k;
        }
    }
    return -1;
}
void    printTmpState(int tmpState[maxN + 1][maxM + 1]) {
    For(i, 0, maxN) {
        For(j, 0, maxM)
            cout << tmpState[i][j];
        cout << endl;
    }
}
/*-------------------bfs từ 4 ô kề của ô unstable hiên tại để tìm nước đi tiếp theo -------------------------------------*/
PR    nextDirectionFromUnstable(int id, bool isEdgeStable[maxN + 1][maxM + 1]) {
    int minDisBlock[maxN + 1][maxM + 1];
    int idEnemy = 1 - id;
    SignTheInSecureStable(posX[idEnemy], posY[idEnemy], findFirstDirectEnemy(), minDisBlock);
    cout << minDisBlock[7][9] << " " << minDisBlock[8][11] << " " << minDisBlock[9][11] << " " << minDisBlock[10][11]<<endl;
    /*int startX = posX[myId - 1];
    int startY = posY[myId - 1];
    int maxD = 0;
    vector<PR> listFinishStable;
    listFinishStable.clear();
    PR finish;
    int isFound = false;
    int minD = oo;
    For(k, 0, 4) {
        int nextX = startX + dx[k];
        int nextY = startY + dy[k];
        if (!isInBoard(nextX, nextY)) continue;
        if (abs(preDirection - k) == 2) continue;
        if (isUnStable(id, nextX, nextY)) continue;
        if (!isStable(id, nextX, nextY)) {
            if (!checkSatisfyUnstable(nextX, nextY,1,k, id, isEdgeStable, minDisBlock)) continue;
            int tam = state[nextX][nextY];
            state[nextX][nextY] = 2 * myId - 1;
            int tmpMaxD = -1, tmpMinD = 10000;
            DFS(1, nextX, nextY, k, tmpMaxD, tmpMinD, 2 * myId - 1, 2 * id, id,
                isEdgeStable, minDisBlock);
            state[nextX][nextY] = tam;
            //cout <<k << " " << tmpMaxD << " " << tmpMinD << endl;
            if (tmpMaxD > maxD) {
                    nextDirection = k;
                    isFound = true;
                    maxD = tmpMaxD;
                    minD = tmpMinD;
                    finish = make_pair(nextX, nextY);
                } else if (tmpMaxD == maxD) {
                    if (tmpMinD < minD) {
                        minD = tmpMinD;
                        nextDirection = k;
                        finish = make_pair(nextX, nextY);
                        isFound = true;
                    }
                    //if (k == preDirection) nextDirection = k;
                }
        } else {
            int tmpState[maxN + 1][maxM + 1];
            initTmpState(tmpState, id);
            int nbStableCapture = countStableCapture(tmpState, 2 * myId - 1);
            if (nbStableCapture >= maxD) {
                nextDirection = k;
                maxD = nbStableCapture;
                finish = make_pair(nextX, nextY);
                isFound = true;
                minD = 0;
            }
        }
        //cout << "nextX = " << nextX << ", nextY = " << nextY << ", maxD = " << maxD << endl;
    }
    if (!isFound) {
            //cout << isFound << endl;
        For(i, 0, 4) {
           int nextX = startX + dx[i];
           int nextY = startY + dy[i];
           if (!isInBoard(nextX, nextY)) continue;
           if (isStable(id, nextX, nextY) && abs(i - preDirection) != 2 && !isBesideOtherPlayer(nextX, nextY)) {
                nextDirection = i;
                //cout << "OK" << endl;
                finish = make_pair(nextX, nextY);
           }
        }
    }
    return finish;*/
    return make_pair(0,0);
}

/*-----------------Tìm hướng đi đến ô stable gần nhất có thể----------------------------------------------------------------*/


/**/
bool    checkSatisfyUnstable(int startX, int startY, int dis,  int preDirection,
                int id, bool isEdgeStable[maxN + 1][maxM + 1], int minDisBlock[maxN + 1][maxM + 1]){
    BfsFromMyCurrentUnStable(startX, startY, preDirection, id, isEdgeStable);
    numChoosenStable = 0;
    chooseStable(startX, startY, dis, id, isEdgeStable, minDisBlock);
    if (numChoosenStable == 0) return false;
    return true;

}
 /*---------------Tìm hướng đi tiếp theo------------------------------------------------------------------------------*/
void    findNextDirection(bool isInStable, int id) {
    PR finish;
    bool isEdgeStableOfMe[maxN + 1][maxM + 1];
    findEdgeStable(id, isEdgeStableOfMe);
    if (isInStable) {
        /* Cần đi đến ô không là stable*/
        BfsFromMyCurrentStable(id);
        numFinishUnstable = 0;
        int alpha = 0;

        FOR(mid, maxN, 0) {
            int cnt = 0;
            For(i, 0, maxN)
                For(j, 0, maxM) {
                    if (isStable(id, i, j) || distances[i][j] == oo) continue;
                    if (distanceToOtherPlayer(i, j) > mid) cnt++;
            }
            if (cnt > 0) {
                alpha = mid;
                if (alpha <= 4) break;
            }
        }

        For(i, 0, maxN)
            For(j, 0, maxM) {
                if (isStable(id, i, j) || distances[i][j] == oo) continue;
                if (distanceToOtherPlayer(i, j) > alpha) {
                    finishUnstable[numFinishUnstable] = make_pair(i, j);
                    numFinishUnstable++;
                }
            }
        int minD = oo;
        vector<PR> listFinishUnstable;
        bool isFinish = false;
        listFinishUnstable.clear();
        For(i, 0, numFinishUnstable) {
            if (finishUnstable[i].first + dx[preDirection] == posX[myId - 1] && finishUnstable[i].second + dy[preDirection] == posY[myId - 1]) continue;
            if (preFinish.first == finishUnstable[i].first && preFinish.second == finishUnstable[i].second) {
                finish = preFinish;
                isFinish = true;
                break;
            }
            int dis = distances[finishUnstable[i].first][finishUnstable[i].second];
            if (minD > dis) {
                listFinishUnstable.clear();
                minD = dis;
                finish = finishUnstable[i];
                listFinishUnstable.push_back(finish);
            } else if (minD == dis) {
                listFinishUnstable.push_back(finishUnstable[i]);
            }
        }
        if (!isFinish) {
            int index = rand() % (listFinishUnstable.size());
            finish = listFinishUnstable[index];
        }
        int finishX = finish.first, startX = posX[myId - 1];
        int finishY = finish.second, startY = posY[myId - 1];
        while (trace[finishX][finishY][0] != startX || trace[finishX][finishY][1] != startY) {
            int tmp = finishX;
            finishX = trace[finishX][finishY][0];
            finishY = trace[tmp][finishY][1];
        }
        For(i, 0, 4)
            if (finishX - startX == dx[i] && finishY - startY == dy[i]) {
                nextDirection = i;
                break;
            }
        // Nếu ô unstable ko có đường đi đến stable an toàn thì chọn ô stable để đi đến
        /*if (!checkSatisfyUnstable(posX[myId - 1] + dx[nextDirection],posY[myId - 1] + dy[nextDirection],
                                   1, nextDirection, id, isEdgeStableOfMe)) {
            For(i, 0, 4) {
                int nextX = posX[myId - 1] + dx[i];
                int nextY = posY[myId - 1] + dy[i];
                int preX = posX[myId - 1] - dx[preDirection];
                int preY = posY[myId - 1] - dy[preDirection];
                if (!isInBoard(nextX, nextY)) continue;
                if (isStable(id, nextX, nextY) && !isBesideOtherPlayer(nextX, nextY) ) {
                    nextDirection = i;
                    finish = make_pair(nextX, nextY);
                }
            }
        }*/
    } else {
        int numUnStable = countNumberUnStable(state, id);
        int threshold = maxN * maxM + 1;
        if (numUnStable < threshold){
                finish = nextDirectionFromUnstable(id, isEdgeStableOfMe);
        }
    }

    preFinish = finish;
}
/**/
void countNumberCell(int currentX, int currentY, int preDirec, int idStable,
                      int idUnStable, int dist, int &maxD, int&minD, int id,
                      bool isEdgeStable[maxN + 1][maxM + 1], int minDisBlock[maxN + 1][maxM + 1])
{
    BfsFromMyCurrentUnStable(currentX, currentY, preDirec, id, isEdgeStable);
    numChoosenStable = 0;
    chooseStable(currentX, currentY, dist, id, isEdgeStable, minDisBlock);
    For(i, 0, numChoosenStable) {
        int tmpState[maxN + 1][maxM + 1];
        initTmpState(tmpState, id);
        tmpState[currentX][currentY] = idStable;
        makeTmpState(choosenStable[i], make_pair(currentX, currentY), tmpState, id);
        int nbStableCapture = countStableCapture(tmpState, idStable);
        if (maxD < nbStableCapture)
        {
            maxD = nbStableCapture;
            minD = distances[choosenStable[i].first][choosenStable[i].second];
        }
        else
        if (maxD == nbStableCapture)
        {
            if (minD < distances[choosenStable[i].first][choosenStable[i].second])
                minD = distances[choosenStable[i].first][choosenStable[i].second];
        }
    }
}
/**/
void DFS(int depth, int currentX, int currentY, int preDirection,
        int &currenMaxV, int &currentMinD, int idStable, int idUnStable,
        int id, bool isEdgeStable[maxN + 1][maxM + 1], int minDisBlock[maxN + 1][maxM + 1])
{
    if (depth > maxDepth)
    {
        int maxD = -1, minD = 10000;
        countNumberCell(currentX, currentY, preDirection, idStable,
                        idUnStable, depth, maxD, minD, id, isEdgeStable, minDisBlock);
        if (maxD > currenMaxV)
        {
            currenMaxV = maxD;
            currentMinD = minD + depth;
        }
        else if (maxD == currenMaxV)
        {
            if (currentMinD > minD + depth)
                currentMinD = minD + depth;
        }
        return;
    }
    for (int k = 0; k < 4; k++)
    if (abs(k - preDirection) != 2)
    {
        int u = currentX + dx[k];
        int v = currentY + dy[k];
        if (isInBoard(u, v))
        {
            if (state[u][v] == idStable)
            {
                int tmpMaxD = -1;
                int tmpState[maxN + 1][maxM + 1];
                initTmpState(tmpState, id);
                //printTmpState(tmpState);
                tmpMaxD = countStableCapture(tmpState, idStable);
                //cout << currentX << " "  << currentY << " "<<u << " " << v << " " << tmpMaxD << endl;
                if (tmpMaxD > currenMaxV)
                {
                    currenMaxV = tmpMaxD;
                    currentMinD = depth + 1;
                }
                else if (tmpMaxD == currenMaxV)
                {
                    if (depth < currentMinD)
                        currentMinD = depth + 1;
                    //cout << depth << " " << currentMinD << endl;
                }

            }
            else
            if (state[u][v] != idUnStable && checkSatisfyUnstable(u, v, depth + 1, k, id, isEdgeStable, minDisBlock))
            {
                int tt = state[u][v];
                state[u][v] = idUnStable;
                DFS(depth + 1, u, v, k, currenMaxV, currentMinD,idStable, idUnStable, id, isEdgeStable, minDisBlock);
                state[u][v] = tt;
            }
        }

    }
}

/**/
int main() {
    freopen("input.txt", "r", stdin);
    cin >> numPlayer >> myId;
    srand(time(NULL));
    currentTurn = 1;
    preDirection = 0;
    nextDirection = -1;
    preFinish = make_pair(-1, -1);
    For(i,0,1) {
        currentTurn ++;
        updateState();
        int startX = posX[myId - 1];
        int startY = posY[myId - 1];
        findNextDirection(isStable(startX, startY, myId), myId);
        preDirection = nextDirection;
        cout << directions[nextDirection] << "\n";
        cout <<flush;
    }

}


