#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h>

#define WIDTH 900
#define HEIGHT 600

#define ANGLE  M_PI / 4   // 30 grados 
#define COLOR_WHITE 0xFFFFFFFF


void draw_line(SDL_Surface* surface, double x, double y, double angle, int length){
    if(length <= 1) return ; 
    
    for(int i = 0 ; i < length; i++){
        x += cos(angle);
        y -= sin(angle);

        SDL_Rect pixel = (SDL_Rect) { x, y, 1, 1 };
        SDL_FillRect(surface, &pixel, COLOR_WHITE);
    }
    
    length = length / 2;
    draw_line(surface, x, y, angle + ANGLE, length);
    draw_line(surface, x, y, angle - ANGLE, length);
}

int main(void){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Fractal Trees", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    
    SDL_Surface* surface = SDL_GetWindowSurface(window);

    SDL_Rect erase_rect = (SDL_Rect) { 0, 0, WIDTH, HEIGHT };

    int running = 1;
    double angle = ANGLE;
    SDL_Event event;
    while(running){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                running = 0;
            }
            if(event.type == SDL_KEYDOWN){
                SDL_Keycode key = event.key.keysym.sym;
                if(key == SDLK_SPACE){
                    angle += 0.1;
                }
            }
        }
        
        draw_line(surface, (double) (WIDTH / 2), (double) HEIGHT, 2 *  angle, 250);

        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


