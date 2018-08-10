//Copyright 2018 UCLA Networked and Embedded Systems Lab

#include "mbed.h"
#include "WebUSBCDC.h"

#include <string>

#define MAX_BUF_SIZE (1024) 
#define NUM_PERIOD_PINS (5) 
#define NUM_DUTY_CYCLE_PINS (7)
#define MIN_NUM_STAMPS (6) //Minimum of 3 cycles, so 6 rises/falls
#define MIN_TIME_UNIT (.0002) //Seconds, also equals 0.2 ms
#define NUM_TIME_UNITS (25000) //Adjust this number to adjust the recording time
#define RECORDING_TIME (MIN_TIME_UNIT * NUM_TIME_UNITS)

//LED's
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);

//PWM Pins
InterruptIn PWM_RECEIVER(p27); //Using built in function
DigitalIn PWM_MANUAL(p25); //Using toggling

DigitalOut req(p21); //Tells F7 to start generating waves

//Period Pins
DigitalOut MSP(p5); //Most significant bit for period
DigitalOut P2(p6);
DigitalOut P3(p7);
DigitalOut P4(p8);
DigitalOut LSP(p9); //Least significant bit for period
DigitalOut perList[] = {MSP, P2, P3, P4, LSP}; //List of period pins

//Duty Cycle Pins
DigitalOut MSD(p11); //Most significant bit for duty cycle
DigitalOut DC2(p12);
DigitalOut DC3(p13);
DigitalOut DC4(p14);
DigitalOut DC5(p15);
DigitalOut DC6(p16);
DigitalOut LSD(p17); //Least significant bit for duty cycle
DigitalOut dutyList[] = {MSD, DC2, DC3, DC4, DC5, DC6, LSD}; //List of duty cycle pins
//For Debugging, terminal output using screen
Serial pc(USBTX, USBRX);

//Helper Function Prototypes; Definitions below main
int readCommand(void); //Takes a command from the browser
void assignmentOne(void); //Records PWM waves
void sendAllData(int list[], int numElements); //Sends list of timestamps to browser
void sendBinary(uint8_t buffer[], DigitalOut list[], int size); //Toggles period/duty cycle pins
void timeLiveGraph(); //Sends timestamps of rises/falls
void convertIntToBuf(int num, uint8_t buf[]); //Converts an integer into a 4 byte uint8_t buffer
void writeToBrowser(uint8_t buffer[]); //Sends data in the buffer to the browser
void readDataFromBrowser(uint8_t * buffer, uint32_t & len); //Reads data from the browser, stores in the passed buffer
void sendStop(int checker); //Sends a 1 to indicate no error or -1 to indicate stop reading OR error

//USB object
WebUSBCDC webUSB(0x1F00,0x2012,0x0001, false);
volatile bool done = false;
Timer pwmTimer;
EventQueue queue(64*EVENTS_EVENT_SIZE);
Thread timeThread;
int *timeList;
uint8_t *liveBuffer;
volatile int numWrites = 0;
volatile int numStamps = 0;
bool canWrite = false;


void writeLive(int stamp)
{
    convertIntToBuf(stamp, liveBuffer);
    writeToBrowser(liveBuffer);
}
void stopInterrupt()
{
    done = true;
}
void riseHandler(){
    int temp = pwmTimer.read_us();
    ++numStamps;
    if(canWrite)
        queue.call(writeLive,temp);
    if(pwmTimer.read() > RECORDING_TIME)
    {
        PWM_RECEIVER.disable_irq();
        queue.call(stopInterrupt); //Bool cannot be reset until all writes are done
    }
}
void fallHandler()
{
    int temp = pwmTimer.read_us();
    ++numStamps;
    if(numStamps >= MIN_NUM_STAMPS)
    {
        queue.call(writeLive, temp);
        canWrite = true;
    }
}



int main() 
{
    PWM_RECEIVER.disable_irq(); //Ensures interrupt will not occur when not needed
    PWM_RECEIVER.rise(&riseHandler); //Defines what to do when a rise is detected
    PWM_RECEIVER.fall(&fallHandler); //Defines what to do when a fall is detected
    timeThread.start(callback(&queue, &EventQueue::dispatch_forever));
    while(1) 
    {
        int choice = readCommand();
        if(choice == 0)
            assignmentOne();
        else if(choice == 1)
            led1 = !led1;
        else if(choice == 2)
            led2 = !led2;
        else if(choice == 3)
            led3 = !led3;
        else
            pc.printf("Command failed. Choice is %d \r\n", choice);
    }
}
//-----------------------------------------------------------
void assignmentOne()
{
    uint32_t lenPer, lenDuty; //Leave undeclared, or else read function will not work
    uint8_t *perBuf = new uint8_t[MAX_BUF_SIZE]; //Dynamically allocated because read function prefers it
    uint8_t *dutyBuf = new uint8_t[MAX_BUF_SIZE];
    readDataFromBrowser(perBuf, lenPer); //Storing binary from browser into buffer
    readDataFromBrowser(dutyBuf, lenDuty);
    sendBinary(perBuf, perList, NUM_PERIOD_PINS); //Sends data for other board to interpret
    sendBinary(dutyBuf, dutyList, NUM_DUTY_CYCLE_PINS);
    delete [] perBuf; 
    delete [] dutyBuf;
    perBuf = 0; 
    dutyBuf = 0; 
    req = 1; //Send signal for other board to generate waves
    timeLiveGraph(); 
    req = 0; //Set signal to 0 so that it can rise again later
}
//-----------------------------------------------------------
void sendBinary(uint8_t buffer[], DigitalOut list[], int size) //Function that sends signals to student board
{
    for(int i=0; i<size; ++i)
        list[i] = static_cast<int>(buffer[i]) - '0'; //Converts '1' and '0' chars to int values. Sets pins ON/OFF
}
//-----------------------------------------------------------
void timeLiveGraph() //Function that times board
{
    int checker = 1; //Checks for timeout error
    //timeList = new int [MAX_BUF_SIZE]; //Holds all of the timestamps
    liveBuffer = new uint8_t[sizeof(int)];
    PWM_RECEIVER.enable_irq();
    led1 = 1;
    pwmTimer.start();
    while(!done);
    if(numStamps <= MIN_NUM_STAMPS) //Checking that at least more than two cycles completed
        checker = -1.0;
    //else
        //sendAllData(timeList,numStamps); //Data sent if minimum number of cycles is met
    //delete [] timeList;
    //timeList = 0;
    delete [] liveBuffer;
    liveBuffer = 0;
    pwmTimer.stop();
    pwmTimer.reset();
    sendStop(-1); //Tells the browser to stop reading
    sendStop(checker); //Tells the browser good(1) or error(-1)
    numStamps = numWrites = done = canWrite = 0; //Resetting all Values
}
//-----------------------------------------------------------
void convertIntToBuf(int num, uint8_t buf[]) //Converts a int to a uint8_t buffer to send
{
    //Reversing bytes of the number; browser/board have different endianness
    num = (((num>>24) & 0x000000ff) | ((num>>8) & 0x0000ff00) | ((num<<8) & 0x00ff0000) | ((num<<24) & 0xff000000));
    memcpy(buf, &num, sizeof(num)); //Storing the bits of the integer into the buffer
}
//----------------------------------------------------------
void writeToBrowser(uint8_t buffer[]) //Sends data to the browser
{
    while(1) //Loop will continue until data is written
    {
        if(!webUSB.configured())
            webUSB.connect();
        if(webUSB.configured() && webUSB.write(buffer, sizeof(int)))
            break;
    }
}
//----------------------------------------------------------
void readDataFromBrowser(uint8_t *buffer, uint32_t & len) //Reads data from browser into buffer
{   
    while(1) //Loop will continue until data is read
    {
        if (!webUSB.configured()) 
            webUSB.connect(); 
        if(webUSB.configured() && webUSB.read(buffer, &len))
            break;
    }
}
//----------------------------------------------------------
int readCommand(void)//receives a command from the website
{   
    uint32_t len; //Leave undeclared, or else read function will not work
    uint8_t *commandBuf = new uint8_t[sizeof(int)]; //Dynamically allocated because read function prefers it
    readDataFromBrowser(commandBuf, len);
    int choice = static_cast<int>(commandBuf[0]) - '0'; //Subtracting ascii value of 0 to get integer value
    delete [] commandBuf;
    commandBuf = 0;
    return choice;
}
//-----------------------------------------------------------
void sendAllData(int list[], int numElements)
{
    uint8_t *dataBuffer = new uint8_t[sizeof(list[0])]; //Making buffer the size of the datatype
    if(numElements % 2 == 0)
        --numElements; //Discards extra timestamp for half cycle 
    //Iterations will alternate between off or on timestamp. Off time is first
    for(int i = MIN_NUM_STAMPS;i < numElements;i++) //i=MIN_NUM_STAMPS to discard first two cycles
    {
            convertIntToBuf(list[i], dataBuffer);
            writeToBrowser(dataBuffer);
    }
    delete [] dataBuffer;
    dataBuffer = 0;
}
//-----------------------------------------------------------
void sendStop(int checkNum)
{
    uint8_t *stop = new uint8_t[sizeof(checkNum)];
    convertIntToBuf(checkNum, stop);
    writeToBrowser(stop);
    delete [] stop;
    stop = 0;
}