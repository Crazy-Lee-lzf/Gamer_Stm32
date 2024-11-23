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

static const uint8_t N = 10, M = 20;
static const uint16_t Startx = 50, Starty = 130;
static const uint16_t Back = 0xAD90;
static const uint8_t Len = 30;

uint8_t Start = 0;

void Darw_Block(uint8_t x, uint8_t y){
    lcd_fill(Startx + x * Len, Starty + y * Len, Startx + (x + 1) * Len, Starty + y * Len + 2, BLACK);
    lcd_fill(Startx + x * Len, Starty + (y + 1) * Len - 2, Startx + (x + 1) * Len, Starty + (y + 1) * Len, BLACK);

    lcd_fill(Startx + x * Len, Starty + y * Len, Startx + x * Len + 2, Starty + (y + 1) * Len, BLACK);
    lcd_fill(Startx + (x + 1) * Len - 2, Starty + y * Len, Startx + (x + 1) * Len, Starty + (y + 1) * Len, BLACK);

    lcd_fill(Startx + x * Len + 6, Starty + y * Len + 6, Startx + (x + 1) * Len - 6, Starty + (y + 1) * Len - 6, BLACK);
}

void Tetris_Init(void){
    lcd_clear(Back);
}

void Tetris_Refresh(void){
    if(Start == 0){
        Tetris_Init();
        Start = 1;
    }
    uint8_t i, j;
    for(i = 1;i <= N;i++){
        for(j = 1;j <= M;j++){
            Darw_Block(i, j);
        }
    }
    
}

