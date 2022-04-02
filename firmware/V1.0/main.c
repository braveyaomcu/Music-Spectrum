#include "display.h"
#include "system.h"
#include "misc.h"
#include "fft.h"


void main()
{    
    display_scan();
	display_reset();
	
    InitADC();
    display_init();
    SCH_Task_Init();
        
    SCH_Task_Add(led_run,0,500);
    SCH_Task_Add(key_run,0,5);
    SCH_Task_Add(func2,0,1000);
    SCH_Task_Start();
    
    while(1)
    {
        SCH_Task_Dispatch();
        fftFSM();
        //display_bar();
        //Delay100us();
    }
    
}




