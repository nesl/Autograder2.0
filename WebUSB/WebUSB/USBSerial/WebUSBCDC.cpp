/*
* Copyright 2016 Devan Lai
* Modifications copyright 2017 Lars Gunder Knudsen
* Modifications copyright 2018 UCLA NESL Lab
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
*
*/


/*
    USE ENPOINT 5 IN JAVASCRIPT 
*/
#include "WebUSB.h"
#include "WebUSBDevice.h"
#include "WebUSBCDC.h"
#include "stdint.h"
#include "WinUSB.h"
#include "USBDescriptor.h" //Shouldn't need but may need
//CHECK VALUES BELOW

static uint8_t cdc_line_coding[7]= {0x80, 0x25, 0x00, 0x00, 0x00, 0x00, 0x08};

#define DEFAULT_CONFIGURATION (1)

#define CDC_SET_LINE_CODING        0x20
#define CDC_GET_LINE_CODING        0x21
#define CDC_SET_CONTROL_LINE_STATE 0x22

//Constrol Line State bits
#define CLS_DTR   (1 << 0)
#define CLS_RTS   (1 << 1)

#define CDC_INT_INTERFACE_NUMBER  (0)
#define CDC_INTERFACE_NUMBER  (1)
#define WEBUSB_INTERFACE_NUMBER  (2)

#define MAX_CDC_REPORT_SIZE MAX_PACKET_SIZE_EPBULK

// Descriptor defines (in addition to those in USBDescriptor.h)
#define USB_VERSION_1_1     (0x0110)
#define USB_VERSION_1_0     (0x0100)

#define IAD_DESCRIPTOR                                  (0x0b)
#define HEADER_FUNCTIONAL_DESCRIPTOR                    (0x00)
#define CALL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR           (0x01)
#define ACM_FUNCTIONAL_DESCRIPTOR                       (0x02)
#define UNION_FUNCTIONAL_DESCRIPTOR                     (0x06)

#define IAD_DESCRIPTOR_LENGTH                           (8)
#define HEADER_FUNCTIONAL_DESCRIPTOR_LENGTH             (5)
#define CALL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR_LENGTH    (5)
#define ACM_FUNCTIONAL_DESCRIPTOR_LENGTH                (4)
#define UNION_FUNCTIONAL_DESCRIPTOR_LENGTH              (5)

#define IAD_INTERFACE_COUNT                             (2)
#define CS_INTERFACE                                    (0x24)
#define CS_ENDPOINT                                     (0x25)

#define CDC_CLASS                                       (0x02)
#define CDC_CLASS_DATA                                  (0x0A)
#define CUSTOM_CLASS                                    (0xFF)

#define ACM_SUBCLASS                                    (0x02)

#define CDC_INTERFACE_COUNT                             (2)
#define CDC_INT_ENDPOINT_COUNT                          (1)
#define CDC_DATA_ENDPOINT_COUNT                         (2)
#define CDC_ENDPOINT_INT                                (EPINT_IN)
#define CDC_ENDPOINT_IN                                 (EPBULK_IN)
#define CDC_ENDPOINT_OUT                                (EPBULK_OUT)
#define CDC_EPINT_INTERVAL                              (16)

#define WEBUSB_INTERFACE_COUNT                          (1)
#define WEBUSB_ENDPOINT_COUNT                           (2)
#define WEBUSB_ENDPOINT_IN                              (EP5IN)
#define WEBUSB_ENDPOINT_OUT                             (EP5OUT)

#define FULL_CONFIGURATION_SIZE   (CONFIGURATION_DESCRIPTOR_LENGTH + \
    (3 * INTERFACE_DESCRIPTOR_LENGTH) + (5 * ENDPOINT_DESCRIPTOR_LENGTH) + \
    IAD_DESCRIPTOR_LENGTH + HEADER_FUNCTIONAL_DESCRIPTOR_LENGTH + CALL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR_LENGTH + \
    ACM_FUNCTIONAL_DESCRIPTOR_LENGTH + UNION_FUNCTIONAL_DESCRIPTOR_LENGTH)
#define NUM_ORIGINS 1
#define TOTAL_ORIGINS_LENGTH (WEBUSB_DESCRIPTOR_SET_LENGTH + \
                              WEBUSB_CONFIGURATION_SUBSET_LENGTH + \
                              WEBUSB_FUNCTION_SUBSET_LENGTH + \
                              NUM_ORIGINS)

WebUSBCDC::WebUSBCDC(uint16_t vendor_id, uint16_t product_id, uint16_t product_release, bool connect): WebUSBDevice(vendor_id, product_id, product_release)
{
	cdc_connected = false;
	if(connect)
	{
		WebUSBDevice::connect();
	}
}
//------------------------------------------------------
bool WebUSBCDC::USBCallback_request(void) 
{
    /* Called in ISR context */

    bool success = false;
    CONTROL_TRANSFER * transfer = getTransferPtr();

    /* Process class-specific requests */

      if ((transfer->setup.bmRequestType.Type == VENDOR_TYPE) &&
             (transfer->setup.bmRequestType.Recipient == DEVICE_RECIPIENT) &&
             (transfer->setup.bRequest == WINUSB_VENDOR_CODE))
    {
        static uint8_t msos20Descriptor[] = {
            0x0A, 0x00,  // Section size
            0x00, 0x00,  // MS OS 2.0 descriptor set header
            0x00, 0x00, 0x03, 0x06,  // Windows version 8.1 (0x06030000)
            0xB2, 0x00,  // Size, MS OS 2.0 descriptor set (total)

            // Configuration subset header
            0x08, 0x00,  // Section size
            0x01, 0x00,  // DescriptorType
            0x00,        // ConfigurationValue
            0x00,        // Reserved
            0xA8, 0x00,  // TotalLength of this subset header

            // Function subset header
            0x08, 0x00,  // Section size
            0x02, 0x00,  // DescriptorType
            WEBUSB_INTERFACE_NUMBER,  // WebUSB interface number (we don't need one for USBCDC)
            0x00,        // Reserved
            0xA0, 0x00,  // TotalLength of this subset header

            // Compatible ID descriptor
            0x14, 0x00,  // Section size
            0x03, 0x00,  // DescriptorType (MS OS 2.0 compatible) 
            'W','I','N','U','S','B',0,0,    // compatible ID - WINUSB
            0, 0, 0, 0, 0, 0, 0, 0,         // subCompatibleID

            // Extended properties descriptor with interface GUID
            0x84, 0x00,   // Section size
            0x04, 0x00,   // DescriptorType
            0x07, 0x00,   // PropertyDataType
            0x2A, 0x00,   // PropertyNameLength

            // Property name : DeviceInterfaceGUIDs
            'D',0,'e',0,'v',0,'i',0,'c',0,'e',0,'I',0,'n',0,'t',0,'e',0,'r',0,'f',0,'a',0,'c',0,'e',0,'G',0,'U',0,'I',0,'D',0,'s',0,0,0,

            0x50, 0x00,   // wPropertyDataLength

            // Property data: {F7008E18-7F37-4E17-8C1A-D37E18C066E4} - generated with https://www.guidgenerator.com/
            '{',0,'F',0,'7',0,'0',0,'0',0,'8',0,'E',0,'1',0,'8',0,'-',0,'7',0,'F',0,'3',0,'7',0,'-',0,'4',0,
            'E',0,'1',0,'7',0,'-',0,'8',0,'C',0,'1',0,'A',0,'-',0,'D',0,'3',0,'7',0,'E',0,'1',0,'8',0,'C',0,
            '0',0,'6',0,'6',0,'E',0,'4',0,'}',0,0,0,0,0,0
        };


        transfer->remaining = sizeof(msos20Descriptor);
        transfer->ptr = msos20Descriptor;
        transfer->direction = DEVICE_TO_HOST;
        success = true;
    }

    // Process CDC class-specific requests
    if (transfer->setup.bmRequestType.Type == CLASS_TYPE) {
        switch (transfer->setup.bRequest) {
            case CDC_GET_LINE_CODING:
                transfer->remaining = 7;
                transfer->ptr = cdc_line_coding;
                transfer->direction = DEVICE_TO_HOST;
                success = true;
                break;
            case CDC_SET_LINE_CODING:
                transfer->remaining = 7;
                transfer->notify = true;
                success = true;
                break;
            case CDC_SET_CONTROL_LINE_STATE:
                // we should handle this specifically for the CDC endpoint.
                if (transfer->setup.wValue & CLS_DTR) {
                    cdc_connected = true;
                } else {
                    cdc_connected = false;
                }
                success = true;
                break;
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
//------------------------------------------------------
bool WebUSBCDC::USBCallback_setConfiguration(uint8_t configuration)
{
	if (configuration != DEFAULT_CONFIGURATION) 
	{
        return false;
    }

    addEndpoint(EPINT_IN, MAX_PACKET_SIZE_EPINT);

    addEndpoint(EPBULK_IN, MAX_PACKET_SIZE_EPBULK);
    addEndpoint(EPBULK_OUT, MAX_PACKET_SIZE_EPBULK);

    addEndpoint(WEBUSB_ENDPOINT_IN, MAX_PACKET_SIZE_EPBULK);
    addEndpoint(WEBUSB_ENDPOINT_OUT, MAX_PACKET_SIZE_EPBULK);

    // We activate the endpoints to be able to recceive data
    readStart(EPBULK_OUT, MAX_PACKET_SIZE_EPBULK);
    readStart(WEBUSB_ENDPOINT_OUT, MAX_PACKET_SIZE_EPBULK);
    return true;
}
//------------------------------------------------------
bool WebUSBCDC::write(uint8_t * buffer, uint32_t size, bool isCDC)
{
	if(isCDC && !cdc_connected)
        return false;

    return USBDevice::write(isCDC ? CDC_ENDPOINT_IN : WEBUSB_ENDPOINT_IN, buffer, size, MAX_CDC_REPORT_SIZE);
}
//------------------------------------------------------
bool WebUSBCDC::read(uint8_t * buffer, uint32_t * size, bool isCDC, bool blocking)
{
	if(isCDC && !cdc_connected)
        return false;
    if (blocking && !USBDevice::readEP(isCDC ? CDC_ENDPOINT_OUT : WEBUSB_ENDPOINT_OUT, buffer, size, MAX_CDC_REPORT_SIZE))
        return false;
    if (!blocking && !USBDevice::readEP_NB(isCDC ? CDC_ENDPOINT_OUT : WEBUSB_ENDPOINT_OUT, buffer, size, MAX_CDC_REPORT_SIZE))
        return false;
    if (!readStart(isCDC ? CDC_ENDPOINT_OUT : WEBUSB_ENDPOINT_OUT, MAX_CDC_REPORT_SIZE))
        return false;
    return true;
}
//------------------------------------------------------
uint8_t * WebUSBCDC::allowedOriginsDesc()
{
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
        WEBUSB_INTERFACE_NUMBER,        /* bFirstInterfaceNumber */
        URL_OFFSET_ALLOWED_ORIGIN,      /* iOrigin[] */
    };

    return allowedOriginsDescriptor;
} 
//------------------------------------------------------
uint8_t * WebUSBCDC::configurationDesc() //NEED TO CHECK DATA IN HERE
{
	static uint8_t configDescriptor[] = {
        // configuration descriptor
        CONFIGURATION_DESCRIPTOR_LENGTH,
        CONFIGURATION_DESCRIPTOR,
        LSB(FULL_CONFIGURATION_SIZE),
        MSB(FULL_CONFIGURATION_SIZE),
        CDC_INTERFACE_COUNT+WEBUSB_INTERFACE_COUNT,
        0x01,
        0x00,
        C_RESERVED,
        C_POWER(100),

        // IAD to associate the two CDC interfaces (this seems to be needed by Windows)
        IAD_DESCRIPTOR_LENGTH,
        IAD_DESCRIPTOR,
        CDC_INT_INTERFACE_NUMBER,
        IAD_INTERFACE_COUNT,
        CDC_CLASS,
        ACM_SUBCLASS,
        0,
        0,

        // CDC BLOCK STARTS

        // CDC INTERRUPT INTERFACE
        INTERFACE_DESCRIPTOR_LENGTH,
        INTERFACE_DESCRIPTOR,
        CDC_INT_INTERFACE_NUMBER,
        0x00,
        CDC_INT_ENDPOINT_COUNT,
        CDC_CLASS,
        ACM_SUBCLASS,
        0x01,
        0x00,

        // CDC Header Functional Descriptor, CDC Spec 5.2.3.1, Table 26
        HEADER_FUNCTIONAL_DESCRIPTOR_LENGTH,
        CS_INTERFACE,
        HEADER_FUNCTIONAL_DESCRIPTOR,
        LSB(USB_VERSION_1_0),
        MSB(USB_VERSION_1_0),

        // CDC Call Management Functional Descriptor,
        CALL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR_LENGTH,
        CS_INTERFACE,
        CALL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR,
        0x03,
        CDC_INTERFACE_NUMBER,

        // CDC Abstract Control Management Functional Descriptor, CDC Spec 5.2.3.3, Table 28
        ACM_FUNCTIONAL_DESCRIPTOR_LENGTH,
        CS_INTERFACE,
        ACM_FUNCTIONAL_DESCRIPTOR,
        0x02,

        // CDC Union Functional Descriptor, CDC Spec 5.2.3.8, Table 33
        UNION_FUNCTIONAL_DESCRIPTOR_LENGTH,
        CS_INTERFACE,
        UNION_FUNCTIONAL_DESCRIPTOR,
        CDC_INT_INTERFACE_NUMBER,
        CDC_INTERFACE_NUMBER,

        // CDC INT EP
        ENDPOINT_DESCRIPTOR_LENGTH,
        ENDPOINT_DESCRIPTOR,
        PHY_TO_DESC(EPINT_IN),
        E_INTERRUPT,
        LSB(MAX_PACKET_SIZE_EPINT),
        MSB(MAX_PACKET_SIZE_EPINT),
        CDC_EPINT_INTERVAL,

        // CDC DATA INTERFACE
        INTERFACE_DESCRIPTOR_LENGTH,
        INTERFACE_DESCRIPTOR,
        CDC_INTERFACE_NUMBER,
        0x00,
        CDC_DATA_ENDPOINT_COUNT,
        CDC_CLASS_DATA,
        0x00,
        0x00,
        0x00,

        // CDC DATA ENDPOINT IN
        ENDPOINT_DESCRIPTOR_LENGTH,
        ENDPOINT_DESCRIPTOR,
        PHY_TO_DESC(EPBULK_IN),
        E_BULK,
        LSB(MAX_PACKET_SIZE_EPBULK),
        MSB(MAX_PACKET_SIZE_EPBULK),
        0x00,

        // CDC DATA ENDPOINT OUT
        ENDPOINT_DESCRIPTOR_LENGTH,
        ENDPOINT_DESCRIPTOR,
        PHY_TO_DESC(EPBULK_OUT),
        E_BULK,
        LSB(MAX_PACKET_SIZE_EPBULK),
        MSB(MAX_PACKET_SIZE_EPBULK),
        0x00,


        // WEBUSB BLOCK

        // WEBUSB INTERFACE
        INTERFACE_DESCRIPTOR_LENGTH,
        INTERFACE_DESCRIPTOR,
        WEBUSB_INTERFACE_NUMBER,
        0x00,
        WEBUSB_ENDPOINT_COUNT,
        CUSTOM_CLASS,
        0x00,
        0x00,
        0x00,

        // WEBUSB ENDPOINT IN
        ENDPOINT_DESCRIPTOR_LENGTH,
        ENDPOINT_DESCRIPTOR,
        PHY_TO_DESC(WEBUSB_ENDPOINT_IN),
        E_BULK,
        LSB(MAX_PACKET_SIZE_EPBULK),
        MSB(MAX_PACKET_SIZE_EPBULK),
        0x00,

        // WEBUSB ENDPOINT OUT
        ENDPOINT_DESCRIPTOR_LENGTH,
        ENDPOINT_DESCRIPTOR,
        PHY_TO_DESC(WEBUSB_ENDPOINT_OUT),
        E_BULK,
        LSB(MAX_PACKET_SIZE_EPBULK),
        MSB(MAX_PACKET_SIZE_EPBULK),
        0x00,

    };
    return configDescriptor;
}
//------------------------------------------------------
uint8_t * WebUSBCDC::stringIproductDesc() //NOT IMPORTANT FOR FUNCTIONALITY
{
	static uint8_t stringIproductDescriptor[] = {
        0x12,
        STRING_DESCRIPTOR,
        'm',0,'b',0,'e',0,'d',0,'1',0,'7',0,'6',0,'8',0,
    };
    return stringIproductDescriptor;
}
//------------------------------------------------------
uint8_t * WebUSBCDC::stringIinterfaceDesc() //NOT IMPORTANT FOR FUNCTIONALITY
{
	static uint8_t stringIinterfaceDescriptor[] = {
        0x08,
        STRING_DESCRIPTOR,
        'C',0,'D',0,'C',0,
    };
    return stringIinterfaceDescriptor;
}
//------------------------------------------------------
uint8_t * WebUSBCDC::stringImanufacturerDesc() //NOT IMPORTANT FOR FUNCTIONALITY
{
	static uint8_t stringImanufacturerDescriptor[] = {
        0x0E,                                            /*bLength*/
        STRING_DESCRIPTOR,                               /*bDescriptorType 0x03*/
        'm',0,'b',0,'e',0,'d',0,'O',0,'S',0,
    };
    return stringImanufacturerDescriptor;
}
//------------------------------------------------------
uint8_t * WebUSBCDC::stringIserialDesc() //NOT IMPORTANT FOR FUNCTIONALITY
{
	static uint8_t stringIserialDescriptor[] = {
        0x0C,                                             /*bLength*/
        STRING_DESCRIPTOR,                                /*bDescriptorType 0x03*/
        '0',0,'0',0,'0',0,'0',0,'1',0,                    /*bString iSerial - 00001*/
    };
    return stringIserialDescriptor;
}
//------------------------------------------------------
uint8_t * WebUSBCDC::urlIlandingPage() //URL
{
	static uint8_t urlIlandingPageDescriptor[] = {
        0x11,                  /* bLength */
        WEBUSB_URL,            /* bDescriptorType */
        WEBUSB_URL_SCHEME_HTTP,/* bScheme */
        'l','o','c','a','l','h','o','s','t',':','8','0','0','0', /* URL - localhost:8000 */
    };
    return urlIlandingPageDescriptor;
}
//------------------------------------------------------
uint8_t * WebUSBCDC::urlIallowedOrigin() //URL
{
	static uint8_t urlIallowedOriginDescriptor[] = {
        0x11,                  /* bLength */
        WEBUSB_URL,            /* bDescriptorType */
        WEBUSB_URL_SCHEME_HTTP,/* bScheme */
        'l','o','c','a','l','h','o','s','t',':','8','0','0','0', /* URL - localhost:8000 */
        //'l','a','r','s','g','k','.','g','i','t','h','u','b','.','i','o','/','w','e','b','u','s','b','-','t','e','s','t','e','r','/' //usb test website
    };
    return urlIallowedOriginDescriptor;
}

