/* descriptors.c */
/* dev2 - Multi-purpose development board using USB and Ethernet. {{{
 *
 * Copyright (C) 2009 Nicolas Schodet
 *
 * APBTeam:
 *        Web: http://apbteam.org/
 *      Email: team AT apbteam DOT org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * }}} */
#include "common.h"

#include "modules/usb/usb.h"

#include <avr/pgmspace.h>

#include "descriptors.h"

/* Configuration structure. */
typedef struct
{
    USB_Descriptor_Configuration_Header_t Config;
    USB_Descriptor_Interface_t TS1_Interface;
    USB_Descriptor_Endpoint_t TS1_DataOutEndpoint;
    USB_Descriptor_Endpoint_t TS1_DataInEndpoint;
} USB_Descriptor_Configuration_t;

USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
    Header: { Size: sizeof (USB_Descriptor_Device_t), Type: DTYPE_Device },
    USBSpecification: VERSION_BCD (01.10),
    Class: 0xFF,
    SubClass: 0x00,
    Protocol: 0x00,
    Endpoint0Size: 8,
    /* Taken from LUFA IDs. */
    VendorID: 0x03EB,
    ProductID: 0x204E,
    ReleaseNumber: 0x0000,
    ManufacturerStrIndex: 0x01,
    ProductStrIndex: 0x02,
    SerialNumStrIndex: NO_DESCRIPTOR,
    NumberOfConfigurations: 1
};

USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
{
    Config:
	{
	    Header: { Size: sizeof (USB_Descriptor_Configuration_Header_t),
			Type: DTYPE_Configuration },
	    TotalConfigurationSize: sizeof (USB_Descriptor_Configuration_t),
	    TotalInterfaces: 1,
	    ConfigurationNumber: 1,
	    ConfigurationStrIndex: NO_DESCRIPTOR,
	    ConfigAttributes: (USB_CONFIG_ATTR_BUSPOWERED |
			       USB_CONFIG_ATTR_SELFPOWERED),
	    MaxPowerConsumption: USB_CONFIG_POWER_MA (100)
	},

    TS1_Interface:
	{
	    Header: { Size: sizeof (USB_Descriptor_Interface_t),
			Type: DTYPE_Interface },
	    InterfaceNumber: 0,
	    AlternateSetting: 0,
	    TotalEndpoints: 2,
	    Class: 0xFF,
	    SubClass: 0x00,
	    Protocol: 0x00,
	    InterfaceStrIndex: NO_DESCRIPTOR
	},

    TS1_DataOutEndpoint:
	{
	    Header: { Size: sizeof (USB_Descriptor_Endpoint_t),
			Type: DTYPE_Endpoint },
	    EndpointAddress: (ENDPOINT_DESCRIPTOR_DIR_OUT | GPIO_RX_EPNUM),
	    Attributes: EP_TYPE_BULK,
	    EndpointSize: GPIO_RX_EPSIZE,
	    PollingIntervalMS: 0x00
	},

    TS1_DataInEndpoint:
	{
	    Header: { Size: sizeof (USB_Descriptor_Endpoint_t),
			Type: DTYPE_Endpoint },
	    EndpointAddress: (ENDPOINT_DESCRIPTOR_DIR_IN | GPIO_TX_EPNUM),
	    Attributes: EP_TYPE_BULK,
	    EndpointSize: GPIO_TX_EPSIZE,
	    PollingIntervalMS: 0x00
	},
};

USB_Descriptor_String_t PROGMEM LanguageString =
{
	Header: { Size: USB_STRING_LEN (1), Type: DTYPE_String },
	UnicodeString: {LANGUAGE_ID_ENG}
};

USB_Descriptor_String_t PROGMEM ManufacturerString =
{
	Header: { Size: USB_STRING_LEN (7), Type: DTYPE_String },
	UnicodeString: L"APBTeam"
};

USB_Descriptor_String_t PROGMEM ProductString =
{
	Header: { Size: USB_STRING_LEN (9), Type: DTYPE_String },
	UnicodeString: L"dev2 gpio"
};

uint16_t
USB_GetDescriptor (const uint16_t wValue, const uint8_t wIndex,
		   void ** const DescriptorAddress)
{
    const uint8_t DescriptorType = wValue >> 8;
    const uint8_t DescriptorNumber = wValue & 0xFF;
    void * Address = NULL;
    uint16_t Size = NO_DESCRIPTOR;
    switch (DescriptorType)
      {
      case DTYPE_Device:
	Address = DESCRIPTOR_ADDRESS (DeviceDescriptor);
	Size = sizeof (USB_Descriptor_Device_t);
	break;
      case DTYPE_Configuration:
	Address = DESCRIPTOR_ADDRESS (ConfigurationDescriptor);
	Size = sizeof (USB_Descriptor_Configuration_t);
	break;
      case DTYPE_String:
	switch (DescriptorNumber)
	  {
	  case 0x00:
	    Address = DESCRIPTOR_ADDRESS (LanguageString);
	    Size = pgm_read_byte (&LanguageString.Header.Size);
	    break;
	  case 0x01:
	    Address = DESCRIPTOR_ADDRESS (ManufacturerString);
	    Size = pgm_read_byte (&ManufacturerString.Header.Size);
	    break;
	  case 0x02:
	    Address = DESCRIPTOR_ADDRESS (ProductString);
	    Size = pgm_read_byte (&ProductString.Header.Size);
	    break;
	  }
	break;
      }
    *DescriptorAddress = Address;
    return Size;
}

