#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h>

#define WIDTH 900
#define HEIGHT 900

#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_GRAY 0x88888888

void plot_normalized_point(SDL_Surface* surface, double x, double y);

void draw_axis(SDL_Surface* surface){
    int x = WIDTH / 2;
    int y = HEIGHT / 2;
    for(int i = 0; i < WIDTH; i++){
        SDL_Rect point = (SDL_Rect) { i, y, 1, 1 };
        SDL_FillRect(surface, &point, COLOR_GRAY);
    }
    for(int i = 0; i < HEIGHT; i++){
        SDL_Rect point = (SDL_Rect) { x, i, 1, 1 };
        SDL_FillRect(surface, &point, COLOR_GRAY);
    }
}

// sin(x) + 10 + x**2 / x * cos(x)
void draw_function(SDL_Surface* surface){
    for(double x = -WIDTH/2; x < WIDTH / 2; x += 0.001){
        double y = sin(x);
        plot_normalized_point(surface, x, y);
    }
}

void plot_normalized_point(SDL_Surface* surface, double x, double y){
    double nx = x + WIDTH / 2;
    double ny = -y + HEIGHT / 2;
    SDL_Rect point = (SDL_Rect) { nx, ny, 1, 1 };
    SDL_FillRect(surface, &point, COLOR_WHITE);
}


int main(int argc, char* argv[]){
    // if(argc < 2) return 0; // necesitamos que nos pasen datos de la funcion


   SDL_Init(SDL_INIT_VIDEO);
   SDL_Window* window = SDL_CreateWindow("Function Plotter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

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

       draw_axis(surface);
       draw_function(surface);

       SDL_UpdateWindowSurface(window);
   }
   SDL_DestroyWindowSurface(window);
   SDL_Quit();
   return 0;
}
