#include <bits/stdc++.h>
using namespace std;

// partition[u] = 1 means u is in partition-X
// partition[u] = 2 means v is in partition-Y
long long int getCutWeight(const vector<vector<pair<int, int>>> &graph, const vector<int> &partition)
{
    long long int weight = 0;
    int n = graph.size() - 1;
    for (int u = 1; u <= n; u++)
    {
        for (pair<int, int> temp : graph[u])
        {
            int v = temp.first, w = temp.second;
            if (u <= v)
            {
                if (partition[u] == 1 && partition[v] == 2 || partition[u] == 2 && partition[v] == 1)
                {
                    weight += w;
                }
            }
        }
    }
    return weight;
}

// sigmaX(v) = sum of weights of all edges (u,v) where u is in partition X
//  even if both v and u are in X
long long int getSigmaX(int v, const vector<vector<pair<int, int>>> &graph, const vector<int> &partition)
{
    long long int sigmaX = 0;
    for (pair<int, int> temp : graph[v])
    {
        int u = temp.first, w = temp.second;
        if (partition[u] == 1)
        {
            sigmaX += w;
        }
    }
    return sigmaX;
}

// sigmaY(v) = sum of weights of all edges (u,v) where u is in partition Y
//  even if both v and u are in Y
long long int getSigmaY(int v, const vector<vector<pair<int, int>>> &graph, const vector<int> &partition)
{
    long long int sigmaY = 0;
    for (pair<int, int> temp : graph[v])
    {
        int u = temp.first, w = temp.second;
        if (partition[u] == 2)
        {
            sigmaY += w;
        }
    }
    return sigmaY;
}

// randomized heuristic
double getRandomMaxCut(const vector<vector<pair<int, int>>> &graph)
{
    long long int totalCutWeight = 0;
    random_device seed; // generates a seed for random generation based on hardware entropy
    mt19937 gen(seed());
    /*
        mt19937 is the Mersen Twister. Its has better random number generation ability as it doesn't repeat until reaching 2^19937-1
        then gen generates a randon number from 0 to 2^19937-1 based on the seed
    */
    uniform_int_distribution<int> distribution(1, 2); // uniformly generates 1 or 2

    int n = graph.size() - 1;
    for (int i = 1; i <= n; i++)
    {
        vector<int> partition(n + 1);
        for (int u = 1; u <= n; u++)
        {
            partition[u] = distribution(gen);
        }
        totalCutWeight += getCutWeight(graph, partition);
    }
    return (double)totalCutWeight / n;
}

// greedy heuristic
double getGreedyMaxCut(const vector<vector<pair<int, int>>> &graph)
{
    // get the max weight
    int n = graph.size() - 1;
    int maxWeight = INT32_MIN;
    pair<int, int> maxEdge = {0, 0};
    for (int u = 1; u <= n; u++)
    {
        for (pair<int, int> temp : graph[u])
        {
            int v = temp.first, w = temp.second;
            if (w >= maxWeight)
            {
                maxWeight = w;
                maxEdge = {u, v};
            }
        }
    }
    // place them in two partitions
    vector<int> partition(n + 1, 0);
    partition[maxEdge.first] = 1, partition[maxEdge.second] = 2;
    for (int u = 1; u <= n; u++)
    {
        if (u == maxEdge.first || u == maxEdge.second)
            continue;
        // try to place u in X
        long long int sigmaY = getSigmaY(u, graph, partition), sigmaX = getSigmaX(u, graph, partition);
        if (sigmaY > sigmaX)
        {
            partition[u] = 1;
        }
        else
        {
            partition[u] = 2;
        }
    }
    return getCutWeight(graph, partition);
}

int main()
{
    freopen("set1/g1.rud", "r", stdin);
    freopen("output.txt", "w", stdout);
    int n, m; // n= #vertices, m=#edges
    cin >> n >> m;
    vector<vector<pair<int, int>>> graph(n + 1); // vertices start from 1
    for (int i = 0; i < m; i++)
    {
        int u, v, w;
        cin >> u >> v >> w;
        graph[u].push_back({v, w});
        graph[v].push_back({u, w});
    }

    for (int i = 1; i < n + 1; i++)
    {
        cout << i << "-> ";
        for (int j = 0; j < graph[i].size(); j++)
        {
            cout << "(" << graph[i][j].first << ", " << graph[i][j].second << ") ";
        }
        cout << "\n";
    }
}