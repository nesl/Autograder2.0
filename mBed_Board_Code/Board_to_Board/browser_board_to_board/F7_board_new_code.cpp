//THIS IS A PROTOTYPE OF THE CODE THAT WILL BE USED TO RECEIVE PWM DATA ON THE STUDENT BOARD AND SEND IT TO THE TEST BOARD

#include "mbed.h"

DigitalOut myled(LED1);
Serial bridge(PE_8,PE_7);
DigitalOut go(PB_10);
PwmOut led(PB_8);
Serial pc(USBTX,USBRX);
int main() 
{
    while(1) 
    {
        //MAY NEED TO USE STATIC_CAST DEPENDING ON HOW DATA IS SENT.
        //OTHERWISE SIMPLY SUBTRACT CHARACTER 0
        char dutyTemp = bridge.getc();
        int duty = dutyTemp - '0';
        //int duty = static_cast<int>(dutyTemp); //Converting from ASCII to digits. Use this if data is encoded.
        pc.printf("Duty received: %d \n\r", duty);
        float cycle = static_cast<float>(duty);
        char periodTemp = bridge.getc();
        int per = periodTemp - '0';
        //int per = static_cast<int>(periodTemp); //Converting from ASCII to digits.Use this if data is encoded.
        pc.printf("Period received: %d \n\r",per);
        led.period_ms((per+1)*10); //Using the formula from the homework assignment.
        pc.printf("NEW: Duty cycle is %f \n\r",cycle/10);
        led.write(cycle/10); //Converts number to a percentage.
        go = 1; //This interrupts the other board and tells it to begin counting.
        wait(.05);
        go = 0;
    }
}