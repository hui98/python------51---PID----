/*------------------------------------------------  
                   author = 辉
									 time = 18 / 5 / 4 / 20.15
------------------------------------------------*/ 
#include<reg52.h>                       

unsigned char  recev_list[20];
int mark = 0;
unsigned int tmp=0;  //电机每秒转速计数变量
float P_para= 2.1,I_para=0.5,D_para= 1;
int set = 200; //预设转速
int ek = 0;  //当前差值
int ek1 = 0; //前次差值
int ek2 = 0; //前前次差值
int count=0; //实际转速
int Uk;  //增量

int time = 0;  //PWM判断值
int speed;	 //PID控制量，大小在0到time的最大值之间
int flag_start=0;
unsigned char test;

sbit  AIN1 = P1^0;   
sbit  AIN2 = P1^1 ;





/*------------------------------------------------  
                   解析函数  
------------------------------------------------*/  
union U{
	float v;
	unsigned char c[4];
}uu;

float b2f_hex(unsigned char * p)
{
		unsigned char * temp = p;
		uu.c[3] = *temp;
		uu.c[2] = *(temp + 1);
		uu.c[1] = *(temp + 2);
		uu.c[0] = *(temp + 3);
		return uu.v;
}

int b2i_hex(unsigned char * p)
{
		unsigned char *temp = p;
		return (*temp) + (*(temp+1))*256 ;
}
/*------------------------------------------------  
                   函数声明  
------------------------------------------------*/  
void SendStr(unsigned char *s);  
void SendByte(unsigned char dat)  ;
/*------------------------------------------------  
               PID增量计算以及更新 
------------------------------------------------*/  
void pid_refrush (int count)
{
	ek = set/60.0 - count/1500.0;   
	Uk = P_para *(ek-ek1)+I_para *ek + D_para *(ek-2*ek1+ek2);
	speed += Uk;
	if (speed >100)
		{
			speed = 100;
		}
	if (speed <0)
		{
			speed = 0;
		}
	ek2 = ek1;
	ek1 = ek;
}


  
/*------------------------------------------------  
                    串口初始化  
------------------------------------------------*/  
void InitUART  (void)  
{  
  
    TMOD|=0X21;    //T1定时器，工作方式2
		TH1=0XFD;    //T1定时器装初值
		TL1=0XFD;    //T1定时器装初值
		TR1=1;        //启动T1定时器
		REN=1;        //允许串口接收
		SM0=0;        //设定串口工作方式1
		SM1=1;        //设定串口工作方式1
		EA=1;        //开总中断
		ES=1;        //开串口中断
}                              
/*------------------------------------------------  
              外部中断0，定时器0初始化  
------------------------------------------------*/  
void Init_Timer0 (void)
{
		TMOD=0x01;//定时器0工作方式1  
    TH0=(65536-1000 )/256;//(65536-10)/256;//赋初值定时  
    TL0=(65536-1000)%256;//(65536-10)%256;//0.01ms  
    EA=1;//开总中断  
    ET0=1;//开定时器0中断  
    TR0=1;//启动定时器0   
		EX0 = 1; //开外部中断0
		IT0=1;
}
/*------------------------------------------------  
                    主函数  
------------------------------------------------*/  
void main (void)  
{ 
	mark = 0;
	PS = 1;
	PT0 = 0;
	PX0 = 0;
  Init_Timer0();
	InitUART();  
  AIN1=0;    //电机
	AIN2=0;		 //
	
	while (1)                         
	{ 
	if(flag_start == 1)
	{
		 if(time>=100)  //
		 {

			time=0;
			count = tmp;
			tmp = 0;
		//	SendByte(speed%256);
			pid_refrush(count);
		}	
		 
     if(time<speed)   //占空比%30,可改        
				AIN2=1;    
     else 
				AIN2=0;
			
		if (recev_list[0] == 0x02)
		{		
			SendByte(count%256);
			SendByte(count/256);

			recev_list[0] = 0;
		}
	}
	else
	{
		time = 0;
		AIN1 =0;
		AIN2 = 0;
		speed = 0;
		count = 0;
		tmp = 0;
		ek=0;
		ek2 = 0;
		ek1 = 0;
	}
	}  
}  
  
/*------------------------------------------------  
                    发送一个字节  
------------------------------------------------*/  
void SendByte(unsigned char dat)  
{  
	SBUF = dat;  
	while(!TI);  
	TI = 0;  
}  
/*------------------------------------------------  
                    发送一个字符串  
------------------------------------------------*/  
void SendStr(unsigned char *s)  
{  
 while(*s!='\0')// \0 表示字符串结束标志，通过检测是否字符串末尾  
  {  
		SendByte(*s);  
		s++;  
  }  
}
/*------------------------------------------------  
                     外部中断0程序  
------------------------------------------------*/  
void external0()  interrupt 0
{
    tmp++;
}
/*------------------------------------------------  
                     串口中断程序  
------------------------------------------------*/  
void UART_SER (void) interrupt 4 //串行中断服务程序  
{  

	
   //unsigned char Temp;	//定义临时变量 
	 
   if(RI)                        //判断是接收中断产生  
     {
		
			RI=0;
			recev_list[mark-1] = SBUF;
			mark ++;
                            //标志位清零  
			if (recev_list[0] == 0x01 && mark == 16)
			{
				mark = 1;
				P_para = b2f_hex(&recev_list[1]);
				I_para = b2f_hex(&recev_list[5]);
				D_para = b2f_hex(&recev_list[9]);
				set = b2i_hex(&recev_list[13]);
			}
			if (recev_list[0] == 0x02)
			{
				mark = 1;
			}
			if (recev_list[0] == 0x03)
			{
				if(flag_start == 0)
					flag_start = 1;
				else 
					flag_start = 0;
				mark = 1;
			}
			 //  Temp=SBUF;                 //读入缓冲区的值  
     // P1=Temp;                   //把值输出到P1口，用于观察  
      //SBUF=Temp;                 //把接收到的值再发回电脑端  
     }  
  // if(TI)                        //如果是发送标志位，清零  
  //   TI=0;  
	 }	 
  
/*------------------------------------------------  
              （定时器0）PWM波产生函数
								同时获得1秒内电机的转速
------------------------------------------------  */
void timer0(void) interrupt 1  
{  
   // TR0=0;//赋初值时，关闭定时器  
    TH0=(65536-1000 )/256;//(65536-100)/256;//赋初值定时  
    TL0=(65536-1000)%256;//(65536-100)%256;//1ms  
    time++;  
   
		//TR0=1;//打开定时器  
}  
