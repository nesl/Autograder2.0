//THIS IS A PROTOTYPE OF THE CODE THAT WILL BE USED TO SEND DATA FROM THE BROWSER TO THE STUDENT BOARD

#include "mbed.h"

DigitalOut myled(LED1);
Serial bridge(p9,p10);
InterruptIn pwmrec(p27);
InterruptIn go(p14);
Serial pc(USBTX, USBRX);
bool ready = false;
void record()
{
    pc.printf("Interrupt successful \n\r");
    Timer onTime;
    Timer totTime;
    while(pwmrec.read()){} //These two loops are to ensure that timing begins at the very beginning of the cycle.
    while(!pwmrec.read()){}
    totTime.start();
    onTime.start();
    //Waits while pwm is high
    while(pwmrec.read()){} 
    onTime.stop();
    //Waits while pwm is low 
    while(!pwmrec.read()){} 
    totTime.stop();
    float period = totTime.read();
    float duty = onTime.read()/period;
    pc.printf("Duty Cycle is %9f \n\r", duty); //These print functions are how python receives the data. 
    wait(0.1);
    pc.printf("Period is %9f \n\r",period); //I have limited them to 7 characters. Python needs to know how many characters they will be in advance.     
    ready = true;
}
int main()
{
    go.rise(&record);
    while(1)
    {
        pc.printf("Enter the period (0-9): \n\r");
        int per = pc.getc();
        pc.printf("Period received 1768: %d \n\r",per);
        pc.printf("Enter the duty cycle(0-9): \n\r");
        int duty  = pc.getc();
        pc.printf("Period received: %d \n\r", duty);
        bridge.putc(per);
        bridge.putc(duty);
        while(!ready)
        {
            pc.printf("Stuck here.\n\r");
            wait(2);
        }
        ready = false;
            
    }
}