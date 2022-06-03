#include <stdio.h>
#include <string.h>

#include <climits>
#include <iostream>
#include <queue>
using namespace std;

#define NORD 0
#define SUD 1
#define EST 2
#define VEST 3

typedef struct {
  int x, y;
  int last_direction;
  int moves;
} coordonate_t;

// directiile dinspre care am venit intr-un punct de coord [x][y]
// dir[x][y][0] == 1 daca deja este o drona venita dinspre NORD ...
int direction[1000][1000][4];

// fct ce verifica daca poz pe care vreau sa ma deplasez este valida
// adica daca se nu depaseste harta si daca este accesibila
int check_poz(int **MAP, int N, int M, int x, int y) {
  if (x >= N || x < 0 || y >= M || y < 0) return 0;
  if (MAP[x][y] == -2) return 0;
  return 1;
}

// fct aux pt a folosi priority_queue din c++
bool operator<(const coordonate_t &c1, const coordonate_t &c2) {
  return c1.moves > c2.moves;
}

int Lee(int **MAP, int N, int M, int x_start, int y_start, int x_finish,
        int y_finish) {
  MAP[x_start][y_start] = 0;
  // vector de directii pe linii, coloane, codificare este
  // explicata in #define-urile de la inceputul codului
  int x_dir[4] = {-1, 1, 0, 0}, y_dir[4] = {0, 0, 1, -1};
  priority_queue<coordonate_t> queue;
  coordonate_t coord;
  coord.x = x_start;
  coord.y = y_start;
  // daca last_direction == -1 nu adaug cost indiferent
  // de directia unde ma deplasez
  coord.last_direction = -1;
  coord.moves = 0;
  queue.push(coord);
  coordonate_t new_coord;
  while (!queue.empty()) {
    coord = queue.top();
    queue.pop();
    // daca nr de schimbari de directie s-a schimbat intre timp si pt
    // pt de coord x si y s-a gasit o solutie mai buna de timp, opresc
    // cautare pt acel path
    if (coord.moves > MAP[coord.x][coord.y] && MAP[coord.x][coord.y] != -1)
      continue;
    for (int i = 0; i < 4; i++) {
      // cel mai bun timp nu va contine niciodata o dubla schimbare de directie
      if (coord.last_direction == 0 && i == 1) continue;
      if (coord.last_direction == 1 && i == 0) continue;
      if (coord.last_direction == 2 && i == 3) continue;
      if (coord.last_direction == 3 && i == 2) continue;
      new_coord.x = coord.x + x_dir[i];
      new_coord.y = coord.y + y_dir[i];
      new_coord.last_direction = i;
      new_coord.moves = coord.moves;
      if (check_poz(MAP, N, M, new_coord.x, new_coord.y)) {
        // daca noua directie dinspre care m-am deplasat este
        // diferita de cea dinspre care m-am deplasat anterior
        // atunci inseamna ca schimb directia, deci timpul creste cu 1
        if ((coord.last_direction != new_coord.last_direction) &&
            (coord.last_direction != -1)) {
          new_coord.moves++;
        }
        // daca urmatoarea deplasare are un nr de schimbari de directie mai mare
        // decat solutia descoperita la acel moment de timp, atunci opresc
        // cautarea pt acel path
        if (new_coord.moves > MAP[x_finish][y_finish] &&
            MAP[x_finish][y_finish] != -1)
          continue;
        // daca timpul pt noua deplasare este <= decat minimul la acel moment
        // de timp descoperit pe harta atunci verific cele 2 cazuri, de
        // egalitate sau de >.
        if ((MAP[new_coord.x][new_coord.y] >= new_coord.moves) ||
            (MAP[new_coord.x][new_coord.y] == -1)) {
          // daca nr de schimbari de directie este egal atunci verific
          // sa nu continui cautarea pt o drona care vine din aceeasi
          // directie cu o alta drona, pentru ca ar avea in continuare
          // exact aceleasi posibilitati de drum
          if (MAP[new_coord.x][new_coord.y] == new_coord.moves) {
            if (direction[new_coord.x][new_coord.y][new_coord.last_direction] ==
                0) {
              // MAP[new_coord.x][new_coord.y] =  coord.moves + cost;
              direction[new_coord.x][new_coord.y][new_coord.last_direction] = 1;
              new_coord.moves = MAP[new_coord.x][new_coord.y];
              queue.push(new_coord);
            }
          } else {
            // daca am gasit un drum mai bun catre un punct de pe harta,
            // atunci in direction[x][y] doar aceasta drona va mai ramane
            // si updatez si restul directiilor vor fi 0
            MAP[new_coord.x][new_coord.y] = new_coord.moves;
            for (int k = 0; k < 4; k++)
              direction[new_coord.x][new_coord.y][k] = 0;
            direction[new_coord.x][new_coord.y][new_coord.last_direction] = 1;
            queue.push(new_coord);
          }
        }
      }
    }
  }
  return MAP[x_finish][y_finish];
}

int main() {
  FILE *fin, *fout;
  fin = fopen("beamdrone.in", "r");
  fout = fopen("beamdrone.out", "w");
  int N, M, x_start, y_start, x_finish, y_finish;
  char c;
  fscanf(fin, "%d%d", &N, &M);
  fscanf(fin, "%d%d%d%d", &x_start, &y_start, &x_finish, &y_finish);
  int **MAP = (int **)calloc(N, sizeof(int *));
  for (int i = 0; i < N; i++) MAP[i] = (int *)calloc(M, sizeof(int));
  fgetc(fin);  // eliberam buffer
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {
      c = fgetc(fin);
      if (c == 'W') {
        // punct inaccesibil
        MAP[i][j] = -2;
      } else {
        // punct nevizitat
        MAP[i][j] = -1;
      }
    }
    c = fgetc(fin);  // citim \n
  }

  fprintf(fout, "%d", Lee(MAP, N, M, x_start, y_start, x_finish, y_finish));

  for (int i = 0; i < N; i++) free(MAP[i]);
  free(MAP);
  fclose(fin);
  fclose(fout);
  return 0;
}
