#ifndef descriptors_h
#define descriptors_h
/* descriptors.h */
/* usb - USB device module using LUFA. {{{
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
#include <LUFA/Drivers/USB/USB.h>
#include <avr/pgmspace.h>

/* Endpoints. */
#define TS1_TX_EPNUM 1
#define TS1_RX_EPNUM 2
#define TS2_TX_EPNUM 3
#define TS2_RX_EPNUM 4
#define TS_TXRX_EPSIZE 16

/* Configuration structure. */
typedef struct
{
    USB_Descriptor_Configuration_Header_t Config;
    USB_Descriptor_Interface_t TS1_Interface;
    USB_Descriptor_Endpoint_t TS1_DataOutEndpoint;
    USB_Descriptor_Endpoint_t TS1_DataInEndpoint;
    USB_Descriptor_Interface_t TS2_Interface;
    USB_Descriptor_Endpoint_t TS2_DataOutEndpoint;
    USB_Descriptor_Endpoint_t TS2_DataInEndpoint;
} USB_Descriptor_Configuration_t;

/* Used by LUFA to get descriptors. */
uint16_t
USB_GetDescriptor (const uint16_t wValue, const uint8_t wIndex,
		   void** const DescriptorAddress)
ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG (3);

#endif /* descriptors_h */
