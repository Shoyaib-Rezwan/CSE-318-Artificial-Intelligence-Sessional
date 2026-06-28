#include <bits/stdc++.h>
using namespace std;

struct Board
{
    vector<vector<int>> grid;
    double f, g, h;
    shared_ptr<Board> parent;
    Board(const vector<vector<int>> &grid, double g, double h, double f, shared_ptr<Board> parent = nullptr) : grid(grid), f(f), g(g), h(h), parent(parent)
    {
    }
};

bool operator<(pair<int, int> &a, pair<int, int> &b)
{
    return a.first < b.first || a.first == b.first && a.second < b.second;
}

class Heuristics
{

public:
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
        double dist = 0;
        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < k; j++)
            {
                int val = grid[i][j];
                if (val > 0)
                {
                    int targetRow = (val - 1) / k;
                    int targetCol = (val - 1) % k;
                    dist += abs(i - targetRow) + abs(j - targetCol);
                }
            }
        }
        return dist;
    }

    static double euclideanDistance(vector<vector<int>> &grid)
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

    static double linearConflict(vector<vector<int>> &grid)
    {
        int k = grid.size();
        int conflict = 0;

        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < k - 1; j++)
            {
                int left = grid[i][j];
                if ((left - 1) / k != i || left == 0)
                    continue;
                for (int l = j + 1; l < k; l++)
                {
                    int right = grid[i][l];
                    if ((right - 1) / k != i || right == 0)
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
                if ((top - 1) % k != i || top == 0)
                    continue;
                for (int l = j + 1; l < k; l++)
                {
                    int bottom = grid[l][i];
                    if ((bottom - 1) % k != i || bottom == 0)
                        continue;
                    if (top > bottom)
                        conflict++;
                }
            }
        }
        return manhattanDistance(grid) + 2.0 * conflict;
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
    int blankRowBottom = k - getBlankPosition(grid).first;
    if (k % 2 == 0 && (blankRowBottom % 2 && inversions % 2 == 0 || blankRowBottom % 2 == 0 && inversions % 2))
        return true;
    return false;
}

struct Comparator
{
    bool operator()(const shared_ptr<Board> &a, const shared_ptr<Board> &b)
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
            if (grid[i][j] > 0 && grid[i][j] != k * i + j + 1)
                return false;
        }
    }
    if (grid[k - 1][k - 1] != 0)
        return false;
    return true;
}

bool isValidMove(int row, int col, int k)
{
    return row >= 0 && row < k && col >= 0 && col < k;
}

void printPath(shared_ptr<Board> src, shared_ptr<Board> dst)
{
    int k = src->grid.size();
    int cnt = 0;
    stack<shared_ptr<Board>> path;
    shared_ptr<Board> curr = dst;
    while (curr != src)
    {
        path.push(curr);
        curr = curr->parent;
        cnt++;
    }
    path.push(src);
    cout << "Minimum number of moves " << cnt << "\n\n";

    while (!path.empty())
    {
        vector<vector<int>> grid = path.top()->grid;
        path.pop();
        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < k; j++)
            {
                cout << grid[i][j] << ' ';
            }
            cout << '\n';
        }
        cout << "\n";
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

void solveNPuzzle(vector<vector<int>> &grid)
{
    if (!isSolvable(grid))
    {
        cout << "Unsolvable puzzle\n";
        return;
    }
    int k = grid.size();
    priority_queue<shared_ptr<Board>, vector<shared_ptr<Board>>, Comparator> openList;
    unordered_set<string> closedList;
    double h = Heuristics::linearConflict(grid);
    shared_ptr<Board> startBoard = make_shared<Board>(grid, 0.0, h, 0 + h);
    openList.push(startBoard);
    vector<int> di = {0, 0, 1, -1}, dj = {1, -1, 0, 0};
    while (!openList.empty())
    {
        shared_ptr<Board> board = openList.top();
        openList.pop();
        string encodedGrid = encodeGrid(board->grid);
        if (closedList.count(encodedGrid))
            continue;
        closedList.insert(encodedGrid);
        if (isGoal(board->grid))
        {
            printPath(startBoard, board);
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
            h = Heuristics::linearConflict(newGrid);
            shared_ptr<Board> newBoard = make_shared<Board>(newGrid, board->g + 1, h, board->g + 1 + h, board);
            openList.push(newBoard);
        }
    }
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

    solveNPuzzle(grid);
}