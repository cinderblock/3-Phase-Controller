/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   USBInterface.h
 * Author: Cameron
 *
 * Created on April 18, 2018, 12:10 PM
 */

#ifndef USBINTERFACE_H
#define USBINTERFACE_H

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <string.h>

#include "Descriptors.h"

#include <LUFA/Drivers/Board/LEDs.h>
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>

namespace ThreePhaseControllerNamespace {

class USBInterface {

private:
  void EVENT_USB_Device_Connect(void);
  void EVENT_USB_Device_Disconnect(void);
  void EVENT_USB_Device_ConfigurationChanged(void);
  void EVENT_USB_Device_ControlRequest(void);
  void EVENT_USB_Device_StartOfFrame(void);

  bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t *const HIDInterfaceInfo, uint8_t *const ReportID,
                                           const uint8_t ReportType, void *ReportData, uint16_t *const ReportSize);
  void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t *const HIDInterfaceInfo, const uint8_t ReportID,
                                            const uint8_t ReportType, const void *ReportData,
                                            const uint16_t ReportSize);
};

} // namespace ThreePhaseControllerNamespace

#endif /* USBINTERFACE_H */
