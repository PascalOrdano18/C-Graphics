#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <time.h>

#define WIDTH 900
#define HEIGHT 600
#define TAMANO 20
#define PISO 450 + TAMANO

#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_GRAY 0xFF888888
#define COLOR_BLACK 0x00000000
#define COLOR_RED 0xFFFF0000

#define MAX_OBSTACLES 10
#define OBSTACLE_WIDTH 25

typedef struct {
    int x;
    int y;
    int tamano;
    Uint32 color;
} Dino;

typedef struct {
    float x;
    float y;
    float vx;
    int width;
    int height;
    Uint32 color;
    int active;
} Obstacle;

Obstacle obstacles[MAX_OBSTACLES];
int spawn_timer = 0;
int spawn_interval = 80;

void draw_dino(SDL_Surface* surface, Dino dinosaurio){
    SDL_Rect dino_rect = { dinosaurio.x, dinosaurio.y, dinosaurio.tamano, dinosaurio.tamano }; 
    SDL_FillRect(surface, &dino_rect, dinosaurio.color); 
}


void init_obstacles(void) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacles[i].active = 0;
    }
}

void spawn_obstacle(void) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!obstacles[i].active) {
            int height = 20 + rand() % 40;
            obstacles[i].x = WIDTH;
            obstacles[i].y = PISO - height;
            obstacles[i].vx = -4.0f - (rand() % 3);
            obstacles[i].width = OBSTACLE_WIDTH;
            obstacles[i].height = height;
            obstacles[i].color = COLOR_RED;
            obstacles[i].active = 1;
            break;
        }
    }
}

void update_obstacles(void) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].active) {
            obstacles[i].x += obstacles[i].vx;
            if (obstacles[i].x + obstacles[i].width < 0) {
                obstacles[i].active = 0;
            }
        }
    }
}

void draw_obstacles(SDL_Surface* surface) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].active) {
            SDL_Rect obs = {
                (int)obstacles[i].x,
                (int)obstacles[i].y,
                obstacles[i].width,
                obstacles[i].height
            };
            SDL_FillRect(surface, &obs, obstacles[i].color);
        }
    }
}

int check_collision(Dino* dino, Obstacle* obs) {
    if (!obs->active) return 0;

    return (dino->x < obs->x + obs->width &&
            dino->x + dino->tamano > obs->x &&
            dino->y < obs->y + obs->height &&
            dino->y + dino->tamano > obs->y);
}

int check_all_collisions(Dino* dino) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (check_collision(dino, &obstacles[i])) {
            return 1;
        }
    }
    return 0;
}


int main(void){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Dino", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

    SDL_Surface* surface = SDL_GetWindowSurface(window);

    srand(time(NULL));
    init_obstacles();

    const Uint32 SIM_MS = 20;

    Uint32 last = SDL_GetTicks();
    Uint32 acc = 0;

    Dino dinosaurio = {
        100,
        450,
        TAMANO,
        COLOR_WHITE
    };

    int vy = 0;
    int gravity = 1;
    int jumpSpeed = -12;

    SDL_Rect piso = { 0, dinosaurio.y + dinosaurio.tamano, WIDTH, HEIGHT - (dinosaurio.y + dinosaurio.tamano) };

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
                    if(dinosaurio.y == 450) vy = jumpSpeed;
                }
            }
        }

        while(acc >= SIM_MS){
            // Dino physics
            vy += gravity;
            dinosaurio.y += vy;
            if(dinosaurio.y >= 450){
                dinosaurio.y = 450;
                vy = 0;
            }

            // Obstacle spawning
            spawn_timer++;
            if(spawn_timer >= spawn_interval){
                spawn_obstacle();
                spawn_timer = 0;
                // Gradually increase difficulty
                if(spawn_interval > 40) spawn_interval--;
            }

            // Update obstacles
            update_obstacles();

            // Collision detection
            if(check_all_collisions(&dinosaurio)){
                printf("Game Over!\n");
                running = 0;
            }

            // Render
            SDL_FillRect(surface, NULL, COLOR_BLACK);
            SDL_FillRect(surface, &piso, COLOR_GRAY);
            draw_dino(surface, dinosaurio);
            draw_obstacles(surface);
            acc -= SIM_MS;
        }
        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}





