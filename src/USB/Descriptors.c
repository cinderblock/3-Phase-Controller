/*
             LUFA Library
     Copyright (C) Dean Camera, 2018.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2018  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  USB Device Descriptors, for library use when in USB device mode. Descriptors are special
 *  computer-readable structures which the host requests upon device enumeration, to determine
 *  the device's capabilities and functions.
 */

#include "Descriptors.h"
#include "Config.h"
#include "LUFAConfig.h"

#include <avr/pgmspace.h>

/** Enum for the device interface descriptor IDs within the device. Each interface descriptor
 *  should have a unique ID index associated with it, which can be used to refer to the
 *  interface from other descriptors.
 */
enum InterfaceDescriptors_t {
  INTERFACE_ID_GenericHID = 0, /**< GenericHID interface descriptor ID */
};

/** Endpoint address of the Generic HID reporting IN endpoint. */
#define GENERIC_IN_EPADDR (ENDPOINT_DIR_IN | 1)

/** Device descriptor structure. This descriptor, located in FLASH memory, describes the overall
 *  device characteristics, including the supported USB version, control endpoint size and the
 *  number of device configurations. The descriptor is read out by the USB host when the enumeration
 *  process begins.
 */
const USB_Descriptor_Device_t PROGMEM DeviceDescriptor = {
    .Header = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

    .USBSpecification = VERSION_BCD(1, 1, 0),
    .Class = USB_CSCP_NoDeviceClass,
    .SubClass = USB_CSCP_NoDeviceSubclass,
    .Protocol = USB_CSCP_NoDeviceProtocol,

    .Endpoint0Size = REPORT_SIZE, // sizeof(USBDataOUTShape),

    .VendorID = 0xDEAD,
    .ProductID = 0xBEEF,
    .ReleaseNumber = VERSION_BCD(0, 0, 1),

    .ManufacturerStrIndex = STRING_ID_Manufacturer,
    .ProductStrIndex = STRING_ID_Product,
    .SerialNumStrIndex = STRING_ID_Serial,

    .NumberOfConfigurations = 1,
};

/** Configuration descriptor structure. This descriptor, located in FLASH memory, describes the usage
 *  of the device in one of its supported configurations, including information about any device interfaces
 *  and endpoints. The descriptor is read out by the USB host during the enumeration process when selecting
 *  a configuration so that the host may correctly communicate with the USB device.
 */
const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor = {
    .Config =
        {
            .Header = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

            .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
            .TotalInterfaces = 1,

            .ConfigurationNumber = 1,
            .ConfigurationStrIndex = NO_DESCRIPTOR,

            .ConfigAttributes = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),

            .MaxPowerConsumption = USB_CONFIG_POWER_MA(100),
        },

    .HID_Interface =
        {
            .Header = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

            .InterfaceNumber = INTERFACE_ID_GenericHID,
            .AlternateSetting = 0x00,

            .TotalEndpoints = 1,

            .Class = HID_CSCP_HIDClass,
            .SubClass = HID_CSCP_NonBootSubclass,
            .Protocol = HID_CSCP_NonBootProtocol,

            .InterfaceStrIndex = NO_DESCRIPTOR,
        },

    .HID_GenericHID =
        {
            .Header = {.Size = sizeof(USB_HID_Descriptor_HID_t), .Type = HID_DTYPE_HID},

            .HIDSpec = VERSION_BCD(1, 1, 1),
            .CountryCode = 0x00,
            .TotalReportDescriptors = 1,
            .HIDReportType = HID_DTYPE_Report,
            .HIDReportLength = sizeof(GenericReport),
        },

    .HID_ReportINEndpoint =
        {
            .Header = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

            .EndpointAddress = GENERIC_IN_EPADDR,
            .Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
            .EndpointSize = REPORT_SIZE, // sizeof(USBDataINShape),
            .PollingIntervalMS = 0x05,
        },
};

/** Language descriptor structure. This descriptor, located in FLASH memory, is returned when the host requests
 *  the string descriptor with index 0 (the first index). It is actually an array of 16-bit integers, which indicate
 *  via the language ID table available at USB.org what languages the device supports for its string descriptors.
 */
const USB_Descriptor_String_t PROGMEM LanguageString = USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);

/** Manufacturer descriptor string. This is a Unicode string containing the manufacturer's details in human readable
 *  form, and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t PROGMEM ManufacturerString = USB_STRING_DESCRIPTOR(L"LastMileRobotics");

/** Product descriptor string. This is a Unicode string containing the product's details in human readable form,
 *  and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t PROGMEM ProductString = USB_STRING_DESCRIPTOR(L"LMR Quantum Driver");

const USB_Descriptor_String_t PROGMEM DefaultSerialString = USB_STRING_DESCRIPTOR(L"None");

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_HID_Device_t Generic_HID_Interface = {
    .Config =
        {
            .InterfaceNumber = INTERFACE_ID_GenericHID,
            .ReportINEndpoint =
                {
                    .Address = GENERIC_IN_EPADDR,
                    .Size = REPORT_SIZE, // sizeof(USBDataINShape),
                    .Banks = 1,
                },
            .PrevReportINBuffer = NULL,
            .PrevReportINBufferSize = REPORT_SIZE, // sizeof(USBDataINShape),
        },
};
