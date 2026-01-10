#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>


#define WIDTH 700
#define HEIGHT 700

#define CELL_LEN 12
#define GRID_WIDTH WIDTH / CELL_LEN
#define GRID_HEIGHT HEIGHT / CELL_LEN

#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_BLACK 0x00000000
#define COLOR_BLUE 0xFF1111DD
#define COLOR_RED 0xFFFF0000
#define COLOR_GREEN 0xFF00FF00
#define COLOR_YELLOW 0xFFFFFF00

struct Cell{
    int x;
    int y;
    Uint32 color;

    bool operator==(const Cell& other) const {
        return x == other.x && y == other.y;
    }
};


void draw_grid(SDL_Surface* surface, Cell grid[GRID_HEIGHT][GRID_WIDTH]){
    for(int y = 0; y < GRID_HEIGHT; y++){
        for(int x = 0; x < GRID_WIDTH; x++){
            SDL_Rect cell = (SDL_Rect) { x * CELL_LEN, y * CELL_LEN, CELL_LEN - 1, CELL_LEN -1  };
            SDL_FillRect(surface, &cell, grid[y][x].color);
        }
    }
}


void bfs(SDL_Surface* surface, Cell grid[GRID_HEIGHT][GRID_WIDTH], int points[2][2]){
    static std::vector<Cell> neighbours;
    static std::vector<Cell> visited;
    static std::map<std::pair<int,int>, std::pair<int,int>> parent;
    static bool initialized = false;
    static bool done = false;
    static bool found = false;

    if(done) return;

    Cell start = { points[0][1], points[0][0], COLOR_RED };
    Cell end = { points[1][1], points[1][0], COLOR_RED };

    if(!initialized){
        neighbours.push_back(start);
        parent[{start.x, start.y}] = {-1, -1};
        initialized = true;
    }

    // Process just one cell per call to animate
    if(neighbours.empty()){
        done = true;
        return;
    }

    Cell current_neighbour = neighbours.front();
    neighbours.erase(neighbours.begin());

    if(std::find(visited.begin(), visited.end(), current_neighbour) != visited.end()) return;

    visited.push_back(current_neighbour);
    if(!(current_neighbour == start) && !(current_neighbour == end)){
        grid[current_neighbour.y][current_neighbour.x].color = COLOR_GREEN;
    }

    if(current_neighbour == end){
        // Reconstruct path
        std::pair<int,int> curr = {end.x, end.y};
        while(curr.first != -1 && curr.second != -1){
            if(!(curr.first == start.x && curr.second == start.y) &&
               !(curr.first == end.x && curr.second == end.y)){
                grid[curr.second][curr.first].color = COLOR_YELLOW;
            }
            curr = parent[curr];
        }
        done = true;
        return;
    }

    for(int i = -1; i <= 1; i++){
        for(int j = -1; j <= 1; j++){
            if(i == 0 && j == 0) continue;
            int x = current_neighbour.x + j;
            int y = current_neighbour.y + i;
            if(x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT || grid[y][x].color == COLOR_BLUE) continue;
            Cell next = { x, y, COLOR_BLACK };

            if(std::find(visited.begin(), visited.end(), next) != visited.end()) continue;
            if(parent.find({x, y}) != parent.end()) continue;

            parent[{x, y}] = {current_neighbour.x, current_neighbour.y};
            neighbours.push_back(next);
        }
    }
}




int main(void){
    SDL_Init(SDL_INIT_VIDEO); 
    SDL_Window* window = SDL_CreateWindow("Algorithm Visualizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);

    const Uint32 RENDER_MS = 16;
    const Uint32 SIM_MS = 20;

    Uint32 last = SDL_GetTicks();
    Uint32 acc = 0;

    static Cell grid[GRID_HEIGHT][GRID_WIDTH];
    for(int i = 0; i < GRID_HEIGHT; i++){
        for(int j = 0; j < GRID_WIDTH; j++){
            grid[i][j] = { j, i , COLOR_WHITE };
        }
    }
    
    int red_cell_flag = 0;
    int start_bfs = 0;
    int points[2][2] = {
        { -1, -1 },
        { -1, -1 }
    };

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
                if(event.key.keysym.sym == SDLK_SPACE){
                    start_bfs = 1;
                }
            }

            if(event.type == SDL_MOUSEBUTTONDOWN || (event.type == SDL_MOUSEMOTION && event.motion.state != 0)){
                int x = event.motion.x / CELL_LEN;
                int y = event.motion.y / CELL_LEN;
                int right = (event.type == SDL_MOUSEBUTTONDOWN) ? (event.button.button == SDL_BUTTON_RIGHT) : (event.motion.state & SDL_BUTTON_RMASK);

                if(right){
                    if(points[0][0] == -1){
                        points[0][0] = y;
                        points[0][1] = x;
                    } else {
                        points[1][0] = y;
                        points[1][1] = x;
                    }
                    grid[y][x] = { x, y, COLOR_RED };
                } else {
                    grid[y][x] = { x, y, COLOR_BLUE };
                }
            }
        }

        draw_grid(surface, grid);
        if(points[0][0] != -1 && points[1][0] != -1) start_bfs = 1; 
        while(acc >= SIM_MS){
            if(start_bfs) bfs(surface, grid, points);
            acc -= SIM_MS;
        }
        SDL_UpdateWindowSurface(window);
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}













