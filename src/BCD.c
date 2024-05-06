/*#include <REGX52.H>

// Adresy wyswietlaczy
volatile unsigned char xdata Lewy _at_ 0xFE00;
volatile unsigned char xdata Prawy _at_ 0xFD00;

// Przycisk do uruchamiania odliczania
sbit Button = P3^2;

// Tablica cyfr dla wyswietlaczy 7-segmentowych
unsigned char code Cyfry[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

// Zmienne do obslugi wyswietlania i licznika
volatile unsigned char data IndL, IndP;
volatile unsigned char data counting = 0;
volatile bit startCountdown = 0;

// Inicjalizacja Timer1 do generowania przerwania co okolo 1 ms
void Timer1_Init() {
    TMOD &= 0xF0;    // Czyszczenie konfiguracji dla Timer1
    TMOD |= 0x01;    // Ustawienie Timer0 w trybie 16-bitowym
    TH1 = 0xFC;      // Wartosc poczatkowa dla odliczania z czestotliwoscia 500 us
    TL1 = 0x66;      // Wartosc poczatkowa dla odliczania z czestotliwoscia 1 ms
    ET1 = 1;         // Wlaczenie przerwan od Timer1
    TR1 = 1;         // Uruchomienie Timer1
}

// Obsluga przerwania od Timer1
void Timer1_ISR(void) interrupt 3 {
    static unsigned int counter = 0;
    TH1 = 0xFC;      
    TL1 = 0x66;

    if(counter++ >= 1000) { // Inkrementacja co 1 ms, wiec 1000 = 1 sekunda
        counter = 0;
        if(startCountdown || counting) {
            startCountdown = 0;  // Reset flagi rozpoczecia odliczania
            counting = 1;        // Rozpocznij odliczanie
            // Logika odliczania
            if(IndP > 0) {
                IndP--; // Dekrementacja prawego wyswietlacza
            } else if(IndL > 0) {
                IndL--; // Dekrementacja lewego wyswietlacza gdy prawy = 0
                IndP = 9;
            }
            // Aktualizacja wyswietlaczy
            Lewy = Cyfry[IndL];
            Prawy = Cyfry[IndP];
        }
    }
}

// Obsluga przerwania zewnetrznego INT0 (przycisk)
void External0_ISR(void) interrupt 0 {
    counting = !counting; // Zmiana stanu licznika
    if (counting) {
        IndL = 9; // Rozpoczecie odliczania od 9
        IndP = 9;
    } else {
        IndL = 0; // Resetowanie licznika
        IndP = 0;
    }
}

// Obsluga przerwania UART
void Serial_ISR(void) interrupt 4 {
    if (RI) {  // Jesli ustawiono flage RI (Received Interrupt)
        RI = 0; // Wyczysc flage RI
        if (SBUF == '*') {  // Sprawdz, czy otrzymano gwiazdke
            startCountdown = 1;  // Ustaw flage rozpoczecia odliczania
        }
    }
    if (TI) {  // Jesli ustawiono flage TI (Transmit Interrupt)
        TI = 0; // Wyczysc flage TI
        // Dodaj tutaj kod, jesli jest potrzebna obsluga wysylania danych
    }
}

void Setup() {
    IndL = 0; // Poczatkowa wartosc na lewym wyswietlaczu
    IndP = 0; // Poczatkowa wartosc na prawym wyswietlaczu
    IT0 = 1;  // Ustawienie przerwania INT0 na zbocze opadajace
    EX0 = 1;  // Wlaczenie zewnetrznego przerwania INT0
    Timer1_Init(); // Inicjalizacja Timer1
    EA = 1;   // Wlaczenie globalnych przerwan
    ES = 1;   // Wlaczenie przerwan od UART
P3_4 = 0;
TCLK = 1;
RCLK = 1;
SCON = 0x50;

}

void main() {
    Setup(); // Wywolanie funkcji ustawiajacej poczatkowe konfiguracje
    while(1) {
        // Petla glówna jest pusta, cala logika oparta jest na przerwaniach
    }
}
*/

#include <REGX52.H>

// Adresy wyswietlaczy
volatile unsigned char xdata Lewy _at_ 0xFE00;
volatile unsigned char xdata Prawy _at_ 0xFD00;

unsigned char Cyfry[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
volatile unsigned char units = 0;
volatile unsigned char tens = 0;  
volatile unsigned char display_flag = 0; // Additional flag for display - timer action

void Send(unsigned char value)
{
P3_4=1;
	TI   = 0;
	SBUF = value;
		while(TI == 0){;}
	TI   = 0;
P3_4=0;
}


void Timer0_ISR(void) interrupt 1 {
    TH0 = 0xF0; // Reload high byte
    TL0 = 0x01; // Reload low byte

    display_flag = 1; // Set flag to update display

   if (units == 0) {
        units = 9;
      if (tens == 0) {
           tens = 9;
      } else {
          tens--;
      }
   } else {
        units--;
   }

    if (tens == 0 && units == 0) {
				Send('0');
        // Zatrzymaj timer, jesli osiagnieto 0
        TR0 = 0;
    }
}


void display(void) {
    if (display_flag) { // Check new flag
        // Tens - left screen

			
       Prawy = Cyfry[units];
        P2_0 = 0; // P1_2 controls - left screen
        P2_1 = 1; // P1_3 controls - right screen

        // Short delay
        TH0 = 0xFF; TL0 = 0xFF; // Delay
        while (!TF0); // Wait for overflow
        TF0 = 0; // Clear overflow flag
/*
        // Turn off - avoid ghosting nessescery?
        P2_0 = 0;
        P2_1 = 0;
        P2 = 0xFF; // Turn off segments
		*/
        Lewy = Cyfry[tens];
        P2_0 = 1; // P1_2 controls - left screen
        P2_1 = 0; // P1_3 controls - right screen
        // Short delay
        TH0 = 0xFF; TL0 = 0xFF; // Delay
        while (!TF0); // Wait for overflow
        TF0 = 0; // Clear overflow flag

/*
        P2_0 = 0; // P1_2 controls - left screen
        P2_1 = 0; // P1_3 controls - right screen
				P2 = 0xff;
				*/
		
		display_flag = 0;
    }
}

void UART_Init(void) {
    SCON = 0x50; // UART mode 1, REN enabled
    TMOD |= 0x20; // Timer1 mode 2
    TH1 = 0xFD; // Set TH1 for baud rate (e.g., 9600 for 11.0592 MHz)
    TR1 = 1; // Start Timer1
    ES = 1; // Enable UART interrupts
		P3_4 = 0;

}

void UART_ISR(void) interrupt 4 {
    if (RI) {
        RI = 0; // Clear RI flag
        if (SBUF == '*') { // Check for 'S' character
            TR0 = 1; // Start Timer0
        }
    }
}

void Timer0_Init(void) {
    TMOD &= 0xF0; // Clear Timer0 mode bits
    TMOD |= 0x01; // Timer0 mode 16-bit
    TH0 = 0xFC;   // High byte
    TL0 = 0x66;   // Low byte
    ET0 = 1;      // Enable Timer0 interrupt
    // TR0 = 1;    // Removed start Timer0 from here
	
	
}

void main(void) {
		units = 9;
		tens = 9;
    UART_Init();  // Initialize UART
    Timer0_Init(); // Initialize Timer0

    IT0 = 1; // Configure INT0 (P3.2)
    IT1 = 1; // Configure INT1 (P3.3)
    EX0 = 1; // Enable External Interrupt 0
    EX1 = 1; // Enable External Interrupt 1
    EA = 1;  // Enable global interrupts

    while(1) {
        display(); // Call display function
    }
}
