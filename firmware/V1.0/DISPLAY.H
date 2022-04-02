#ifndef _DISPLAY_H
#define _DISPLAY_H


#include "system.h"

#define ROW1    P17
#define ROW2    P32
#define ROW3    P34
#define ROW4    P36
#define ROW5    P00
#define ROW6    P02
#define ROW7    P04
#define ROW8    P16

#define COL1    P33
#define COL2    P35
#define COL3    P37
#define COL4    P01
#define COL5    P03
#define COL6    P05
#define COL7    P06
#define COL8    P07
#define COL9    P27
#define COL10   P26
#define COL11   P25
#define COL12   P24
#define COL13   P23
#define COL14   P22
#define COL15   P21
#define COL16   P20

 
#ifdef MODE1T
#define T100US (65536-FOSC/5000)      //1ms timer calculation method in 1T mode
#else
#define T100US (65536-FOSC/12/5000)   //1ms timer calculation method in 12T mode
#endif


typedef struct _display_st{
    union   {
        Byte    rowAll;
        struct  bRow_st{
            Byte    Row1:1;
            Byte    Row2:1;
            Byte    Row3:1;
            Byte    Row4:1;
            Byte    Row5:1;
            Byte    Row6:1;
            Byte    Row7:1;
            Byte    Row8:1;
        }bRow;
    }displayRow;
    
    union   {
        Word    colAll;
        struct  bCol_st{
            Word    Col1:1;
            Word    Col2:1;
            Word    Col3:1;
            Word    Col4:1;
            Word    Col5:1;
            Word    Col6:1;
            Word    Col7:1;
            Word    Col8:1;
            Word    Col9:1;
            Word    Col10:1;
            Word    Col11:1;
            Word    Col12:1;
            Word    Col13:1;
            Word    Col14:1;
            Word    Col15:1;
            Word    Col16:1;
        }bCol;
    }displayCol;
    
}display_t;

extern Byte displayBar[16];
extern bit flag_200us;

void display_init(void);
void display_reset(void);
void display_bar(void);
void display_scan(void);
void func2(void);

#endif
