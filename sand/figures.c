#include <stdio.h>
#include <stdlib.h>

#include "piece.h"

Piece piece_spawn(int grid_width){
    //int shape = rand() % 7;  
    int shape = 0; // hardcodeado para que sea una linea ahora.
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


void draw_piece(SDL_Surface* surface, Piece piece){
    switch (piece.shape) {
        case 0: // I
            SDL_Rect line = (SDL_Rect) { piece.x, piece.y, piece.x + SAND_LEN, piece.y + (SAND_LEN * 4) };
            SDL_FillRect(surface, &line, piece.color);
            break;
        break;
    }
    
}




