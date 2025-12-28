#include <stdio.h>
#include <stdlib.h>

#include "piece.h"

Piece piece_spawn(){
    int shape = rand() % 7;     
    Uint32 color = colors[rand() % COLORS];
   
    Piece piece;

    piece.x = GRID_WIDTH / 2;
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

         SDL_Rect block = (SDL_Rect) { x0 * SAND_SIZE, y0 * SAND_SIZE, SAND_SIZE, SAND_SIZE };
        SDL_FillRect(surface, &block, piece.color);
    }
}



int piece_can_move_down(Piece piece, int sandId[GRID_HEIGHT][GRID_WIDTH]){
    for(int i = 0; i < 4; i++){
        int gx = piece.x + SHAPES[piece.shape][piece.rot][i][0];
        int gy = 1 + piece.y + SHAPES[piece.shape][piece.rot][i][1];

        if(gy >= GRID_HEIGHT || sandId[gy][gx] != -1){
            return 0;
        }
    } 

    return 1;
}


void make_piece_sand(Piece piece, struct Sand** sands, int* sandAmount, int* allocatedSand){
    for(int i = 0; i < 4; i++){
        int gx = piece.x + SHAPES[piece.shape][piece.rot][i][0];
        int gy = piece.y + SHAPES[piece.shape][piece.rot][i][1];
        if(gy < 0) continue ; // fuera de la grilla, esot es perder??????? 

        if(*sandAmount >= *allocatedSand){
            *allocatedSand += MEMORY_BLOCK;
            *sands = realloc(*sands, sizeof(struct Sand) * (*allocatedSand));
        }

        (*sands)[*sandAmount].x = gx;
        (*sands)[*sandAmount].y = gy;

        (*sands)[*sandAmount].size = SAND_SIZE;
        (*sands)[*sandAmount].color = piece.color;
        
        sandId[gy][gx] = *sandAmount;

        (*sandAmount)++;
    }
}
