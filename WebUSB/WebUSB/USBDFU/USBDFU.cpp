/* Copyright (c) 2010-2011 mbed.org, MIT License
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "stdint.h"
#include "USBHAL.h"
#include "USBDFU.h"
#include "DFU.h"

USBDFU::USBDFU(uint16_t vendor_id, uint16_t product_id, uint16_t product_release, bool connect)
    : USBDevice(vendor_id, product_id, product_release),
      detach(no_op)
{
    if (connect) {
        USBDevice::connect();
    }
}

void USBDFU::attach(Callback<void()> func) {
    if (func) {
        detach.attach(func);
    } else {
        detach.attach(no_op);
    }
}

//
//  Route callbacks from lower layers to class(es)
//


// Called in ISR context
// Called by USBDevice on Endpoint0 request
// This is used to handle extensions to standard requests
// and class specific requests
// Return true if class handles this request
bool USBDFU::USBCallback_request() {
    bool success = false;
    CONTROL_TRANSFER * transfer = getTransferPtr();

    // Process class-specific requests
    if (transfer->setup.bmRequestType.Type == CLASS_TYPE)
    {
        switch (transfer->setup.bRequest)
        {
            case DFU_DETACH:
                 detach.call();
                 success = true;
            default:
                break;
        }
    }

    return success;
}


#define DEFAULT_CONFIGURATION (1)


// Called in ISR context
// Set configuration. Return false if the
// configuration is not supported
bool USBDFU::USBCallback_setConfiguration(uint8_t configuration) {
    if (configuration != DEFAULT_CONFIGURATION) {
        return false;
    }

    return true;
}


uint8_t * USBDFU::stringIinterfaceDesc() {
    static uint8_t stringIinterfaceDescriptor[] = {
        0x08,               //bLength
        STRING_DESCRIPTOR,  //bDescriptorType 0x03
        'D',0,'F',0,'U',0,  //bString iInterface - DFU
    };
    return stringIinterfaceDescriptor;
}

uint8_t * USBDFU::stringIproductDesc() {
    static uint8_t stringIproductDescriptor[] = {
        0x16,                                                       //bLength
        STRING_DESCRIPTOR,                                          //bDescriptorType 0x03
        'D',0,'F',0,'U',0,' ',0,'D',0,'E',0,'V',0,'I',0,'C',0,'E',0 //bString iProduct - DFU device
    };
    return stringIproductDescriptor;
}

#define DEFAULT_CONFIGURATION (1)
#define DFU_DESCRIPTOR_LENGTH (9)
#define TOTAL_DESCRIPTOR_LENGTH ((1 * CONFIGURATION_DESCRIPTOR_LENGTH) \
                               + (1 * INTERFACE_DESCRIPTOR_LENGTH) \
                               + (1 * DFU_DESCRIPTOR_LENGTH))

#define DETACH_TIMEOUT 255
#define DFU_TRANSFER_SIZE 1024

uint8_t * USBDFU::configurationDesc() {
    static uint8_t configurationDescriptor[] = {
        CONFIGURATION_DESCRIPTOR_LENGTH,// bLength
        CONFIGURATION_DESCRIPTOR,       // bDescriptorType
        LSB(TOTAL_DESCRIPTOR_LENGTH),   // wTotalLength (LSB)
        MSB(TOTAL_DESCRIPTOR_LENGTH),   // wTotalLength (MSB)
        0x01,                           // bNumInterfaces
        DEFAULT_CONFIGURATION,          // bConfigurationValue
        STRING_OFFSET_ICONFIGURATION,   // iConfiguration
        C_RESERVED | C_SELF_POWERED,    // bmAttributes
        C_POWER(0),                     // bMaxPower

        INTERFACE_DESCRIPTOR_LENGTH,    // bLength
        INTERFACE_DESCRIPTOR,           // bDescriptorType
        0x00,                           // bInterfaceNumber
        0x00,                           // bAlternateSetting
        0x00,                           // bNumEndpoints
        DFU_CLASS_APP_SPECIFIC,         // bInterfaceClass
        DFU_SUBCLASS_DFU,               // bInterfaceSubClass
        DFU_PROTO_RUNTIME,              // bInterfaceProtocol
        STRING_OFFSET_IINTERFACE,       // iInterface

        DFU_DESCRIPTOR_LENGTH,          // bLength
        DFU_DESCRIPTOR,                 // bDescriptorType
        (DFU_ATTR_WILL_DETACH           // bmAttributes
        |DFU_ATTR_CAN_DOWNLOAD),
        LSB(DETACH_TIMEOUT),            // wDetachTimeOut (LSB)
        MSB(DETACH_TIMEOUT),            // wDetachTimeOut (MSB)
        LSB(DFU_TRANSFER_SIZE),         // wTransferSize (LSB)
        MSB(DFU_TRANSFER_SIZE),         // wTransferSize (MSB)
        LSB(DFU_VERSION_1_00),          // bcdDFUVersion (LSB)
        MSB(DFU_VERSION_1_00),          // bcdDFUVersion (MSB)
    };
    return configurationDescriptor;
}
