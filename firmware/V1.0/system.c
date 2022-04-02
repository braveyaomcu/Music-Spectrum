#include "system.h"

Task_t  xdata SCH_Task_G[SCH_TASK_MAX];

#define SERIAL_ENABLE

#ifdef SERIAL_ENABLE

char putchar (char c) {
    
    SBUF=c;
    while(!TI);
    TI=0;
    return c;
}

void SerialInit(void)
{
    SCON = 0x50;        
                        
    AUXR |= 0x05;       //AUXR^0 串口波特率发生器选择位
                        //AUXR^2 串口波特率发生器BRT 1T模式
                        
    BRT=256-(FOSC/1/32/BAUD);
    AUXR |= 0x10;       //AUXR^4 串口独立波特率发生器运行位
    
    //用于调试的时候只发送数据，可以不开中断
    //ES = 1;                 //Enable UART interrupt
    //EA = 1;                 //Open master interrupt switch
}

void Uart_Isr() interrupt 4
{
    if (RI)
    {
        RI = 0;             //Clear receive interrupt flag
        //TO DO
    }
    if (TI)
    {
        TI = 0;             //Clear transmit interrupt flag
        //busy = 0;           //Clear transmit busy flag
    }
}

#endif


void timer0_Init(void)
{
#ifdef MODE1T
    AUXR |= 0x80;                    //timer0 work in 1T mode
#endif
    TMOD |= 0x01;                    //set timer0 as mode1 (16-bit)
    TL0 = T10MS;                     //initial timer0 low byte
    TH0 = T10MS >> 8;                //initial timer0 high byte
    TR0 = 1;                        //timer0 start running
    ET0 = 1;                        //enable timer0 interrupt
    //EA = 1;                         //open global interrupt switch
    
}

Byte SCH_Task_Delete(Byte Task_Cur)
{
    Byte xdata ErrorCode;
    ErrorCode=ERROR_SCH_OK;
    
    if(SCH_Task_G[Task_Cur].pfun == 0)
    {
        ErrorCode=ERROR_SCH_CANNOT_DELETE_TASK;
    }
   
    SCH_Task_G[Task_Cur].pfun=0x0000;
    SCH_Task_G[Task_Cur].Delay=0;
    SCH_Task_G[Task_Cur].Period=0;
    SCH_Task_G[Task_Cur].Runme=0;
    
    return ErrorCode;
}

void SCH_Task_Init(void)
{
    Byte xdata i;

    for (i = 0; i < SCH_TASK_MAX; i++) 
    {
        SCH_Task_Delete(i);
    }

#ifdef SERIAL_ENABLE    
    SerialInit();
#endif    
    timer0_Init();
}

void SCH_Task_Start(void)
{
    EA=1;
}

Byte SCH_Task_Add(void (code *pfun)(void),
                        Word Delay,
                        Word Period)
{
    Byte xdata Index,ErrorCode;
    ErrorCode=ERROR_SCH_OK;
    
    for(Index=0;Index<SCH_TASK_MAX;Index++)
    {
        if(SCH_Task_G[Index].pfun == 0)
        {
            SCH_Task_G[Index].pfun=pfun;
            SCH_Task_G[Index].Delay=Delay;
            SCH_Task_G[Index].Period=Period;
            SCH_Task_G[Index].Runme=0;
            break;
        }
    }
    
    if(Index == SCH_TASK_MAX)
    {
        ErrorCode=ERROR_SCH_TOO_MANY_TASKS;
    }
    
    return ErrorCode;
}

void SCH_Task_Dispatch(void)
{
    Byte xdata Index,ErrorCode;
    ErrorCode=ERROR_SCH_OK;
    
    for(Index=0;Index<SCH_TASK_MAX;Index++)
    {
        if(SCH_Task_G[Index].Runme >0)
        {
            SCH_Task_G[Index].pfun();
            SCH_Task_G[Index].Runme-=1; 
            if(SCH_Task_G[Index].Period == 0)
            {
                SCH_Task_Delete(Index);
            }
            
        }
        
    }
    //Todo others
       
}

void tm0_isr() interrupt 1
{
    Byte xdata Index;
    
    TR0=0;
    
    TL0 = T10MS;                     //reload timer0 low byte
    TH0 = T10MS >> 8;                //reload timer0 high byte
    
    TR0=1;
    
    for(Index=0;Index<SCH_TASK_MAX;Index++)
    {
        if(SCH_Task_G[Index].pfun)
        {
            
            if(SCH_Task_G[Index].Delay == 0)
            {
                SCH_Task_G[Index].Runme+=1;
                if(SCH_Task_G[Index].Period)
                {
                    SCH_Task_G[Index].Delay=SCH_Task_G[Index].Period;
                }
            }
            else
            {
                SCH_Task_G[Index].Delay-=1;
            }
            
        }
        
    }
    
}

