#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


#define BLOCK 20

typedef struct{
    int x;
    int y;
    int shape;  // 0 a 6, con I, T, L, S, Z, O (cuadrado)
    int rot; // de 0 a 3
    Uint32 color;
    char active; // 0 si no esta cayendo, 1 si
} Piece;


void piece_spawn(){
   srand((unsigned)time(NULL));

   int random_piece = rand() % 7;   // 7 porque hay 6 piezas
    

}




