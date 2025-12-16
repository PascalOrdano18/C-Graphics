#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h>

#define WIDTH 900
#define HEIGHT 600

#define ANGLE M_PI / 6   // 30 grados 
#define COLOR_WHITE 0xFFFFFFFF


void draw_line(SDL_Surface* surface, int x, int y, double angle, int length){
    if(length <= 1) return ; 
    
    for(int i = 0 ; i < length; i++){
        x += cos(angle);
        y += sin(angle);

        SDL_Rect pixel = (SDL_Rect) { x, y, 1, 1 };
        SDL_FillRect(surface, &pixel, COLOR_WHITE);
    }
    
    length = length / 2;
    draw_line(surface, x, y, angle, length);
    draw_line(surface, x, y, -angle, length);
}

int main(void){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Raytracing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    
    SDL_Surface* surface = SDL_GetWindowSurface(window);

    SDL_Rect erase_rect = (SDL_Rect) { 0, 0, WIDTH, HEIGHT };

    int running = 1;
    SDL_Event event;
    while(running){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                running = 0;
            }
        }
        
        draw_line(surface, HEIGHT, WIDTH / 2, ANGLE, 100);

        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


