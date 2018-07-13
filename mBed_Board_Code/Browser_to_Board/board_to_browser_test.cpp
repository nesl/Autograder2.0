#include "mbed.h"
#include "USBSerial.h"
#include "WebUSBCDC.h"
//Virtual serial port over USB
//USBSerial serial;
DigitalOut  myled(LED1);

int main(void) {
   // uint8_t buffer[] = {'3','.','1','4','1','5','9'};
    //uint8_t buffer1[] = {'h','e','l','l','o'};
    uint8_t readBuffer[32];
    uint32_t * len;
    *len = 32;
    WebUSBCDC usbDFU(0x1F00,0x2012,0x0001, false);
    while(1)
    {
        //serial.printf("I am a virtual serial port\r\n");
        wait(1);
        // Check the DFU status
        if (!usbDFU.configured()) {
            usbDFU.connect();
        }
        /*if(usbDFU.configured() && usbDFU.write(buffer1,8))
        {
            wait(1);
        }  */   
        if(usbDFU.configured() && usbDFU.read(readBuffer, len, false, true))
        {//MAY WANT TO CHECK IF TRUE SHOULD BE FALSE
            wait(1);    
        }  
        if(usbDFU.configured() && usbDFU.write(readBuffer,32))
        {
            wait(1);
        }
    }
}