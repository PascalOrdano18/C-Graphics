#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h>

#define WIDTH 900
#define HEIGHT 900

#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_GRAY 0x88888888
#define COLOR_BLACK 0x00000000

static inline int sx(double x, double cx, double zoom){
    return (int) llround((x - cx) * zoom + WIDTH / 2.0);
}

static inline int sy(double x, double cx, double zoom){
    return (int) llround((x - cx) * zoom + HEIGHT / 2.0);
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

void draw_function(SDL_Surface* surface, double cx, double cy, double zoom){
   for(int px = 0; px < WIDTH; px++){
       double x = cx + (px - WIDTH / 2.0) / zoom;
       double y = sin(x);

       int py = sy(y, cy, zoom);
       if(py >= 0 && py < HEIGHT){
            SDL_Rect point = (SDL_Rect) { px, py, 1, 1 };
            SDL_FillRect(surface, &point, COLOR_WHITE);
       }
   } 
}

int main(int argc, char* argv[]){
    // if(argc < 2) return 0; // necesitamos que nos pasen datos de la funcion

    double zoom = WIDTH / 200.0;
    double cx = 0;
    double cy = 0;


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
           if(event.type == SDL_KEYDOWN){
               switch(event.key.keysym.sym){
                case SDLK_ESCAPE: running = 0; break;

                // zoom
                case SDLK_EQUALS: // '+' usually shift+'='
                case SDLK_PLUS:   zoom *= 1.1; break;
                case SDLK_MINUS:  zoom /= 1.1; break;

                case SDLK_LEFT:  cx -= 10.0 / zoom * 50; break;  // or simpler: cx -= 5;
                case SDLK_RIGHT: cx += 10.0 / zoom * 50; break;
                case SDLK_UP:    cy += 10.0 / zoom * 50; break;
                case SDLK_DOWN:  cy -= 10.0 / zoom * 50; break;
            }
               }
       }

       SDL_FillRect(surface, &erase_rect, COLOR_BLACK);
       draw_axis(surface, cx, cy, zoom);
       draw_function(surface, cx, cy, zoom);

       SDL_UpdateWindowSurface(window);
   }
   SDL_DestroyWindowSurface(window);
   SDL_Quit();
   return 0;
}
