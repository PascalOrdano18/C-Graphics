#include <SDL2/SDL.h>

#define WIDTH 500
#define HEIGHT 800
#define SAND_SIZE 10

#define GRID_WIDTH WIDTH / SAND_SIZE
#define GRID_HEIGHT HEIGHT / SAND_SIZE

#define COLORS 4
#define COLOR_BLACK 0x00000000
#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_BLUE 0xFF0000FF
#define COLOR_RED 0xFFFF0000
#define COLOR_GREEN 0xFF00FF00
#define BLOCK 20

#define SPEED 1

#define MEMORY_BLOCK 2000

// -1 si no hay sand, id int si hay
int sandId[GRID_HEIGHT][GRID_WIDTH];


struct Sand {
    int x;
    int y;
    int size;   // lado del cuadrado 
    Uint32 color; 
};

typedef struct{
    int x;
    int y;
    int shape;  // 0 a 5, con I, T, L, S, Z, O, J  
    int rot; // de 0 a 3
    Uint32 color;
    char active; // 0 si no esta cayendo, 1 si
} Piece;

static const Uint32 colors[COLORS] = {COLOR_WHITE, COLOR_BLUE, COLOR_GREEN, COLOR_RED};


// [shape][rotation][bloque][x , y]

static const int SHAPES[7][4][4][2] = {

    // ===== I =====
    {
        {{0,0},{1,0},{2,0},{3,0}},
        {{1,-1},{1,0},{1,1},{1,2}},
        {{0,1},{1,1},{2,1},{3,1}},
        {{2,-1},{2,0},{2,1},{2,2}}
    },

    // ===== O =====
    {
        {{0,0},{1,0},{0,1},{1,1}},
        {{0,0},{1,0},{0,1},{1,1}},
        {{0,0},{1,0},{0,1},{1,1}},
        {{0,0},{1,0},{0,1},{1,1}}
    },

    // ===== T =====
    {
        {{1,0},{0,1},{1,1},{2,1}},
        {{1,0},{1,1},{2,1},{1,2}},
        {{0,1},{1,1},{2,1},{1,2}},
        {{1,0},{0,1},{1,1},{1,2}}
    },

    // ===== L =====
    {
        {{0,0},{0,1},{1,1},{2,1}},
        {{1,0},{2,0},{1,1},{1,2}},
        {{0,1},{1,1},{2,1},{2,2}},
        {{1,0},{1,1},{0,2},{1,2}}
    },

    // ===== J =====
    {
        {{2,0},{0,1},{1,1},{2,1}},
        {{1,0},{1,1},{1,2},{2,2}},
        {{0,1},{1,1},{2,1},{0,2}},
        {{0,0},{1,0},{1,1},{1,2}}
    },

    // ===== S =====
    {
        {{1,0},{2,0},{0,1},{1,1}},
        {{1,0},{1,1},{2,1},{2,2}},
        {{1,1},{2,1},{0,2},{1,2}},
        {{0,0},{0,1},{1,1},{1,2}}
    },

    // ===== Z =====
    {
        {{0,0},{1,0},{1,1},{2,1}},
        {{2,0},{1,1},{2,1},{1,2}},
        {{0,1},{1,1},{1,2},{2,2}},
        {{1,0},{0,1},{1,1},{0,2}}
    }
};



Piece piece_spawn();
void draw_piece(SDL_Surface* surface, Piece piece);
int piece_can_move_down(Piece piece, int sandId[GRID_HEIGHT][GRID_WIDTH]);
void make_piece_sand(Piece piece, struct Sand** sands, int* sandAmount, int* allocatedSand);
