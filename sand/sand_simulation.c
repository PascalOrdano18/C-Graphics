#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <time.h>


#define WIDTH 900
#define HEIGHT 600

#define SAND_SIZE 5

#define GRID_WIDTH WIDTH / SAND_SIZE
#define GRID_HEIGHT HEIGHT / SAND_SIZE

#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_BLUE 0xFF0000FF
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
    for(int y0 = GRID_HEIGHT - 1; y0 > 0; y0--){      // en realidad deberia cortar cuando no tiene sand ese casillero
        if(sandId[y0][0] == -1) continue;
        bfs(sands, y0, 0);
    }
}

// BFS de lado a lado chequeando por si la arena de mismo color cruza todo el mapa
void bfs(struct Sand* sands, int y, int x){
    struct Sand* visited = (struct Sand*)malloc(sizeof(struct Sand) * 100);

    int queueLen = 100;
    struct Sand* queue = (struct Sand*)malloc(sizeof(struct Sand) * queueLen);
    int queueIndex = 0;
    Uint32 color = sands[sandId[y][x]].color;
    queue[queueIndex++] = sands[sandId[y][x]]; // primer nodo 
    while(queueIndex){
        for(int i = -1; i <= 1; i++){
            for(int j = -1; j <= 1; j++){
                int nx = x + j;
                int ny = y + i;
                if(nx < 0 || nx == GRID_WIDTH || ny < 0 || ny > GRID_HEIGHT) continue;
                if(sandId[ny][nx] != -1 && sands[sandId[ny][nx]].color == color){
                    if(queueIndex == queueLen){
                        queueLen = queueLen + queueIndex;
                        queue = realloc(queue, sizeof(struct Sand) * queueLen);
                    }
                    queue[queueIndex++] = sands[sandId[ny][nx]];

                }
            }
        }
    }
}


int main(void){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Sand", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    SDL_Rect erase_rect = (SDL_Rect) { 0, 0, WIDTH, HEIGHT };
    
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

        simulate_fall(sands, sandAmount);

        SDL_FillRect(surface, &erase_rect, COLOR_BLACK);


        for(int i = 0; i < sandAmount; i++){
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
