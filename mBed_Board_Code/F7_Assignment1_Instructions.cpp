#include "mbed.h"
#include <cmath>

#define NUM_PERIOD_PINS (5)
#define NUM_DUTY_CYCLE_PINS (7)

InterruptIn req(PE_0); //Tells F7 board when to generate waves

//PWM pins
PwmOut PWM_API(PC_8); //Generate waves using built in function
DigitalOut PWM_MANUAL(PC_10); //Manual toggling

//Period Pins
DigitalIn MSP(PB_10); //Most significant period bit
DigitalIn P2(PE_15);
DigitalIn P3(PE_14);
DigitalIn P4(PE_12);
DigitalIn LSP(PE_10); //Least significant period bit
DigitalIn perList[] = {MSP, P2, P3, P4, LSP}; //List that holds all bits in binary sequence

//Duty Cycle Pins
DigitalIn MSD(PE_7); //Most significant duty cycle bit
DigitalIn DC2(PE_8);
DigitalIn DC3(PG_9);
DigitalIn DC4(PG_14);
DigitalIn DC5(PF_15);
DigitalIn DC6(PE_13);
DigitalIn LSD(PF_14); //Least significant duty cycle bit
DigitalIn dutyList[] = {MSD, DC2, DC3, DC4, DC5, DC6, LSD}; //List that holds all bits in binary sequence

//For Debugging
Serial pc(USBTX,USBRX);

//Helper Functions
void generate(void);
int convertPeriodBinary();
float convertDutyBinary();

int main() 
{
    req.rise(&generate); //This function is only called when req rises
    
    while(1) {} //Wait indefinitely for req to rise
}
//---------------------------------------
void generate(void)
{
    int periodSum = convertPeriodBinary();
    float dutySum = convertDutyBinary();
    dutySum /= 100; //Converting to percentage
    periodSum = (periodSum+1)*10; //Converting to number using formula specified in homework
    
    //PWM_MANUAL
    //PWM_MANUAL
    PWM_API.period_ms(periodSum); 
    PWM_API.write(dutySum); 
    while(req){}
}
//---------------------------------------
int convertPeriodBinary()
{
    int periodSum = 0;
    for(int i=0; i<NUM_PERIOD_PINS;++i) //Convert the period pins to a decimal
    {
        if(perList[i]) //Only adds to the sum if the pin is set to 1
            periodSum += pow(2.0,NUM_PERIOD_PINS - 1 - i); //Power is based on how significant bit is
    }    
    return periodSum;
}
//---------------------------------------
float convertDutyBinary()
{
    float dutySum = 0;
    for(int i=0; i<NUM_DUTY_CYCLE_PINS;++i)
    {
        if(dutyList[i])
            dutySum += pow(2.0,NUM_DUTY_CYCLE_PINS - 1 - i);
    }
    return dutySum;
}
