#include <REGX52.H>

volatile unsigned char data rowIndex, colIndex;
unsigned char code rowPattern[12] = {0x7B, 0xE7, 0xEB, 0xED, 0xD7, 0xDB, 0xDD, 0xB7, 0xBB, 0xBD, 0x77, 0x7D}; // values encoded (read left to right in bin)
unsigned char code columnPattern[3] = {0xFD, 0xFB, 0xF7};
unsigned char code keypadValues[12] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '#'};

volatile unsigned char data sequenceBuffer[5]; // Bufor na 5 ostatnich znaków
volatile unsigned char data sequenceIndex = 0;

volatile unsigned char data key;

void SerialInit() {
P3_4=0;
//Serial
TCLK = 1;
//T2CON = 0x34; //ustawia odpowiednio timer czesciowo
TH2   = RCAP2H = 0xFF; //Pierwsza czesc RCAP
TL2   = RCAP2L = 0xDC; //Druga czesc RCAP
TR2  = 1; //wlaczenie timera2
	  SCON = 0x50; // UART mode 1, REN enabled
    TMOD |= 0x20; // Timer1 mode 2
    TH1 = 0xFD; // Set TH1 for baud rate (e.g., 9600 for 11.0592 MHz)
}


void Send(unsigned char value)
{
P3_4=1;
TI   = 0;
SBUF = value;
while(TI == 0){;}
TI   = 0;
P3_4=0;
}

void CheckSequence(void)
{
if(sequenceBuffer[0]  == '3' && sequenceBuffer[1]  == '5' &&
sequenceBuffer[2]  == '6' && sequenceBuffer[3]  == '9' && sequenceBuffer[4]  == '#')
{
Send('*');
}
else
{
Send('7');
}
}
void AddToSequence(unsigned char value)
{
sequenceBuffer[sequenceIndex] = value;
sequenceIndex = (sequenceIndex + 1) % 5; //zapetalanie bufora
if(value == '#')
{
CheckSequence(); //sprawdz w momencie wprowadzenia #
}
}

void main(void) {
    SerialInit();
    P2 = 0xFF; // All high states

    while(1) {
        for(colIndex = 0; colIndex < 3; colIndex++) { // Scan each column
            P2 = columnPattern[colIndex];

            for(rowIndex = 0; rowIndex < 12; rowIndex++) { // Check each row
                if (P2 == rowPattern[rowIndex]) {
key = keypadValues[rowIndex];
                    Send(key); // Send corresponding character
AddToSequence(key);
                    while (P2 == rowPattern[rowIndex]); // Wait for key release
                    break;
                }
            }
        }
    }
}
