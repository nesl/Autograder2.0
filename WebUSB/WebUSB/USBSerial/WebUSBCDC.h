

#ifndef WEBUSBCDC_H
#define WEBUSBCDC_H
#include "WebUSB.h"
#include "WebUSBDevice.h"
class WebUSBCDC: public WebUSBDevice
{
public:
	WebUSBCDC(uint16_t vendor_id, uint16_t product_id, uint16_t product_release = 0x0001, bool connect = true);
	
	bool write(uint8_t * buffer, uint32_t size, bool isCDC=false);
    bool read(uint8_t * buffer, uint32_t * size, bool isCDC=false, bool blocking=false);
    virtual uint8_t * allowedOriginsDesc(); //pure virtual
    virtual uint8_t * urlIlandingPage();  //pure virtual
    virtual uint8_t * urlIallowedOrigin();  //pure virtual

protected:
    virtual bool USBCallback_request();
    virtual bool USBCallback_setConfiguration(uint8_t configuration);
    virtual uint8_t * stringIproductDesc();
    virtual uint8_t * stringIinterfaceDesc();
    virtual uint8_t * configurationDesc();
    virtual uint8_t * stringImanufacturerDesc();
    virtual uint8_t * stringIserialDesc();
    
private:
    volatile bool cdc_connected;
};

#endif