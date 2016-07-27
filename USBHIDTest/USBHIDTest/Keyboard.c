/*
             LUFA Library
     Copyright (C) Dean Camera, 2015.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2015  Dean Camera (dean [at] fourwalledcubicle [dot] com)

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
 *  Main source file for the Keyboard demo. This file contains the main tasks of
 *  the demo and is responsible for the initial application hardware configuration.
 */
 //----------------------------------------------
 //------------------2016------------------------
 //----------------------------------------------
 // Project:	USBHIDTest 
 // Author:		Nikola Vitanovic
 // Email:		nikola@vitanovic.net
 // Descr:		Modified Keyboard.c file added
 //				support for loading semi
 //				rubber ducky scripts
 // Tutorial:	
 //				All you have to do is to replace
 //				The Payload.h file in the project
 //				with your own that was generated
 //				with USBPayloadGenerator and 
 //				flash your AT90USB162 DFU.
 //----------------------------------------------
 //----------------------------------------------
 //----------------------------------------------
#include "Keyboard.h"
#include "Payload.h"
/** Buffer to hold the previously generated Keyboard HID report, for comparison purposes inside the HID class driver. */
static uint8_t PrevKeyboardHIDReportBuffer[sizeof(USB_KeyboardReport_Data_t)];

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_HID_Device_t Keyboard_HID_Interface =
	{
		.Config =
			{
				.InterfaceNumber              = 0,
				.ReportINEndpoint             =
					{
						.Address              = (ENDPOINT_DIR_IN | 1),
						.Size                 = 8, //KEYBOARD_EPSIZE,
						.Banks                = 1,
					},
				.PrevReportINBuffer           = PrevKeyboardHIDReportBuffer,
				.PrevReportINBufferSize       = sizeof(PrevKeyboardHIDReportBuffer),
			},
	};


/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
 int ran = 0;

int main(void)
{
	InitPayload();
	SetupHardware();

	//LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
	GlobalInterruptEnable();

	for (;;)
	{
		HID_Device_USBTask(&Keyboard_HID_Interface);
		USB_USBTask();
	}
}
//void CreatePacket(USB_KeyboardReport_Data_t* d)

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware()
{
 DDRB = 0xFF;
 DDRD = 0xFF;
 PORTD = 0xFF;
  PORTB = 0xFF;
#if (ARCH == ARCH_AVR8)
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	//clock_prescale_set(clock_div_1);
	CLKPR = (1 << CLKPCE);
	CLKPR = (0 << CLKPS3) | (0 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0);
#elif (ARCH == ARCH_XMEGA)
	/* Start the PLL to multiply the 2MHz RC oscillator to 32MHz and switch the CPU core to run from it */
	//XMEGACLK_StartPLL(CLOCK_SRC_INT_RC2MHZ, 2000000, F_CPU);
	//XMEGACLK_SetCPUClockSource(CLOCK_SRC_PLL);

	/* Start the 32MHz internal RC oscillator and start the DFLL to increase it to 48MHz using the USB SOF as a reference */
	//XMEGACLK_StartInternalOscillator(CLOCK_SRC_INT_RC32MHZ);
	//XMEGACLK_StartDFLL(CLOCK_SRC_INT_RC32MHZ, DFLL_REF_INT_USBSOF, F_USB);

	//PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
#endif

	/* Hardware Initialization */
	//Joystick_Init();
	//LEDs_Init();
	//Buttons_Init();
	USB_Init();
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	//LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
	 PORTD = 0xFF;
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	//LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= HID_Device_ConfigureEndpoints(&Keyboard_HID_Interface);

	USB_Device_EnableSOFEvents();
	if(ConfigSuccess)
		 PORTD = 0x00;
	//LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	HID_Device_ProcessControlRequest(&Keyboard_HID_Interface);
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
	HID_Device_MillisecondElapsed(&Keyboard_HID_Interface);
}

/** HID class driver callback function for the creation of HID reports to the host.
 *
 *  \param[in]     HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in,out] ReportID    Report ID requested by the host if non-zero, otherwise callback should set to the generated report ID
 *  \param[in]     ReportType  Type of the report to create, either HID_REPORT_ITEM_In or HID_REPORT_ITEM_Feature
 *  \param[out]    ReportData  Pointer to a buffer where the created report should be stored
 *  \param[out]    ReportSize  Number of bytes written in the report (or zero if no report is to be sent)
 *
 *  \return Boolean \c true to force the sending of the report, \c false to let the library determine if it needs to be sent
 */
bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                         uint8_t* const ReportID,
                                         const uint8_t ReportType,
                                         void* ReportData,
                                         uint16_t* const ReportSize)
{
	USB_KeyboardReport_Data_t* KeyboardReport = (USB_KeyboardReport_Data_t*)ReportData;
	uint8_t UsedKeyCodes = 0;
	if(ran < max)
	{
	/*
		if(ran == 0)
		{
		//KeyboardReport->Modifier
			//KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_LEFT_GUI;
			
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_LEFT_GUI;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_R;
		}
		else if(ran == 1)
		{
			_delay_ms(100);
			(*ReportSize) = 0;
			//KeyboardReport->Modifier = 0;
			ran++;
			return false;
		}
		else
		{
			if(payload[ran] == 0)
			{
				(*ReportSize) = 0;
				_delay_ms(100);
				ran++;
				return false;
			}
			else
				KeyboardReport->KeyCode[UsedKeyCodes++] = payload[ran];
		}*/
		if(payload[ran] == 0)
		{
			_delay_ms(100);
			//(*ReportSize) = 0;
			ran++;
			//return false;
		}
		else if(payload[ran] == HID_KEYBOARD_SC_LEFT_GUI)
		{
			_delay_ms(5);
			KeyboardReport->KeyCode[UsedKeyCodes++] = payload[ran++];
			_delay_ms(5);
			KeyboardReport->KeyCode[UsedKeyCodes++] = payload[ran];
		}
		else
		{
			_delay_ms(5);
			KeyboardReport->KeyCode[UsedKeyCodes++] = payload[ran];
		}
		ran++;
	}
	//*ReportSize = 0;
	/*
	KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_A;
	_delay_ms(500);
	KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_B;
	_delay_ms(300);
	KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_C;*/
	//PORTD = 0xFF;
	//PORTB = 0xFF;
	/*
	if(ran == 0)
	{
		KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_LEFT_GUI;
		_delay_ms(500);
		//_delay_ms(1000);
		
		//_delay_ms(500);
		
		//KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_C;
		//KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_M;
		//KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_D;
		//_delay_ms(100);
		ran++;
	}
	else if(ran == 1)
	{
		KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_R;
		_delay_ms(500);
		ran++;
	}
	else if(ran == 2)
	{
		KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_C;
		KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_M;
		KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_D;
		ran++;
	}
	else if(ran == 3)
	{
		_delay_ms(1000);
		ran++;
	}
	else if(ran == 4)
	{
		KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_ENTER;
		ran++;
	}
	else
	{
		PORTD = 0x00;
		PORTB = 0x00;
	}*/
	
	/*
	uint8_t JoyStatus_LCL    = Joystick_GetStatus();
	uint8_t ButtonStatus_LCL = Buttons_GetStatus();

	

	if (JoyStatus_LCL & JOY_UP)
	  KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_A;
	else if (JoyStatus_LCL & JOY_DOWN)
	  KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_B;

	if (JoyStatus_LCL & JOY_LEFT)
	  KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_C;
	else if (JoyStatus_LCL & JOY_RIGHT)
	  KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_D;

	if (JoyStatus_LCL & JOY_PRESS)
	  KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_E;

	if (ButtonStatus_LCL & BUTTONS_BUTTON1)
	  KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_F;
	  */
	/*if (UsedKeyCodes)
	  KeyboardReport->Modifier = HID_KEYBOARD_MODIFIER_LEFTSHIFT;*/

	
	//PORTD = 0xFF;
	//PORTB = 0xFF;
	*ReportSize = sizeof(USB_KeyboardReport_Data_t);
	return true;
}

/** HID class driver callback function for the processing of HID reports from the host.
 *
 *  \param[in] HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in] ReportID    Report ID of the received report from the host
 *  \param[in] ReportType  The type of report that the host has sent, either HID_REPORT_ITEM_Out or HID_REPORT_ITEM_Feature
 *  \param[in] ReportData  Pointer to a buffer where the received report has been stored
 *  \param[in] ReportSize  Size in bytes of the received HID report
 */
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize)
{
	 PORTD = 0x00;
	 PORTB = 0x00;
	/*uint8_t  LEDMask   = LEDS_NO_LEDS;
	uint8_t* LEDReport = (uint8_t*)ReportData;

	if (*LEDReport & HID_KEYBOARD_LED_NUMLOCK)
	  LEDMask |= LEDS_LED1;

	if (*LEDReport & HID_KEYBOARD_LED_CAPSLOCK)
	  LEDMask |= LEDS_LED3;

	if (*LEDReport & HID_KEYBOARD_LED_SCROLLLOCK)
	  LEDMask |= LEDS_LED4;

	LEDs_SetAllLEDs(LEDMask);*/
}

