#include "mbed.h"
PwmOut led(PB_8);
DigitalOut go(PB_10);
Serial pc(USBTX, USBRX);
int main() 
{
    while(1) 
    {
        char dutyTemp = pc.getc();
        int duty = static_cast<int>(dutyTemp); //Converting from ASCII to digits.
        float cycle = static_cast<float>(duty);
        char periodTemp = pc.getc();
        int per = static_cast<int>(periodTemp); //Converting from ASCII to digits.
        led.period_ms((per+1)*10); //Using the formula from the homework assignment.
        led.write(cycle/100); //Converts number to a percentage.
        go = 1; //This interrupts the other board and tells it to begin counting.
        wait(.05);
        go = 0;
    }
}