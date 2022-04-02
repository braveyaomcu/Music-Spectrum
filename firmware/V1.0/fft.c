#include "system.h"
#include "display.h"
#include <intrins.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#define ADC_CH1 4
#define ADC_CH2 5

#define ADC_POWER   0x80            //ADC power control bit
#define ADC_FLAG    0x10            //ADC complete flag
#define ADC_START   0x08            //ADC start control bit
#define ADC_SPEEDLL 0x00            //420 clocks
#define ADC_SPEEDL  0x20            //280 clocks
#define ADC_SPEEDH  0x40            //140 clocks
#define ADC_SPEEDHH 0x60            //70 clocks

code float iw[64]=
{
	1.000,0,0.9952,-0.0980,0.9808,-0.1951,0.9569,-0.2903,0.9239,-0.3827,0.8819,-0.4714,0.8315,-0.5556,
	0.7730,-0.6344,0.7071,-0.7071,0.6344,-0.7730,0.5556,-0.8315,0.4714,-0.8819,0.3827,-0.9239,0.2903,-0.9569,
	0.1951,-0.9808,0.0980,-0.9952,0.0,-1.0000,-0.0980,-0.9952,-0.1951,-0.9808,-0.2903,0.9569,-0.3827,-0.9239,
	-0.4714,-0.8819,-0.5556,-0.8315,-0.6344,-0.7730,-0.7071,-0.7071,-0.7730,-0.6344,-0.8315,-0.5556,-0.8819,-0.4714,
	-0.9239,-0.3827,-0.9569,-0.2903,-0.9808,-0.1951,-0.9952,-0.0980
};

struct compx
{
	float real;
	float imag;
};
xdata struct compx dd[65]; //FFT数据   
xdata struct compx temp;  

//复数乘法
void ee(struct compx b1,Byte data b2)
{ 
	temp.real=b1.real*iw[2*b2]-b1.imag*iw[2*b2+1];
	temp.imag=b1.real*iw[2*b2+1]+b1.imag*iw[2*b2]; 
} 
//乘方函数  
Word mypow(Byte data nbottom,Byte data ntop)
{
    Word xdata result=1;
    Byte xdata t;    
    for(t=0;t<ntop;t++)result*=nbottom; 
    return result;
}
//快速傅立叶变换  
void fft(struct compx *xin,Byte data N)
{
	Byte xdata  fftnum,i,j,k,l,m,n,disbuff,dispos,dissec;
	xdata struct compx t;
	fftnum=N;                         //傅立叶变换点数
	for(m=1;(fftnum=fftnum/2)!=1;m++);//求得M的值 
	for(k=0;k<=N-1;k++)               //码位倒置
	{
		n=k;
		j=0; 
		for(i=m;i>0;i--)             //倒置
		{
			j=j+((n%2)<<(i-1));
			n=n/2;
		} 
		if(k<j){t=xin[j];xin[j]=xin[k];xin[k]=t;}//交换数据
	}  
	for(l=1;l<=m;l++)                //FFT运算
	{
		disbuff=mypow(2,l);          //求得碟间距离
		dispos=disbuff/2;            //求得碟形两点之间的距离
		for(j=0;j<dispos;j++)
			for(i=j;i<N;i=i+disbuff) //遍历M级所有的碟形
			{
				dissec=i+dispos;     //求得第二点的位置
				ee(xin[dissec],(Word)(j)*(Word)N/disbuff);//复数乘法
				t=temp;
				xin[dissec].real=xin[i].real-t.real;
				xin[dissec].imag=xin[i].imag-t.imag;
				xin[i].real=xin[i].real+t.real;
				xin[i].imag=xin[i].imag+t.imag;
			}
	}
} 

void Delay(Word n)
{
    Word x;

    while (n--)
    {
        x = 5000;
        while (x--);
    }
}

void InitADC(void)
{
    P1ASF = B(00110000);                   //Set all P1 as analog input port
    ADC_RES = 0;                    //Clear previous result
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL ;//|ADC_START | ADC_CH1;
    Delay(2);                      //ADC power-on delay and Start A/D conversion
    
    //EADC=1;
}

static xdata Word AdcValue1=0;
static xdata Word AdcValue2=0;
static xdata Byte AdcIndex=0;
static xdata Byte adc_state=0;
static xdata float abs_min=FLT_MAX;

void fftFSM(void)
{
    Byte i=0;
    switch(adc_state)
    {
        case 0:
            if(flag_200us)
            {
                flag_200us=0;
                ADC_CONTR= ADC_POWER | ADC_SPEEDLL|ADC_START | ADC_CH1;
                adc_state=1;
            }
            //printf("state 0");
            break;
        case 1:
            if(ADC_CONTR & ADC_FLAG)
            {
                ADC_CONTR &= ~ADC_FLAG;
                AdcValue1=ADC_RES;
                AdcValue1<<=2;
                AdcValue1|=ADC_RESL;
                //dd[AdcIndex].real += (float)AdcValue*1.0/1024*5.0;
                //dd[AdcIndex].real += AdcValue;
                adc_state=2;
                //printf("AdcValue1:%d \r\n",AdcValue1);
            }
            break;
            
        case 2:
            //if(flag_200us)
            {
                flag_200us=0;
                ADC_CONTR= ADC_POWER | ADC_SPEEDLL|ADC_START | ADC_CH2;
                adc_state=3;
            }
            //printf("state 2");
            break;
        case 3:
            if(ADC_CONTR & ADC_FLAG)
            {
                ADC_CONTR &= ~ADC_FLAG;
                AdcValue2=ADC_RES;
                AdcValue2<<=2;
                AdcValue2|=ADC_RESL;
                //printf("AdcValue2:%d \r\n",AdcValue2);
                //dd[AdcIndex].real += (float)AdcValue*1.0/1024*5.0;
                
                dd[AdcIndex].real=(AdcValue1+AdcValue2)<<3;
                //printf("Index:%bd   Value:%f\r\n",AdcIndex,dd[AdcIndex].real);
                AdcIndex++;
                if(AdcIndex>=65)
                {
                    AdcIndex=0;
                    //TO DO
                    
                    adc_state=10;
                    printf("state Over\r\n");
                }
                else
                    adc_state=0;
            }
            break;
        case 10:
            for(i=0;i<65;i++)
                dd[i].imag=0; 
            adc_state++;
            //printf("state 10");
            break;
        case 11:
            fft(dd,64);
            adc_state++;
            //printf("state 11");
            break;
        case 12:
            for(i=1;i<64/2+1;i++)
                dd[i].real=sqrt(dd[i].real*dd[i].real+dd[i].imag*dd[i].imag);//取均方根 
            adc_state++;
            abs_min=FLT_MAX;
            //printf("state 12");
            break;
        case 13:
            for(i=1;i<64/2+1;i++)
                if(abs_min>dd[i].real)
                {
                    abs_min=dd[i].real;
                    //dd[i].real=dd[i].real-
                }
            printf("min:%f\r\n",abs_min);
            printf("*******************************\r\n");
            adc_state++;
            break;
        case 14:
            for(i=1;i<64/2+1;i++)
            {
                dd[i].real=dd[i].real/abs_min;
                printf("value:%f\r\n",dd[i].real);
            }
            adc_state++;
            break;
        case 15:
            for(i=0;i<16;i++)
                displayBar[i]=dd[i+1+1].real-1;
            adc_state=0;
        break;
        default:
            break;
        
    }
    
}
