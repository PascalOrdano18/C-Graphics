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
            SDL_Rect line = (SDL_Rect) { piece.x, piece.y, SAND_SIZE * 4, (SAND_SIZE * 4 *4) };
            SDL_FillRect(surface, &line, piece.color);
            break;
        break;
    }
    
}

void check_borders(Piece piece){
    for(int i = 0; i < 4; i++){
       int gx = piece.x + SHAPES[piece.shape][piece.rot][i][0];
       int gy = piece.y + SHAPES[piece.shape][piece.rot][i][1];
       if(gx <= 0 || gx >= GRID_WIDTH) return ;
       if(gy <= 0 || gy >= GRID_HEIGHT) return ;
    }
}


