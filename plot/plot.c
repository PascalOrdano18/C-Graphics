#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h>

#define WIDTH 900
#define HEIGHT 900

#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_GRAY 0x88888888
#define COLOR_BLUE 0xFF0000FF
#define COLOR_RED 0xFFFF0000
#define COLOR_GREEN 0xFF00FF00
#define COLOR_BLACK 0x00000000

static inline int sx(double x, double cx, double zoom){
    return (int) llround((x - cx) * zoom + WIDTH / 2.0);

static inline int sy(double y, double cy, double zoom){
    return (int) llround(HEIGHT / 2.0 - (y - cy) * zoom);
}

static inline int in_bounds(int x, int y){
    return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT; 
}


void draw_axis(SDL_Surface* surface, double cx, double cy, double zoom){
    int x0 = sx(0.0, cx, zoom); 
    int y0 = sy(0.0, cy, zoom); 

    if(y0 >= 0 && y0 < HEIGHT){
        for(int i = 0; i < WIDTH; i++){
            SDL_Rect point = (SDL_Rect) { i, y0, 1, 1 };
            SDL_FillRect(surface, &point, COLOR_GRAY);
        }
    }
    if(x0 >= 0 && x0 < WIDTH){
        for(int i = 0; i < HEIGHT; i++){
            SDL_Rect point = (SDL_Rect) { x0, i, 1, 1 };
            SDL_FillRect(surface, &point, COLOR_GRAY);
        }
    }
}

// recibe un puntero a funcion
// tipo sin(x), cos(x), x*x 
// entonces => double f(double x)
// ==> double (*f) (double)
void draw_function(SDL_Surface* surface, Uint32 color, double cx, double cy, double zoom, double (*f)(double)){
   for(int px = 0; px < WIDTH; px++){
       double x = cx + (px - WIDTH / 2.0) / zoom;
       double y = f(x);

       int py = sy(y, cy, zoom);
       if(py >= 0 && py < HEIGHT){
            SDL_Rect point = (SDL_Rect) { px, py, 1, 1 };
            SDL_FillRect(surface, &point, color);
       }
   } 
}


double cuadratica(double x){
    return x * x;
}

int main(int argc, char* argv[]){
    double zoom = 1;
    double cx = 0.0;
    double cy = 0.0;

   SDL_Init(SDL_INIT_VIDEO);
   SDL_Window* window = SDL_CreateWindow("Function Plotter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

   SDL_Surface* surface = SDL_GetWindowSurface(window);
   SDL_Rect erase_rect = (SDL_Rect) { 0, 0, WIDTH, HEIGHT };

   int running = 1;
   SDL_Event event;

   double movement_speed = 1.0;
   while(running){
       while(SDL_PollEvent(&event)){
           if(event.type == SDL_QUIT){
               running = 0;
           }
           if(event.type == SDL_KEYDOWN){
                switch(event.key.keysym.sym){
                    // zoom
                    case SDLK_EQUALS:
                    case SDLK_PLUS:   zoom *= 1.1; break;
                    case SDLK_MINUS:  zoom /= 1.1; break;
                   }
             }
       }
       SDL_PumpEvents();
       const Uint8* keys = SDL_GetKeyboardState(NULL);

        if(keys[SDL_SCANCODE_ESCAPE]) running = 0;
            movement_speed = 1.0;
            if(keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT]){
                movement_speed = 2.0;
            }
            double step = movement_speed / zoom;

            if(keys[SDL_SCANCODE_LEFT])  cx -= step;
            if(keys[SDL_SCANCODE_RIGHT]) cx += step;
            if(keys[SDL_SCANCODE_UP])    cy += step;
            if(keys[SDL_SCANCODE_DOWN])  cy -= step;

       SDL_FillRect(surface, &erase_rect, COLOR_BLACK);
       draw_axis(surface, cx, cy, zoom);
       draw_function(surface, COLOR_GREEN, cx, cy, zoom, cuadratica);
       draw_function(surface, COLOR_BLUE, cx, cy, zoom, cos);
       draw_function(surface, COLOR_WHITE, cx, cy, zoom, sin);
       draw_function(surface, COLOR_RED, cx, cy, zoom, tan);

       SDL_UpdateWindowSurface(window);
   }
   SDL_DestroyWindowSurface(window);
   SDL_Quit();
   return 0;
}

