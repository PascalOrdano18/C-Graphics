#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>
#include <sstream>
#include <string>

#define WIDTH 700
#define HEIGHT 700

#define GRAVITY 3600.0f

struct Player{
    float x = 100.0f;
    float y = 100.0f;
    float speed = 300.0f;
    int size = 32;
};

struct Enemy{
    float x;
    float y;
    int size = 20;
};


struct Obstacle{
    float x;
    float y;
    int height = 25;
    int length;
};

struct Level{
    float spawnX;
    float spawnY;
    std::vector<Obstacle> obstacles;
    float objX;
    float objY;
};

struct Object{
    float x;
    float y;
    int size = 25;
};


void draw_obstacles(SDL_Renderer* renderer, const std::vector<Obstacle>& obstacles){
    SDL_SetRenderDrawColor(renderer, 200, 80, 80, 255);

    for(const Obstacle& obs : obstacles){
        SDL_Rect rect = {
            (int)obs.x,
            (int)obs.y,
            obs.length,
            obs.height,
        };
        SDL_RenderFillRect(renderer, &rect);
    }
}


void draw_object(SDL_Renderer* renderer, const Object& obj){
    SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
    SDL_Rect rect = { (int)obj.x, (int)obj.y, obj.size, obj.size };
    SDL_RenderFillRect(renderer, &rect);
}


bool intersects(const Player& p, const Obstacle& obs){
    float epsilon = 0.00001f;
    return !(
               p.x + p.size <= obs.x + epsilon
            || p.x >= obs.x + obs.length - epsilon
            || p.y + p.size <= obs.y + epsilon
            || p.y >= obs.y + obs.height - epsilon
        );
}

bool intersects_obj(const Player& p, const Object& obs){
    float epsilon = 0.00001f;
    return !(
               p.x + p.size <= obs.x + epsilon
            || p.x >= obs.x + obs.size - epsilon
            || p.y + p.size <= obs.y + epsilon
            || p.y >= obs.y + obs.size - epsilon
        );
}



Level loadLevel(const std::string& path){
    Level level;

    std::ifstream file(path);
    if(!file.is_open()) return level;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "spawn") {
            iss >> level.spawnX >> level.spawnY;
        } else if (type == "obstacle") {
            Obstacle obs;
            iss >> obs.x >> obs.y >> obs.length >> obs.height;
            level.obstacles.push_back(obs);
        } else if (type == "object"){
            iss >> level.objX >> level.objY;
        }
    }

    return level;
}



int main(){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Move Around", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    std::mt19937 rng(std::random_device{}());

    float yVel = 0.0f;
    bool onGround = false;
    bool up = false, down = false, right = false, left = false;
    bool jmp = false;

    // std::uniform_int_distribution<int> distY(0.0f, 600.f);
    // std::uniform_int_distribution<int> distL(25, 300);
    //
    // int obs_size = 20;
    // std::vector<Obstacle> obstacles;
    // obstacles.reserve(obs_size);
    // for(int i = 0; i < obs_size; i++){
    //     obstacles.push_back({
    //         (float)distY(rng),
    //         (float)distY(rng),
    //         25,
    //         (int)distL(rng)
    //     });
    // }

    Level currentLevel = loadLevel("levels/level1.txt");
    Player p;
    p.x = currentLevel.spawnX;
    p.y = currentLevel.spawnY;

    Object obj = {
        currentLevel.objX,
        currentLevel.objY,
        25,
    };

    std::vector<Obstacle> obstacles = currentLevel.obstacles;

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
                    case SDLK_SPACE: jmp = true; break;
                }
            }
            if(event.type == SDL_KEYUP && event.key.repeat == 0){
                switch(event.key.keysym.sym){
                    case SDLK_w: up = false; break;
                    case SDLK_s: down = false; break;
                    case SDLK_d: right = false; break;
                    case SDLK_a: left = false; break;
                    case SDLK_SPACE: jmp = false; break;
                }
            }
        }

        Uint64 nowCounter = SDL_GetPerformanceCounter();
        double dt = (nowCounter - prevCounter) / freq;
        prevCounter = nowCounter;


        float vx = 0.0f;
        float vy = 0.0f;

        //if(up) vy -= 1.0f;
        //if(down) vy += 1.0f;
        if(right) vx += 1.0f;
        if(left) vx -= 1.0f;

        float dx = vx * p.speed * (float) dt;
        float friction = 0.9f;

        yVel += GRAVITY * (float)dt;
        if(jmp && onGround){
            yVel = -790.0f;
            onGround = false;
        }

        float dy = yVel * (float) dt;


        p.x += dx;

        for(const Obstacle& obs : obstacles){
            if(intersects(p, obs)){
                if(dx > 0){
                    p.x = obs.x - p.size;
                } else if(dx < 0){
                    p.x = obs.x + obs.length;
                }
            }
        }

        p.y += dy;
        for(const Obstacle& obs : obstacles){
            if(intersects(p, obs)){
                if(dy > 0){
                    p.y = obs.y - p.size;
                    yVel = 0.0f;
                    onGround = true;
                } else if(dy < 0){
                    p.y = obs.y + obs.height;
                    yVel = 0.0f;
                }
            }
        }

        if(p.x < 0) p.x = 0;
        if(p.y < 0) p.y = 0;

        if(p.x >= WIDTH - p.size) p.x = WIDTH - p.size;
        if(p.y >= HEIGHT - p.size){
            p.y = HEIGHT - p.size;
            onGround = true;
        }

        

        SDL_SetRenderDrawColor(renderer, 10, 10, 12, 255);
        SDL_RenderClear(renderer);

        SDL_Rect rect = { (int)p.x, (int) p.y, p.size, p.size };
        SDL_SetRenderDrawColor(renderer, 80, 220, 120, 255);
        SDL_RenderFillRect(renderer, &rect);

        draw_obstacles(renderer, obstacles);
        draw_object(renderer, obj);
        if(intersects_obj(p, obj)){
            SDL_Rect rect = { 0, 0, WIDTH, HEIGHT };
            SDL_SetRenderDrawColor(renderer, 80, 220, 120, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}










