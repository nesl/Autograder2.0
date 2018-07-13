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

#ifndef WEBUSB_DEVICE_H
#define WEBUSB_DEVICE_H

#include "USBDevice.h"

class WebUSBDevice: public USBDevice
{
public:
    WebUSBDevice(uint16_t vendor_id, uint16_t product_id, uint16_t product_release);

    /*
    * Called by USBDevice on Endpoint0 request. Warning: Called in ISR context
    * This is used to handle extensions to standard requests
    * and class specific requests
    *
    * @returns true if class handles this request
    */
    virtual bool USBCallback_request();

    /*
    * Get device descriptor. Warning: this method has to store the length of the report descriptor in reportLength.
    *
    * @returns pointer to the device descriptor
    */
    virtual uint8_t * deviceDesc();

    /*
    * Get binary object store descriptor
    *
    * @returns pointer to the binary object store descriptor
    */
    virtual uint8_t * binaryObjectStoreDesc();
    
    /*
    * Get the WebUSB allowed origin descriptor
    *
    * @returns pointer to the WebUSB allowed origin descriptor
    */
    virtual uint8_t * allowedOriginsDesc() = 0;
    
    /*
    * Get WebUSB landing page URL descriptor
    *
    * @returns pointer to the landing page URL descriptor
    */
    virtual uint8_t * urlIlandingPage() = 0;
    
    /*
    * Get WebUSB allowed origin URL descriptor
    *
    * @returns pointer to the allowed origin URL descriptor
    */
    virtual uint8_t * urlIallowedOrigin() = 0;

protected:
    virtual bool requestGetDescriptor(void);
    virtual bool requestWebUSB(void);

private:

};

#endif
