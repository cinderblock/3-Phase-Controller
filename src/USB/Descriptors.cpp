/**
 * The parts of Descriptors that can be built with C++
 */

#include "Descriptors.h"
#include "idBlock.hpp"

using namespace ThreePhaseControllerNamespace;

/** This function is called by the library when in device mode, and must be overridden (see library "USB Descriptors"
 *  documentation) by the application code so that the address and size of a requested descriptor can be given
 *  to the USB library. When the device receives a Get Descriptor request on the control endpoint, this function
 *  is called so that the descriptor details can be passed back and the appropriate descriptor sent back to the
 *  USB host.
 */
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue, const uint16_t wIndex,
                                    const void **const DescriptorAddress) {
  const uint8_t DescriptorType = (wValue >> 8);
  const uint8_t DescriptorNumber = (wValue & 0xFF);

  const void *Address = NULL;
  uint16_t Size = NO_DESCRIPTOR;

  switch (DescriptorType) {
  case DTYPE_Device:
    Address = &DeviceDescriptor;
    Size = sizeof(USB_Descriptor_Device_t);
    break;
  case DTYPE_Configuration:
    Address = &ConfigurationDescriptor;
    Size = sizeof(USB_Descriptor_Configuration_t);
    break;
  case DTYPE_String:
    switch (DescriptorNumber) {
    case STRING_ID_Language:
      Address = &LanguageString;
      Size = pgm_read_byte(&LanguageString.Header.Size);
      break;
    case STRING_ID_Manufacturer:
      Address = &ManufacturerString;
      Size = pgm_read_byte(&ManufacturerString.Header.Size);
      break;
    case STRING_ID_Product:
      Address = &ProductString;
      Size = pgm_read_byte(&ProductString.Header.Size);
      break;
    case STRING_ID_Serial:
      if (ID::valid) {
        Address = (const void *)(ID::usbSerial::address);
        Size = pgm_read_byte(Address);
      } else {
        Address = &DefaultSerialString;
        Size = pgm_read_byte(&DefaultSerialString.Header.Size);
      }
      break;
    }

    break;
  case HID_DTYPE_HID:
    Address = &ConfigurationDescriptor.HID_GenericHID;
    Size = sizeof(USB_HID_Descriptor_HID_t);
    break;
  case HID_DTYPE_Report:
    Address = &GenericReport;
    Size = sizeof(GenericReport);
    break;
  }

  *DescriptorAddress = Address;
  return Size;
}