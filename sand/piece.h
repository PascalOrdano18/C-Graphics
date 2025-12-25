#include <SDL2/SDL.h>

#define COLORS 4
#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_BLUE 0xFF0000FF
#define COLOR_RED 0xFFFF0000
#define COLOR_GREEN 0xFF00FF00
#define BLOCK 20

typedef struct{
    int x;
    int y;
    int shape;  // 0 a 5, con I, T, L, S, Z, O, J  
    int rot; // de 0 a 3
    Uint32 color;
    char active; // 0 si no esta cayendo, 1 si
} Piece;

static const Uint32 colors[COLORS] = {COLOR_WHITE, COLOR_BLUE, COLOR_GREEN, COLOR_RED};

Piece piece_spawn(int grid_width);
