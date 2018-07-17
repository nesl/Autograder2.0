#include "mbed.h"
#include "USBSerial.h"
#include "WebUSBCDC.h"
//Virtual serial port over USB
//USBSerial serial;
DigitalOut  myled(LED1);

/*
bool write(uint8_t * buffer, uint32_t size, bool isCDC=false);
bool read(uint8_t * buffer, uint32_t * size, bool isCDC=false, bool blocking=false);
Keep these here to see the definitions
*/
#define MAX_BUF_SIZE 1024

int main(void) {
    //uint8_t buffer[] = {'3','.','1','4','1','5','9'};
    uint32_t len;
    uint8_t * buf; //Pointer necessary for read function. Should be needed for write as well but works without pointer for some reason
    
    WebUSBCDC usbDFU(0x1F00,0x2012,0x0001, false);
    while(1)
    {
        buf = new uint8_t[MAX_BUF_SIZE];
        // Check the DFU status
        if (!usbDFU.configured()) {
            usbDFU.connect();
        }
        /*
        if(usbDFU.configured() && usbDFU.write(buffer,8))
        {
            wait(1);
        } 
        */
        pc.printf("Entering read\n\r");
       if(usbDFU.configured() && usbDFU.read(buf, &len))
        {
            wait(1);
            usbDFU.write(buf, 8);//Triesto write what it just read
        } 
    delete buf;
    }
}