#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/MPU/mpu.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/SDRAM/sdram.h"
#include "./BSP/BTIM/btim.h"
#include "Task.h"
#include "Tetris.h"
#include <string.h> 


static const uint8_t N = 10, M = 20;
static const uint16_t Startx = 50, Starty = 130;
static const uint16_t Back = 0xAD90;
static const uint16_t Len = 30;

uint8_t Start = 0;

static uint8_t map[12][22];


void Draw_Block(uint8_t x, uint8_t y){
    lcd_fill(Startx + x * Len, Starty + y * Len, Startx + (x + 1) * Len, Starty + y * Len + 2, BLACK);
    lcd_fill(Startx + x * Len, Starty + (y + 1) * Len - 2, Startx + (x + 1) * Len, Starty + (y + 1) * Len, BLACK);

    lcd_fill(Startx + x * Len, Starty + y * Len, Startx + x * Len + 2, Starty + (y + 1) * Len, BLACK);
    lcd_fill(Startx + (x + 1) * Len - 2, Starty + y * Len, Startx + (x + 1) * Len, Starty + (y + 1) * Len, BLACK);

    lcd_fill(Startx + x * Len + 6, Starty + y * Len + 6, Startx + (x + 1) * Len - 6, Starty + (y + 1) * Len - 6, BLACK);
}

Tetris Block[7];
static Tetris cur;

void Update(Tetris New){
    uint8_t i;
    for(i = 0;i < 4;i++){
        uint8_t xx = New.x + New.type[New.cur][i][0], yy = New.y + New.type[New.cur][i][1];
        map[xx][yy] = 2;
    }
}

void ReMove(Tetris Raw){
    uint8_t i;
    for(i = 0;i < 4;i++){
        uint8_t xx = Raw.x + Raw.type[Raw.cur][i][0], yy = Raw.y + Raw.type[Raw.cur][i][1];
        map[xx][yy] = 0;
    }
}

void Tetris_Init(void) {
    uint8_t t11[4][2] = {{1, 0}, {1, 1}, {1, 2}, {1, 3}};
    uint8_t t12[4][2] = {{0, 1}, {1, 1}, {2, 1}, {3, 1}};
    uint8_t t13[4][2] = {{1, 0}, {1, 1}, {1, 2}, {1, 3}};
    uint8_t t14[4][2] = {{0, 1}, {1, 1}, {2, 1}, {3, 1}};

    Block[0].cur = 0;
    Block[0].x = 4;
    Block[0].y = 1;

    memcpy(Block[0].type[0], t11, sizeof(t11));
    memcpy(Block[0].type[1], t12, sizeof(t12));
    memcpy(Block[0].type[2], t13, sizeof(t13));
    memcpy(Block[0].type[3], t14, sizeof(t14));
}

void New_Block(void){
    cur = Block[0];
}

void Tetris_Refresh(void){
    if(Start == 0){
        Tetris_Init();
        Start = 1;
        New_Block();
    }
    lcd_clear(Back);
    Update(cur);
    uint8_t i, j;

    lcd_fill(Startx + Len - 5, Starty + Len - 5, Startx + (N + 1) * Len + 5, Starty + Len, BLUE);
    lcd_fill(Startx + Len - 5, Starty + (M + 1) * Len, Startx + (N + 1) * Len + 5, Starty + (M + 1) * Len + 5, BLUE);

    lcd_fill(Startx + Len - 5, Starty + Len - 5, Startx + Len, Starty + (M + 1) * Len + 5, BLUE);
    lcd_fill(Startx + (N + 1) * Len, Starty + Len - 5, Startx + (N + 1) * Len + 5, Starty + (M + 1) * Len + 5, BLUE);

    for(i = 1;i <= N;i++){
        for(j = 1;j <= M;j++){
            if(map[i][j] != 0){
                Draw_Block(i ,j);
            }
        }
    }
}


void Tetris_Up(void){
    ReMove(cur);
    cur.cur ++;
    if(cur.cur >= 4) cur.cur = 0;
    Tetris_Refresh();
}

void Tetris_Down(void){
    ReMove(cur);
    cur.y ++;

    Tetris_Refresh();
}

void Tetris_Right(void){
    ReMove(cur);
    cur.x ++;
    
    Tetris_Refresh();
}

void Tetris_Left(void){
    ReMove(cur);
    cur.x --;
    
    Tetris_Refresh();
}
