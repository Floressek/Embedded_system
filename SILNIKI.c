#include <REGX52.H>
#define Max 255 //define maxval for microcontr.
#define EN2	P2_4
#define IN3	P2_5
#define IN4	P2_6


volatile unsigned char data Nastawa; //bo nastawa w dwoch funkcjach bedzie uzywana
volatile unsigned char data Suwak;
volatile bit motor_stopped;

unsigned char data Min; //dla min value for jumps


void Innit(void) 
{
	//setup pod krecenie silnika w jedna strone
	IN3	=	1;
	IN4	=	0;
	if(Nastawa == 0x00) {EN2	=	0;}
	else if(Nastawa == Max) {EN2	=	1;}
	
	
	P3_4 = 0; //kontrola bitu modulu komuinikacji U6
	
	Suwak = 0x40;
	Nastawa = 0x80; //0x00(min) <1log>	0x80(mid) <0log>	0xFF(max)
	
	TCON = TCON & 0x0F;
	TCON = TCON | 0x00;
	//TMOD = 0x20; //koslawo gdy uzywasz timer0 wiec NIE
	TMOD = TMOD & 0x0F; //logicznie dla lewej strony timer1
	TMOD = TMOD | 0x20; //dla prawej dla modu 8 bit autoreload
	TH1 = TL1 = 0x00;
	ET1 = 1; //enable timer1
	EA = 1; //enableAccess for interrupts
	TR1 = 1; //timer 1 run
	SCON = 0x50; // UART mode 1, REN enabled
  TMOD |= 0x20; // Timer1 mode 2
  TH1 = 0xFD; // Set TH1 for baud rate (e.g., 9600 for 11.0592 MHz)
	ES = 1;
}

void UART_ISR(void) interrupt 4 {
    if (RI) {
        RI = 0; // Clear RI flag
        if (SBUF == '7') { //mialo to stopowac silnik zawsze ale dziala jak to jest tylko 1 wartosc z klawiatury ;(
            motor_stopped = 1; // Ustaw flage zatrzymania silnika
        }
    }
}


void ISR_PWM(void) interrupt 3
{
	
	if(motor_stopped){ EN2 = 0; TR1 = 0; return;}
	if(SBUF != '7'){
	Suwak++;
	if(Nastawa	==	0) 	//musimy tez kontrolowac suwak
		{
			EN2	=	0;
		}
	else
	{
		if(Nastawa	== Max) 
			{
				EN2	=	1;
			}
		else
		{
			if(Suwak	< Nastawa) {EN2	=	1;} else {EN2	=	0;}
		}
	}
	if(Suwak	==	Max) {Suwak	=	0;}
}
}
			
	/*
	
	Suwak++;
	if(Suwak<Nastawa) 
	{
		P2_3 = 1;
	} 
	else 
	{
		P2_3 = 0;
	} // wplywamy na P2_3 enable
		if (Suwak<Max)
		{
			P2_3 = 0;
		}
		else 
		{
			Suwak = 0; 
			P2_3 = 1;
		}
}
*/

void main(void) 
{
	Innit();
	for(;;) {;} //endless func
}
