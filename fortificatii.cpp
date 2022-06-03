#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <climits>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

// oraseB[k] = 1 daca k este oras barbar, 0 in caz contrar
int B, oraseB[100001];

typedef pair<long long, int> iPair;

// fct aux pt sort(sorteaza in functie de distanta)
bool sortbysec(const pair<pair<int, int>, long long> &a,
               const pair<pair<int, int>, long long> &b) {
  return (a.second < b.second);
}

void Dijkstra(vector<vector<pair<int, int>>> graph, int N, long long *dist) {
  priority_queue<iPair, vector<iPair>, greater<iPair>> queue;
  queue.push(make_pair(0, 1));
  for (int i = 0; i <= N; i++) {
    dist[i] = LLONG_MAX - INT_MAX;
  }

  dist[1] = 0;

  while (!queue.empty()) {
    int node = queue.top().second;

    queue.pop();
    // parcurg lista de adiacenta a lui k
    for (size_t k = 0; k < graph[node].size(); k++) {
      int cost = graph[node][k].first;
      int next_node = graph[node][k].second;
      if (dist[next_node] > dist[node] + cost) {
        dist[next_node] = dist[node] + cost;
        // adaug in coada doar daca nu este un oras barbar, pt ca
        // daca am trecut printr-un oras barbar deoarece ce s-ar afla dupa
        // nu ma mai intereseaza sa calculez
        if (oraseB[next_node] == 0)
          queue.push(make_pair(dist[next_node], next_node));
      }
    }
  }
}

int main() {
  FILE *fin, *fout;
  fin = fopen("fortificatii.in", "r");
  fout = fopen("fortificatii.out", "w");
  int N, M;
  int K;

  fscanf(fin, "%d%d%d", &N, &M, &K);
  vector<vector<pair<int, int>>> graph(N + 1, vector<pair<int, int>>());
  vector<pair<pair<int, int>, long long>> edges;
  fscanf(fin, "%d", &B);
  int oras;
  for (int i = 0; i < B; i++) {
    fscanf(fin, "%d", &oras);
    oraseB[oras] = 1;
  }
  int n1, n2;
  int cost;
  for (int i = 0; i < M; i++) {
    fscanf(fin, "%d%d%d", &n1, &n2, &cost);
    graph[n1].push_back(make_pair(cost, n2));
    graph[n2].push_back(make_pair(cost, n1));
    if (oraseB[n1] && oraseB[n2] == 0)
      edges.push_back(make_pair(make_pair(n2, n1), cost));
    else if (oraseB[n2] && oraseB[n1] == 0)
      edges.push_back(make_pair(make_pair(n1, n2), cost));
    // daca este nod de "granita"(este incident cu un oras barbar)
    // adaug muchia in vectorul edges
  }
  pair<int, int> edge;

  long long *dist = (long long *)calloc(N + 1, sizeof(long long));
  Dijkstra(graph, N, dist);
  int size = edges.size();
  // pentru fiecare muchie retin distanta pana la orasul barbar prin muchia
  // respectiva pe care ulterior poate fi fortificata, drumul pana la nodul
  // de "granita" ramanand neschimbat
  for (int i = 0; i < size; i++) {
    edge = edges[i].first;
    edges[i].second += dist[edge.first];
  }
  // sortez vectorul de muchii de la granita cu orasele barbare
  sort(edges.begin(), edges.end(), sortbysec);
  int second_min_index = 0;
  // primul minim este pe prima pozitie(vectorul fiind sortat)
  // caut al doilea minim din vector, si cresc toate muchiile
  // pana la el(daca am suficiente fortificatii)
  while (K > 0) {
    int nr = 0;
    long long mn = edges[second_min_index].second;
    while (second_min_index < size && edges[second_min_index].second == mn) {
      second_min_index++;
      nr++;
    }
    if (second_min_index == size) {
      edges[0].second += K / size;
      break;
    }
    long long to_increase = edges[second_min_index].second - mn;
    if (to_increase > K / (second_min_index))
      to_increase = K / (second_min_index);
    if (to_increase != 0) {
      for (int i = 0; i < second_min_index && K > 0; i++) {
        edges[i].second += to_increase;
        K -= to_increase;
      }
    } else {
      break;
    }
  }

  fprintf(fout, "%lld", edges[0].second);
  fclose(fin);
  fclose(fout);
}
