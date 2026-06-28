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
private:
    static vector<pair<int, int>> getCurrentPosition(vector<vector<int>> &grid)
    {
        int k = grid.size();
        vector<pair<int, int>> current(k * k);
        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < k; j++)
            {
                current[grid[i][j]] = make_pair(i, j);
            }
        }
        return current;
    }
    static vector<pair<int, int>> getActualPosition(vector<vector<int>> &grid)
    {
        int k = grid.size();
        vector<pair<int, int>> actual(k * k);
        for (int i = 1; i < k * k; i++)
        {
            actual[i] = make_pair((i - 1) / k, (i - 1) % k);
        }
        return actual;
    }

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
        vector<pair<int, int>> current = getCurrentPosition(grid), actual = getActualPosition(grid);
        for (int i = 1; i < k * k; i++)
        {
            double x1 = current[i].first, y1 = current[i].second;
            double x2 = actual[i].first, y2 = actual[i].second;
            cnt += sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
        }
        return cnt;
    }

    static double linearConflict(vector<vector<int>> &grid)
    {
        int k = grid.size();
        int conflict = 0;
        vector<pair<int, int>> current = getCurrentPosition(grid), actual = getActualPosition(grid);
        for (int i = 1; i < k * k; i++)
        {
            for (int j = 1; j < k * k; j++)
            {
                if (current[i] < current[j])
                {
                    if (current[i].first == actual[i].first && current[j].first == actual[j].first && current[i].first == current[j].first && i > j)
                        conflict++;
                    if (current[i].second == actual[i].second && current[j].second == actual[j].second && current[i].second == current[j].second && i > j)
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
}

string encodeGrid(vector<vector<int>> &grid)
{
    int k = grid.size();
    string s;
    s.reserve(k*k);
    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < k; j++)
        {
            s+=(char)grid[i][j];
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