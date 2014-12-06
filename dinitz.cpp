/*
DESC: given N, M and M edges (1-based vertices); find the max-flow from 1 to N
PROB: http://informatics.mccme.ru/moodle/mod/statements/view3.php?chapterid=2784
KEYW: Dinic max-flow, O(N^2*M) but O(N*M) in average (?)
NOTE: worst-case optimization: not looking for augmenting paths through edges
      that already didn't lead to target within the current BFS iteration
NOTE: N (bfs iterations) * (M + M*N) (amortized M dfs iterations saturating all
      M edges and finding M paths of lenght N)
NOTE: works with multiple edges between the same pair of vertices (both reverse or unidirected)
AUTHOR: Petar Ivanov <peter.ivanov89@gmail.com>
*/

#include<iostream>
#include<queue>
using namespace std;

typedef long long flow_t;

const int     MAX_N = 500;
const int     MAX_M = 2 * 10000;  // 2x for the reversed edges
const flow_t  MAX   = 1e9 + 5;  // max total flow value

struct Edge {
  int to;     // ending vertex of an oriented edge
  flow_t rd;  // residual flow
  int next;   // the index of the next Edge in E

  Edge() {}
  Edge(int _to, flow_t _rd, int _next)
    : to (_to), rd (_rd), next (_next) {}
};

int N, M;
int source, target;  // all vertex numbers should be zero-based

int head[MAX_N];  // head[v] points to the first edge of a linked list of the edges starting at v; the value -1 indicates the end of the list
Edge E[MAX_M];    // every edge (starting from E[0]) is followed by its reverse edge with odd index

int lvl[MAX_N];  // lvl[v] -- # of edges from v to target, or -1 if no path exists
int ptr[MAX_N];  // like head[] but skips edges which didn't lead to target
int path[MAX_N], path_len;  // list of edges of the current augmenting path

// added a new edge from a to b with residual flow c
void add_edge(int from, int to, flow_t cap) {
  static int E_len = 0;
  E[E_len] = Edge(to, cap, head[from]);
  head[from] = E_len++;
}

void input() {
  int from, to;
  flow_t rd;

  cin >> N >> M;
  source = 0;
  target = N-1;

  fill(head, head+N, -1);
  for (int i = 0; i < M; ++i) {
    cin >> from >> to >> rd;
    --from; --to;  // to zero-based vertex numbers

    add_edge(from, to, rd);  // direct edge with c initial residual flow
    add_edge(to, from, 0);   // reverse edge with no initial residual flow
  }
}

// compute lvl[v] spreading from target going through the reversed edges
// so lvl[v] is the shortest edge-distance from v to target
// returns whether there is a path from source to target
bool bfs() {
  queue<int> Q;
  fill(lvl, lvl+N, -1);

  Q.push(target);
  lvl[target] = 0;
  while(!Q.empty()) {
    int v = Q.front();
    Q.pop();
    for (int e = head[v]; e != -1; e = E[e].next)
      if (lvl[E[e].to] == -1 && E[e ^ 1].rd > 0) {
        lvl[E[e].to] = lvl[v] + 1;
        Q.push(E[e].to);
      }
  }

  return lvl[source] != -1;
}

// finds one augmenting path from source to target
// returns the maximum flow throung the path <=> min edge
flow_t dfs(int v) {
  if (v == target)
    return MAX;

  for (int i = ptr[v]; i != -1; i = ptr[v] = E[i].next)
    // move only to a closer vertex to target so that a shortest path is found
    if (lvl[E[i].to] == lvl[v] - 1 && E[i].rd > 0) {
      path[path_len++] = i;
      if (flow_t tmp = dfs(E[i].to))
        return min(tmp, E[i].rd);
      --path_len;
    }

  return 0;
}

// propagate the max possible flow through the current augmenting path
void update_graph(flow_t cflow) {
  for (int i = 0; i < path_len; ++i) {
    E[path[i]].rd -= cflow;
    E[path[i] ^ 1].rd += cflow;
  }
}

flow_t dinitz() {
  flow_t maxflow = 0, cflow;

  // every consequitive BFS finds strictly longer shortest paths
  // so there are no more than N phases of BFS
  while (bfs()) {
    copy(head, head+N, ptr);

    // cflow > 0 <=> augmenting path is found
    while (path_len = 0, cflow = dfs(source)) {
      maxflow += cflow;
      update_graph(cflow);
    }
  }

  return maxflow;
}

int main () {
  input();
  cout << dinitz() << endl;

  return 0;
}
