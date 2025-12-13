#include <stdio.h>
#include <stdlib.h>
#include <complex.h>  // me estaba volviendo loco
#include <SDL2/SDL.h>


#define WIDTH 900
#define HEIGHT 600

#define COLOR_BLACK 0x00000000
#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_F 0xFFFF0000

#define MAX_ITER 200


int check_pixel(double complex c);

static inline Uint32 mandelbrot_color(SDL_PixelFormat* fmt, int it, int max_iter) {
    if (it >= max_iter) {
        return SDL_MapRGB(fmt, 0, 0, 0); // adentro: negro
    }

    // gradiente suave "fractal clásico"
    double t = (double)it / (double)max_iter;

    Uint8 r = (Uint8)(9.0  * (1 - t) * t * t * t * 255.0);
    Uint8 g = (Uint8)(15.0 * (1 - t) * (1 - t) * t * t * 255.0);
    Uint8 b = (Uint8)(8.5  * (1 - t) * (1 - t) * (1 - t) * t * 255.0);

    return SDL_MapRGB(fmt, r, g, b);
}

void draw_mandelbrot(SDL_Surface* surface){
    double scale = 3.0 / WIDTH;
    
    for(int x = 0; x < WIDTH; x++){
        for(int y = 0; y < HEIGHT; y++){

            double real = (x - WIDTH/2.0) * scale - 0.5;
            double imag = (y - HEIGHT/2.0) * scale;
            double complex c = real + imag * I;            

            int it = check_pixel(c);

            Uint32 color = mandelbrot_color(surface->format, it, MAX_ITER); 

            SDL_Rect pixel = (SDL_Rect) { x, y, 1, 1 };
            SDL_FillRect(surface, &pixel, color);
        }
    }
}

 
int check_pixel(double complex c){
    double complex z = 0;
    for(int i = 0; i < MAX_ITER; i++){
        z = z*z + c;
        if(cabs(z) > 2){
            return i; // aca exploto
        }    
    }
    return MAX_ITER;
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
