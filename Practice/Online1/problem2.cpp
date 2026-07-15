// use this template for practice problems

// destination patterns

/*
    1 2 3
    4 0 5
    6 7 8

    1 2 3 4
    5 0 6 7
    8 9 10 11
    12 13 14 15

*/
#include <bits/stdc++.h>
using namespace std;

const unsigned long long int THRESHOLD = 4000000;

map<int, pair<int, int>> dstPos;

void setDstPos(int k)
{
    int m = 1, blankRow = (k - 1) / 2, blankCol = (k - 1) / 2;

    dstPos[0] = make_pair(blankRow, blankCol);

    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < k; j++)
        {
            if (i == blankRow && j == blankCol)
                continue;
            dstPos[m++] = make_pair(i, j);
        }
    }
}

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
    static double hammingDistance(const vector<vector<int>> &grid)
    {
        int k = grid.size();
        double cnt = 0;
        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < k; j++)
            {
                if (grid[i][j] == 0)
                    continue;
                if (dstPos[grid[i][j]].first != i || dstPos[grid[i][j]].second != j)
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
                cnt += abs(dstPos[grid[i][j]].first - i) + abs(dstPos[grid[i][j]].second - j);
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
                int i1 = dstPos[grid[i][j]].first, j1 = dstPos[grid[i][j]].second;
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
                if (dstPos[left].second != i || left == 0)
                    continue;
                for (int l = j + 1; l < k; l++)
                {
                    int right = grid[i][l];
                    if (dstPos[right].second != i || right == 0)
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
                if (dstPos[top].second != i || top == 0)
                    continue;
                for (int l = j + 1; l < k; l++)
                {
                    int bottom = grid[l][i];
                    if (dstPos[bottom].second != i || bottom == 0)
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
        // h(n)= manhattan+2*(corner_conflict+ linear_conflict)
        double cornerConflict = 0.0;

        // top-left
        cornerConflict += (grid[0][0] == 1 && grid[0][1] == k + 1 && grid[1][0] == 2) ? 1 : 0;

        // top right
        cornerConflict += (grid[0][k - 1] == k && grid[0][k - 2] == 2 * k - 1 && grid[1][k - 1] == k - 1) ? 1 : 0;

        // bottom-left
        if (k == 3)
            cornerConflict += (grid[k - 1][0] == k * (k - 1) && grid[k - 1][1] == 4 && grid[k - 2][0] == k * (k - 1) + 1) ? 1 : 0;
        else
            cornerConflict += (grid[k - 1][0] == k * (k - 1) && grid[k - 1][1] == k * (k - 2) && grid[k - 2][0] == k * (k - 1) + 1) ? 1 : 0;

        // bottom right
        cornerConflict += (grid[k - 1][k - 1] == k * k - 1 && grid[k - 1][k - 2] == (k - 1) * k - 1 && grid[k - 2][k - 1] == k * k - 2) ? 1 : 0;

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
    int blankRowBottom = k - getBlankPosition(grid).first;
    if (k % 2 == 0 && (blankRowBottom + inversions) % 2 == (k / 2 + 1 + 0) % 2)
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
            if (i != dstPos[grid[i][j]].first || j != dstPos[grid[i][j]].second)
                return false;
        }
    }
    return true;
}

bool isValidMove(int row, int col, int k)
{
    return row >= 0 && row < k && col >= 0 && col < k;
}

int printPath(Board *src, Board *dst)
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
    cout << "Minimum number of moves = " << cnt << "\n\n";

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
        delete board;
    }
    return cnt;
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
void emptyOpenList(priority_queue<Board *, vector<Board *>, Comparator> &openList)
{
    while (!openList.empty())
    {
        Board *temp = openList.top();
        openList.pop();
        delete temp;
    }
}

void solveNPuzzle(vector<vector<int>> &grid, double (*heuristic)(const vector<vector<int>> &), unsigned long long int &explored, const double &weight, int &cost)
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

        // check of code exceeds the threshold
        if (explored > THRESHOLD)
        {
            cout << "Resource limit exceeded. Terminating solving puzzle......\n";
            emptyOpenList(openList);
            return;
        }

        explored++;
        Board *board = openList.top();
        openList.pop();
        string encodedGrid = encodeGrid(board->grid);
        if (closedList.count(encodedGrid))
        {
            delete board;
            continue;
        }
        closedList.insert(encodedGrid);

        if (isGoal(board->grid))
        {
            cost = printPath(startBoard, board);
            emptyOpenList(openList);
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
    int fileInput = -1;
    cout << "\nChoose I/O method:\n";
    cout << "\n1. Console";
    cout << "\n2. File\n";
    cout << "\nEnter your choice: ";
    cin >> fileInput;
    cin.ignore();

    // console IO
    if (fileInput == 1)
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

        setDstPos(k);

        for (int i = 0; i < k * k; i++)
        {
            cout << dstPos[i].first << ' ' << dstPos[i].second << '\n';
        }

        double (*heuristic)(const vector<vector<int>> &) = chooseHeuristic();
        if (heuristic == nullptr)
        {
            cout << "Invalid heuristic function!!!\n";
            return -1;
        }

        double weight;
        cout << "\nEnter a weight: ";
        cin >> weight;

        unsigned long long int explored;
        int cost = 0;
        solveNPuzzle(grid, heuristic, explored, weight, cost);
        // if (explored < THRESHOLD)
        // {
        //     cout << "\nTotal explored nodes: " << explored << '\n';
        //     cout << "\nSolution cost: " << cost << '\n';
        //     cout << "\nPerformance metric((Solution cost / exlored node) * 100%) = " << ((double)cost / double(explored)) * 100 << "%\n";
        // }
    }

    // file IO
    else if (fileInput == 2)
    {
        string input = "";
        cout << "\nEnter your input file name:";
        cin >> input;
        freopen(input.c_str(), "r", stdin);

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

        setDstPos(k);

        vector<string> output = {"hamming", "manhatton", "euclidean", "linearConflict", "custom"};
        vector<double (*)(const vector<vector<int>> &)> heuristics = {Heuristics::hammingDistance, Heuristics::manhattanDistance, Heuristics::euclideanDistance, Heuristics::linearConflict, Heuristics::custom};

        vector<double> weights = {1.0, 1.2, 2.0, 5.0};
        unsigned long long int explored;
        int cost = 0;
        for (int i = 0; i < heuristics.size(); i++)
        {
            for (int j = 0; j < weights.size(); j++)
            {
                string outputfile = output[i] + to_string(weights[j]) + ".txt";
                freopen(outputfile.c_str(), "w", stdout);
                solveNPuzzle(grid, heuristics[i], explored, weights[j], cost);

                // if (explored < THRESHOLD)
                // {
                //     cout << "\nTotal explored nodes: " << explored << '\n';
                //     cout << "\nSolution cost: " << cost << '\n';
                //     cout << "\nPerformance metric((Solution cost / exlored node) * 100%) = " << ((double)cost / double(explored)) * 100 << "%\n";
                // }
            }
        }
    }

    else
    {
        cout << "\nInvalid File format.\n";
        return -1;
    }
    return 0;
}