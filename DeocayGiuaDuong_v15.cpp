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
bool isEdgeStable[maxN + 1][maxM + 1];
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
bool    isMyStable(int i, int j) {
    return state[i][j] == 2 * myId - 1;
}
/*-----------------Trả về True nếu ô (i, j)là ô unstable của mình -------------------------------------------------*/
bool    isMyUnStable(int i, int j) {
    return state[i][j] == 2 * myId;
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
void    findEdgeStable() {
    For(i, 0, maxN)
        For(j, 0, maxM) {
            isEdgeStable[i][j] = false;
            if (!isMyStable(i, j)) continue;
            For(k, 0, 4) {
                int x = i + dx[k];
                int y = j + dy[k];
                if (!isInBoard(x, y)) continue;
                if (!isMyStable(x, y)) {
                    isEdgeStable[i][j] = true;
                }
            }
        }

}
/* */
void    makeTmpState(PR finish, PR start, int tmpState[maxN + 1][maxM + 1]) {
    int finishX = finish.first;
    int finishY = finish.second;
    while (finishX != start.first || finishY != start.second) {
        int tmp = finishX;
        finishX = trace[finishX][finishY][0];
        finishY = trace[tmp][finishY][1];
        tmpState[finishX][finishY] = 2 * myId - 1;
    }
}
/*----------------Chọn các ô stable thích hợp trong các ô edgeStable---------------------------------------------*/
void    chooseStable(int startX, int startY) {
    bool isStart = false;
    if (startX == posX[myId - 1] && startY == posY[myId - 1]) isStart = true;
    For(i, 0, maxN)
        For(j, 0, maxM) {
            if (!isEdgeStable[i][j]) continue;
            int finishX = i;
            int finishY = j;
            if (distances[finishX][finishY] == oo) continue;
           // makeTmpState(make_pair(finishX, finishY), make_pair(startX, startY));
            bool satisfy = true;
            while (finishX != startX || finishY != startY) {
                int tmp = finishX;
                finishX = trace[finishX][finishY][0];
                finishY = trace[tmp][finishY][1];
                int dist = distances[i][j] - 1;
                int distToOther = distanceToOtherPlayer(finishX, finishY);
                if (!isStart) {
                    dist++;
                    distToOther--;
                }
                if (dist >= distToOther) {
                    satisfy = false;
                    break;
                }
            }

            For(u, 0, maxN) {
                if (!satisfy) break;
                For(v, 0, maxM) {
                    if (!isMyUnStable(u, v)) continue;
                    int dist = distances[i][j] - 1;
                    int distToOther = distanceToOtherPlayer(u, v);
                    if (!isStart) {
                        dist++;
                        distToOther--;
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
int     countNumberUnStable(int a[maxN + 1][maxM + 1]) {
    int counter = 0;
    For(i, 0, maxN)
        For(j, 0, maxM) {
            if (!isMyUnStable(i, j)) continue;
            counter ++;
        }
    return counter;
}
/*--------------BFS từ vị trí Stable hiện tại tới tất cả các ô trong bàn chơi-----------------------------------
- distance[x][y] là khảng cách ngắn nhất từ ô stable hiện tại đến ô (x, y)
- trace[x][y][0] là chỉ số hàng của ô liền trước (x,y) trên đường đi ngắn nhất đến ô (x,y)
- trace[x][y][1] là chỉ số cột của ô liền trước (x,y) trên đường đi ngắn nhất đến ô (x,y)
-----------------------------------------------------------------------------------------------------------------*/
void    BfsFromMyCurrentStable() {
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
        if (!isMyStable(currentPos.first, currentPos.second)) continue;
        For(i, 0, 4) {
            //if (preDirection != -1 && isStart && abs(preDirection - i) == 2) continue;
            PR nextPos = make_pair(currentPos.first + dx[i], currentPos.second + dy[i]);
            int nextX = nextPos.first;
            int nextY = nextPos.second;
            if (!isInBoard(nextX, nextY)) continue;
            if (isStart && isBesideOtherPlayer(nextX, nextY)) continue;
            if (!isMyUnStable(nextX, nextY) && distances[nextX][nextY] == oo) {
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
void    BfsFromMyCurrentUnStable(int startX, int startY, int preDirec) {
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
            if (!isMyUnStable(nextX, nextY) && distances[nextX][nextY] == oo) {
                distances[nextX][nextY] = distances[currentPos.first][currentPos.second] + 1;
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
void    initTmpState(int tmpState[maxN + 1][maxM + 1]) {
    For(i, 0, maxN)
        For(j, 0, maxM) {
            tmpState[i][j] = state[i][j];
            if (isMyUnStable(i, j)) tmpState[i][j] = 2 * myId - 1;
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
                res = res + tmp;
                break;
            }
        }

    return res;
}
/**/
void    printTmpState(int tmpState[maxN + 1][maxM + 1]) {
    For(i, 0, maxN) {
        For(j, 0, maxM)
            cout << tmpState[i][j];
        cout << endl;
    }
}
/*-------------------bfs từ 4 ô kề của ô unstable hiên tại để tìm nước đi tiếp theo -------------------------------------*/
PR    nextDirectionFromUnstable() {
    int startX = posX[myId - 1];
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
        if (isMyUnStable(nextX, nextY)) continue;
        if (!isMyStable(nextX, nextY)) {
            BfsFromMyCurrentUnStable(nextX, nextY, k);
            numChoosenStable = 0;
            chooseStable(nextX, nextY);
            //Chọn ô có đường đi ngắn nhất từ ô hiện tại là dài nhất
            For(i, 0, numChoosenStable) {
                int tmpState[maxN + 1][maxM + 1];
                initTmpState(tmpState);
                tmpState[startX][startY] = 2 * myId - 1;
                makeTmpState(choosenStable[i], make_pair(nextX, nextY), tmpState);
                int nbStableCapture = countStableCapture(tmpState, 2 * myId - 1);
                if (choosenStable[i].first + dx[preDirection] == posX[myId - 1] && choosenStable[i].second + dy[preDirection] == posY[myId - 1]) continue;
                if (distances[choosenStable[i].first][choosenStable[i].second] > maxTurn - currentTurn) continue;
                if (nbStableCapture > maxD) {
                    nextDirection = k;
                    isFound = true;
                    maxD = nbStableCapture;
                    finish = choosenStable[i];
                } else if (nbStableCapture == maxD) {
                    int dist = distances[choosenStable[i].first][choosenStable[i].second];
                    if (dist < minD) {
                        minD = dist;
                        nextDirection = k;
                        finish = choosenStable[i];
                        isFound = true;
                    }
                    //if (k == preDirection) nextDirection = k;
                }
            }
        } else {
            int tmpState[maxN + 1][maxM + 1];
            initTmpState(tmpState);
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
           if (isMyStable(nextX, nextY) && abs(i - preDirection) != 2 && !isBesideOtherPlayer(nextX, nextY)) {
                nextDirection = i;
                //cout << "OK" << endl;
                finish = make_pair(nextX, nextY);
           }
        }
    }
    return finish;
}

/*-----------------Tìm hướng đi đến ô stable gần nhất có thể----------------------------------------------------------------*/
PR    findDirecToNearestStable() {
    int startX = posX[myId - 1];
    int startY = posY[myId - 1];
    PR finish;

    BfsFromMyCurrentUnStable(startX, startY, preDirection);
    numChoosenStable = 0;
    chooseStable(startX, startY);
    //Chọn ô có đường đi ngắn nhất từ ô hiện tại là ngắn nhất
    int minD = oo;
    vector<PR> listFinishStable;
    listFinishStable.clear();
    For(i, 0, numChoosenStable) {
        if (choosenStable[i].first + dx[preDirection] == posX[myId - 1] && choosenStable[i].second + dy[preDirection] == posY[myId - 1]) continue;
        int dis = distances[choosenStable[i].first][choosenStable[i].second];
        if (dis < minD) {
            minD = dis;
            finish = choosenStable[i];
            listFinishStable.clear();
            listFinishStable.push_back(finish);
        } else if (dis == minD) {
            listFinishStable.push_back(choosenStable[i]);
        }
    }

    int finishX = finish.first;
    int finishY = finish.second;

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
    return finish;
}

/**/
bool    checkSatisfyUnstable(int direct) {
    int startX = posX[myId - 1];
    int startY = posY[myId - 1];
    int nextX = startX + dx[direct];
    int nextY = startY + dy[direct];
    if (isMyStable(nextX, nextY)) return true;
    BfsFromMyCurrentUnStable(nextX, nextY, direct);
    numChoosenStable = 0;
    chooseStable(nextX, nextY);
    if (numChoosenStable == 0) return false;
    return true;

}
 /*---------------Tìm hướng đi tiếp theo------------------------------------------------------------------------------*/
void    findNextDirection(bool isInStable) {
    PR finish;
    findEdgeStable();
    if (isInStable) {
        /* Cần đi đến ô không là stable*/
        BfsFromMyCurrentStable();
        numFinishUnstable = 0;
        int alpha = 0;

        FOR(mid, maxN, 0) {
            int cnt = 0;
            For(i, 0, maxN)
                For(j, 0, maxM) {
                    if (isMyStable(i, j) || distances[i][j] == oo) continue;
                    if (distanceToOtherPlayer(i, j) > mid) cnt++;
            }
            if (cnt > 0) {
                alpha = mid;
                if (alpha <= 4) break;
            }
        }

        For(i, 0, maxN)
            For(j, 0, maxM) {
                if (isMyStable(i, j) || distances[i][j] == oo) continue;
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
        if (!checkSatisfyUnstable(nextDirection)) {
            For(i, 0, 4) {
                int nextX = posX[myId - 1] + dx[i];
                int nextY = posY[myId - 1] + dy[i];
                int preX = posX[myId - 1] - dx[preDirection];
                int preY = posY[myId - 1] - dy[preDirection];
                if (!isInBoard(nextX, nextY)) continue;
                if (isMyStable(nextX, nextY) && !isBesideOtherPlayer(nextX, nextY) ) {
                    if (isMyUnStable(preX, preY) && abs(i - preDirection) == 2) continue;
                    nextDirection = i;
                    finish = make_pair(nextX, nextY);
                }
            }
        }
    } else {
        int numUnStable = countNumberUnStable(state);
        int threshold = maxN * maxM + 1;
        if (numUnStable < threshold) finish = nextDirectionFromUnstable();
        else {
            finish = findDirecToNearestStable();
        }
    }

    preFinish = finish;
}


int main() {
    //freopen("input.txt", "r", stdin);
    cin >> numPlayer >> myId;
    srand(time(NULL));
    currentTurn = 1;
    preDirection = -1;
    nextDirection = -1;
    preFinish = make_pair(-1, -1);
    while (true){
        currentTurn ++;
        updateState();
        int startX = posX[myId - 1];
        int startY = posY[myId - 1];
        findNextDirection(isMyStable(startX, startY));
        preDirection = nextDirection;
        cout << directions[nextDirection] << "\n";
        cout <<flush;
    }

}

