#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>


#define WIDTH 700
#define HEIGHT 700

#define CELL_LEN 10
#define GRID_WIDTH WIDTH / CELL_LEN
#define GRID_HEIGHT HEIGHT / CELL_LEN

#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_BLACK 0x00000000
#define COLOR_BLUE 0xFF1111DD
#define COLOR_RED 0xFFFF0000


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


void game_of_life(SDL_Surface * surface, unsigned char grid[GRID_HEIGHT][GRID_WIDTH]){
    unsigned char new_grid[GRID_HEIGHT][GRID_WIDTH];
    for(int y = 0; y < GRID_HEIGHT; y++){
        for(int x = 0; x < GRID_WIDTH; x++){
            // itero cada celda
            unsigned char neighbours = 0;
            for(int i = -1; i <= 1; i++){
                for(int j = -1; j <= 1; j++){
                    if(!i && !j || (y + i < 0 || y + 1 >= GRID_HEIGHT || x + j < 0 || x + j >= GRID_WIDTH)) continue;
                    if(grid[y + i][x + j]) neighbours+= 1;
                }
            }
            if(grid[y][x]){
                new_grid[y][x] = (neighbours == 2 || neighbours == 3) ? 1 : 0;
            } 
            if(!grid[y][x]){
                new_grid[y][x] = neighbours == 3 ? 1 : 0;
            }
        }
    }

    for(int i = 0; i < GRID_HEIGHT; i++){
        for(int j = 0; j < GRID_WIDTH; j++){
            grid[i][j] = new_grid[i][j];
        }
    }
    draw_grid(surface, grid);
} 


int main(void){
    SDL_Init(SDL_INIT_VIDEO); 
    SDL_Window* window = SDL_CreateWindow("Game of Life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);

    const Uint32 RENDER_MS = 16;
    const Uint32 SIM_MS = 20;

    Uint32 last = SDL_GetTicks();
    Uint32 acc = 0;

    static unsigned char grid[GRID_HEIGHT][GRID_WIDTH];
    for(int i = 0; i < GRID_HEIGHT; i++){
        for(int j = 0; j < GRID_WIDTH; j++){
            grid[i][j] = 0;
        }
    }
    
    int red_cell_flag = 0;
    int gof_flag = 0;

    int running = 1;
    SDL_Event event;
    while(running){
        Uint32 now = SDL_GetTicks();
        Uint32 dt = now - last;
        last = now;
        if(dt > 250) dt = 250;
        acc += dt;

        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                running = 0;
            }
            if(event.type == SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_SPACE){
                    gof_flag = !gof_flag;
                    game_of_life(surface, grid);
                }
            }

            if(event.type == SDL_MOUSEBUTTONDOWN|| event.type == SDL_MOUSEMOTION && event.motion.state != 0){
                grid[event.motion.y / CELL_LEN][event.motion.x / CELL_LEN] = 1;
            }
        }

        //code
        draw_grid(surface, grid);
        while(acc >= SIM_MS){
            if(gof_flag) game_of_life(surface, grid);
            acc -= SIM_MS;
        }
        SDL_UpdateWindowSurface(window);
    }
    SDL_DestroyWindowSurface(window);
    SDL_Quit();
    return 0;
}













