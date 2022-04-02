#include "display.h"
#include "misc.h"
#include <intrins.h>

static xdata display_t    display;
bit flag_200us=0;

Byte displayBar[16]={0x00,0x08,0x10,0x3F,0x40,0x02,0x14,0x28,0x41,0x21,0x3E,0x20,0x28,0x24,0x26,0x00};
code Byte displayCode[][16]={
    {0x00,0x08,0x10,0x3F,0x40,0x02,0x14,0x28,0x41,0x21,0x3E,0x20,0x28,0x24,0x26,0x00},/*"ƒ„",0*/
    {0x00,0x21,0x2D,0x76,0x24,0x3A,0x2A,0x08,0x48,0x49,0x5F,0x68,0x48,0x48,0x00,0x00},/*"∫√",0*/
    {0x00,0x04,0x04,0x08,0x10,0x00,0x01,0x01,0x7E,0x00,0x00,0x10,0x10,0x08,0x00,0x00},/*"–°",0*/    
    {0x00,0x00,0x53,0x55,0xFD,0x2D,0x2A,0x5A,0x5A,0x2A,0x7C,0x24,0x50,0x50,0x10,0x00},/*"√Œ",0*/
    
    {0x00,0x10,0x10,0x50,0x5F,0x3E,0x3E,0xDE,0x5E,0x5E,0x3E,0x3F,0x58,0x10,0x10,0x00},/*"“Ù",0*/
    {0x00,0x00,0x02,0x32,0x54,0x54,0x51,0x51,0x7F,0x50,0x50,0x54,0x14,0x12,0x00,0x00},/*"¿÷",1*/
    {0x00,0x15,0x79,0x5A,0xFE,0x32,0x54,0x59,0x7D,0x62,0x62,0x7C,0x62,0x62,0x3D,0x00},/*"∆µ",2*/
    {0x00,0x00,0x10,0x5E,0x02,0x04,0x30,0xCF,0x75,0x4A,0x75,0xCA,0xD7,0x68,0x00,0x00},/*"∆◊",3*/
    {0x00,0x00,0x0A,0x06,0x72,0x52,0x5E,0x52,0x52,0x5E,0x52,0x76,0x06,0x0A,0x00,0x00},/*"œ‘",4*/
    {0x00,0x02,0x12,0x14,0x58,0x58,0x51,0x51,0x5E,0x50,0x58,0x58,0x54,0x12,0x00,0x00},/*" æ",5*/
};

static void _display_update(void)
{
    ROW8=display.displayRow.bRow.Row1;
    ROW7=display.displayRow.bRow.Row2;
    ROW6=display.displayRow.bRow.Row3;
    ROW5=display.displayRow.bRow.Row4;
    ROW4=display.displayRow.bRow.Row5;
    ROW3=display.displayRow.bRow.Row6;
    ROW2=display.displayRow.bRow.Row7;
    ROW1=display.displayRow.bRow.Row8;
    
    COL1=display.displayCol.bCol.Col1;
    COL2=display.displayCol.bCol.Col2;
    COL3=display.displayCol.bCol.Col3;
    COL4=display.displayCol.bCol.Col4;
    COL5=display.displayCol.bCol.Col5;
    COL6=display.displayCol.bCol.Col6;
    COL7=display.displayCol.bCol.Col7;
    COL8=display.displayCol.bCol.Col8;    
    COL9=display.displayCol.bCol.Col9;
    COL10=display.displayCol.bCol.Col10;
    COL11=display.displayCol.bCol.Col11;
    COL12=display.displayCol.bCol.Col12;
    COL13=display.displayCol.bCol.Col13;
    COL14=display.displayCol.bCol.Col14;
    COL15=display.displayCol.bCol.Col15;
    COL16=display.displayCol.bCol.Col16;
}

void Delay10ms()		//@24.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 1;
	j = 234;
	k = 113;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void display_scan(void)
{
    Byte    i=0,j=0;
    Byte    temp=0x00;        
    
    for(i=0;i<8;i++)
    {
        display.displayRow.rowAll=0x01<<i;
        for(j=0;j<16;j++)
        {
            display.displayCol.colAll=0x01<<j;
            _display_update();
            Delay10ms();
        }
    }
}

void display_reset(void)
{
	display.displayCol.colAll=0;
	display.displayRow.rowAll=0;
	_display_update();
}

void timer1Init(void)
{
#ifdef MODE1T
    AUXR |= 0x40;                    //timer1 work in 1T mode
#endif
    TMOD |= 0x10;                    //set timer1 as mode1 (16-bit)
    TL1 = T100US;                     //initial timer1 low byte
    TH1 = T100US >> 8;                //initial timer1 high byte
    TR1 = 1;                        //timer1 start running
    ET1 = 1;                        //enable timer1 interrupt
    //EA = 1;                         //open global interrupt switch
}

static Byte rowIndex;
static Byte colIndex;

void display_bar(void)
{
    //Byte    i=0,j=0;

    display_reset();
    
    if(displayBar[colIndex]>8)
        displayBar[colIndex]=8;
    
    if(displayBar[colIndex]>0)
    {
        display.displayCol.colAll=0x01<<colIndex;
        display.displayRow.rowAll=0xFF>>(8-displayBar[colIndex]);
        
    }
    else
        display.displayCol.colAll=0;
    
    _display_update();
    
    colIndex++;
    if(colIndex>=16)
        colIndex=0;
    
}

void display_code(Byte x)
{
    display_reset();
    display.displayCol.colAll=0x01<<colIndex;
    display.displayRow.rowAll=displayCode[x][colIndex];
    
    _display_update();
    colIndex++;
    if(colIndex>=16)
        colIndex=0;
}

void func2(void)
{
    if(systemMode==0)
        return;
    codeIndex++;
    if(codeIndex>=10)
        codeIndex=0;
    
}

void display_init(void)
{
    timer1Init();
}

void tm1_isr() interrupt 3
{
    TL1 = T100US;                     //reload timer1 low byte
    TH1 = T100US >> 8;                //reload timer1 high byte
    //display_code(0);
    if(systemMode==0)
        display_bar();
    else
        display_code(codeIndex);
    flag_200us=1;
}