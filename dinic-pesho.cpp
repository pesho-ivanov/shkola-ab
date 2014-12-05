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

// all vertex numbers should be zero-based
int N, M;
int head[MAX_N];  // head[v] is a -1 ending linked-list of edges
int ptr[MAX_N];   // ptr[v] starts equal to head[v] but skips edges which don't lead to target
Edge E[MAX_M];    // every edge (starting from E[0]) is followed by its reverse edge with odd index
int source, target;

int lvl[MAX_N]; // lvl[v] -- edge distance from v to target, or -1 if no path to target
int path[MAX_N], path_len;  // list of edges of the current flow

// added a new edge from a to b with residual flow c
void add_edge(int from, int to, flow_t cap) {
  static int E_len = 0;
  E[E_len] = Edge(to, cap, head[from]);
  head[from] = E_len++;
}

void input() {
  int a, b;
  flow_t c;

  cin >> N >> M;
  source = 0;
  target = N-1;

  fill(head, head+N, -1);
  for (int i = 0; i < M; ++i) {
    cin >> a >> b >> c;
    --a; --b;  // to zero-based vertex numbers

    add_edge(a, b, c);  // direct edge with c initial residual flow
    add_edge(b, a, 0);  // reverse edge with no initial residual flow
  }
}

// compute lvl[v] starting from target to all vertices through the reversed edges
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
    if (lvl[E[i].to] == lvl[v] - 1 && E[i].rd > 0) {
      path[path_len++] = i;
      if (flow_t tmp = dfs(E[i].to))
        return min(tmp, E[i].rd);
      --path_len;
    }

  return 0;
}

flow_t dinitz() {
  flow_t maxflow = 0, cflow;

  // no more than N iteration
  while (bfs()) {
    copy(head, head+N, ptr);

    // while cflow > 0 <=> augmenting path is found
    while (path_len = 0, cflow = dfs(source)) {
      maxflow += cflow;
      // let the max possible flow go through the current path
      for (int i = 0; i < path_len; ++i) {
        E[path[i]].rd -= cflow;
        E[path[i] ^ 1].rd += cflow;
      }
    }
  }

  return maxflow;
}

int main () {
  input();
  cout << dinitz() << endl;

  return 0;
}
