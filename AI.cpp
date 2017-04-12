
#include    <iostream>
#include    <time.h>
#include <stdlib.h>
#include <queue>
#include <string.h>



using namespace std;

typedef pair<int, int> PR;


int t;
int k;
int c[20][30];
int ex[4],ey[4];
string lSol[4]={"LEFT","UP","RIGHT","DOWN"};
int xt[]={0,-1,0,1};
int yt[]={-1,0,1,0};
const int NR=20;
const int MR=30;
int d[NR][MR];
int trace[NR][MR][2];
int preStep;

void readAInput(){
    for(int i=0;i<20;i++)
        for(int j=0;j<30;j++){
            char cc;
            cin >>cc;
            c[i][j]=cc-'0';
        }
    for(int i=0;i<t;i++)
        cin >>ex[i]>> ey[i];
}
void writeAInput(){
    for(int i=0;i<20;i++){
        for(int j=0;j<30;j++){
            cout <<c[i][j]<<" ";
        }
        cout <<endl;
    }
    for(int i=0;i<t;i++)
        cout <<ex[i]<<" "<< ey[i]<<endl;
}
bool isInRange(int xx,int yy){
    if(xx>=0 && xx<NR && yy>=0&& yy<MR) return true;
    return false;
}

bool isMyStable(int xx,int yy){
    return c[xx][yy]==k*2-1;
}
bool isMyUnStable(int xx,int yy){
    return c[xx][yy]==k*2;
}
int countUnstable(int kk){
    int count=0;
    for(int i=0;i<NR;i++)
        for(int j=0;j<MR;j++)
            if(c[i][j]==kk*2) count++;
    return count;
}
string loang(int flag){
    memset(d,0,sizeof(d));
    int io = 0, jo = 0;
    bool ok = false;
    bool mark = false;
    queue<PR> que;
    while (!que.empty()) {
        que.pop();
    }
    que.push(make_pair(ex[k-1], ey[k-1]));
    d[ex[k - 1]][ey[k - 1]] = 1;
    int x = ex[k - 1];
    int y = ey[k - 1];
    while (!que.empty()) {
        if (ok) break;
        PR v = que.front();
        que.pop();
        int uu = v.first;
        int tt = v.second;
        if (uu == x && tt == y) mark = true;
        else mark = false;
        for(int i=0;i<4;i++){
            int xx=uu+xt[i];
            int yy=tt+yt[i];
            if (mark && preStep == 0 && i == 2) continue;
            if (mark && preStep == 2 && i == 0) continue;
            if (mark && preStep == 1 && i == 3) continue;
            if (mark && preStep == 3 && i == 1) continue;
            if (!isInRange(xx, yy)) continue;
            if (!isMyUnStable(xx, yy) && d[xx][yy] == 0) {
                que.push(make_pair(xx, yy));
                d[xx][yy] = 1;
                trace[xx][yy][0] = uu;
                trace[xx][yy][1] = tt;
                if (flag == true && isMyStable(xx, yy)) {
                    io = xx;
                    jo = yy;
                    ok = true;
                    break;
                }
                if (flag == false && !isMyStable(xx, yy)) {
                    io = xx;
                    jo = yy;
                    ok = true;
                    break;
                }
            }
        }
    }
    while (trace[io][jo][0] != x || trace[io][jo][1] != y) {
        int xx = io;
        io = trace[io][jo][0];
        jo = trace[xx][jo][1];
        if (trace[io][jo][0] == x && trace[io][jo][1] == y) break;
    }
    for (int i = 0; i < 4; i++)
        if (io - x == xt[i] && jo - y == yt[i]) {
            preStep = i;
            return lSol[i];
        }
}

int main(){
    //freopen("input.txt","r",stdin);
    cin >> t >> k;
    preStep = -1;
    while(true){
        readAInput();
        int num_unstable = countUnstable(k);
        if (num_unstable != 0) cout << loang(true) << "\n";
        else
            cout << loang(false) << "\n";
        //cout <<lSol[]+"\n";

        cout <<flush;
    }
}
