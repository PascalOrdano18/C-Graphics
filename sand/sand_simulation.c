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


static unsigned char grid[GRID_HEIGHT][GRID_WIDTH];

struct Sand{
    int x;
    int y;
    int size;   // lado del cuadrado -> lo uso como pixel size = 1
    Uint32 color;
};



void simulate_fall(struct Sand* sands, int sandAmount){
    for(int i = sandAmount - 1; i >= 0; i--){
        int x = sands[i].x;
        int y = sands[i].y;

        
        // si llegamos al fondo
        if(y + 1 >= GRID_HEIGHT) continue;

        // abajo libree
        if(!grid[y + 1][x]){
            grid[y][x] = 0;
            y++;
            grid[y][x] = 1;
            sands[i].y = y;
            continue;   
        }
        
        // abajo ocupado, hacemos random la caida
        int side = (rand() & 1) ? 1 : -1;

        if(x <= GRID_WIDTH && !grid[y+1][x + side]){
            grid[y][x] = 0;
            grid[y + 1][x + side] = 1;
            sands[i].x = x + side;
            sands[i].y = y + 1;
        } else if(x > 1 && !grid[y + 1][x - side]){
            grid[y][x] = 0;
            grid[y + 1][x - side] = 1;
            sands[i].x = x - side;
            sands[i].y = y + 1;
        }

    }
}



int main(void){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Sand", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    SDL_Rect erase_rect = (SDL_Rect) { 0, 0, WIDTH, HEIGHT };
    
    srand((unsigned)time(NULL));


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

                if(!grid[my][mx]){


                    sands[sandAmount].x = mx;
                    sands[sandAmount].y = my;
                    sands[sandAmount].size = SAND_SIZE;
                    sands[sandAmount].color = currentColor;
                    
                    grid[sands[sandAmount].y][sands[sandAmount].x] = 1;
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
