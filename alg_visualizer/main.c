#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>


#define WIDTH 700
#define HEIGHT 700

#define CELL_LEN 20
#define GRID_WIDTH WIDTH / CELL_LEN
#define GRID_HEIGHT HEIGHT / CELL_LEN

#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_BLACK 0x00000000
#define COLOR_BLUE 0xFF1111DD
#define COLOR_RED 0xFFFF0000


typedef struct {
    double x;
    double y;
    unsigned char occupied;
} Cell;

void draw_grid(SDL_Surface* surface, unsigned char grid[GRID_HEIGHT][GRID_WIDTH]){
    for(int y = 0; y < GRID_HEIGHT; y++){
        for(int x = 0; x < GRID_WIDTH; x++){
            SDL_Rect cell = (SDL_Rect) { x * CELL_LEN, y * CELL_LEN, CELL_LEN - 1, CELL_LEN -1  };
            if(grid[y][x]){
                SDL_FillRect(surface, &cell, COLOR_BLUE);
            } else {
                SDL_FillRect(surface, &cell, COLOR_WHITE);
            }
        }
    }
}




int main(void){
    SDL_Init(SDL_INIT_VIDEO); 
    SDL_Window* window = SDL_CreateWindow("Alg Visualizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);

    static unsigned char grid[GRID_HEIGHT][GRID_WIDTH];
    for(int i = 0; i < GRID_HEIGHT; i++){
        for(int j = 0; j < GRID_WIDTH; j++){
            grid[i][j] = 0;
        }
    }
    
    int running = 1;
    SDL_Event event;
    while(running){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                running = 0;
            }
            if(event.type == SDL_MOUSEBUTTONDOWN|| event.type == SDL_MOUSEMOTION && event.motion.state != 0){
                grid[event.motion.y / CELL_LEN][event.motion.x / CELL_LEN] = 1;
            }
        }

        //code
        draw_grid(surface, grid);
        SDL_UpdateWindowSurface(window);
    }
    SDL_DestroyWindowSurface(window);
    SDL_Quit();
    return 0;
}













