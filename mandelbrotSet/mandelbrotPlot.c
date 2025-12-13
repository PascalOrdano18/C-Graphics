#include <stdio.h>
#include <stdlib.h>
#include <complex.h>  // me estaba volviendo loco
#include <SDL2/SDL.h>


#define WIDTH 900
#define HEIGHT 600

#define COLOR_BLACK 0x00000000
#define COLOR_WHITE 0xFFFFFFFF

#define MAX_ITER 20


int check_pixel(double complex c);

void draw_mandelbrot(SDL_Surface* surface){
    for(int x = 0; x < WIDTH; x++){
        for(int y = 0; y < HEIGHT; y++){
            double real = (x - WIDTH/2.0) * (3.0 / WIDTH);
            double imag = (y - HEIGHT/2.0) * (2.0 / HEIGHT);
            double complex c = real + imag * I;            
            int n = 0;

            int is_in_set = check_pixel(c);
            Uint32 color = COLOR_WHITE;
            
            if (is_in_set) color = COLOR_BLACK; 

            SDL_Rect pixel = (SDL_Rect) { x, y, 1, 1 };
            SDL_FillRect(surface, &pixel, color);
        }
    }
}

 
int check_pixel(double complex c){
    double complex z = 0;
    int i = 0;
    for(; i < MAX_ITER; i++){
        z = z*z + c;
        if(cabs(z) > 2){
            return 0; // aca exploto
        }    
    }
    return i;
} 

   

int main(void){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Mandelbrot", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

    SDL_Surface* surface = SDL_GetWindowSurface(window);


    SDL_Rect white_eraser = (SDL_Rect) { 0, 0, WIDTH, HEIGHT };
    SDL_FillRect(surface, &white_eraser, COLOR_WHITE);
    SDL_Rect pixel = (SDL_Rect) { 100, 100, 10, 10 };
    
    int running = 1;
    SDL_Event event;
    while(running){
        while(SDL_PollEvent(&event))
            if(event.type == SDL_QUIT) running = 0;

        draw_mandelbrot(surface);

        SDL_UpdateWindowSurface(window);
    }


    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
