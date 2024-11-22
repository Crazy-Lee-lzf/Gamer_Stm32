#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/MPU/mpu.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/SDRAM/sdram.h"
#include "Task.h"

TASK_COM_INFO myTask[MaxTaskNum];
uint8_t TaskNum = 0;

uint16_t Key0Cnt = 0;
uint16_t Key1Cnt = 0;
uint16_t Key2Cnt = 0;
uint16_t Key3Cnt = 0;

//添加任务	是否运行（初始化的时候都是0）		第一次什么时候开始运行			运行的间隔				运行的函数
void Task_Add(uint8_t run, uint16_t t, uint16_t Time, void(*Task) (void)){
	if(TaskNum >= MaxTaskNum) return;
	myTask[TaskNum].run = run;
	myTask[TaskNum].t = t;
	myTask[TaskNum].Time = Time;
	myTask[TaskNum].Task = Task;
	TaskNum ++;
}

//检测四个按键
/*********************************
板子上按键位置
			KEY3
KEY2	KEY1	KEY0
**********************************/
void Check_Key0(void){
	//如果按键处于按下状态
	if(KEY0 == 0){
		Key0Cnt ++;
	}
	else {
		//按键抬起
		if(Key0Cnt > 10 && Key0Cnt < 100){
			
		}
		Key0Cnt = 0;
	}
}

void Check_Key1(void){
	
}
void Check_Key2(void){
	
}

//注意KEY3 是下拉 默认低电平 按下高电平
void Check_Key3(void){
	
}

//LED控制
void Led0(void){
		;
}

void Led1(void){
		;
}

//串口输出两个灯的周期和占空比
void SendMess(void){
	;
}

// 
void Refresh(){
	lcd_clear(RED);
	LED0_TOGGLE();
}

//初始化 添加任务
void Task_Init(void){
	Task_Add(0, 1, 5, Check_Key0);		//按键是50ms扫描一次
	Task_Add(0, 1, 5, Check_Key1);
	Task_Add(0, 1, 5, Check_Key2);
	Task_Add(0, 1, 5, Check_Key3);
	
	Task_Add(1, 50, 50, Refresh);
	
}
	






