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
//
int tmpState[maxN + 1][maxM + 1];
// (posX,posY) vị trí hiện tại của người chơi
int posX[maxN], posY[maxN];
// đánh dấu các ô stable ở ngoài cùng
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
void    makeTmpState(PR finish, PR start, int trace[maxN + 1][maxM + 1][2]) {
    For(i, 0, maxN)
        For(j, 0, maxM)
            tmpState[i][j] = state[i][j];
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
void    chooseStable(int startX, int startY, int trace[maxN + 1][maxM + 1][2], int distances[maxN + 1][maxM + 1]) {
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
void    BfsFromMyCurrentStable(int trace[maxN + 1][maxM + 1][2], int distances[maxN + 1][maxM + 1]) {
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
            if (preDirection != -1 && isStart && abs(preDirection - i) == 2) continue;
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
void    BfsFromMyCurrentUnStable(int startX, int startY, int preDirec, int trace[maxN + 1][maxM + 1][2], int distances[maxN + 1][maxM + 1]) {
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

/**/
PR    nextDirectionFromUnstable() {
    int startX = posX[myId - 1];
    int startY = posY[myId - 1];
    int maxD = 0;
    vector<PR> listFinishStable;
    listFinishStable.clear();
    PR finish;
    int isFound = false;
    For(k, 0, 4) {
        int nextX = startX + dx[k];
        int nextY = startY + dy[k];
        if (!isInBoard(nextX, nextY)) continue;
        if (abs(preDirection - k) == 2) continue;
        if (isMyStable(nextX, nextY) || isMyUnStable(nextX, nextY)) continue;
        int trace[maxN + 1][maxM + 1][2];
        int distances[maxN + 1][maxM + 1];
        BfsFromMyCurrentUnStable(nextX, nextY, k, trace, distances);
        numChoosenStable = 0;
        chooseStable(nextX, nextY, trace, distances);
        //Chọn ô có đường đi ngắn nhất từ ô hiện tại là dài nhất
        For(i, 0, numChoosenStable) {
            if (choosenStable[i].first + dx[preDirection] == posX[myId - 1] && choosenStable[i].second + dy[preDirection] == posY[myId - 1]) continue;
            int dis = distances[choosenStable[i].first][choosenStable[i].second];
            if (dis > maxD) {
                nextDirection = k;
                isFound = true;
                maxD = dis;
                finish = choosenStable[i];
                listFinishStable.clear();
                listFinishStable.push_back(finish);
            } else if (dis == maxD) {
                if (k == preDirection) nextDirection = k;
                listFinishStable.push_back(choosenStable[i]);
            }
        }
    }
    if (!isFound) {
        For(i, 0, 4) {
           int nextX = startX + dx[i];
           int nextY = startY + dy[i];
           if (isMyStable(nextX, nextY) && abs(i - preDirection) != 2) {
                nextDirection = i;
                finish = make_pair(nextX, nextY);
           }
        }
    }
   // cout << isFound << endl;
    return finish;
}

/**/
PR    findDirecToNearestStable() {
    int startX = posX[myId - 1];
    int startY = posY[myId - 1];
    PR finish;
    int trace[maxN + 1][maxM + 1][2];
    int distances[maxN + 1][maxM + 1];
    BfsFromMyCurrentUnStable(startX, startY, preDirection, trace, distances);
    numChoosenStable = 0;
    chooseStable(startX, startY, trace, distances);
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
/*find direction*/
int     findDirection(int startX, int startY, int finishX, int finishY) {
    For(i, 0, 4) {
        if (finishX - startX == dx[i] && finishY - startY == dy[i]) return i;
    }
}
/*check satisfy unstable*/
bool    checkSatisfyUnstable(int startX, int startY, int preX, int preY, int distToOrigin) {
    //TODO
    int trace[maxN + 1][maxM + 1][2];
    int distances[maxN + 1][maxM + 1];
    int preDirec = findDirection(preX, preY, startX, startY);
    BfsFromMyCurrentUnStable(startX, startY, preDirec, trace, distances);
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
                if (dist >= distToOther - distToOrigin) {
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
 /*---------------Tìm hướng đi tiếp theo------------------------------------------------------------------------------*/
void    findNextDirection(bool isInStable) {
    PR finish;
    findEdgeStable();
    if (isInStable) {
        /* Cần đi đến ô không là stable*/
        int trace[maxN + 1][maxM + 1][2], distances[maxN + 1][maxM + 1];
        BfsFromMyCurrentStable(trace, distances);
        numFinishUnstable = 0;
        int alpha = 0;
        For(i, 0, maxN)
            For(j, 0, maxM) {
                if (isMyStable(i, j) || distances[i][j] == oo) continue;
                if (checkSatisfyUnstable(i, j, trace[i][j][0], trace[i][j][1])) {
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
    } else {
        int numUnStable = countNumberUnStable(state);
        int threshold = 15;
        if (numUnStable < 15) finish = nextDirectionFromUnstable();
        else {
            finish = findDirecToNearestStable();
        }

    }
    preFinish = finish;
}


int main() {
    freopen("input.txt", "r", stdin);
    cin >> numPlayer >> myId;
    srand(time(NULL));
    preDirection = 3;
    preFinish = make_pair(-1, -1);
    For(i, 0, 1) {
        updateState();
        int startX = posX[myId - 1];
        int startY = posY[myId - 1];
        findNextDirection(isMyStable(startX, startY));
        preDirection = nextDirection;
        cout << directions[nextDirection] << "\n";
        cout <<flush;
    }

}
