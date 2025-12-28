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

int check_borders(Piece piece){
    for(int i = 0; i < 4; i++){
       int gx = piece.x + SHAPES[piece.shape][piece.rot][i][0];
       int gy = piece.y + SHAPES[piece.shape][piece.rot][i][1];
       if(gx < 0 || gx >= GRID_WIDTH) return 0;
       if(gy < 0 || gy >= GRID_HEIGHT) return 0;
    }
    return 1;
}

void draw_piece(SDL_Surface* surface, Piece piece){
    if(!check_borders(piece)) return ;
    for(int i = 0; i < 4; i++){
        int x0 = piece.x + SHAPES[piece.shape][piece.rot][i][0];
        int y0 = piece.y + SHAPES[piece.shape][piece.rot][i][1];

         SDL_Rect block = (SDL_Rect) { x0 * SAND_SIZE, y0 * (SAND_SIZE + 20), SAND_SIZE, SAND_SIZE };
        SDL_FillRect(surface, &block, piece.color);
    }
}



