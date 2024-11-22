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
		if(Key0Cnt > 10 && Key0Cnt < 100){
			
		}
		Key0Cnt = 0;
	}
}

void Check_Key1(void){
	
}
void Check_Key2(void){
	
}

//ע��KEY3 ������ Ĭ�ϵ͵�ƽ ���¸ߵ�ƽ
void Check_Key3(void){
	
}

//LED����
void Led0(void){
		;
}

void Led1(void){
		;
}

//������������Ƶ����ں�ռ�ձ�
void SendMess(void){
	;
}

// 
void Refresh(){
	lcd_clear(RED);
	LED0_TOGGLE();
}

//��ʼ�� �������
void Task_Init(void){
	Task_Add(0, 1, 5, Check_Key0);		//������50msɨ��һ��
	Task_Add(0, 1, 5, Check_Key1);
	Task_Add(0, 1, 5, Check_Key2);
	Task_Add(0, 1, 5, Check_Key3);
	
	Task_Add(1, 50, 50, Refresh);
	
}
	






