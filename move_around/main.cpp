#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <algorithm>


#define WIDTH 700
#define HEIGHT 700


int main(){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWidnow("Move Around", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);

    const Uint32 RENDER_MS = 16;
    const Uint32 SIM_MS = 20;

    Uint32 last = SDL_GetTicks();
    Uint32 acc = 0;

    int running = 1;
    SDL_Event event;

    while(running){
        Uint32 now = SDL_GetTicks();
        Uint32 dt = now - last;
        last = now;
        if(dt > 250) dt = 250;
        acc += dt;

        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT) running = 0;
        }

        // while(acc >= SIM_MS){
        //     // asdads
        //     acc -= SIM_MS;
        // }

        SDL_UpdateWindowSurface(window);
    }
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}










