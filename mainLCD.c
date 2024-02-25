
// Autor: dr inz. Krzysztof Murawski - uzyte/edytowane przez SZYMON FLOREK do projektu WCY22IY2S1
#include <REGX52.H>
#define LCD_SHIFT_LEFT       0x18
#define LCD_SHIFT_RIGHT      0x1C
#define LCD_CLEAR            0x01

// Deklaracje funkcji zewnetrznych
extern void LcdInit();
extern void LcdWelcome();
extern void Lcd_DisplayCharacter(char a_char);
extern void Lcd_WriteControl(unsigned char LcdCommand);
extern void Lcd_DisplayScreen(char *ptr);

// Zmienne globalne
unsigned char data Var1, Var2, Var3;
volatile unsigned char data Bufor[] = {" Odebrano:      "};
volatile unsigned int interruptCounter = 0;
volatile unsigned char currentAnimationFrame = 0;

static unsigned char code Anim1[] = 
    "                "
    "       *        "
    "       *        "
    "      ***       ";
    
static unsigned char code Anim2[] = 
    "                "
    "       *        "
    "      *.*       "
    "       *        ";
    
static unsigned char code Anim3[] = 
    "                "
    "      *.*       "
    "       *        "
    "                ";
    
static unsigned char code Anim4[] = 
    "       *        "
    "      *.*       "
    "                "
    "                ";
    
static unsigned char code Anim5[] = 
    "     *.*.*     "
    "    *..*..*    "
    "     *.*.*     "
    "               ";  

void wait(){
    volatile unsigned int x;
    volatile unsigned int y;
    for(x = 0; x < 100; x++) {for(y = 0; y < 100; y++);}
}

// Obsluga przerwania UART
void UART_ISR(void) interrupt 4 {
    if (RI) {
        RI = 0; // Clear RI flag
        if (SBUF == '0') {
            Lcd_WriteControl(LCD_CLEAR);
            wait();
            // Wyswietl napis "NUKE SENT"
          Lcd_DisplayScreen("                "
                            "      NUKE      "
                            "      SENT      "
                            "                ");
           wait();
            // Rozpocznij animacje
            TR0 = 1;
            currentAnimationFrame = 1;
        }
    }
}



// Obsluga przerwania timera 0
void Timer0_ISR(void) interrupt 1 {
    TH0 = 0xE4;      // Wartosci startowe
    TL0 = 0xB6;

    // Increment the interrupt counter
    interruptCounter++;
    if (interruptCounter >= 42) { // Przyblizona wartosc dla 2 sekund
        interruptCounter = 0;
        // Zmiana klatki animacji
        switch (currentAnimationFrame) {
            case 1:
                Lcd_DisplayScreen(Anim1);
                break;
            case 2:
                Lcd_DisplayScreen(Anim2);
                break;
            case 3:
                Lcd_DisplayScreen(Anim3);
                break;
            case 4:
                Lcd_DisplayScreen(Anim4);
                break;
            case 5:
                Lcd_DisplayScreen(Anim5);
                TR0 = 0; // Zatrzymaj timer i animacje
                currentAnimationFrame = 0; // Resetuj animacje
                break;
        }
        currentAnimationFrame++;
    }
}

// Inicjalizacja timera 0
void Timer0_Init(void) {
    TMOD &= 0xF0; // Ustawienie Timer0 w trybie 16-bitowym
    TMOD |= 0x01;
    TH0 = 0xE4;      // Wartosci startowe
    TL0 = 0xB6;
    ET0 = 1;      // Wlaczenie przerwan od Timer0
    // TR0 = 1;    // Uruchomienie Timer0 - bedzie uruchomione przez UART ISR
}

// Inicjalizacja systemu
void Init(void) {
    P3_4 = 0;
    EX0 = 1;
    IT0 = 1;
    EA = 1;
    SCON = 0x50; // Tryb 1 UART
    ES = 1;
    TMOD |= 0x20; // Timer1 w trybie 2
    TH1 = 0xFD;   // Baud rate dla 9600 przy 11.0592 MHz
    TR1 = 1;      // Start Timer1
    Timer0_Init(); // Inicjalizacja Timer0
}

// Glówna funkcja programu
void main(void) {
    LcdInit();       // Inicjalizacja wyswietlacza LCD
    Init();          // Inicjalizacja systemu
    LcdWelcome();    // Wyswietlenie powitania

    while (1) {
        // Petla glówna programu
    }
}



