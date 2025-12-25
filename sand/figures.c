#include <stdio.h>
#include <stdlib.h>

#include "piece.h"


Piece piece_spawn(int grid_width){
    srand((unsigned)time(NULL));

    int shape = rand() % 7;  
    Uint32 color = colors[rand() % COLORS];
   
    Piece piece;

    piece.x = grid_width / 2;
    piece.y = 0;
    piece.shape = shape;
    piece.color = color;
    piece.active = 1;
    piece.rot = 0;
    
    return piece;
}







