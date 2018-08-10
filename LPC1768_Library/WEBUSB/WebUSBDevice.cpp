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

#include "USBEndpoints.h"
#include "USBDevice.h"
#include "USBDescriptor.h"
#include "WebUSB.h"
#include "WebUSBDevice.h"

WebUSBDevice::WebUSBDevice(uint16_t vendor_id, uint16_t product_id, uint16_t product_release)
    : USBDevice(vendor_id, product_id, product_release)
{

};

bool WebUSBDevice::requestGetDescriptor(void)
{
    bool success = false;
    CONTROL_TRANSFER * transfer = getTransferPtr();
    switch (DESCRIPTOR_TYPE(transfer->setup.wValue))
    {
        case BINARY_OBJECT_STORE_DESCRIPTOR:
            if (binaryObjectStoreDesc() != NULL)
            {
                transfer->remaining = (binaryObjectStoreDesc()[2]
                                    | (binaryObjectStoreDesc()[3] << 8));
                transfer->ptr = binaryObjectStoreDesc();
                transfer->direction = DEVICE_TO_HOST;
                success = true;
            }
            break;
        default:
            success = USBDevice::requestGetDescriptor();
            break;
    }

    return success;
}

bool WebUSBDevice::requestWebUSB(void)
{
    bool success = false;

    CONTROL_TRANSFER * transfer = getTransferPtr();
    switch (transfer->setup.wIndex)
    {
        case WEBUSB_GET_ALLOWED_ORIGINS:
            if (allowedOriginsDesc())
            {
                transfer->remaining = (allowedOriginsDesc()[2]
                                   |  (allowedOriginsDesc()[3] << 8));
                transfer->ptr = allowedOriginsDesc();
                transfer->direction = DEVICE_TO_HOST;
                success = true;
            }
            break;
        case WEBUSB_GET_URL:
            if (transfer->setup.wValue == URL_OFFSET_LANDING_PAGE)
            {
                transfer->remaining = urlIlandingPage()[0];
                transfer->ptr = urlIlandingPage();
                transfer->direction = DEVICE_TO_HOST;
                success = true;
            }
            else if (transfer->setup.wValue == URL_OFFSET_ALLOWED_ORIGIN)
            {
                transfer->remaining = urlIallowedOrigin()[0];
                transfer->ptr = urlIallowedOrigin();
                transfer->direction = DEVICE_TO_HOST;
                success = true;
            }
            break;
        default:
            break;
    }

    return success;
}

bool WebUSBDevice::USBCallback_request()
{
    bool success = false;
    /* Process WebUSB requests */
    CONTROL_TRANSFER * transfer = getTransferPtr();
    if ((transfer->setup.bmRequestType.Type == VENDOR_TYPE) &&
        (transfer->setup.bRequest == WEBUSB_VENDOR_CODE))
    {
        success = requestWebUSB();
    }
    
    return success;
}

uint8_t * WebUSBDevice::deviceDesc() {
    static uint8_t deviceDescriptor[] = {
        DEVICE_DESCRIPTOR_LENGTH,       /* bLength */
        DEVICE_DESCRIPTOR,              /* bDescriptorType */
        LSB(USB_VERSION_1_0),           /* bcdUSB (LSB) */
        MSB(USB_VERSION_1_0),           /* bcdUSB (MSB) */
        0x00,                           /* bDeviceClass */
        0x00,                           /* bDeviceSubClass */
        0x00,                           /* bDeviceprotocol */
        MAX_PACKET_SIZE_EP0,            /* bMaxPacketSize0 */
        (uint8_t)(LSB(VENDOR_ID)),                 /* idVendor (LSB) */
        (uint8_t)(MSB(VENDOR_ID)),                 /* idVendor (MSB) */
        (uint8_t)(LSB(PRODUCT_ID)),                /* idProduct (LSB) */
        (uint8_t)(MSB(PRODUCT_ID)),                /* idProduct (MSB) */
        (uint8_t)(LSB(PRODUCT_RELEASE)),           /* bcdDevice (LSB) */
        (uint8_t)(MSB(PRODUCT_RELEASE)),           /* bcdDevice (MSB) */
        STRING_OFFSET_IMANUFACTURER,    /* iManufacturer */
        STRING_OFFSET_IPRODUCT,         /* iProduct */
        STRING_OFFSET_ISERIAL,          /* iSerialNumber */
        0x01                            /* bNumConfigurations */
    };
    return deviceDescriptor;
}

#define WEBUSB_BOS_TOTAL_LENGTH (BINARY_OBJECT_STORE_DESCRIPTOR_LENGTH \
                                 + WEBUSB_PLATFORM_DESCRIPTOR_LENGTH)

uint8_t * WebUSBDevice::binaryObjectStoreDesc() {
    static uint8_t binaryObjectStoreDescriptor[] = {
        BINARY_OBJECT_STORE_DESCRIPTOR_LENGTH, /* bLength index 0 */
        BINARY_OBJECT_STORE_DESCRIPTOR, /* bDescriptorType index 1*/
        LSB(WEBUSB_BOS_TOTAL_LENGTH),   /* wTotalLength (LSB) index 2*/
        MSB(WEBUSB_BOS_TOTAL_LENGTH),   /* wTotalLength (MSB) index 3*/
        0x01,                           /* bNumDeviceCaps index 4*/
        WEBUSB_PLATFORM_DESCRIPTOR_LENGTH, /* bLength index 5*/
        DEVICE_CAPABILITY_DESCRIPTOR,   /* bDescriptorType index 6*/
        USB_DC_PLATFORM,                /* bDevCapabilityType index 7*/
        0x00,                           /* bReserved index 8*/
        0x34, 0x08, 0xB6, 0x38,         /* PlatformCapabilityUUID index 9,10,11,12*/
        0x09, 0xA9, 0x47, 0xA0,         /* index 13,14,15,16*/
        0x8B, 0xFD, 0xA0, 0x76,         /* index 17,18,19,20*/
        0x88, 0x15, 0xB6, 0x65,         /* index 21,22,23,24*/
        LSB(WEBUSB_VERSION_1_0),        /* bcdVersion (LSB) index 25*/
        MSB(WEBUSB_VERSION_1_0),        /* bcdVersion (MSB) index 26*/
        WEBUSB_VENDOR_CODE,             /* bVendorCode index 27*/
        URL_OFFSET_LANDING_PAGE,        /* iLandingPage index 28*/
    };
    return binaryObjectStoreDescriptor;
}
