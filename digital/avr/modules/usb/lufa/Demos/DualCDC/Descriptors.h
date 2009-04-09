/*
             LUFA Library
     Copyright (C) Dean Camera, 2009.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2009  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
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
 *  Header file for Descriptors.c.
 */
 
#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

	/* Includes: */
		#include <LUFA/Drivers/USB/USB.h>

		#include <avr/pgmspace.h>

	/* Macros: */
		/** Macro to define a CDC class-specific functional descriptor. CDC functional descriptors have a
		 *  uniform structure but variable sized data payloads, thus cannot be represented accurately by
		 *  a single typedef struct. A macro is used instead so that functional descriptors can be created
		 *  easily by specifying the size of the payload. This allows sizeof() to work correctly.
		 *
		 *  \param DataSize  Size in bytes of the CDC functional descriptor's data payload
		 */
		#define CDC_FUNCTIONAL_DESCRIPTOR(DataSize)        \
		     struct                                        \
		     {                                             \
		          USB_Descriptor_Header_t Header;          \
			      uint8_t                 SubType;         \
		          uint8_t                 Data[DataSize];  \
		     }

		/** Endpoint number of the first CDC interface's device-to-host notification IN endpoint. */
		#define CDC1_NOTIFICATION_EPNUM        3

		/** Endpoint number of the first CDC interface's device-to-host data IN endpoint. */
		#define CDC1_TX_EPNUM                  1	

		/** Endpoint number of the first CDC interface's host-to-device data OUT endpoint. */
		#define CDC1_RX_EPNUM                  2	

		/** Endpoint number of the second CDC interface's device-to-host notification IN endpoint. */
		#define CDC2_NOTIFICATION_EPNUM        4

		/** Endpoint number of the second CDC interface's device-to-host data IN endpoint. */
		#define CDC2_TX_EPNUM                  5	

		/** Endpoint number of the second CDC interface's host-to-device data OUT endpoint. */
		#define CDC2_RX_EPNUM                  6	

		/** Size in bytes of the CDC device-to-host notification IN endpoints. */
		#define CDC_NOTIFICATION_EPSIZE        8

		/** Size in bytes of the CDC data IN and OUT endpoints. */
		#define CDC_TXRX_EPSIZE                16	

	/* Type Defines: */
		/** Type define for the device configuration descriptor structure. This must be defined in the
		 *  application code, as the configuration descriptor contains several sub-descriptors which
		 *  vary between devices, and which describe the device's usage to the host.
		 */
		typedef struct
		{
			USB_Descriptor_Configuration_Header_t    Config;
			USB_Descriptor_Interface_Association_t   IAD1;
			USB_Descriptor_Interface_t               CDC1_CCI_Interface;
			CDC_FUNCTIONAL_DESCRIPTOR(2)             CDC1_Functional_IntHeader;
			CDC_FUNCTIONAL_DESCRIPTOR(2)             CDC1_Functional_CallManagement;
			CDC_FUNCTIONAL_DESCRIPTOR(1)             CDC1_Functional_AbstractControlManagement;
			CDC_FUNCTIONAL_DESCRIPTOR(2)             CDC1_Functional_Union;
			USB_Descriptor_Endpoint_t                CDC1_ManagementEndpoint;
			USB_Descriptor_Interface_t               CDC1_DCI_Interface;
			USB_Descriptor_Endpoint_t                CDC1_DataOutEndpoint;
			USB_Descriptor_Endpoint_t                CDC1_DataInEndpoint;
			USB_Descriptor_Interface_Association_t   IAD2;
			USB_Descriptor_Interface_t               CDC2_CCI_Interface;
			CDC_FUNCTIONAL_DESCRIPTOR(2)             CDC2_Functional_IntHeader;
			CDC_FUNCTIONAL_DESCRIPTOR(2)             CDC2_Functional_CallManagement;
			CDC_FUNCTIONAL_DESCRIPTOR(1)             CDC2_Functional_AbstractControlManagement;
			CDC_FUNCTIONAL_DESCRIPTOR(2)             CDC2_Functional_Union;
			USB_Descriptor_Endpoint_t                CDC2_ManagementEndpoint;
			USB_Descriptor_Interface_t               CDC2_DCI_Interface;
			USB_Descriptor_Endpoint_t                CDC2_DataOutEndpoint;
			USB_Descriptor_Endpoint_t                CDC2_DataInEndpoint;
		} USB_Descriptor_Configuration_t;

	/* Function Prototypes: */
		uint16_t USB_GetDescriptor(const uint16_t wValue, const uint8_t wIndex, void** const DescriptorAddress)
		                           ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif
