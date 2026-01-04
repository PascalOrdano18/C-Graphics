
#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>

#define WIDTH 1200
#define HEIGHT 900

#define COLOR_WHITE 0xFFFFFFFF   // es un int, 4 bytes
#define COLOR_BLACK 0x0
#define COLOR_BLUE 0x000000FF
#define COLOR_YELLOW 0xFFFFFF00
#define COLOR_GRAY 0x88888888

#define RAYS_AMOUNT 100
#define OBJECTS_AMOUNT 0

struct Ray{
    double x_start;
    double y_start;
    double angle;
};

typedef struct{
    double x;
    double y;
    double radius;
    double angle;
} Cone;

typedef struct{
    double x;
    double y;
    double width;
    double height;
    double n;  // indice de refraccion. El del aire es 1, este va a ser un medio con n != 1 para refractar la luz
} Medium;

void set_pixel(SDL_Surface* surface, int x, int y, Uint32 color){
    if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return ;
    Uint32* pixels = (Uint32*)surface->pixels;
    pixels[y * surface->w + x] = color;
}

void draw_medium(SDL_Surface* surface, Medium* medium){
    SDL_Rect med = (SDL_Rect) { medium->x, medium->y, medium->width, medium->height }; 
    SDL_FillRect(surface, &med, COLOR_GRAY);
}  


void draw_ray(SDL_Surface* surface, Medium* medium, double angle){
    double x = 200.0;
    double y = 200.0;

    double dx = cos(angle);
    double dy = sin(angle);

    double n1 = 1.0;

    for(int i = 0; i < 4000; i++){
        set_pixel(surface, (int)x, (int)y, COLOR_YELLOW);
        if(y >= medium->y && y < medium->y + medium->height && x >= medium->x && x < medium->x + medium->width){

            double n2 = medium->n;
            double sin_theta1 = dx;
            double sin_theta2 = (n1 / n2) * sin_theta1;

            // refleccion total
            if(fabs(sin_theta2) <= 1.0){
                double cos_theta2 = sqrt(1.0 - sin_theta2 * sin_theta2);
                dx = sin_theta2;
                dy = cos_theta2;
            }
            n1 = n2;
        }
        x += dx;
        y += dy;
        if( x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) break;
    } 
}

int main(void){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("LIGHT", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    
    SDL_Surface* surface = SDL_GetWindowSurface(window);
    SDL_Rect erase_rect = (SDL_Rect) { 0, 0, WIDTH, HEIGHT };

    Medium medium = { 0, 600, WIDTH, HEIGHT - 600, 2 };

    double angle = M_PI / 4;

    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {   // es el evento de cuando aprento la X
                running = 0;
            }
            if(event.type == SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_ESCAPE){
                    running = 0;
                } 
                if(event.key.keysym.sym == SDLK_UP){
                    angle += 0.01;
                }
                if(event.key.keysym.sym == SDLK_DOWN){
                    angle -= 0.01;
                }
            }
        }
        
        SDL_FillRect(surface, &erase_rect, COLOR_BLACK);
        draw_medium(surface, &medium);

        draw_ray(surface, &medium, angle);

        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;

}
