#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define WIDTH 900
#define HEIGHT 600
#define TAMANO 20

#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_GRAY 0xFF888888
#define COLOR_BLACK 0x00000000

typedef struct {
    int x;
    int y;
    int tamano;
    Uint32 color;
} Dino;

void draw_dino(SDL_Surface* surface, Dino dinosaurio){
    SDL_Rect dino_rect = { dinosaurio.x, dinosaurio.y, dinosaurio.tamano, dinosaurio.tamano }; 
    SDL_FillRect(surface, &dino_rect, dinosaurio.color); 
}



int main(void){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Dino", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

    SDL_Surface* surface = SDL_GetWindowSurface(window);

    const Uint32 RENDER_MS = 16;
    const Uint32 SIM_MS = 20;

    Uint32 last = SDL_GetTicks();
    Uint32 acc = 0;

    Dino dinosaurio = {
        100,
        450,
        TAMANO,
        COLOR_WHITE
    };
    SDL_Rect piso = { 0, dinosaurio.y + dinosaurio.tamano, WIDTH, HEIGHT - (dinosaurio.y + dinosaurio.tamano) };
    SDL_FillRect(surface, &piso, COLOR_GRAY);

    SDL_Rect erase_rect = { 0, 0, WIDTH, 450 };

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
                SDL_Keycode key = event.key.keysym.sym;
                if(key == SDLK_SPACE){
                    if(dinosaurio.y == 450){
                        while(dinosaurio.y >= 400){
                            dinosaurio.y -= 1; 
                            draw_dino(surface, dinosaurio);
                        }
                    }
                   
                }
            }
        }

        while(acc >= SIM_MS){
            if(dinosaurio.y <= 400){
                while(dinosaurio.y < 450){
                    dinosaurio.y += 1;
                    draw_dino(surface, dinosaurio);
                }
            } 
            draw_dino(surface, dinosaurio);
            acc -= SIM_MS;
        }
        SDL_FillRect(surface, &erase_rect, COLOR_BLACK);
        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}





