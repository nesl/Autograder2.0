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

#ifndef WEB_USB_H
#define WEB_USB_H

/* USB 2.1 Standard descriptor types */
#define BINARY_OBJECT_STORE_DESCRIPTOR (15)
#define DEVICE_CAPABILITY_DESCRIPTOR (16)

/* WebUSB descriptor types */
#define WEBUSB_DESCRIPTOR_SET_HEADER       0
#define WEBUSB_CONFIGURATION_SUBSET_HEADER 1
#define WEBUSB_FUNCTION_SUBSET_HEADER      2
#define WEBUSB_URL                         3

/* WebUSB URL schemes */
#define WEBUSB_URL_SCHEME_HTTP 0
#define WEBUSB_URL_SCHEME_HTTPS 1

/* WebUSB descriptor lengths */
#define BINARY_OBJECT_STORE_DESCRIPTOR_LENGTH 0x05
#define WEBUSB_PLATFORM_DESCRIPTOR_LENGTH  0x18
#define WEBUSB_DESCRIPTOR_SET_LENGTH       5
#define WEBUSB_CONFIGURATION_SUBSET_LENGTH 4
#define WEBUSB_FUNCTION_SUBSET_LENGTH      3

/* WebUSB URL offsets */
#define URL_OFFSET_ALLOWED_ORIGIN          1
#define URL_OFFSET_LANDING_PAGE            2

/* USB Specification Release Number */
#define USB_VERSION_2_1 (0x0210)
#define WEBUSB_VERSION_1_0 (0x0100)
#define USB_VERSION_1_0 (0x0100)

/* bDevCapabilityTypes in device capability descriptors*/
#define USB_DC_WIRELESS_USB                1
#define USB_DC_USB_2_0_EXTENSION           2
#define USB_DC_SUPERSPEED_USB              3
#define USB_DC_CONTAINER_ID                4
#define USB_DC_PLATFORM                    5
#define USB_DC_POWER_DELIVERY_CAPABILITY   6
#define USB_DC_BATTERY_INFO_CAPABILITY     7
#define USB_DC_PD_CONSUMER_PORT_CAPABILITY 8
#define USB_DC_PD_PROVIDER_PORT_CAPABILITY 9
#define USB_DC_SUPERSPEED_PLUS             10
#define USB_DC_PRECISION_TIME_MEASUREMENT  11
#define USB_DC_WIRELESS_USB_EXT            12

/* WebUSB Vendor code */
#define WEBUSB_VENDOR_CODE                 0x57

/* WebUSB requests */
#define WEBUSB_GET_ALLOWED_ORIGINS         0x01
#define WEBUSB_GET_URL                     0x02

#endif