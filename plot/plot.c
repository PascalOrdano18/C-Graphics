#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define WIDTH 900
#define HEIGHT 600

#define COLOR_WHITE 0xFFFFFFFF;

// sin(x) + 10 + x**2 / x * cos(x)
void draw_function(){
    for(int x = 0; x < WIDTH; x++){
        
    }
}


int main(int argc, char* argv[]){
    if(argc < 2) return 0; // necesitamos que nos pasen datos de la funcion

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

       draw_function();

       SDL_UpdateWindowSurface(window);
   }
   SDL_DestroyWindowSurface(window);
   SDL_Quit();
   return 0;
}
