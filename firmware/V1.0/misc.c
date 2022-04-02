#include "misc.h"
#include "system.h"
#include "display.h"
#include <stdio.h>
#include <string.h>

xdata Byte systemMode=0;
xdata Byte codeIndex=0;

static xdata Byte led_status=0;

//#define EVENT_CB(ev)   if(handle->cb[ev])handle->cb[ev]((Button_t *)handle)

//static xdata Button_t   button_handle[BUTTON_MAX];

void led_run(void)
{
    
    switch(led_status)
    {
        case 0:
            LED_ON;
            led_status=1;
            break;
        case 1:
            LED_OFF;
            led_status=0;
            break;
        default:
            LED_ON;
            led_status=1;
            break;
    }
    
    
}

static Byte key_last=1;
static Byte debounce_cnt=0;
static Byte key_tick=0;
static Byte key_state=0;

void key_run(void)
{
    if(key_state>0)
        key_tick++;
    
    if(key_last!=KEY){
        if((debounce_cnt++) >= DEBOUNCE_TICKS)
        {
            key_last=KEY;
            debounce_cnt=0;
        }
    }
    else
        debounce_cnt=0;
        
    switch(key_state)
    {
        case 0:
            if(key_last == 0)
            {
                //TO DO: KEY_DOWN
                printf("key down\r\n");
                key_tick=0;
                key_state=1;
            }
            break;
        case 1:
            if(key_last != 0)
            {
                //TO DO: KEY_RELEASE
                printf("key release\r\n");
                codeIndex++;
                key_tick=0;
                key_state=0;
            }
            else if(key_tick>LONG_TICKS)
            {
                key_state=3;
            }
            break;
        case 3:
            if(key_last != 0)
            {
                //TO DO: KEY_DOWN
                printf("key long press\r\n");
                if(systemMode==0)
                    systemMode=1;
                else
                    systemMode=0;
                codeIndex=0;
                key_tick=0;
                key_state=0;
            }
            break;    
        
    }
}



#if 0
void button_init(void)
{
    Byte i=0;
    for(i=0;i<BUTTON_MAX;i++)
    {
        memset(&button_handle[i], 0, sizeof(Button_t));
        
    }
}

void button_add(Byte (*pin_level)(), 
                Byte active_level,
                PressEvent event, 
                BtnCallback cb)
{
    Byte i=0;
    for(i=0;i<BUTTON_MAX;i++)
    {
        
        if(button_handle[i].hal_button_Level==0)
        {
            button_handle[i].event = (Byte)NONE_PRESS;
            button_handle[i].hal_button_Level = pin_level;
            button_handle[i].button_level = button_handle[i].hal_button_Level();
            button_handle[i].active_level = active_level;
            button_handle[i].cb[event] = cb;
            break;
        }
    }
    
    if(i>=BUTTON_MAX)
    {
        
    }
}

void button_handler(Button_t    *handle)
{
    
	Byte     read_gpio_level = handle->hal_button_Level();

	//ticks counter working..
	if((handle->state) > 0) handle->ticks++;

	/*------------button debounce handle---------------*/
	if(read_gpio_level != handle->button_level) { //not equal to prev one
		//continue read 3 times same new level change
		if(++(handle->debounce_cnt) >= DEBOUNCE_TICKS) {
			handle->button_level = read_gpio_level;
			handle->debounce_cnt = 0;
		}
	} else { //leved not change ,counter reset.
		handle->debounce_cnt = 0;
	}

	/*-----------------State machine-------------------*/
	switch (handle->state) {
	case 0:
		if(handle->button_level == handle->active_level) {	//start press down
			handle->event = (Byte)PRESS_DOWN;
			EVENT_CB(PRESS_DOWN);
			handle->ticks = 0;
			handle->repeat = 1;
			handle->state = 1;
		} else {
			handle->event = (Byte)NONE_PRESS;
		}
		break;

	case 1:
		if(handle->button_level != handle->active_level) { //released press up
			handle->event = (Byte)PRESS_UP;
			EVENT_CB(PRESS_UP);
			handle->ticks = 0;
			handle->state = 2;

		} else if(handle->ticks > LONG_TICKS) {
			handle->event = (Byte)LONG_PRESS_START;
			EVENT_CB(LONG_PRESS_START);
			handle->state = 5;
		}
		break;

	case 2:
		if(handle->button_level == handle->active_level) { //press down again
			handle->event = (Byte)PRESS_DOWN;
			EVENT_CB(PRESS_DOWN);
			handle->repeat++;
			EVENT_CB(PRESS_REPEAT); // repeat hit
			handle->ticks = 0;
			handle->state = 3;
		} else if(handle->ticks > SHORT_TICKS) { //released timeout
			if(handle->repeat == 1) {
				handle->event = (Byte)SINGLE_CLICK;
				EVENT_CB(SINGLE_CLICK);
			} else if(handle->repeat == 2) {
				handle->event = (Byte)DOUBLE_CLICK;
				EVENT_CB(DOUBLE_CLICK); // repeat hit
			}
			handle->state = 0;
		}
		break;

	case 3:
		if(handle->button_level != handle->active_level) { //released press up
			handle->event = (Byte)PRESS_UP;
			EVENT_CB(PRESS_UP);
			if(handle->ticks < SHORT_TICKS) {
				handle->ticks = 0;
				handle->state = 2; //repeat press
			} else {
				handle->state = 0;
			}
		}
		break;

	case 5:
		if(handle->button_level == handle->active_level) {
			//continue hold trigger
			handle->event = (Byte)LONG_PRESS_HOLD;
			EVENT_CB(LONG_PRESS_HOLD);

		} else { //releasd
			handle->event = (Byte)PRESS_UP;
			EVENT_CB(PRESS_UP);
			handle->state = 0; //reset
		}
		break;
	}
}

void button_ticks(void)
{
	Byte i=0;
	for(i=0; i<BUTTON_MAX; i++) {
		button_handler(&button_handle[i]);
	}
}

Byte key_read(void)
{
    return KEY;
}

void key_callback(void *p)
{
    ROW1=1;
    COL3=1;
}

void app_button_init(void)
{
    button_init();
    button_add(key_read,0,PRESS_UP,key_callback);
    SCH_Task_Add(button_ticks,0,5);
}

#endif