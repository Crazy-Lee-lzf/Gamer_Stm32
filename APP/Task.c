#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/MPU/mpu.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/SDRAM/sdram.h"
#include "Task.h"
#include "Snake.h"
#include "Tetris.h"

TASK_COM_INFO myTask[MaxTaskNum];
uint8_t TaskNum = 0;

uint16_t Key0Cnt = 0;
uint16_t Key1Cnt = 0;
uint16_t Key2Cnt = 0;
uint16_t Key3Cnt = 0;

uint8_t GameNum = 2;
extern uint8_t SnakeDir;

//�������	�Ƿ����У���ʼ����ʱ����0��		��һ��ʲôʱ��ʼ����			���еļ��				���еĺ���
void Task_Add(uint8_t run, uint16_t t, uint16_t Time, void(*Task) (void)){
	if(TaskNum >= MaxTaskNum) return;
	myTask[TaskNum].run = run;
	myTask[TaskNum].t = t;
	myTask[TaskNum].Time = Time;
	myTask[TaskNum].Task = Task;
	TaskNum ++;
}

//����ĸ�����
/*********************************
�����ϰ���λ��
		KEY3
KEY2	KEY1	KEY0
**********************************/
void Check_Key0(void){
	//����������ڰ���״̬
	if(KEY0 == 0){
		Key0Cnt ++;
	}
	else {
		//����̧��
		if(Key0Cnt > 0 && Key0Cnt < 20){
			if(GameNum == 1)	Sanke_Right();
			else if(GameNum == 2) Tetris_Right();
		}
		Key0Cnt = 0;
	}
}

void Check_Key1(void){
	//����������ڰ���״̬
	if(KEY1 == 0){
		Key1Cnt ++;
	}
	else {
		//����̧��
		if(Key1Cnt > 0 && Key1Cnt < 20){
			if(GameNum == 1) Sanke_Down();
			else if(GameNum == 2) Tetris_Down();
		}
		Key1Cnt = 0;
	}
}
void Check_Key2(void){
	//����������ڰ���״̬
	if(KEY2 == 0){
		Key2Cnt ++;
	}
	else {
		//����̧��
		if(Key2Cnt > 0 && Key2Cnt < 20){
			if(GameNum == 1) Sanke_Left();
			else if(GameNum == 2) Tetris_Left();
		}
		Key2Cnt = 0;
	}
}

//ע��KEY3 ������ Ĭ�ϵ͵�ƽ ���¸ߵ�ƽ
void Check_Key3(void){
	//����������ڰ���״̬
	if(WK_UP == 1){
		Key3Cnt ++;
	}
	else {
		//����̧��
		if(Key3Cnt > 0 && Key3Cnt < 20){
			if(GameNum == 1) Sanke_Up();
			else if(GameNum == 2) Tetris_Up();
		}
		Key3Cnt = 0;
	}
}

//LEE0 �����˸ ����ָʾ��
void Led0(void){
	LED0_TOGGLE();
}

void Led1(void){
		;
}

//��ʼ�� �������
void Task_Init(void){
	//Task_Add(1, 150, 40, Refresh_Snake);
	Task_Add(1, 200, 40, Tetris_Refresh);
	
	Task_Add(0, 5, 5, Check_Key0);		//������50msɨ��һ��
	Task_Add(0, 5, 5, Check_Key1);
	Task_Add(0, 5, 5, Check_Key2);
	Task_Add(0, 5, 5, Check_Key3);
	
	Task_Add(0, 25, 50, Led0);
	
}
	






