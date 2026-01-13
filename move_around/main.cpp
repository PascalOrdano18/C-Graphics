#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <algorithm>


#define WIDTH 700
#define HEIGHT 700

struct Player{
    float x = 100.0f;
    float y = 100.0f;
    float speed = 200.0f;
    int size = 32;
};

int main(){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Move Around", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    Player p;
    bool up = false, down = false, right = false, left = false;

    Uint64 prevCounter = SDL_GetPerformanceCounter();
    const double freq = (double)SDL_GetPerformanceFrequency();

    bool running = true;
    while(running){

        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT) running = false;

            if(event.type == SDL_KEYDOWN && event.key.repeat == 0){
                switch(event.key.keysym.sym){
                    case SDLK_ESCAPE: running = false; break;
                    case SDLK_w: up = true; break;
                    case SDLK_s: down = true; break;
                    case SDLK_d: right = true; break;
                    case SDLK_a: left = true; break;
                }
            }
            if(event.type == SDL_KEYUP && event.key.repeat == 0){
                switch(event.key.keysym.sym){
                    case SDLK_w: up = false; break;
                    case SDLK_s: down = false; break;
                    case SDLK_d: right = false; break;
                    case SDLK_a: left = false; break;
                }
            }
        }

        Uint64 nowCounter = SDL_GetPerformanceCounter();
        double dt = (nowCounter - prevCounter) / freq;
        prevCounter = nowCounter;


        float vx = 0.0f;
        float vy = 0.0f;

        if(up) vy -= 1.0f;
        if(down) vy += 1.0f;
        if(right) vx += 1.0f;
        if(left) vx -= 1.0f;


        p.x += vx * p.speed * (float) dt;
        p.y += vy * p.speed * (float) dt;

        if(p.x < 0) p.x = 0;
        if(p.y < 0) p.y = 0;

        if(p.x >= WIDTH - p.size) p.x = WIDTH - p.size;
        if(p.y >= HEIGHT - p.size) p.y = HEIGHT - p.size;

        SDL_SetRenderDrawColor(renderer, 10, 10, 12, 255);
        SDL_RenderClear(renderer);

        SDL_Rect rect = { (int)p.x, (int) p.y, p.size, p.size };
        SDL_SetRenderDrawColor(renderer, 80, 220, 120, 255);
        SDL_RenderFillRect(renderer, &rect);

        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}










