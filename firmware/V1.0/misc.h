#ifndef _MISC_H
#define _MISC_H

#include "system.h"

#define LED     P13
#define KEY     P12

#define LED_ON  LED=0
#define LED_OFF LED=1

#define BUTTON_MAX          1

#define TICKS_INTERVAL      5	//ms
#define DEBOUNCE_TICKS      3	//MAX 8
#define SHORT_TICKS         (300 /TICKS_INTERVAL)
#define LONG_TICKS          (1000 /TICKS_INTERVAL)

typedef void (*BtnCallback)(void*);

typedef enum {
	PRESS_DOWN = 0,
	PRESS_UP,
	PRESS_REPEAT,
	SINGLE_CLICK,
	DOUBLE_CLICK,
	LONG_PRESS_START,
	LONG_PRESS_HOLD,
	number_of_event,
	NONE_PRESS
}PressEvent;


typedef struct _button_st{
    
    Word    ticks;
    Byte    repeat:4;
    Byte    event:4;
    Byte    state:3;
    Byte    debounce_cnt:3;
    Byte    active_level:1;
    Byte    button_level:1;
    
    Byte    (*hal_button_Level)(void);
    BtnCallback  cb[number_of_event];
}Button_t;

extern xdata Byte systemMode;
extern xdata Byte codeIndex;

void led_run(void);
void key_run(void);

#if 0
void button_init(void);
void button_add(Byte (*pin_level)(), 
                Byte active_level,
                PressEvent event, 
                BtnCallback cb);
void button_ticks(void);
void app_button_init(void);
#endif

#endif
