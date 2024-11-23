#ifndef __TETRIS_H
#define __TETRIS_H

#include "./SYSTEM/sys/sys.h"

typedef struct {
    uint8_t x, y;
    uint8_t cur;

    uint8_t type[4][4][2];
}Tetris;

void Tetris_Init(void);
void Tetris_Refresh(void);
void Tetris_Up(void);
void Tetris_Down(void);
void Tetris_Left(void);
void Tetris_Right(void);


#endif


