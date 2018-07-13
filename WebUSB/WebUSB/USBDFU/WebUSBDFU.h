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

#ifndef WEB_USB_DFU_H
#define WEB_USB_DFU_H

#include "WebUSBDevice.h"

class WebUSBDFU : public WebUSBDevice {
public:
    /**
    * Constructor
    *
    * @param vendor_id Your vendor_id
    * @param product_id Your product_id
    * @param product_release Your product_release
    * @param connect Connect the device
    */
    WebUSBDFU(uint16_t vendor_id = 0x1234, uint16_t product_id = 0x0006, uint16_t product_release = 0x0001, bool connect = true);

    /**
     *  Attach a callback called when a DFU detach request is received 
     *
     *  @param tptr pointer to the object to call the member function on
     *  @param mptr pointer to the member function to be called
     */
    template<typename T>
    void attach(T* tptr, void (T::*mptr)(void)) {
        if((mptr != NULL) && (tptr != NULL)) {
            detach.attach(tptr, mptr);
        }
    }

    /**
     * Attach a callback called when a DFU detach request is received
     *
     * @param func function pointer
     */
    void attach(Callback<void()> func);

protected:
    /*
    * Get string product descriptor
    *
    * @returns pointer to the string product descriptor
    */
    virtual uint8_t * stringIproductDesc();

    /*
    * Get string interface descriptor
    *
    * @returns pointer to the string interface descriptor
    */
    virtual uint8_t * stringIinterfaceDesc();

    /*
    * Get configuration descriptor
    *
    * @returns pointer to the configuration descriptor
    */
    virtual uint8_t * configurationDesc();

    /*
    * Called by USBDevice on Endpoint0 request. Warning: Called in ISR context
    * This is used to handle extensions to standard requests
    * and class specific requests
    *
    * @returns true if class handles this request
    */
    virtual bool USBCallback_request();

    /*
    * Called by USBDevice layer. Set configuration of the device.
    * For instance, you can add all endpoints that you need on this function.
    *
    * @param configuration Number of the configuration
    * @returns true if class handles this request
    */
    virtual bool USBCallback_setConfiguration(uint8_t configuration);
    
    /*
    * Get the WebUSB allowed origin descriptor
    *
    * @returns pointer to the WebUSB allowed origin descriptor
    */
    virtual uint8_t * allowedOriginsDesc();
    
    /*
    * Get WebUSB landing page URL descriptor
    *
    * @returns pointer to the landing page URL descriptor
    */
    virtual uint8_t * urlIlandingPage();
    
    /*
    * Get WebUSB allowed origin URL descriptor
    *
    * @returns pointer to the allowed origin URL descriptor
    */
    virtual uint8_t * urlIallowedOrigin();
    
private:
    Callback<void()> detach;
    
    static void no_op() {};
};

#endif