#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <time.h>


#define WIDTH 500
#define HEIGHT 800

#define SAND_SIZE 5

#define GRID_WIDTH WIDTH / SAND_SIZE
#define GRID_HEIGHT HEIGHT / SAND_SIZE

#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_BLUE 0xFF0000FF
#define COLOR_RED 0xFFFF0000
#define COLOR_BLACK 0x00000000
#define SPEED 1

#define MEMORY_BLOCK 200


static int sandId[GRID_HEIGHT][GRID_WIDTH];  // grilla de sands
                                                       // -1 si no hay sand
                                                       // id int si hay, con el que accedo a Sand*

struct Sand{
    int x;
    int y;
    int size;   // lado del cuadrado -> lo uso como pixel size = 1
    Uint32 color;
};


void bfs(struct Sand* sands, int y, int x);

void simulate_fall(struct Sand* sands, int sandAmount){
    for(int i = sandAmount - 1; i >= 0; i--){
        if(sands[i].x == -1 || sands[i].y == -1) continue;
        int x = sands[i].x;
        int y = sands[i].y;

        // si llegamos al fondo
        if(y + 1 >= GRID_HEIGHT) continue;

        // abajo libree
        if(sandId[y + 1][x] == -1){
            sandId[y][x] = -1;
            y++;
            sandId[y][x] = i;
            sands[i].y = y;
            continue;   
        }
        
        // abajo ocupado, hacemos random la caida
        int side = (rand() & 1) ? 1 : -1;
        int nx = x + side;
        if(nx >= 0 && nx < GRID_WIDTH && sandId[y + 1][nx] == -1) {
            sandId[y][x] = -1;
            sandId[y + 1][nx] = i;
            sands[i].x = nx;
            sands[i].y = y + 1;
        } else {
            nx = x - side;
            if(nx >= 0 && nx < GRID_WIDTH && sandId[y + 1][nx] == -1) {
                sandId[y][x] = -1;
                sandId[y + 1][nx] = i;
                sands[i].x = nx;
                sands[i].y = y + 1;
            }
        }
    }
}

void bfsWrapper(struct Sand* sands){
    for(int y0 = GRID_HEIGHT - 1; y0 >= 0; y0--){      // en realidad deberia cortar cuando no tiene sand ese casillero
        if(sandId[y0][0] == -1) continue;
        bfs(sands, y0, 0);
    }
}


char isVisited(struct Sand* visited, int visitedIndex, int x, int y){
    for(int i = 0; i < visitedIndex; i++){
        if(visited[i].x == x && visited[i].y == y){
            return 1;
        } 
    }
    return 0;
}

// BFS de lado a lado chequeando por si la arena de mismo color cruza todo el mapa
void bfs(struct Sand* sands, int start_y, int start_x){

    static unsigned char visitedGrid[GRID_HEIGHT][GRID_WIDTH]; 
    for(int yy = 0; yy < GRID_HEIGHT; yy++){
        for(int xx = 0; xx < GRID_WIDTH; xx++){
            visitedGrid[yy][xx] = 0; // inicializo la grilla de visitados en cero
        }
    }

    int visitedLen = 100;
    int queueLen = 100;

    int head = 0;
    int tail = 0;
    int visitedIndex = 0;

    struct Sand* visited = (struct Sand*)malloc(sizeof(struct Sand) * visitedLen);
    struct Sand* queue = (struct Sand*)malloc(sizeof(struct Sand) * queueLen);

    int startId = sandId[start_y][start_x];
    if(startId == -1){
        free(visited);
        free(queue);
        return ;
    }

    Uint32 color = sands[startId].color;
    queue[tail++] = sands[startId]; // primer nodo 
    visited[visitedIndex++] = sands[startId];
    visitedGrid[start_y][start_x] = 1;
    
    int reachedRight = 0;

    while(head < tail){
        struct Sand current = queue[head++];
        int x = current.x;
        int y = current.y;

        if(x == GRID_WIDTH - 1){
            reachedRight = 1;
            break;
        }

        for(int i = -1; i <= 1; i++){
            for(int j = -1; j <= 1; j++){
                if(i == 0 && j == 0) continue;
                int nx = x + j;
                int ny = y + i;
                if(nx < 0 || nx >= GRID_WIDTH || ny < 0 || ny >= GRID_HEIGHT) continue;

                if(sandId[ny][nx] != -1 && sands[sandId[ny][nx]].color == color){
                    
                    int nid = sandId[ny][nx];
                    if(nid == -1) continue;
                    if(sands[nid].color != color) continue;
                    if(isVisited(visited, visitedIndex, nx, ny)) continue;
                    if(visitedGrid[ny][nx]) continue;
                    visitedGrid[ny][nx] = 1;


                    // encolar
                    if(tail == queueLen){
                        queueLen *= 2;
                        queue = realloc(queue, sizeof(struct Sand) * queueLen);
                    }
                    queue[tail++] = sands[nid];
                    
                    // push de visited
                    if(visitedIndex == visitedLen){
                        visitedLen *= 2;
                        visited = realloc(visited, sizeof(struct Sand) * visitedLen);
                    }
                    visited[visitedIndex++] = sands[nid];
                }
            }
        }
    }

       if(reachedRight){
            for(int i = 0; i < visitedIndex; i++){
                int vx = visited[i].x;
                int vy = visited[i].y;
                int vId = sandId[vy][vx];

                if(vId != -1){
                    sands[vId].color = COLOR_BLACK;
                    sands[vId].x = -1;
                    sands[vId].y = -1;
                    sandId[vy][vx] = -1;
                }
            }
        }
    free(queue);
    free(visited);
}


int main(void){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Sand", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    SDL_Rect erase_rect = (SDL_Rect) { 0, 0, WIDTH, HEIGHT };

    const Uint32 RENDER_MS = 16; // 60 FPS
    const Uint32 SIM_MS = 20;    // ticks de física por segundo
    
    Uint32 last = SDL_GetTicks();
    Uint32 acc = 0;

    
    srand((unsigned)time(NULL));
    
    for(int i = 0; i < GRID_HEIGHT; i++){
        for(int j = 0; j < GRID_WIDTH; j++){
            sandId[i][j] = -1;
        }
    }

    int sandAmount = 0;
    int allocatedSand = 100;
    struct Sand* sands = malloc(allocatedSand * (sizeof(struct Sand)));

    Uint32 currentColor = COLOR_WHITE;
    int color = 0;

    int running = 1;
    SDL_Event event;
    while(running){


        Uint32 now = SDL_GetTicks();
        Uint32 dt = now - last;
        last = now;

        // por seguridad si hiciste alt-tab o se colgó un frame:
        if (dt > 250) dt = 250;

        acc += dt;

        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                running = 0;
            }
            if(event.type == SDL_MOUSEMOTION){
                // creo el grano de arena
                
                if(sandAmount >= allocatedSand){
                    allocatedSand += MEMORY_BLOCK;
                    sands = realloc(sands, allocatedSand * sizeof(struct Sand));
                }
                
                int mx = event.motion.x / SAND_SIZE;
                int my = event.motion.y / SAND_SIZE;

                if(sandId[my][mx] == -1){
                    sands[sandAmount].x = mx;
                    sands[sandAmount].y = my;
                    sands[sandAmount].size = SAND_SIZE;
                    sands[sandAmount].color = currentColor;
                    
                    sandId[my][mx] = sandAmount;
                    sandAmount++;
                }
            }
            if (event.type == SDL_KEYDOWN) {
                SDL_Keycode key = event.key.keysym.sym;

                if (key == SDLK_SPACE) {
                    currentColor = (currentColor == COLOR_WHITE) ? COLOR_BLUE : COLOR_WHITE;
                }
            }
        }

        while(acc >= SIM_MS){
            simulate_fall(sands, sandAmount);
            bfsWrapper(sands);
            acc -= SIM_MS;
        }

        SDL_FillRect(surface, &erase_rect, COLOR_BLACK);

        for(int i = 0; i < sandAmount; i++){
            if(sands[i].x == -1 || sands[i].y == -1) continue ;
            SDL_Rect rect = (SDL_Rect) {sands[i].x * SAND_SIZE, sands[i].y * SAND_SIZE, sands[i].size, sands[i].size };
            SDL_FillRect(surface, &rect, sands[i].color);
        }

        SDL_UpdateWindowSurface(window);
    }

    free(sands);

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
