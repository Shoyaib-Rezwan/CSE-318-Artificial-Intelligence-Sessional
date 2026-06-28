#include <iostream>
#include <vector>
#include <cmath>
#include <queue>
#include <set>
#include <map>
#include <algorithm>
#include <memory>

using namespace std;

// Forward declaration of HeuristicStrategy to use in Board definition
class HeuristicStrategy;

// ==========================================
// 1. BOARD AND NODE REPRESENTATIONS
// ==========================================
struct Board {
    int k;
    vector<vector<int>> grid;
    int blankRow, blankCol; // 0-indexed positions

    Board(int size, const vector<vector<int>>& g) : k(size), grid(g) {
        for (int i = 0; i < k; ++i) {
            for (int j = 0; j < k; ++j) {
                if (grid[i][j] == 0) {
                    blankRow = i;
                    blankCol = j;
                }
            }
        }
    }

    bool isGoal() const {
        int expected = 1;
        for (int i = 0; i < k; ++i) {
            for (int j = 0; j < k; ++j) {
                if (i == k - 1 && j == k - 1) {
                    if (grid[i][j] != 0) return false;
                } else {
                    if (grid[i][j] != expected) return false;
                    expected++;
                }
            }
        }
        return true;
    }

    // Overloading operators for map/set indexing
    bool operator<(const Board& other) const {
        return this->grid < other.grid;
    }

    bool operator==(const Board& other) const {
        return this->grid == other.grid;
    }

    void print() const {
        for (int i = 0; i < k; ++i) {
            for (int j = 0; j < k; ++j) {
                cout << grid[i][j] << (j == k - 1 ? "" : " ");
            }
            cout << "\n";
        }
    }
};

struct Node {
    Board board;
    int g; // cost from start
    double f; // priority value: g + W * h
    shared_ptr<Node> parent;

    Node(Board b, int moves, double priority, shared_ptr<Node> p = nullptr)
        : board(b), g(moves), f(priority), parent(p) {}
};

struct CompareNode {
    bool operator()(const shared_ptr<Node>& a, const shared_ptr<Node>& b) {
        return a->f > b->f; // Min-heap based on f-value
    }
};

// ==========================================
// 2. HEURISTIC STRATEGY (Strategy Pattern)
// ==========================================
class HeuristicStrategy {
protected:
    pair<int, int> getGoalPos(int val, int k) const {
        if (val == 0) return {k - 1, k - 1};
        val--;
        return {val / k, val % k};
    }
public:
    virtual ~HeuristicStrategy() = default;
    virtual double calculate(const Board& board) const = 0;
};

class HammingDistance : public HeuristicStrategy {
public:
    double calculate(const Board& board) const override {
        int k = board.k;
        int distance = 0;
        int expected = 1;
        for (int i = 0; i < k; ++i) {
            for (int j = 0; j < k; ++j) {
                if (i == k - 1 && j == k - 1) continue; // skip last position checking for blank
                if (board.grid[i][j] != 0 && board.grid[i][j] != expected) {
                    distance++;
                }
                expected++;
            }
        }
        return distance;
    }
};

class ManhattanDistance : public HeuristicStrategy {
public:
    double calculate(const Board& board) const override {
        int k = board.k;
        int distance = 0;
        for (int i = 0; i < k; ++i) {
            for (int j = 0; j < k; ++j) {
                int val = board.grid[i][j];
                if (val != 0) {
                    auto [goalR, goalC] = getGoalPos(val, k);
                    distance += abs(i - goalR) + abs(j - goalC);
                }
            }
        }
        return distance;
    }
};

class LinearConflict : public HeuristicStrategy {
public:
    double calculate(const Board& board) const override {
        int k = board.k;
        ManhattanDistance md;
        double h_manhattan = md.calculate(board);
        int conflicts = 0;

        // Row conflicts
        for (int r = 0; r < k; ++r) {
            for (int c1 = 0; c1 < k; ++c1) {
                int t1 = board.grid[r][c1];
                if (t1 == 0) continue;
                auto [g_r1, g_c1] = getGoalPos(t1, k);
                if (g_r1 != r) continue; // Not in its goal row

                for (int c2 = c1 + 1; c2 < k; ++c2) {
                    int t2 = board.grid[r][c2];
                    if (t2 == 0) continue;
                    auto [g_r2, g_c2] = getGoalPos(t2, k);
                    if (g_r2 != r) continue; // Not in its goal row

                    if (g_c1 > g_c2) { // Linear Conflict detected
                        conflicts++;
                    }
                }
            }
        }

        // Column conflicts
        for (int c = 0; c < k; ++c) {
            for (int r1 = 0; r1 < k; ++r1) {
                int t1 = board.grid[r1][c];
                if (t1 == 0) continue;
                auto [g_r1, g_c1] = getGoalPos(t1, k);
                if (g_c1 != c) continue; // Not in its goal column

                for (int r2 = r1 + 1; r2 < k; ++r2) {
                    int t2 = board.grid[r2][c];
                    if (t2 == 0) continue;
                    auto [g_r2, g_c2] = getGoalPos(t2, k);
                    if (g_c2 != c) continue; // Not in its goal column

                    if (g_r1 > g_r2) { // Linear Conflict detected
                        conflicts++;
                    }
                }
            }
        }

        return h_manhattan + 2.0 * conflicts;
    }
};

// Custom Admissible Heuristic: Formulates Euclidean distance 
class CustomHeuristic : public HeuristicStrategy {
public:
    double calculate(const Board& board) const override {
        int k = board.k;
        double distance = 0;
        for (int i = 0; i < k; ++i) {
            for (int j = 0; j < k; ++j) {
                int val = board.grid[i][j];
                if (val != 0) {
                    auto [goalR, goalC] = getGoalPos(val, k);
                    distance += sqrt((i - goalR) * (i - goalR) + (j - goalC) * (j - goalC));
                }
            }
        }
        return distance; // True straight-line distance is mathematically <= Manhattan distance (Admissible)
    }
};

// ==========================================
// 3. SOLVABILITY DETECTOR
// ==========================================
bool isSolvable(const Board& board) {
    int k = board.k;
    vector<int> arr;
    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < k; ++j) {
            if (board.grid[i][j] != 0) {
                arr.push_back(board.grid[i][j]);
            }
        }
    }

    int inversions = 0;
    for (size_t i = 0; i < arr.size(); ++i) {
        for (size_t j = i + 1; j < arr.size(); ++j) {
            if (arr[i] > arr[j]) inversions++;
        }
    }

    if (k % 2 != 0) {
        // If k is odd, puzzle is solvable if inversions are even
        return (inversions % 2 == 0);
    } else {
        // If k is even, check blank position counted from bottom (1-indexed)
        int blankRowFromBottom = k - board.blankRow;
        if (blankRowFromBottom % 2 == 0) {
            return (inversions % 2 != 0);
        } else {
            return (inversions % 2 == 0);
        }
    }
}

// ==========================================
// 4. A* / WEIGHTED A* SEARCH ALGORITHM
// ==========================================
void solveNPuzzle(const Board& initialBoard, const HeuristicStrategy& heuristic, double weight, int& outNodesExplored) {
    outNodesExplored = 0;
    
    priority_queue<shared_ptr<Node>, vector<shared_ptr<Node>>, CompareNode> openList;
    set<vector<vector<int>>> closedList;

    double initial_h = heuristic.calculate(initialBoard);
    auto startNode = make_shared<Node>(initialBoard, 0, 0 + weight * initial_h);
    openList.push(startNode);

    // Direction arrays for moving blank spot: Up, Down, Left, Right
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    while (!openList.empty()) {
        auto curr = openList.top();
        openList.pop();

        if (closedList.count(curr->board.grid)) continue;
        closedList.insert(curr->board.grid);
        outNodesExplored++;

        if (curr->board.isGoal()) {
            // Track steps back to the root node
            vector<Board> path;
            auto trace = curr;
            while (trace != nullptr) {
                path.push_back(trace->board);
                trace = trace->parent;
            }
            reverse(path.begin(), path.end());

            cout << "Minimum number of moves = " << curr->g << "\n";
            for (const auto& step : path) {
                step.print();
                cout << "\n";
            }
            return;
        }

        // Neighbors discovery
        for (int i = 0; i < 4; ++i) {
            int nr = curr->board.blankRow + dr[i];
            int nc = curr->board.blankCol + dc[i];

            if (nr >= 0 && nr < curr->board.k && nc >= 0 && nc < curr->board.k) {
                vector<vector<int>> nextGrid = curr->board.grid;
                swap(nextGrid[curr->board.blankRow][curr->board.blankCol], nextGrid[nr][nc]);

                if (closedList.count(nextGrid)) continue;

                Board nextBoard(curr->board.k, nextGrid);
                int next_g = curr->g + 1;
                double next_h = heuristic.calculate(nextBoard);
                double next_f = next_g + weight * next_h;

                auto neighborNode = make_shared<Node>(nextBoard, next_g, next_f, curr);
                openList.push(neighborNode);
            }
        }
    }
}

// ==========================================
// 5. MAIN EXECUTION CONTROLLER
// ==========================================
int main() {
    int k;
    if (!(cin >> k)) return 0;

    vector<vector<int>> startGrid(k, vector<int>(k));
    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < k; ++j) {
            cin >> startGrid[i][j];
        }
    }

    Board initialBoard(k, startGrid);

    if (!isSolvable(initialBoard)) {
        cout << "Unsolvable puzzle\n";
        return 0;
    }

    // Initialize Heuristic selection 
    // You can swap dynamically between: 
    // 1. HammingDistance
    // 2. ManhattanDistance
    // 3. LinearConflict
    // 4. CustomHeuristic
    HammingDistance selectedHeuristic; 
    
    // Weight parameter W >= 1.0 (Set W = 1.0 for normal optimal A*)
    double W = 1.0; 
    int nodesExplored = 0;

    solveNPuzzle(initialBoard, selectedHeuristic, W, nodesExplored);

    return 0;
}