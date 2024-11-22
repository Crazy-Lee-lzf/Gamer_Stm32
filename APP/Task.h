#ifndef __TASK_H
#define __TASK_H

#include "./SYSTEM/sys/sys.h"

#define MaxTaskNum 20

typedef struct {
	uint8_t run;
	uint16_t t;
	uint16_t Time;
	void (*Task) (void);
}TASK_COM_INFO;



void Task_Init(void);
void Check_Key0(void);
void Check_Key1(void);
void Check_Key2(void);
void Check_Key3(void);

#endif



