/*
* Copyright 2016 Devan Lai
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "stdint.h"

#include "USBHAL.h"
#include "WebUSBDFU.h"
#include "WebUSB.h"
#include "DFU.h"
#include "WinUSB.h"

#include "USBDescriptor.h"

#define DEFAULT_CONFIGURATION (1)
#define DFU_INTERFACE_NUMBER  (0)

WebUSBDFU::WebUSBDFU(uint16_t vendor_id, uint16_t product_id, uint16_t product_release, bool connect)
    : WebUSBDevice(vendor_id, product_id, product_release),
      detach(no_op)
{
    if (connect) {
        WebUSBDevice::connect();
    }
}

void WebUSBDFU::attach(Callback<void()> func) {
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
bool WebUSBDFU::USBCallback_request() {
    bool success = false;
    CONTROL_TRANSFER * transfer = getTransferPtr();
    
    // Handle the Microsoft OS Descriptors 1.0 special string descriptor request
    if ((transfer->setup.bmRequestType.Type == STANDARD_TYPE) &&
        (transfer->setup.bRequest == GET_DESCRIPTOR) &&
        (DESCRIPTOR_TYPE(transfer->setup.wValue) == STRING_DESCRIPTOR) &&
        (DESCRIPTOR_INDEX(transfer->setup.wValue) == 0xEE))
    {
        static uint8_t msftStringDescriptor[] = {
            0x12,                                      /* bLength */
            STRING_DESCRIPTOR,                         /* bDescriptorType */
            'M',0,'S',0,'F',0,'T',0,'1',0,'0',0,'0',0, /* qWSignature - MSFT100 */
            WINUSB_VENDOR_CODE,                        /* bMS_VendorCode */
            0x00,                                      /* bPad */
        };
        
        transfer->remaining = msftStringDescriptor[0];
        transfer->ptr = msftStringDescriptor;
        transfer->direction = DEVICE_TO_HOST;
        success = true;
    }
    // Process Microsoft OS Descriptors 1.0 Compatible ID requests
    else if ((transfer->setup.bmRequestType.Type == VENDOR_TYPE) &&
             (transfer->setup.bmRequestType.Recipient == DEVICE_RECIPIENT) &&
             (transfer->setup.bRequest == WINUSB_VENDOR_CODE) &&
             (transfer->setup.wIndex == WINUSB_GET_COMPATIBLE_ID_FEATURE_DESCRIPTOR))
    {
        static uint8_t msftCompatibleIdDescriptor[] = {
            0x28, 0x00, 0x00, 0x00,         /* dwLength */
            LSB(COMPATIBLE_ID_VERSION_1_0), /* bcdVersion (LSB) */
            MSB(COMPATIBLE_ID_VERSION_1_0), /* bcdVersion (MSB) */
            LSB(WINUSB_GET_COMPATIBLE_ID_FEATURE_DESCRIPTOR), /* wIndex (LSB) */
            MSB(WINUSB_GET_COMPATIBLE_ID_FEATURE_DESCRIPTOR), /* wIndex (MSB) */
            0x01,                           /* bCount */
            0, 0, 0, 0, 0, 0, 0,            /* reserved */
            DFU_INTERFACE_NUMBER,           /* bFirstInterfaceNumber */
            0x00,                           /* reserved */
            'W','I','N','U','S','B',0,0,    /* compatible ID - WINUSB */
            0, 0, 0, 0, 0, 0, 0, 0,         /* subCompatibleID */
            0, 0, 0, 0, 0, 0,               /* reserved */
        };
        
        transfer->remaining = sizeof(msftCompatibleIdDescriptor);
        transfer->ptr = msftCompatibleIdDescriptor;
        transfer->direction = DEVICE_TO_HOST;
        success = true;
    }
    // Process DFU class-specific requests
    else if (transfer->setup.bmRequestType.Type == CLASS_TYPE)
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

    // Process WebUSB vendor requests 
    if (!success)
    {
        success = WebUSBDevice::USBCallback_request();
    }

    return success;
}

// Called in ISR context
// Set configuration. Return false if the
// configuration is not supported
bool WebUSBDFU::USBCallback_setConfiguration(uint8_t configuration) {
    if (configuration != DEFAULT_CONFIGURATION) {
        return false;
    }

    return true;
}


uint8_t * WebUSBDFU::stringIinterfaceDesc() {
    static uint8_t stringIinterfaceDescriptor[] = {
        0x08,               //bLength
        STRING_DESCRIPTOR,  //bDescriptorType 0x03
        'D',0,'F',0,'U',0,  //bString iInterface - DFU
    };
    return stringIinterfaceDescriptor;
}

uint8_t * WebUSBDFU::stringIproductDesc() {
    static uint8_t stringIproductDescriptor[] = {
        0x16,                                                       //bLength
        STRING_DESCRIPTOR,                                          //bDescriptorType 0x03
        'D',0,'F',0,'U',0,' ',0,'D',0,'E',0,'V',0,'I',0,'C',0,'E',0 //bString iProduct - DFU device
    };
    return stringIproductDescriptor;
}

#define DFU_DESCRIPTOR_LENGTH (9)
#define TOTAL_DESCRIPTOR_LENGTH ((1 * CONFIGURATION_DESCRIPTOR_LENGTH) \
                               + (1 * INTERFACE_DESCRIPTOR_LENGTH) \
                               + (1 * DFU_DESCRIPTOR_LENGTH))

#define DETACH_TIMEOUT 255
#define DFU_TRANSFER_SIZE 1024

uint8_t * WebUSBDFU::configurationDesc() {
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
        DFU_INTERFACE_NUMBER,           // bInterfaceNumber
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

#define NUM_ORIGINS 1
#define TOTAL_ORIGINS_LENGTH (WEBUSB_DESCRIPTOR_SET_LENGTH + \
                              WEBUSB_CONFIGURATION_SUBSET_LENGTH + \
                              WEBUSB_FUNCTION_SUBSET_LENGTH + \
                              NUM_ORIGINS)

uint8_t * WebUSBDFU::allowedOriginsDesc() {
    static uint8_t allowedOriginsDescriptor[] = {
        WEBUSB_DESCRIPTOR_SET_LENGTH,   /* bLength */
        WEBUSB_DESCRIPTOR_SET_HEADER,   /* bDescriptorType */
        LSB(TOTAL_ORIGINS_LENGTH),      /* wTotalLength (LSB) */
        MSB(TOTAL_ORIGINS_LENGTH),      /* wTotalLength (MSB) */
        0x01,                           /* bNumConfigurations */
        
        WEBUSB_CONFIGURATION_SUBSET_LENGTH, /* bLength */
        WEBUSB_CONFIGURATION_SUBSET_HEADER, /* bDescriptorType */
        DEFAULT_CONFIGURATION,          /* bConfigurationValue */
        0x01,                           /* bNumFunctions */
        
        (WEBUSB_FUNCTION_SUBSET_LENGTH+NUM_ORIGINS),/* bLength */
        WEBUSB_FUNCTION_SUBSET_HEADER,  /* bDescriptorType */
        DFU_INTERFACE_NUMBER,           /* bFirstInterfaceNumber */
        URL_OFFSET_ALLOWED_ORIGIN,      /* iOrigin[] */
    };
    
    return allowedOriginsDescriptor;    
}

uint8_t * WebUSBDFU::urlIallowedOrigin() {
    static uint8_t urlIallowedOriginDescriptor[] = {
        0x16,                  /* bLength */
        WEBUSB_URL,            /* bDescriptorType */
        WEBUSB_URL_SCHEME_HTTPS,/* bScheme */
        'd','e','v','a','n','l','a','i','.','g','i','t','h','u','b','.','i','o','/',/* URL - devanlai.github.io */
    };
    return urlIallowedOriginDescriptor;
}

uint8_t * WebUSBDFU::urlIlandingPage() {
    static uint8_t urlIlandingPageDescriptor[] = {
        0x26,                  /* bLength */
        WEBUSB_URL,            /* bDescriptorType */
        WEBUSB_URL_SCHEME_HTTPS,/* bScheme */
        'd','e','v','a','n','l','a','i','.','g','i','t','h','u','b','.','i','o','/',/* URL - devanlai.github.io/webdfu/dfu-util/ */
        'w','e','b','d','f','u','/','d','f','u','-','u','t','i','l','/'
    };
    return urlIlandingPageDescriptor;
}