# Dinic algorithm realisation

Here I provide comments on how & why my realisation of Dinic algorithm works correctly.
***
### Code with explanation

Just some includes.
```c++
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>

const int INF = static_cast<int>(1e9);
```
**1. Graph** 

We make a structure for edges of the graph. 
Every edge ((u,v), C) will contain informaition about "to" vertex (v), 
current flow going throught the edge (flow), 
it's capacity (C) and index of the reversed edge in the vector of edges.
```c++
struct Edge {
    int v;
    int flow;
    int C;
    int rev;
};
```
Structure for graph storage contains number of vertices in the graph `V`, 
vector `level` &mdash; distance from start of the flow (would be calculated via `BFS`), and vector of vectors of edges
&mdash; adjacency list `adj`. All fields are private for avoiding further mistakes.
```c++
class Graph{
    int V;
    std::vector<int> level;
    std::vector<std::vector<Edge>> adj;
public:
    Graph(int V) : V(V), level(std::vector<int>(V)), adj(std::vector<std::vector<Edge>>(V)) {}
    ...
};
```

Edges to the graph can be added via `addEdge`. 
We add both edge and it's reversed copy with 0 capacity. 
Initial flow is 0.

```c++
    void addEdge(int u, int v, int C) {
        Edge e{ v, 0, C, (int)adj[v].size()};
        Edge e_rev{ u, 0, 0, (int)adj[u].size()};
        adj[u].push_back(e);
        adj[v].push_back(e_rev);
    }
```
And here also are defined the functions.
```c++
    bool BFS(int s, int t);
    int sendFlow(int s, int flow, int t, std::vector<int> &start);
    int DinicMaxflow(int s, int t);
```

**2. Algorithm realisation**

Now let's look closer at the realisation of the functions. 

`DinicMaxflow` function requires only start and terminal vertices of the flow. 
It counts the answer: calls `BFS` while it can build level graph, 
pushes the maximal possible flow through this graph using `sendFlow` 
and adds it to the answer. 

Vector `start` will contain indices of first edges 
not yet used for pushing flow in this leveled graph for each vertex. 
Initially it contains zeros. It'll be changed every run of the `sendFlow`.

`sendFlow` tries to send flow (yeah) through the leveled graph we got from `BFS`. 
After the flow is sent (while possible) we add it to the answer `total`.

`total` is the required maximum flow.

**Time complexity:** Depends on implementation of `BFS` and `sendFlow`, 
but the logic of algorithm says 
that the first *while*-cycle runs less or equal to V times. 
There is some fun with the inner *while*-cycle, so for now let's say, 
that `DinicMaxflow` works in _O_(V*[T(`BFS`)+T(inner *while*-cycle)])

```c++
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
```
**Now let's look closer on functions `BFS` and `sendFlow`, 
where all the magic happends.**

The `BFS` function is very straightforward. 
The main thing there &mdash; the calculation of levels of vertices &mdash; 
is also implemented in a very classic way.

**Time complexity**: `BFS` works, as it is expected from BFS to work, in _O(V+E)_.
```c++
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
```
The most complicated part of the whole code is the function `sendFlow`,
that must calculate the flow through the leveled graph. 
It finds path from s to t (if possible) and a flow through it. 
It's mostly similar to DFS, but with some modifications. 
Let's look closer.

Vector `start` is given via a link, so it can be changed inside a function.
`u` is current "start" vertex 
(if we are somewhere deep in recursion, we maybe
have already found a part of the path). 
`flow` is current maximal flow through the path. 
Initially (in function `DinicMaxflow`) it's infinite.
```c++
int Graph::sendFlow(int u, int flow, int t, std::vector<int> &start) {
```
If `u`==`t`, then we found the path and the flow through it.
```c++
    if (u == t)
        return flow;
```

Else, as in DFS, 
let's go through all not yet used the edges from `u` and look, where we can go.
For every `u` we use edges in the same order as they are arranged in `adj[u]`,
the `start[u]` shows current edge. 
So by increasing it we avoid returning to used edges. 

```c++
    for (; start[u] < adj[u].size(); start[u]++) {
        Edge& e = adj[u][start[u]];
```
Every subpath from s to v on the shortest path from `s` to `t` is the shortest, 
so if we are in vertex `u`, there's no need in going to vertex, 
which level is not equal to `level[u]`+1. 
Also the flow through edge we are using must be less, then capacity. 
Otw the flow we will find would be zero.
```c++
        if (level[e.v] == level[u] + 1 && e.flow < e.C) {
```
Then we go to recursion. 
Before it we just update the flow in case if current flow can't go through edge `e`.

When we get the answer from the recursion, 
we check if it's nonzero, update edge `e` and return it. 
Otw we continue the cycle, bc maybe with next edge our luck would be better.

If not, if after the whole cycle we didn't find the path, we return 0.
```c++
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
```
**Time complexity**: and here the promised fun appears. 
Because of changings in vector `start` all the runs of `sendFlow` 
use every edge no more than once. And the DFS, as usual, uses every vertex only once. 
Also, there are as usual in DFS O(V) calls to vertices. 
And, returning to the *while*-cycle in `DinicMaxflow`, 
which gives E calls of `sendFlow` in worst case 
(because there may be E paths from s to t), so with the *while*-cycle it's _O_(V*E+E)

***

**Time complexity of the algorithm:** 
taking formula from the asymtotic of `DinicMaxflow`,
and replacing the unknown functions we get 
_O_(V*[T(`BFS`)+T(inner *while*-cycle)]) = _O_(V*[_O_(V+E)+_O_(V*E+E)]) = _O_(V^2*E)

Exactly as expected.
***
### Tests

Now let's provide some tests that prove that algorithm works correctly.

1. 
![Пример](graph(7).png)

Input
```c++
2 1 0 1
0 1 1
```
Output
```c++
1
```
*** 
 2. 
![Пример](graph(6).png)

Input
```c++
4 5 0 3
0 1 100
0 2 1
1 2 100
1 3 1
2 3 100
```
Output
```c++
101
```
***
3.
![Пример](graph(8).png)
Input
```c++
6 9 0 5
0 1 10
0 2 10
1 2 2
1 3 4
1 4 8
2 4 9
4 3 6
3 5 10
4 5 10
```
Output
```c++
19
```
***
4.
![Пример](graph(9).png) 
Input
```c++
5 9 0 4
0 1 10
0 2 10
0 3 1
1 2 10
2 3 1
3 1 10
1 4 1
2 4 1
3 4 20
```

Output
```c++
4
```

***
5.

![Пример](graph(10).png)
Input
```c++
5 6 2 4
2 1 1
2 3 1
0 2 1
3 4 1
0 4 1
1 0 1 
```
Output
```c++
2
```
***
6.

![Пример](graph(11).png) 
Input
```c++
5 5 2 3
3 4 1
3 0 1
0 1 1
4 1 1
1 2 100
```
Output
```c++
2
```



Also I've tried to submit it in homework contest
(as undirected graph) and it successfully passed tests.


