#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>

const int INF = static_cast<int>(1e9);

struct Edge {
    int v;
    int flow;
    int C;
    int rev;
};

class Graph{
    int V;
    std::vector<int> level;
    std::vector<std::vector<Edge>> adj;
public:
    Graph(int V) : V(V), level(std::vector<int>(V)), adj(std::vector<std::vector<Edge>>(V)) {}

    void addEdge(int u, int v, int C) {
        Edge e{ v, 0, C, (int)adj[v].size()};
        Edge e_rev{ u, 0, 0, (int)adj[u].size()};
        adj[u].push_back(e);
        adj[v].push_back(e_rev);
    }

    bool BFS(int s, int t);
    int sendFlow(int s, int flow, int t, std::vector<int> &start);
    int DinicMaxflow(int s, int t);
};

bool Graph::BFS(int s, int t) {
    for (int i = 0; i < V; i++)
        level[i] = -1;
    level[s] = 0;
    std::queue<int> q;
    q.push(s);

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (Edge& e: adj[u]){
            if (level[e.v] < 0 && e.flow < e.C) {
                level[e.v] = level[u] + 1;
                q.push(e.v);
            }
        }
    }
    return level[t] >= 0;
}


int Graph::sendFlow(int u, int flow, int t, std::vector<int> &start) {
    if (u == t)
        return flow;
    for (; start[u] < adj[u].size(); start[u]++) {
        Edge& e = adj[u][start[u]];

        if (level[e.v] == level[u] + 1 && e.flow < e.C) {
            int curr_flow = std::min(flow, e.C - e.flow);
            int temp_flow = sendFlow(e.v, curr_flow, t, start);
            if (temp_flow > 0) {
                e.flow += temp_flow;
                adj[e.v][e.rev].flow -= temp_flow;
                return temp_flow;
            }
        }
    }
    return 0;
}

int Graph::DinicMaxflow(int s, int t) {
    if (s == t)
        return -1;
    int total = 0;
    while (BFS(s, t)) {
        std::vector<int> start(V);
        while (int flow = sendFlow(s, INF, t, start)) {
            total += flow;
        }
    }
    return total;
}

int main(){
    int V, E, s, t;
    std::cin >> V >> E >> s >> t;
    Graph G(V);
    while (E--){
        int u, v, C;
        std::cin >> u >> v >> C;
        G.addEdge(u, v, C);
    }
    std::cout << G.DinicMaxflow(s, t);
}

/*
2 1 0 1
0 1 1
 */