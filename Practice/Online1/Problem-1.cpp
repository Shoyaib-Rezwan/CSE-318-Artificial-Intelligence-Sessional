/*
    modify the assignment such that it's goal state is
    0 1 2
    3 4 5
    6 7 8
    same for all k*k puzzles with k=3,4,5,....

*/
#include <bits/stdc++.h>
using namespace std;

vector<pair<int, int>> goalPos;

struct Board
{
    vector<vector<int>> grid;
    double f, g, h;
    Board *parent;
    Board(const vector<vector<int>> &grid, double g, double h, double f, Board *parent = nullptr) : grid(grid), f(f), g(g), h(h), parent(parent)
    {
    }
};

class Heuristics
{

public:
    static void setGoalPos(int k)
    {
        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < k; j++)
            {
                goalPos.push_back(make_pair(i, j));
            }
        }
    }

    static double hammingDistance(const vector<vector<int>> &grid)
    {
        int k = grid.size();
        double cnt = 0;
        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < k; j++)
            {
                if (grid[i][j] > 0 && grid[i][j] != k * i + j + 1)
                {
                    cnt++;
                }
            }
        }
        return cnt;
    }

    static double manhattanDistance(const vector<vector<int>> &grid)
    {
        int k = grid.size();
        double cnt = 0.0;
        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < k; j++)
            {
                if (grid[i][j] == 0)
                    continue;
                cnt += abs(i - goalPos[grid[i][j]].first) + abs(j - goalPos[grid[i][j]].second);
            }
        }
        return cnt;
    }

    static double euclideanDistance(const vector<vector<int>> &grid)
    {
        int k = grid.size();
        double cnt = 0.0;
        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < k; j++)
            {
                if (grid[i][j] == 0)
                    continue;
                int i1 = (grid[i][j] - 1) / k, j1 = (grid[i][j] - 1) % k;
                cnt += sqrt((i1 - i) * (i1 - i) + (j1 - j) * (j1 - j));
            }
        }
        return cnt;
    }

    static double linearConflict(const vector<vector<int>> &grid)
    {
        int k = grid.size();
        int conflict = 0;

        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < k - 1; j++)
            {
                int left = grid[i][j];
                if (goalPos[left].first != i || left == 0)
                    continue;
                for (int l = j + 1; l < k; l++)
                {
                    int right = grid[i][l];
                    if (goalPos[right].first != i || right == 0)
                        continue;
                    if (left > right)
                        conflict++;
                }
            }
        }

        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < k - 1; j++)
            {
                int top = grid[j][i];
                if (goalPos[top].second != i || top == 0)
                    continue;
                for (int l = j + 1; l < k; l++)
                {
                    int bottom = grid[l][i];
                    if (goalPos[bottom].second != i || bottom == 0)
                        continue;
                    if (top > bottom)
                        conflict++;
                }
            }
        }
        return manhattanDistance(grid) + 2.0 * conflict;
    }

    static double custom(const vector<vector<int>> &grid)
    {
        int k = grid.size();
        // h(n)= linear_conflict+ 2*corner_conflict
        double cornerConflict = 0.0;

        // top-left
        cornerConflict += (grid[0][0] == 1 && grid[0][1] == k + 1 && grid[1][0] == 2) ? 1 : 0;

        // top right
        cornerConflict += (grid[0][k - 1] == k && grid[0][k - 2] == 2 * k && grid[1][k - 1] == k - 1) ? 1 : 0;

        // bottom-left
        cornerConflict += (grid[k - 1][0] == k * (k - 1) + 1 && grid[k - 1][1] == k * (k - 2) + 1 && grid[k - 2][0] == k * (k - 1) + 2) ? 1 : 0;
        return 2 * cornerConflict + linearConflict(grid);
    }
};

pair<int, int> getBlankPosition(const vector<vector<int>> &grid)
{
    int row = -1, col = -1, k = grid.size();
    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < k; j++)
        {
            if (grid[i][j] == 0)
            {
                row = i;
                col = j;
                break;
            }
        }
        if (row != -1)
            break;
    }
    return {row, col};
}

bool isSolvable(const vector<vector<int>> &grid)
{
    const int k = grid.size();
    int inversions = 0, cnt = 0;
    vector<int> arr(k * k);

    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < k; j++)
        {
            arr[cnt++] = grid[i][j];
        }
    }

    for (int i = 0; i < cnt; i++)
    {
        for (int j = i + 1; j < cnt; j++)
        {
            if (arr[i] > arr[j] && arr[j] != 0)
                inversions++;
        }
    }

    if (k % 2 && inversions % 2 == 0)
        return true;
    int blankRowTop = getBlankPosition(grid).first + 1;
    if (k % 2 == 0 && (blankRowTop % 2 && inversions % 2 == 0 || blankRowTop % 2 == 0 && inversions % 2))
        return true;
    return false;
}

struct Comparator
{
    bool operator()(const Board *a, const Board *b)
    {
        return a->f > b->f;
    }
};

bool isGoal(const vector<vector<int>> &grid)
{
    int k = grid.size();
    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < k; j++)
        {
            if (i != goalPos[grid[i][j]].first || j != goalPos[grid[i][j]].second)
                return false;
        }
    }

    return true;
}

bool isValidMove(int row, int col, int k)
{
    return row >= 0 && row < k && col >= 0 && col < k;
}

void printPath(Board *src, Board *dst)
{
    int k = src->grid.size();
    int cnt = 0;
    stack<Board *> path;
    Board *curr = dst;
    while (curr != src)
    {
        path.push(curr);
        curr = curr->parent;
        cnt++;
    }
    path.push(src);
    cout << "Minimum number of moves " << cnt << "\n\n";

    cnt = 0;
    while (!path.empty())
    {
        Board *board = path.top();
        path.pop();
        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < k; j++)
            {
                cout << board->grid[i][j] << ' ';
            }
            cout << '\n';
        }
        cout << "\n";
        free(board);
    }
}

string encodeGrid(vector<vector<int>> &grid)
{
    int k = grid.size();
    string s;
    s.reserve(k * k);
    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < k; j++)
        {
            s += (char)grid[i][j];
        }
    }
    return s;
}

void solveNPuzzle(vector<vector<int>> &grid, double (*heuristic)(const vector<vector<int>> &), unsigned long long int &explored, const double &weight)
{
    explored = 0;
    if (!isSolvable(grid))
    {
        cout << "Unsolvable puzzle\n";
        return;
    }
    int k = grid.size();
    priority_queue<Board *, vector<Board *>, Comparator> openList;
    unordered_set<string> closedList;
    double h = heuristic(grid);
    Board *startBoard = new Board(grid, 0.0, weight * h, 0 + weight * h);
    openList.push(startBoard);
    vector<int> di = {0, 0, 1, -1}, dj = {1, -1, 0, 0};
    while (!openList.empty())
    {
        explored++;
        Board *board = openList.top();
        openList.pop();
        string encodedGrid = encodeGrid(board->grid);
        if (closedList.count(encodedGrid))
        {
            free(board);
            continue;
        }
        closedList.insert(encodedGrid);

        if (isGoal(board->grid))
        {
            printPath(startBoard, board);

            while (!openList.empty())
            {
                Board *board = openList.top();
                openList.pop();
                free(board);
            }
            return;
        }

        pair<int, int> p = getBlankPosition(board->grid);
        int i = p.first, j = p.second;
        for (int m = 0; m < 4; m++)
        {
            int newI = i + di[m], newJ = j + dj[m];
            if (!isValidMove(newI, newJ, k))
                continue;
            vector<vector<int>> newGrid = board->grid;
            swap(newGrid[i][j], newGrid[newI][newJ]);
            if (closedList.count(encodeGrid(newGrid)))
                continue;
            h = heuristic(newGrid);
            Board *newBoard = new Board(newGrid, board->g + 1, weight * h, board->g + 1 + weight * h, board);
            openList.push(newBoard);
            
        }
    }
}

auto chooseHeuristic()
{
    cout << "\nType the corresponding number to choose a heuristic function:\n\n";
    cout << "1. Hamming Distance\n";
    cout << "2. Manhattan Distance\n";
    cout << "3. Euclidean Distance\n";
    cout << "4. Linear Conflict\n";
    cout << "5. Custom (Linear Conflict + 2*Corner Conflict)\n";
    cout << "\nEnter your choice: ";
    int choice = -1;
    cin >> choice;
    double (*funcPtr)(const vector<vector<int>> &) = nullptr;
    switch (choice)
    {
    case 1:
        funcPtr = Heuristics::hammingDistance;
        break;
    case 2:
        funcPtr = Heuristics::manhattanDistance;
        break;
    case 3:
        funcPtr = Heuristics::euclideanDistance;
        break;
    case 4:
        funcPtr = Heuristics::linearConflict;
        break;
    case 5:
        funcPtr = Heuristics::custom;
        break;

    default:
        break;
    }
    return funcPtr;
}

int main()
{
    int k;
    cin >> k;
    vector<vector<int>> grid(k, vector<int>(k));
    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < k; j++)
        {
            cin >> grid[i][j];
        }
    }

    Heuristics::setGoalPos(k);
    for (int i = 0; i < k * k; i++)
    {
        cout << goalPos[i].first << ' ' << goalPos[i].second << '\n';
    }
    double (*heuristic)(const vector<vector<int>> &) = chooseHeuristic();
    if (heuristic == nullptr)
    {
        cout << "Invalid heuristic function!!!\n";
        return -1;
    }
    unsigned long long int explored;
    solveNPuzzle(grid, heuristic, explored,1);
    if (explored)
    {
        cout << "\nTotal explored nodes: " << explored << '\n';
    }
    return 0;
}