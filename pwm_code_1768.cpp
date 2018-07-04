#include "mbed.h"
#include "Timer.h"
#include <string>
Serial pc(USBTX, USBRX); // tx, rx
InterruptIn pwmrec(p27);
InterruptIn go(p14);
void record()
{
    Timer onTime;
    Timer totTime;
    while(pwmrec.read()){} //These two loops are to ensure that timing begins at the very beginning of the cycle.
    while(!pwmrec.read()){}
    totTime.start();
    onTime.start();
    while(pwmrec.read()){} //Waits while pwm is high
    onTime.stop();
    while(!pwmrec.read()){} //Waits while pwm is low 
    totTime.stop();
    float period = totTime.read();
    float duty = onTime.read()/period;
    pc.printf("%7f", duty); //These print functions are how python receives the data. 
    wait(0.1);
    pc.printf("%7f",period); //I have limited them to 7 characters. Python needs to know how many characters they will be in advance.     
}
int main()
{
    go.rise(&record);
    while(1){}
}