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

const uint8_t N = 21, M = 31;
const uint16_t Startx = 20, Starty = 100;
uint8_t Headx = 3, Heady = 3;

uint16_t map[23][33];
uint16_t SnakeLenth;

//вСиосроб
int8_t dx[5] = {0, 1, 0, -1, 0};
int8_t dy[5] = {0, 0, -1, 0, 1};

uint8_t SnakeDir;
uint8_t Foodx, Foody;

uint8_t Start = 0;

uint16_t transx(uint8_t x){
	return Startx + 20 * x - 1;
}

uint16_t transy(uint8_t y){
	return Starty + 20 * y - 1;
}


void Snake_Init(void){
	Headx = 6, Heady = 3;
	map[Headx][Heady] = 1;
	map[Headx - 1][Heady] = 2;
	map[Headx - 2][Heady] = 3;
	Start = 1;
	lcd_clear(BLACK);

	SnakeDir = 1;
	Foodx = Foody = 5;
	SnakeLenth = 3;



}

uint8_t CheckDie(uint8_t x, uint8_t y){
	if(x <= 0 || x >= N || y <= 0 || y >= M || map[x][y] != 0) return 0;
	return 1;
}


void Snake_Run(void){
	uint8_t Nextx = Headx + dx[SnakeDir], Nexty = Heady + dy[SnakeDir];
	if(CheckDie(Nextx, Nexty) == 0){
		SnakeDir = 0;
		return ;
	}
	
	uint8_t i, j, tx, ty;
	for(i = 1;i < N;i++){
		for(j = 1;j < M;j++){
			if(map[i][j] != 0){
				if(map[i][j] == SnakeLenth){
					tx = i;
					ty = j;
				}
				map[i][j] ++;	
			}
			
		}
	}
	Headx = Nextx;
	Heady = Nexty;
	map[Headx][Heady] = 1;
	if(Nextx == Foodx && Nexty == Foody){
		map[tx][ty] = ++SnakeLenth;
		Foodx = 1 + rand() % 20;
		Foody = 1 + rand() % 30;
	}
	else{
		map[tx][ty] = 0;
	}

}

void Refresh_Snake(void){
	if(Start == 0){
		Snake_Init();
		Start = 1;
	}
	Snake_Run();
	uint8_t i, j;
	for(i = 1;i < N;i ++){
		for(j = 1;j < M;j ++){
			if(i == Headx && j == Heady){
				lcd_draw_circle(transx(i) + 10, transy(j) + 10, 10, RED);
			}
			else if(i == Foodx && j == Foody){
				lcd_fill(transx(i), transy(j), transx(i + 1) - 1, transy(j + 1) - 1, YELLOW);
			}
			else if(map[i][j] == 0){
				lcd_fill(transx(i), transy(j), transx(i + 1) - 1, transy(j + 1) - 1, BLACK);
			}
			else{
				lcd_fill(transx(i), transy(j), transx(i + 1) - 1, transy(j + 1) - 1, RED);
			}
		}
	}

	for(i = 0;i <= N;i++){
		lcd_fill(transx(i), transy(0), transx(i + 1), transy(1), BLUE);
		lcd_fill(transx(i), transy(M), transx(i + 1), transy(M + 1), BLUE);
	}
	for(i = 0;i <= M;i++){
		lcd_fill(transx(0), transy(i), transx(1) - 1, transy(i + 1) , BLUE);
		lcd_fill(transx(N), transy(i), transx(N + 1), transy(i + 1), BLUE);
	}
	
}








