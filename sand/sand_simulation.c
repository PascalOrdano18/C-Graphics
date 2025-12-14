#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>


#define WIDTH 900
#define HEIGHT 600

#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_BLUE 0xFFFF0000
#define COLOR_BLACK 0x00000000
#define SPEED 1
#define SAND_SIZE 5

#define MEMORY_BLOCK 200


struct Sand{
    double x;
    double y;
    int size;   // lado del cuadrado -> lo uso como pixel size = 1
    Uint32 color;
};

int collides(struct Sand* sands, int sandAmount, int x, int y, int sand_index);

void simulate_fall(struct Sand* sands, int sandAmount){
    for(int i = sandAmount - 1; i >= 0; i--){
        int x = sands[i].x;
        int y = sands[i].y;

        int newY = y + SPEED;
        
        // si llegamos al fondo
        if(newY + SAND_SIZE >= HEIGHT) continue;



        if(!collides(sands, sandAmount, x, newY, i)){
            sands[i].y = newY;
        }
    }
}

int collides(struct Sand* sands, int sandAmount, int x, int y, int sand_index){
    for(int i = 0; i < sandAmount; i++){
        if(i == sand_index) continue;

        if(x == sands[i].x && y + SAND_SIZE - 1 == sands[i].y) return 1;
    }
    return 0;
}


int main(void){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Sand", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    SDL_Rect erase_rect = (SDL_Rect) { 0, 0, WIDTH, HEIGHT };

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
                sands[sandAmount].x = event.motion.x;
                sands[sandAmount].y = event.motion.y;
                sands[sandAmount].size = SAND_SIZE;
                sands[sandAmount].color = currentColor;

                sandAmount++;
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
            SDL_Rect rect = (SDL_Rect) {sands[i].x, sands[i].y, sands[i].size, sands[i].size };
            SDL_FillRect(surface, &rect, sands[i].color);
        }

        SDL_UpdateWindowSurface(window);
    }

    free(sands);

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
