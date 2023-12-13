#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <Windows.h>

#define objectSpawnCount 6
#define skillCount 10

#define ESC 27
#define ENTER 13

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define MAP_X 3
#define MAP_Y 2
#define MAP_WIDTH 30
#define MAP_HEIGHT 20

typedef struct _objectData
{
    int num;
    int weight;
    int score;
} objectData;

typedef struct _targetData
{
    int num;
    int x;
    int y;
} targetData;

int map[30][30];  //전체 맵, -3: 벽, -2: 시작점, -1: 이동가능한 공터

int gameScene; // 0-준비 씬, 1-맵 씬, 2-스킬 씬, 3-게임 씬

objectData object[objectSpawnCount + 1];  //가방에 넣을 수 있는 물건들들
int objectVisit[objectSpawnCount + 1]; //가방에 이미 넣은 적 있는지
int objectVisitThisTurn[objectSpawnCount + 1]; //이번 턴에 넣는 물건들
int objectHaveCount; //넣을수 있는 물건 갯수
int objectPos[objectSpawnCount + 1][2];
int objectDist[objectSpawnCount + 1][objectSpawnCount + 1];
int** tspDP;
int* objectHaveIndex;
int objectHaveCountThisTurn;

int allcoin;
int coin;
int score;
int day;

int clearCoin;

// UI추가 변수
int defaultMap[30][30] =
{
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3},
    {-3,-2,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3},
    {-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3},
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3},

    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3},
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3},
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3,-3,-3,-3},
    {-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-1,-3,-3,-3,-3},

    {-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-1,-3,-3,-3,-3},
    {-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-1,-3,-3,-3,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-1,-3},
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-1,-3},

    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-1,-3},
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-1,-3},
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-1,-3},
    {-3,-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-1,-3,-3,-3},

    {-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-1,-3,-3,-3},
    {-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-1,-3,-3,-3},
    {-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-1,-3,-3,-3},
    {-3,-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3,-3,-3,-3,-3,-1,-3,-3,-3},
    {-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-1,-3,-3,-3},

    {-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-1,-3,-3,-3},
    {-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-1,-3,-3,-3},
    {-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-1,-3,-3,-3},
    {-3,-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3,-3,-3},
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3},
};
int tempmap[30][30];
int map1[30][30] =
{
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3},
    {-3,-2,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3},
    {-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3},
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3},

    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3},
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3},
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-1,-3,-3,-1,-3},

    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-1,-3,-3,-1,-3},
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-1,-3,-3,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3},
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3},

    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3},
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3},
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3},
    {-3,-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3},
    {-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3},

    {-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3},
    {-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3},
    {-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3},
    {-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3},
    {-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-3,-3,-3},

    {-3,-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3,-3},
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-1,-3,-3},
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-3,-3,-3,-3,-3,-3,-1,-3,-3},
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-1,-1,-1,-1,-1,-1,-1,-1,-3,-3},
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3},
};
int map2[30][30] =
{
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-3},
    {-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3}
};

int skillMap[skillCount][skillCount] = {
                                        {0,0,1,0,0,0,0,0,0,0},
                                        {0,0,1,0,0,0,0,0,0,0},
                                        {0,0,0,1,1,0,0,0,0,0},
                                        {0,0,0,0,0,1,0,0,0,0},
                                        {0,0,0,0,0,1,0,0,0,0},
                                        {0,0,0,0,0,0,1,1,0,0},
                                        {0,0,0,0,0,0,0,0,1,0},
                                        {0,0,0,0,0,0,0,0,1,0},
                                        {0,0,0,0,0,0,0,0,0,1},
                                        {0,0,0,0,0,0,0,0,0,0}
};     //스킬트리 순서, 1->2 순서대로 스킬을 찍어야 한다면 skillMap[1][2] = 1 로 설정해야 함.

int skillCheck[skillCount];
int skillActive[skillCount]; //이미 찍은 스킬이면 1 안찍은 스킬이면 0

int skillUpgradeCoin[skillCount] = { 100,100,200,200,200,300,300,300,400,1000 };
int skillUpgradeBag[skillCount] = { 0,2,0,0,2,0,0,2,0,0 };
int skillUpgradeMove[skillCount] = { 20,0,0,20,0,0,20,0,0,0 };
int skillInput[3];

int bagWeight; //가방무게
int myBagScore;
int myBagWeight; //현재 가방 무게게
int maxBag;
int playerSpeed; //속도

int input;

int x;
int y;
targetData target;
targetData targetHeapArray[100];
int heapSize;
int move[4][2] = { {-1,0}, {1,0}, {0,-1}, {0,1} };

int minStep;
int moveStep;


int isGameClear;

void gamePlayScene();

int _max(int x, int y)
{
    return x > y ? x : y;
}

void heapSet(targetData data)
{
    heapSize++;
    int cur = heapSize;

    while (cur != 1)
    {
        if (targetHeapArray[cur / 2].num > data.num)
        {
            targetHeapArray[cur] = targetHeapArray[cur / 2];
            cur /= 2;
        }
        else {
            break;
        }
    }
    targetHeapArray[cur] = data;
}
void heapGet()
{
    target = targetHeapArray[1];
    targetHeapArray[1] = targetHeapArray[heapSize];
    heapSize--;

    int parent = 1;
    int sun = 2;

    while (sun <= heapSize)
    {
        if (sun + 1 <= heapSize && targetHeapArray[sun + 1].num < targetHeapArray[sun].num)
        {
            sun++;
        }

        if (targetHeapArray[parent].num > targetHeapArray[sun].num)
        {
            targetData temp = targetHeapArray[parent];
            targetHeapArray[parent] = targetHeapArray[sun];
            targetHeapArray[sun] = temp;

            parent = sun;
            sun *= 2;
        }
        else
        {
            break;
        }
    }
}

void bfs(int n)
{
    int queue[1000][2];
    int left = 0;
    int right = 0;

    int distMaps[30][30];
    for (int i = 0; i < 30; i++)
    {
        for (int j = 0; j < 30; j++)
        {
            distMaps[i][j] = 100000;
        }
    }
    distMaps[objectPos[n][0]][objectPos[n][1]] = 0;

    queue[right][0] = objectPos[n][0];
    queue[right][1] = objectPos[n][1];
    right++;

    while (left < right)
    {
        int curY = queue[left][0];
        int curX = queue[left][1];
        left++;

        for (int i = 0; i < 4; i++)
        {
            int nextY = curY + move[i][0];
            int nextX = curX + move[i][1];

            if (nextY < 0 || nextX < 0 || 30 <= nextY || 30 <= nextX)
            {
                continue;
            }

            if (map[nextY][nextX] == -3)
            {
                continue;
            }

            if (distMaps[curY][curX] + 1 < distMaps[nextY][nextX])
            {
                queue[right][0] = nextY;
                queue[right][1] = nextX;
                right++;

                distMaps[nextY][nextX] = distMaps[curY][curX] + 1;

                if (0 <= map[nextY][nextX])
                {
                    objectDist[n][map[nextY][nextX]] = distMaps[nextY][nextX];
                }
            }
        }
    }
}

int bitMasking(int cur, int bit, int level)
{
    if (level == objectHaveCountThisTurn - 1)
    {
        return objectDist[objectHaveIndex[cur]][objectHaveIndex[0]] != 0 ? objectDist[objectHaveIndex[cur]][objectHaveIndex[0]] : 100000000;
    }
    if (tspDP[cur][bit] != 0)
    {
        return tspDP[cur][bit];
    }

    int minDist = 100000000;
    for (int i = 1; i < objectHaveCountThisTurn; i++)
    {
        if ((bit & (1 << i)) == 0 && objectDist[objectHaveIndex[cur]][objectHaveIndex[i]] != 0)
        {
            int data = bitMasking(i, bit | (1 << i), level + 1);
            if (objectDist[objectHaveIndex[cur]][objectHaveIndex[i]] + data < minDist)
            {
                minDist = objectDist[objectHaveIndex[cur]][objectHaveIndex[i]] + data;
            }
        }
    }
    tspDP[cur][bit] = minDist;

    return minDist;
}

int tsp()
{
    tspDP = (int**)malloc(sizeof(int*) * objectHaveCountThisTurn);
    for (int i = 0; i < objectHaveCountThisTurn; i++)
    {
        tspDP[i] = (int*)malloc(sizeof(int) * (1 << objectHaveCountThisTurn));
    }

    for (int i = 0; i < objectHaveCountThisTurn; i++)
    {
        for (int j = 0; j < (1 << objectHaveCountThisTurn); j++)
        {
            tspDP[i][j] = 0;
        }
    }

    return bitMasking(0, 0, 0); //시작점을 objectVisitThisTurn에서 가장 가까운 1
}

void init()
{
    gameScene = 0;
    coin = 0;
    score = 0;
    clearCoin = 3000;
    allcoin = 0;
    day = 1;
    skillInput[0] = 0;
    skillInput[1] = 1;
    skillInput[2] = 2;


    bagWeight = 10;
    playerSpeed = 100;

    for (int i = 0; i < skillCount; i++)
    {
        skillCheck[i] = 0;
        skillActive[i] = 0;
    }
    for (int i = 0; i < skillCount; i++)
    {
        for (int j = 0; j < skillCount; j++)
        {
            if (skillMap[i][j] == 1)
            {
                skillCheck[j] += 1;
            }
        }
    }
}

void initDays()
{
    for (int i = 0; i < 30; i++)
    {
        for (int j = 0; j < 30; j++)
        {
            map[i][j] = defaultMap[i][j];
        }
    }

    srand(time(NULL));
    for (int i = 0; i <= objectSpawnCount; i++)
    {
        if (i != objectSpawnCount)
        {
            int thisWeight = rand() % 9 + 1;
            int thisScore = rand() % 30 + 1;

            object[i].num = i;
            object[i].weight = thisWeight;
            object[i].score = thisScore;

            objectVisit[i] = 0;
            objectVisitThisTurn[i] = 0;

            while (1)
            {
                int y = rand() % 30;
                int x = rand() % 30;

                if (map[y][x] == -1)
                {
                    map[y][x] = i;
                    objectPos[i][0] = y;
                    objectPos[i][1] = x;
                    break;
                }
            }
        }
        else
        {
            object[i].num = i;
            object[i].weight = 0;
            object[i].score = 0;

            map[1][1] = i;
            objectPos[i][0] = 1;
            objectPos[i][1] = 1;
        }
        for (int j = 0; j <= objectSpawnCount; j++)
        {
            objectDist[i][j] = 0;
        }
    }
    objectHaveCount = objectSpawnCount;

    for (int i = 0; i <= objectSpawnCount; i++)
    {
        bfs(i);
    }
}

void initGame()
{
    x = 1;
    y = 1;
    isGameClear = 0;
    gameScene = 2;
    moveStep = 0;

    maxBag = knap();

    heapSize = 0;

    objectHaveIndex = (int*)malloc(sizeof(int) * objectHaveCountThisTurn);
    int cur = 0;

    for (int i = 0; i <= objectSpawnCount; i++)
    {
        if (objectVisitThisTurn[i] == 1)
        {
            objectVisitThisTurn[i] = 0;
            objectVisit[i] = 1;
            objectHaveCount -= 1;
            objectHaveIndex[cur] = i;
            cur++;

            targetData tempData;
            tempData.num = i;
            tempData.y = objectPos[i][0];
            tempData.x = objectPos[i][1];

            heapSet(tempData);
        }
    }
    objectHaveCount += 1;

    heapGet();

    minStep = tsp();
}

int knap()
{
    int** bagTable = (int**)malloc(sizeof(int*) * (objectHaveCount + 1));
    for (int i = 0; i <= objectHaveCount; i++)
    {
        bagTable[i] = (int*)malloc(sizeof(int) * (bagWeight + 1));
    }

    int cur = 0;
    for (int i = 0; i <= objectHaveCount; i++)
    {
        for (int j = 0; j <= bagWeight; j++)
        {
            bagTable[i][j] = 0;
        }
    }

    for (int i = 0; i < objectSpawnCount; i++)
    {
        if (objectVisit[i] == 1)
        {
            continue;
        }
        cur++;

        for (int j = 1; j <= bagWeight; j++)
        {
            if (j < object[i].weight)
            {
                bagTable[cur][j] = bagTable[cur - 1][j];
                continue;
            }
            bagTable[cur][j] = _max(bagTable[cur - 1][j], bagTable[cur - 1][j - object[i].weight] + object[i].score);
        }
    }
    return bagTable[objectHaveCount][bagWeight];
}

int skillChecker(int n) //스킬을 찍을수 있는 상태라면 위상정렬 후 1 반환, 찍을수 없다면 0 반환
{
    if (skillActive[n] == 1)
    {
        return 0;
    }
    if (coin < skillUpgradeCoin[n])
    {
        return 0;
    }
    if (skillCheck[n] == 0)
    {
        for (int i = 0; i < skillCount; i++)
        {
            if (skillMap[n][i] == 1)
            {
                skillCheck[i] -= 1;
            }
        }
        skillActive[n] = 1;
        coin -= skillUpgradeCoin[n];
        return 1;
    }
    return 0;
}

void bagChange()
{
    int target = input - 'a';

    if (objectVisit[target] == 1)
    {
        return;
    }

    if (objectVisitThisTurn[target] == 1)
    {
        objectVisitThisTurn[target] = 0;
        myBagWeight -= object[target].weight;
        myBagScore -= object[target].score;
        objectHaveCountThisTurn--;
    }
    else
    {
        if (bagWeight - myBagWeight >= object[target].weight)
        {
            objectVisitThisTurn[target] = 1;
            myBagWeight += object[target].weight;
            myBagScore += object[target].score;
            objectHaveCountThisTurn++;
        }
        else
        {
        }
    }
}

void skillChange()
{
    int inp = input - 'a';
    int target = skillInput[inp];

    if (skillCount <= target)
    {
        return;
    }

    int checker = skillChecker(target);
    if (checker == 1)
    {
        bagWeight += skillUpgradeBag[target];
        playerSpeed += skillUpgradeMove[target];
        skillInput[inp] += 3;
    }
}


//인게임 관련 함수
void gotoxy(int x, int y)
{
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}
int keyControl() {
    if ((input == 'w') || (input == 'W')) {
        return UP;
    }
    else if ((input == 'a') || (input == 'A')) {
        return LEFT;
    }
    else if ((input == 's') || (input == 'S')) {
        return DOWN;
    }
    else if ((input == 'd') || (input == 'D')) {
        return RIGHT;
    }

}


// 각 씬에 필요한 UI그리기 함수
void drawGameSceneUI() {

    gotoxy(MAP_X + 40, MAP_Y);
    printf(" < 물건을 배달하고 출발지로 돌아오세요 > ");
    gotoxy(MAP_X + 40, MAP_Y + 2);
    printf(" ◇ W,A,S,D To Move    ");
    gotoxy(MAP_X + 40, MAP_Y + 4);
    printf(" ◇ 이동거리 : %d ", moveStep);
    gotoxy(MAP_X + 40, MAP_Y + 6);
    printf(" ◇ 남은 방문지 수 : %d ", heapSize);
    gotoxy(MAP_X + 40, MAP_Y + 8);
    if (1 <= heapSize)
    {
        printf(" ◇ 다음 목표 방문지 : %c", 'A' + target.num);
    }
    else
    {
        printf(" ◇ 배달을 전부 마쳤습니다. 출발지로 돌아가세요.");
    }

    gotoxy(MAP_X + 70, MAP_Y + 2);
    printf(" ◇ 현재 소지금액 : %d원", coin);
    gotoxy(MAP_X + 70, MAP_Y + 4);
    printf(" ◇ 배달을 마치고 받을 수 있는 기본금액 : %d원", (objectHaveCountThisTurn - 1) * 50);

}
void drawReadySceneUI() {
    int index = 0;
    int sum = 0;

    gotoxy(MAP_X + 35, MAP_Y);
    printf(" < 화물 허브 > ");
    gotoxy(MAP_X + 35, MAP_Y + 2);
    printf(" ◇ 가방의 용량 이내만큼 물건을 선택하세요. ( 가방용량 : %d kg)  ", bagWeight);
    gotoxy(MAP_X + 35, MAP_Y + 4);
    printf(" ================================================================");
    for (int i = 0; i < objectSpawnCount; i++)
    {
        gotoxy(MAP_X + 35, MAP_Y + 6 + (i * 2));
        char tempChar;
        if (objectVisit[i] == 1)
        {
            tempChar = 'X';
        }
        else if (objectVisitThisTurn[i] == 1)
        {
            tempChar = 'v';
        }
        else
        {
            tempChar = ' ';
        }
        printf("%c ◇ 물건 %c : %d kg  가치 : %d", tempChar, 'A' + i, object[i].weight, object[i].score);
    }


    gotoxy(MAP_X + 70, MAP_Y + 6);
    printf(" ◇ 물건 입력 : A ~ F");
    gotoxy(MAP_X + 70, MAP_Y + 8);
    printf(" ◇ 선택한 물건의 무게 총합 : %d", myBagWeight);
    gotoxy(MAP_X + 70, MAP_Y + 10);
    printf(" ◇ 선택한 물건의 무게 총가치 : %d", myBagScore);

    gotoxy(MAP_X + 62, MAP_Y + 18);
    printf("-------------------** 주의사항 **---------------------");
    gotoxy(MAP_X + 62, MAP_Y + 19);
    printf("|                                                     |");
    gotoxy(MAP_X + 62, MAP_Y + 20);
    printf("| 가방에 넣은 물건은 최소 힙 구조로 되어있습니다.     |");
    gotoxy(MAP_X + 62, MAP_Y + 21);
    printf("|                                                     |");
    gotoxy(MAP_X + 62, MAP_Y + 22);
    printf("| 물건은 알파벳 순서대로 정렬됩니다.                  |");
    gotoxy(MAP_X + 62, MAP_Y + 23);
    printf("|                                                     |");
    gotoxy(MAP_X + 62, MAP_Y + 24);
    printf("| 물건은 알파벳 순서대로 가방에서 꺼낼 수 있습니다.   |");
    gotoxy(MAP_X + 62, MAP_Y + 25);
    printf("|                                                     |");
    gotoxy(MAP_X + 62, MAP_Y + 26);
    printf("| 선택한 물건들 중 알파벳이 빠른 순서대로 배달.       |");
    gotoxy(MAP_X + 62, MAP_Y + 27);
    printf("|                                                     |");
    gotoxy(MAP_X + 62, MAP_Y + 28);
    printf("------------------------------------------------------");
}
void drawSkillSceneUI() {
    system("cls");
    gotoxy(MAP_X + 5, MAP_Y);
    printf(" < 업그레이드 상점 > ");
    gotoxy(MAP_X + 5, MAP_Y + 2);
    printf(" =============================================================================================================");
    gotoxy(MAP_X + 5, MAP_Y + 4);
    printf(" A. 오토바이 업그레이드 Lv.%d", skillInput[0] / 3 + 1);
    if (skillCount <= skillInput[0])
    {
        printf("(완료)");
    }
    printf("\n");
    gotoxy(MAP_X + 5, MAP_Y + 6);
    printf("    o    \n");
    gotoxy(MAP_X + 5, MAP_Y + 7);
    printf("   (`-&  \n");
    gotoxy(MAP_X + 5, MAP_Y + 8);
    printf(" (*)'(*) \n");
    gotoxy(MAP_X + 5, MAP_Y + 9);
    printf("-----------------------------------------------------------------");
    gotoxy(MAP_X + 5, MAP_Y + 10);
    printf("| 효과 : 이동속도 %d 증가                                        |", skillUpgradeMove[skillInput[0]]);
    gotoxy(MAP_X + 5, MAP_Y + 11);
    printf("| 금액 : %d원                                                  |", skillUpgradeCoin[skillInput[0]]);
    gotoxy(MAP_X + 5, MAP_Y + 12);
    printf("| 오토바이 업그레이드를 구매하면 오토바이의 이동속도가 증가한다.|");
    gotoxy(MAP_X + 5, MAP_Y + 13);
    printf("| 더 빠르게 물건을 배달할 수 있다.                              |");
    gotoxy(MAP_X + 5, MAP_Y + 14);
    printf("-----------------------------------------------------------------");

    gotoxy(MAP_X + 5, MAP_Y + 16);
    printf(" B. 가방 업그레이드 Lv.%d", skillInput[0] / 3 + 1);
    if (skillCount <= skillInput[1])
    {
        printf("(완료)");
    }
    printf("\n");
    gotoxy(MAP_X + 5, MAP_Y + 17);
    printf("    ___    \n");
    gotoxy(MAP_X + 5, MAP_Y + 18);
    printf("   |   | \n");
    gotoxy(MAP_X + 5, MAP_Y + 19);
    printf("o---------o \n");
    gotoxy(MAP_X + 5, MAP_Y + 20);
    printf("|   Bag   | \n");
    gotoxy(MAP_X + 5, MAP_Y + 21);
    printf("|         | \n");
    gotoxy(MAP_X + 5, MAP_Y + 22);
    printf("o---------o \n");
    gotoxy(MAP_X + 5, MAP_Y + 23);
    printf("-----------------------------------------------------------------");
    gotoxy(MAP_X + 5, MAP_Y + 24);
    printf("| 효과 : 가방 용량 %d증가                                       |", skillUpgradeBag[skillInput[1]]);
    gotoxy(MAP_X + 5, MAP_Y + 25);
    printf("| 금액 : %d원                                                  |", skillUpgradeCoin[skillInput[1]]);
    gotoxy(MAP_X + 5, MAP_Y + 26);
    printf("| 가방 업그레이드 구매 시 가방의 최대용량 증가                  |");
    gotoxy(MAP_X + 5, MAP_Y + 27);
    printf("| 더 많은 물건, 더 무거운 물건을 배달할 수 있다.                |");
    gotoxy(MAP_X + 5, MAP_Y + 28);
    printf("-----------------------------------------------------------------");

    gotoxy(MAP_X + 80, MAP_Y + 6);
    printf(" C. 현재 업그레이드 등급 : %d  ", (skillInput[2] / 3));
    if (skillCount <= skillInput[2])
    {
        printf("(완료)");
    }
    printf("\n");
    gotoxy(MAP_X + 80, MAP_Y + 7);
    printf("등급에 따라 업그레이드 가능 범위 존재\n");
    gotoxy(MAP_X + 80, MAP_Y + 9);
    printf("< 1등급 > 오토바이 +2, 가방 +2 까지\n");
    gotoxy(MAP_X + 80, MAP_Y + 10);
    printf("< 2등급 > 오토바이 +3, 가방 +3 까지\n");
    gotoxy(MAP_X + 80, MAP_Y + 11);
    printf("다음 업그레이드 금액 : %d원", skillUpgradeCoin[skillInput[2]]);

    gotoxy(MAP_X + 80, MAP_Y + 14);
    printf(" ◇ 현재 플레이어 현황         \n");
    gotoxy(MAP_X + 80, MAP_Y + 15);
    printf(" ______________________________\n");
    gotoxy(MAP_X + 80, MAP_Y + 16);
    printf(" - 오토바이 이동속도 : %d     \n", playerSpeed);
    gotoxy(MAP_X + 80, MAP_Y + 17);
    printf(" - 가방 최대 용량 : %d        \n", bagWeight);
    gotoxy(MAP_X + 80, MAP_Y + 18);
    printf(" - Gold : %d                  \n", coin);
    gotoxy(MAP_X + 80, MAP_Y + 19);
    printf(" ------------------------------\n");
    gotoxy(MAP_X + 40, MAP_Y);
    printf(" ◇ 구매할 업그레이드 선택 :  A B C");
}
void drawOpeningUI() {
    system("cls");
    printf(" \n\n\n\n               ");
    printf("  _   __ _                         __  ______        _  _                           \n               ");
    printf(" | | / /(_)                       / _| |  _  `      | |(_)                          \n               ");
    printf(" | |/ /  _  _ __    __ _    ___  | |_  | | | |  ___ | | _ __   __  ___  _ __  _   _ \n               ");
    printf(" |    ` | || '_ `  / _` |  / _ ` |  _| | | | | / _ `| || |` ` / / / _ `| '__|| | | |\n               ");
    printf(" | |ㄱ || || | | || (_| | | (_) || |   | |/ / |  __/| || | ` V / |  __/| |   | |_| |\n               ");
    printf(" |_| |_/|_||_| |_| `__, |  `___/ |_|   |___/   `___||_||_|  `_/   `___||_|    `__, |\n               ");
    printf("                    __/ |                                                      __/ |\n               ");
    printf("                   |___/                                                      |___/ \n               ");
    printf("                                    - 배달의왕족 -                                  \n               ");
    printf("                                                                _                   \n               ");
    printf("       o                                                       | ㄴ   _____         \n               ");
    printf("      (`-&                                                     | o | | ㅁ  |___     \n               ");
    printf("    (*)'(*)                                                    |   | |    ㅁ   |    \n               ");
    printf(" ===================================================================================\n               ");
    printf(" |                                                                                 |\n               ");
    printf(" |                             < PRESS ANY KEY TO START >                          |\n               ");
    printf(" |                                                                                 |\n               ");
    printf(" |                      Made By <에이플맞조> 조성주 김도경 김동영                  |\n               ");
    printf(" |                                                                                 |\n               ");
    printf(" ===================================================================================\n               ");
    printf(" \n     ");
    printf(" \n     ");
}
void drawHowToPlay() {
    system("cls");
    printf(" \n\n               ");
    printf("                                  < 게임 플레이 방법 >                               \n               ");
    printf(" \n               ");
    printf(" ====================================================================================\n               ");
    printf(" |                                                                                  |\n               ");
    printf(" | 돈을 모아서 건물주가 되어보자!                                                   |\n               ");
    printf(" |                                                                                  |\n               ");
    printf(" | 1. 플레이어는 물건 배달을 통해 건물을 구입할 금액을 모아야 합니다                |\n               ");
    printf(" |                                                                                  |\n               ");
    printf(" | 2. 물건 배달을 마치면 일당이 주어집니다.                                         |\n               ");
    printf(" |                                                                                  |\n               ");
    printf(" | 3. 얼마나 빠르게, 얼마나 많은 물건을 배달했는지에 따라 보너스 금액이 주어집니다. |\n               ");
    printf(" |                                                                                  |\n               ");
    printf(" | 4. 보너스 금액으로는 배달 오토바이와 가방을 업그레이드 할 수 있습니다.           |\n               ");
    printf(" |                                                                                  |\n               ");
    printf(" | 5. 최종 목표인 건물의 금액만큼 모으면 GAME CLEAR !!!                             |\n               ");
    printf(" |                                                                                  |\n               ");
    printf(" ====================================================================================\n               ");
    printf(" \n\n               ");
    printf("                               < PRESS ANY KEY TO START >                            \n               ");


}
void drawDayEndUI(int defcoin, int distcoin, int bagcoin) {
    system("cls");
    printf(" \n\n               ");
    printf("                                  < %d일차 배달 기록 >                               \n               ", day);
    printf(" \n               ");
    printf(" ====================================================================================\n               ");
    printf("                                                                                   \n               ");
    printf("  당신은 오늘 할당된 배달을 완료했습니다                                           \n               ");
    printf("                                                                                   \n               ");
    printf("  1. 배달 완료 기본 지급 금액 : %d                                                 \n               ", defcoin);
    printf("                                                                                   \n               ");
    printf("  2. 최단거리 보너스 금액 : %d                                                     \n               ", distcoin);
    printf("                                                                                   \n               ");
    printf("  4. 물건 보너스 금액 : %d                                                         \n               ", bagcoin);
    printf("                                                                                   \n               ");
    printf("  5. %d일차 총 일당 은 < %d > 원 입니다                                            \n               ", day, defcoin + distcoin + bagcoin);
    printf("                                                                                   \n               ");
    printf("  4. 목표까지 남은 금액 : %d                                                       \n               ", clearCoin - allcoin);
    printf("                                                                                   \n               ");
    printf(" ====================================================================================\n               ");
    printf("                                                                                     \n               ");
    printf(" 오늘 하루 배달을 무사히 마쳤다... 목표 금액을 모으기 위해 내일도 열심히 일하자!!    \n               ");
    printf("                                                                                     \n               ");
    printf("                                                                                     \n               ");
    printf(" \n\n               ");
    printf("                               < PRESS ANY KEY TO SKIP >                            \n               ");
}
void drawEndingUI() {
    system("cls");
    printf(" \n\n\n\n               ");
    printf("         _____                           _____  _                                   \n               ");
    printf("        |  __ `                         /  __ `| |                                  \n               ");
    printf("        | |  |/  __ _  _ __ ___    ___  | /  |/| |  ___   __ _  _ __                \n               ");
    printf("        | | __  / _` || '_ ` _ `  / _ ` | |    | | / _ ` / _` || '__|               \n               ");
    printf("        | |_| `| (_| || | | | | ||  __/ | `__/|| ||  __/| (_| || |                  \n               ");
    printf("         `____/ `__,_||_| |_| |_| `___|  `____/|_| `___| `__,_||_|                  \n               ");
    printf("                                                                                    \n               ");
    printf("                                  - 배달의왕족 -                                    \n               ");
    printf("                                                                _                   \n               ");
    printf("       o                                                       | ㄴ   _____         \n               ");
    printf("      (`-&                                                     | o | | ㅁ  |___     \n               ");
    printf("    (*)'(*)                                                    |   | |    ㅁ   |    \n               ");
    printf(" ===================================================================================\n               ");
    printf("                               당신의 점수 : %d                                     \n               ", score);
    printf("                게임을 즐겨주셔서 감사합니다. 게임이 끝났습니다.                    \n               ");
}

// GameScene과 ReadyScene에서 맵을 그리는 함수
void drawGameSceneMap() {
    system("cls");
    int i, j;
    // 선택한 맵을 복사해서 사용
    for (i = 0; i < 30; i++) {
        for (j = 0; j < 30; j++) {
            tempmap[i][j] = map[i][j];
        }
    }

    // 맵 출력
    for (i = 0; i < 30; i++) {
        for (j = 0; j < 30; j++) {
            int temp = tempmap[i][j];

            if (i == y && j == x)
            {
                printf("P");
            }
            else if (target.y == i && target.x == j)
            {
                char c = 'A' + target.num;
                printf("%c", c);
            }
            else if (temp == objectSpawnCount) {
                printf("q");
            }
            else if (temp == -3)
            {
                printf("■");
            }
            else
            {
                printf(" ");
            }
        }
        printf("\n");
        //ui 출력
    }
    drawGameSceneUI();
}
void drawReadySceneMap() {
    system("cls");
    int i, j;
    // 선택한 맵을 복사해서 사용
    for (i = 0; i < 30; i++) {
        for (j = 0; j < 30; j++) {
            tempmap[i][j] = map[i][j];
        }
    }


    // 맵 출력
    printf("  o------------------------------o\n");
    for (i = 0; i < 30; i++) {
        printf("  |");
        for (j = 0; j < 30; j++) {
            int temp = tempmap[i][j];
            if (temp == -1) {
                printf(" ");
            }
            else if (temp == -3) {
                printf("■");
            }
            else if (temp == objectSpawnCount) {
                printf("q");
            }
            else
            {
                int tag = tempmap[i][j];
                if (objectVisit[tag] == 1)
                {
                    continue;
                }
                char tempC = 'A' + tempmap[i][j];
                printf("%c", tempC);
            }
        }
        printf("|");
        printf("\n");
    }
    printf("  o------------------------------o\n\n\n");
    //ui 출력
    printf("-------------------------------------------------------------------------------------------------------------------\n");
    printf("< 물건에 따른 배달주소가 적힌 지도 >\n");
    drawReadySceneUI();
}


void readySceneInput()
{
    if ('a' <= input && input < 'a' + objectSpawnCount)
    {
        bagChange();
    }
    else if (input == '1')
    {
        gameScene = 1;
    }
}

void skillSceneInput() // 스킬 씬의 입력 처리 
{
    if ('a' <= input && input <= 'c')
    {
        skillChange();
    }
    else if (input == '0')
    {
        gameScene = 0;
    }
}



void gameSceneInput()
{
    int ty = y;
    int tx = x;

    switch (keyControl()) {
    case UP:
        ty--;
        break;
    case DOWN:
        ty++;
        break;
    case LEFT:
        tx--;
        break;
    case RIGHT:
        tx++;
        break;
    }

    if (ty < 0 || tx < 0 || 30 <= ty || 30 <= tx || map[ty][tx] == -3)
    {
        return;
    }

    y = ty;
    x = tx;
    moveStep++;
}

void playerInput()
{
    switch (gameScene)
    {
    case 0:
    {
        readySceneInput();
        break;
    }
    case 1:
    {
        skillSceneInput();
        break;
    }
    case 2:
    {
        gameSceneInput();
        break;
    }
    }
}


// 각 씬의 UI출력 함수
void printGameSceneUI() {
    drawGameSceneMap();
}
void printReadySceneUI() {
    drawReadySceneMap();
}
void printSkillSceneUI() {
    drawSkillSceneUI();
}
void openingUI() {
    drawOpeningUI();
    _getch();
    drawHowToPlay();
    _getch();
}
void openingScriptUI() {

}
void dayEndsScriptUI(int defcoin, int distcoin, int bagcoin) {
    drawDayEndUI(defcoin, distcoin, bagcoin);
    _getch();
}
void endingUI() {
    drawEndingUI();
    _getch();
}

void gameClear()
{
    int reward = objectHaveCountThisTurn * 50;
    int distreward = 0;
    int bagreward = 0;

    if (maxBag == myBagScore)
    {
        bagreward = 100;
    }

    if (moveStep < minStep + (int)(playerSpeed * 0.2f))
    {
        distreward = reward;
    }

    int allreward = reward + distreward + bagreward;

    coin += allreward;
    allcoin += allreward;
    score += allreward;

    if (allcoin >= clearCoin)
    {
        endingUI();
        exit(1);
    }

    isGameClear = 1;
    if (objectHaveCount == 0)
    {
        initDays();
    }
    dayEndsScriptUI(reward, distreward, bagreward);
    day++;
}

void inGamePlay()
{
    if (x == target.x && y == target.y)
    {
        if (heapSize == 0)
        {
            gameClear();
        }
        else
        {
            heapGet();
        }
    }
}

void printUI() // 화면 출력 작성성
{
    switch (gameScene)
    {
    case 0:
    {
        printReadySceneUI(); // 레디씬 출력
        break;
    }
    case 1:
    {
        printSkillSceneUI(); // 스킬선택씬 출력
        break;
    }
    case 2:
    {
        printGameSceneUI(); // 게임씬 출력
        break;
    }
    }
}

void readyScene() //게임씬 제외 나머지 3개 씬 처리리
{
    myBagWeight = 0;
    myBagScore = 0;
    gameScene = 0;
    objectHaveCountThisTurn = 1;

    objectVisitThisTurn[objectSpawnCount] = 1;

    while (_kbhit())
    {
        _getch();
    }
    printUI();


    while (1)
    {
        if (_kbhit())
        {
            input = _getch();

            if (input == ESC)
            {
                exit(0);
            }
            else if (input == ENTER && objectHaveCountThisTurn != 1)
            {
                break;
            }
            else
            {
                playerInput();
            }
            printUI();
        }
    }

    gamePlayScene();
}

void gamePlayScene()
{
    initGame();

    while (_kbhit())
    {
        _getch();
    }

    printUI();

    while (1)
    {
        if (isGameClear == 1)
        {
            break;
        }
        if (_kbhit())
        {
            input = _getch();

            if (input == ESC)
            {
                exit(0);
            }
            else
            {
                playerInput();
                inGamePlay();
            }
            printUI();
        }
    }
    //라운드 종료 처리
    readyScene();
}


int main()
{
    system("mode con: cols=800 lines=8200 | title KingOfDelevery");

    openingUI();
    init();
    initDays();

    readyScene();

}