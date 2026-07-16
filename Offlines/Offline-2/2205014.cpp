#include <bits/stdc++.h>
using namespace std;

random_device seed; // generates a seed for random generation based on hardware entropy
mt19937 gen(seed());
/*
    mt19937 is the Mersen Twister. Its has better random number generation ability as it doesn't repeat until reaching 2^19937-1
    then gen generates a randon number from 0 to 2^19937-1 based on the seed
*/

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

pair<int, int> getMaxEdge(const vector<vector<pair<int, int>>> &graph)
{
    int n = graph.size() - 1;
    int maxWeight = INT32_MIN;
    vector<pair<int, int>> maxEdges;
    for (int u = 1; u <= n; u++)
    {
        for (pair<int, int> temp : graph[u])
        {
            int v = temp.first, w = temp.second;
            if (w > maxWeight)
            {
                maxWeight = w;
            }
        }
    }
    for (int u = 1; u <= n; u++)
    {
        for (pair<int, int> temp : graph[u])
        {
            int v = temp.first, w = temp.second;
            if (w == maxWeight)
            {
                maxEdges.push_back({u, v});
            }
        }
    }
    uniform_int_distribution<int> edgeDistr(0, maxEdges.size() - 1);
    return maxEdges[edgeDistr(gen)];
}

// greedy heuristic
long long int getGreedyMaxCut(const vector<vector<pair<int, int>>> &graph)
{
    // get the max weight
    int n = graph.size() - 1;
    pair<int, int> maxEdge = getMaxEdge(graph);

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

// semi-greedy heuristic
long long int getSemiGreedyMaxCut(const vector<vector<pair<int, int>>> &graph, double alpha, vector<int> &partition)
{
    int n = graph.size() - 1;
    pair<int, int> maxEdge = getMaxEdge(graph);
    partition[maxEdge.first] = 1, partition[maxEdge.second] = 2;
    for (int i = 0; i < n - 2; i++)
    {
        long long int wmax = INT64_MIN, wmin = INT64_MAX;
        vector<long long int> g(n + 1, 0);
        for (int u = 1; u <= n; u++)
        {
            if (partition[u] != 0)
                continue;
            long long int sigmaY = getSigmaY(u, graph, partition);
            long long int sigmaX = getSigmaX(u, graph, partition);
            g[u] = max(sigmaX, sigmaY);
            wmax = max(wmax, max(sigmaX, sigmaY));
            wmin = min(wmin, min(sigmaX, sigmaY));
        }
        double threshold = wmin + alpha * (wmax - wmin);
        vector<int> RCL;
        for (int u = 1; u <= n; u++)
        {
            if (partition[u] != 0)
                continue;
            if (g[u] >= threshold)
            {
                RCL.push_back(u);
            }
        }
        uniform_int_distribution<int> distr(0, RCL.size() - 1);
        int u = RCL[distr(gen)];
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

// local search algorithm
long long int localSearch(const vector<vector<pair<int, int>>> &graph, vector<int> &partition, long long int &iterations, double &averageVal)
{
    averageVal = 0, iterations = 0;
    int n = graph.size() - 1;
    while (true)
    {
        iterations++;
        long long int maxDiff = INT64_MIN;
        int bestVertex = 0;
        for (int u = 1; u <= n; u++)
        {
            long long int sigmaY = getSigmaY(u, graph, partition);
            long long int sigmaX = getSigmaX(u, graph, partition);
            // u in partition X
            if (partition[u] == 1)
            {
                long long int diff = sigmaX - sigmaY;
                if (diff > maxDiff)
                {
                    maxDiff = diff;
                    bestVertex = u;
                }
            }
            // u in partition Y
            else if (partition[u] == 2)
            {
                long long int diff = sigmaY - sigmaX;
                if (diff > maxDiff)
                {
                    maxDiff = diff;
                    bestVertex = u;
                }
            }
        }
        if (maxDiff > 0)
        {
            if (partition[bestVertex] == 1)
                partition[bestVertex] = 2;
            else
                partition[bestVertex] = 1;
            averageVal += getCutWeight(graph, partition);
        }
        else
            break;
    }
    iterations--; // because even if there is no improvement in the last move, iteration will
    // get increased
    averageVal = (iterations != 0) ? averageVal / iterations: averageVal;
    // some iteration count can be 0 so handle them manually.
    return getCutWeight(graph, partition);
}

// finally the GRASP (Greedy Randomized Adaptive Search Procedure)
long long int GRASP(const vector<vector<pair<int, int>>> &graph, double alpha, int maxIteration, long long int &iteration)
{
    iteration = 0;
    long long int localIteration;
    double temp;
    int n = graph.size() - 1;
    long long int bestMaxCut = INT64_MIN;
    for (int i = 0; i < maxIteration; i++)
    {
        vector<int> partition(n + 1, 0);
        getSemiGreedyMaxCut(graph, alpha, partition);
        long long int localMaxCut = localSearch(graph, partition, localIteration, temp);
        bestMaxCut = max(bestMaxCut, localMaxCut);
        iteration += localIteration;
    }
    return bestMaxCut;
}
int main()
{
    cout << "Enter value of alpha[0.0-1.0]: ";
    double alpha;
    cin >> alpha;
    if (alpha < 0 || alpha > 1)
    {
        cout << "Invalid value of alpha.\n";
        return -1;
    }
    cout << "Enter number of max-iterations: ";
    int maxIteration;
    cin >> maxIteration;
    if (maxIteration < 0)
    {
        cout << "Max-Iteration can't be negative.\n";
        return -1;
    }

    int fileCount = 54;
    ofstream ofs("2205014.csv");
    ofs << "Name,|V| or n,|E| or m,Simple Randomized or Randomized-1,Simple Greedy or Greedy-1,Semi-greedy-1,Simple local or local-1 (No. of iterations),Simple local or local-1 (Average value),GRASP-1 (No. of iterations),GRASP-1 (Best value)\n";

    for (int i = 1; i <= 54; i++)
    {
        string fileName = "g" + to_string(i);
        ifstream ifs("set1/" + fileName + ".rud");
        if (!ifs.is_open())
        {
            cout << "Can't open the input file.\n";
            ofs.close();
            return -1;
        }
        int n, m;
        ifs >> n >> m;
        vector<vector<pair<int, int>>> graph(n + 1); // vertices start from 1
        for (int i = 0; i < m; i++)
        {
            int u, v, w;
            ifs >> u >> v >> w;
            graph[u].push_back({v, w});
            graph[v].push_back({u, w});
        }

        if (!ofs.is_open())
            ofs.open("2205014.csv", ios::app);

        double randomizedCut = getRandomMaxCut(graph);
        long long int greedyCut = getGreedyMaxCut(graph);
        vector<int> partition(n + 1, 0);
        long long int semiGreedyCut = getSemiGreedyMaxCut(graph, alpha, partition);
        long long int localIteration, totalIteration;
        double averageVal;
        long long int localCut = localSearch(graph, partition, localIteration, averageVal);
        long long int graspCut = GRASP(graph, alpha, maxIteration, totalIteration);
        ofs << fileName << "," << n << "," << m << "," << randomizedCut << "," << greedyCut << "," << semiGreedyCut << "," << localIteration << "," << averageVal << "," << totalIteration << "," << graspCut << "\n";
        ofs.close();
    }
}