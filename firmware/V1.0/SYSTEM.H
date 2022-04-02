#ifndef _SYSTEM_H
#define _SYSTEM_H

#include <STC12C5A60S2.h>

#define FOSC 24000000L
#define BAUD 19200

#define SCH_TASK_MAX    3
#define SCH_TICK_MS     1

#define MODE1T                      //Timer clock mode, comment this line is 12T mode, uncomment is 1T mode

#ifdef MODE1T
#define T10MS (65536-FOSC/1000*SCH_TICK_MS)      //1ms timer calculation method in 1T mode
#else
#define T10MS (65536-FOSC/12/SCH_TICK_MS/1000)   //1ms timer calculation method in 12T mode
#endif




#define ERROR_SCH_OK                    (0)
#define ERROR_SCH_TOO_MANY_TASKS        (1)
#define ERROR_SCH_CANNOT_DELETE_TASK    (2)

#define BinToHex(n) (((n>>21)&0x80)|((n>>18)&0x40)|((n>>15)&0x20)|((n>>12)&0x10)|((n>>9)& 0x08)|((n>>6)&0x04)|((n>>3)&0x02)|((n)&0x01))
#define B(n) BinToHex(0x##n##l)

typedef unsigned char   Byte;
typedef unsigned int    Word; 

typedef struct{
    void (*pfun)(void);
    
    Word    Delay;    
    Word    Period;
    
    Byte    Runme;
}Task_t;

extern Task_t  xdata SCH_Task_G[SCH_TASK_MAX];

void SCH_Task_Init(void);
Byte SCH_Task_Add(void (code *pfun)(void),
                        Word Delay,
                        Word Period);
void SCH_Task_Start(void);
void SCH_Task_Dispatch(void);

#endif
