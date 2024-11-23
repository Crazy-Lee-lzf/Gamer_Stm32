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

static uint8_t Start = 0;
static uint8_t over = 1;

static uint8_t map[12][22];


void Draw_Block(uint8_t x, uint8_t y){
    lcd_fill(Startx + x * Len, Starty + y * Len, Startx + (x + 1) * Len, Starty + y * Len + 2, BLACK);
    lcd_fill(Startx + x * Len, Starty + (y + 1) * Len - 2, Startx + (x + 1) * Len, Starty + (y + 1) * Len, BLACK);

    lcd_fill(Startx + x * Len, Starty + y * Len, Startx + x * Len + 2, Starty + (y + 1) * Len, BLACK);
    lcd_fill(Startx + (x + 1) * Len - 2, Starty + y * Len, Startx + (x + 1) * Len, Starty + (y + 1) * Len, BLACK);

    lcd_fill(Startx + x * Len + 6, Starty + y * Len + 6, Startx + (x + 1) * Len - 6, Starty + (y + 1) * Len - 6, BLACK);
}

Tetris Block[8];
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
    uint8_t t21[4][2] = {{0, 0}, {1, 0}, {0, 1}, {1, 1}};
    Block[1].cur = 0;
    Block[1].x = 5;
    Block[1].y = 1;
    memcpy(Block[1].type[0], t21, sizeof(t21));
    memcpy(Block[1].type[1], t21, sizeof(t21));
    memcpy(Block[1].type[2], t21, sizeof(t21));
    memcpy(Block[1].type[3], t21, sizeof(t21));

    uint8_t t31[4][2] = {{1, 0}, {0, 1}, {1, 1}, {2, 1}};
    uint8_t t32[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, 2}};
    uint8_t t33[4][2] = {{0, 1}, {1, 1}, {2, 1}, {1, 2}};
    uint8_t t34[4][2] = {{1, 0}, {1, 1}, {1, 2}, {2, 1}};
    Block[2].cur = 0;
    Block[2].x = 5;
    Block[2].y = 1;
    memcpy(Block[2].type[0], t31, sizeof(t31));
    memcpy(Block[2].type[1], t32, sizeof(t32));
    memcpy(Block[2].type[2], t33, sizeof(t33));
    memcpy(Block[2].type[3], t34, sizeof(t34));

    uint8_t t11[4][2] = {{1, 0}, {1, 1}, {1, 2}, {1, 3}};
    uint8_t t12[4][2] = {{0, 1}, {1, 1}, {2, 1}, {3, 1}};
    Block[3].cur = 0;
    Block[3].x = 4;
    Block[3].y = 1;
    memcpy(Block[3].type[0], t11, sizeof(t11));
    memcpy(Block[3].type[1], t12, sizeof(t12));
    memcpy(Block[3].type[2], t11, sizeof(t11));
    memcpy(Block[3].type[3], t12, sizeof(t12));

    uint8_t t41[4][2] = {{0, 0}, {0, 1}, {1, 1}, {2, 1}};
    uint8_t t42[4][2] = {{1, 0}, {1, 1}, {1, 2}, {0, 2}};
    uint8_t t43[4][2] = {{0, 0}, {1, 0}, {2, 0}, {2, 1}};
    uint8_t t44[4][2] = {{0, 0}, {1, 0}, {0, 1}, {0, 2}};
    Block[4].cur = 0;
    Block[4].x = 5;
    Block[4].y = 1;
    memcpy(Block[4].type[0], t41, sizeof(t41));
    memcpy(Block[4].type[1], t42, sizeof(t42));
    memcpy(Block[4].type[2], t43, sizeof(t43));
    memcpy(Block[4].type[3], t44, sizeof(t44));

    uint8_t t51[4][2] = {{2, 0}, {0, 1}, {1, 1}, {2, 1}};
    uint8_t t52[4][2] = {{0, 0}, {1, 0}, {1, 1}, {1, 2}};
    uint8_t t53[4][2] = {{0, 0}, {1, 0}, {2, 0}, {0, 1}};
    uint8_t t54[4][2] = {{0, 0}, {1, 2}, {0, 1}, {0, 2}};
    Block[5].cur = 0;
    Block[5].x = 5;
    Block[5].y = 1;
    memcpy(Block[5].type[0], t51, sizeof(t51));
    memcpy(Block[5].type[1], t52, sizeof(t52));
    memcpy(Block[5].type[2], t53, sizeof(t53));
    memcpy(Block[5].type[3], t54, sizeof(t54));

    uint8_t t61[4][2] = {{0, 0}, {1, 0}, {1, 1}, {2, 1}};
    uint8_t t62[4][2] = {{1, 0}, {0, 1}, {1, 1}, {0, 2}};
    Block[6].cur = 0;
    Block[6].x = 5;
    Block[6].y = 1;
    memcpy(Block[6].type[0], t61, sizeof(t61));
    memcpy(Block[6].type[1], t62, sizeof(t62));
    memcpy(Block[6].type[2], t61, sizeof(t61));
    memcpy(Block[6].type[3], t62, sizeof(t62));

    uint8_t t71[4][2] = {{1, 0}, {2, 0}, {0, 1}, {1, 1}};
    uint8_t t72[4][2] = {{0, 0}, {0, 1}, {1, 1}, {1, 2}};
    Block[7].cur = 0;
    Block[7].x = 5;
    Block[7].y = 1;
    memcpy(Block[7].type[0], t71, sizeof(t71));
    memcpy(Block[7].type[1], t72, sizeof(t72));
    memcpy(Block[7].type[2], t71, sizeof(t71));
    memcpy(Block[7].type[3], t72, sizeof(t72));


}

uint8_t Check(Tetris New){
    uint8_t i;
    for(i = 0;i < 4;i++){
        uint8_t xx = New.x + New.type[New.cur][i][0], yy = New.y + New.type[New.cur][i][1];
        if(xx <= 0 || xx > N || yy > M || yy <= 0 || map[xx][yy] != 0) return 0;
    }
    return 1;
}


void Tetris_Refresh(void){
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

void New_Block(void){
    uint8_t next = 1 + rand() % 7;
    cur = Block[next];
    if(Check(cur) == 0){
        over = 1;
        Update(cur);
        Tetris_Refresh();
        cur = Block[0];
    }
}

void Tetris_Up(void){
    Tetris New = cur;
    New.cur ++;
    if(New.cur >= 4) New.cur = 0;
    ReMove(cur);
    if(Check(New) == 1){
        cur.cur ++;
        if(cur.cur >= 4) cur.cur = 0;
        Tetris_Refresh();
    }

    if(over == 1){
        over = 0;
        Start = 0;
        uint8_t i, j;
        for(i = 1;i <= N;i++){
            for(j = 1;j <= M;j++){
                map[i][j] = 0;
            }
        }
    }
}

void Tetris_Down(void){
    if(over == 1){
        Tetris_Refresh();
        return ;
    }
    if(Start == 0){
        Tetris_Init();
        Start = 1;
        New_Block();
        Tetris_Refresh();
        return ;
    }
    Tetris New = cur;
    New.y ++;
    ReMove(cur);
    if(Check(New) == 1){
        cur.y ++;
    }
    else {
        uint8_t i, j, k;
        for(i = 0;i < 4;i++){
            uint8_t xx = cur.x + cur.type[cur.cur][i][0], yy = cur.y + cur.type[cur.cur][i][1];
            map[xx][yy] = 1;
        }
        for(j = M;j > 0;j --){
            uint8_t ok = 1;
            for(i = 1;i <= N;i++){
                if(map[i][j] == 0){
                    ok = 0;
                    break;
                }
            }
            if(ok == 1){
                for(k = j;k > 0;k --){
                    for(i = 1;i <= N;i++){
                        map[i][k] = map[i][k - 1];
                    }
                }
                j ++;
            }
        }
        New_Block();
    }
    Tetris_Refresh();
}

void Tetris_Right(void){
    Tetris New = cur;
    New.x ++;
    ReMove(cur);
    if(Check(New) == 1){
        cur.x ++;
        Tetris_Refresh();
    }
}

void Tetris_Left(void){
    Tetris New = cur;
    New.x --;
    ReMove(cur); 
    if(Check(New) == 1){
        cur.x --;
        Tetris_Refresh();
    }
}
