#include <iostream>
#include <vector>
#include <algorithm>

const int INF = static_cast<int>(1e9);

struct Edge {
    int w;
};

struct Step_res{
    int s;
    int t;
    int Cut;
};

class Graph {
    int V;
    std::vector<bool> unioned;
    std::vector<std::vector<Edge>> adj;
public:
    Graph(int V) : V(V),
                   unioned(std::vector<bool>(V, false)),
                   adj(std::vector<std::vector<Edge>>(V, std::vector<Edge>(V, Edge{INF}))) {}
    void addEdge(int u, int v, int w){
        adj[u][v].w = w;
        adj[v][u].w = w;
    }
    Step_res SW_Step(int k);
    int SW_MinCut();
};

enum class rel_with_A{
    in, connected, not_connected
};

Step_res Graph::SW_Step(int k){
    std::vector<rel_with_A> A(V, rel_with_A::not_connected);
    std::vector<int> weight_v_A(V, 0);
    Step_res res{-1, -1, INF};
    for (int i = 0; i < k; i++){
        int u = 0;
        for (int v = 0; v < V; v++){
            if (!unioned[v] && A[v] == rel_with_A::connected) {
                if (A[u] != rel_with_A::connected|| unioned[u]) {
                    u = v;
                } else {
                    u = (weight_v_A[v] > weight_v_A[u]) ? v : u;
                }
            }
        }
        A[u] = rel_with_A::in;
        res.s = res.t;
        res.t = u;
        res.Cut = weight_v_A[u];
        for (int v = 0; v < V; v++){
            if (adj[u][v].w != INF && A[v] != rel_with_A::in) {
                weight_v_A[v] += adj[u][v].w;
                A[v] = rel_with_A::connected;
            }
        }
    }
    return res;
}

int Graph::SW_MinCut() {
    int MinCut = INF;
    for (int i = 0; i < V-1; i++){
        Step_res SW = SW_Step(V-i);
        MinCut = std::min(MinCut, SW.Cut);
        unioned[SW.t] = true;
        for (int v = 0; v < V; v++){
            if (adj[SW.t][v].w != INF && SW.s != v) {
                adj[SW.s][v].w = (adj[SW.s][v].w == INF) ? adj[SW.t][v].w : adj[SW.s][v].w + adj[SW.t][v].w;
                adj[v][SW.s].w = adj[SW.s][v].w;
            }
        }
    }
    return MinCut;
}

int main(){
    int V, E;
    std::cin >> V >> E;
    Graph G{V};
    while (E--){
        int u, v, w;
        std::cin >> u >> v >> w;
        G.addEdge(u, v, w);
    }
    std::cout << G.SW_MinCut()<<'\n';
}
