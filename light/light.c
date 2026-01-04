
#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>

#define WIDTH 1200
#define HEIGHT 900

#define COLOR_WHITE 0xFFFFFFFF   // es un int, 4 bytes
#define COLOR_BLACK 0x0
#define COLOR_BLUE 0x000000FF
#define COLOR_YELLOW 0xFFFFFEFF
#define COLOR_GRAY 0x88888888

#define RAYS_AMOUNT 100
#define OBJECTS_AMOUNT 1

struct Ray{
    double x_start;
    double y_start;
    double angle;
};

struct Circle {
    double x;
    double y;
    double radius;
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

void FillCircle(SDL_Surface* surface, struct Circle circle, Uint32 color){
    // circle.x - circle.radius te da el vertice superior izquierdo del cuadrado que contiene al circulo
    double radius_squared = pow(circle.radius, 2);
    for(double x = circle.x - circle.radius; x <= circle.x + circle.radius; x++){
        for(double y = circle.y - circle.radius; y <= circle.y + circle.radius; y++){
            double distance_squared = pow(x - circle.x, 2) + pow(y - circle.y, 2); 
            if(distance_squared < radius_squared){
                SDL_Rect pixel = (SDL_Rect){x, y, 1, 1};   // dibujo un pixel 
                SDL_FillRect(surface, &pixel, color);
            }
        }
    }
}

// en cada momento, se generan N rays desde el centro del circulo, cada uno tiene su propio angulo y el mismo x_start y y_start
void generate_rays(struct Circle circle, struct Ray rays[RAYS_AMOUNT]){
    double max_angle = 2 * M_PI;
    for(int i = 0; i < RAYS_AMOUNT; i++){
        double angle = ((double) i / RAYS_AMOUNT) * 2 * M_PI;
        struct Ray ray = {circle.x, circle.y, angle};
        rays[i] = ray;
    }
}


void FillRays(SDL_Surface* surface, struct Ray rays[RAYS_AMOUNT], struct Circle objects[], Uint32 color){
    for(int i = 0; i < RAYS_AMOUNT; i++){
        struct Ray ray = rays[i];

        int out_of_screen = 0;
        int hit_object = 0;

        // x_start 
        // y_start += sin(angle)

        double x_draw = ray.x_start;
        double y_draw = ray.y_start; 
        while(!out_of_screen && !hit_object){
            x_draw += cos(ray.angle);
            y_draw += sin(ray.angle);

            SDL_Rect pixel_ray = (SDL_Rect) {x_draw, y_draw, 1, 1};
            SDL_FillRect(surface, &pixel_ray, color);

            for(int i = 0; i < OBJECTS_AMOUNT; i++){
                double radius_squared = pow(objects[i].radius, 2);
                double distance_squared = pow(x_draw - objects[i].x, 2) + pow(y_draw - objects[i].y, 2); 
                if(distance_squared < radius_squared){
                    hit_object = 1; 
                }
            }

            if(x_draw > WIDTH || x_draw < 0 || y_draw > HEIGHT || y_draw < 0){
                out_of_screen = 1;
            }
        }
    }
}

void draw_medium(SDL_Surface* surface, Medium* medium){
    SDL_Rect med = (SDL_Rect) { medium->x, medium->y, medium->width, medium->height }; 
    SDL_FillRect(surface, &med, COLOR_GRAY);
}  

int main(void){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Raytracing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    
    SDL_Surface* surface = SDL_GetWindowSurface(window);


   
    SDL_Rect erase_rect = (SDL_Rect) { 0, 0, WIDTH, HEIGHT };


    Medium medium = { 0, 600, WIDTH, 100, 2 };

    struct Circle circle = { 500, 200, 35 };  // Luz

    struct Circle shadow_circle = { 300, 300, 75 }; // circulo azul objeto

    struct Circle objects[OBJECTS_AMOUNT];

    objects[0] = shadow_circle;

    struct Ray rays[RAYS_AMOUNT];
    generate_rays(circle, rays);

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
            }
            if(event.type == SDL_MOUSEMOTION && event.motion.state != 0){
                circle.x = event.motion.x;
                circle.y = event.motion.y;
                generate_rays(circle, rays);
            }
        }
        
        SDL_FillRect(surface, &erase_rect, COLOR_BLACK);

        FillCircle(surface, circle, COLOR_WHITE);
        FillCircle(surface, shadow_circle, COLOR_BLUE);
        
        FillRays(surface, rays, objects, COLOR_WHITE);

        draw_medium(surface, &medium);

        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;

}
