/**************************************************************************//**
  \file zclZllSecurity.h

  \brief
    Functions and constants used by the ZLL security module.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    22.06.11 A. Taradov - Created.
******************************************************************************/
#ifndef _ZCLZLLSECURITY_H
#define	_ZCLZLLSECURITY_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <types.h>
#include <sspCommon.h>

/******************************************************************************
                    Definitions section
******************************************************************************/
#define ZCL_ZLL_DEVELOPMENT_KEY_INDEX        0
#define ZCL_ZLL_MASTER_KEY_INDEX             4
#define ZCL_ZLL_CERTIFICATION_KEY_INDEX      15

//#define ZCL_ZLL_SUPPORTED_KEY_INDEX          ZCL_ZLL_CERTIFICATION_KEY_INDEX
//#define ZCL_ZLL_SUPPORTED_SECURITY_KEYS      (1u << ZCL_ZLL_SUPPORTED_KEY_INDEX)

#define ZCL_ZLL_SUPPORTED_KEY_INDEX          ZCL_ZLL_DEVELOPMENT_KEY_INDEX
#define ZCL_ZLL_SUPPORTED_SECURITY_KEYS      (1u << ZCL_ZLL_DEVELOPMENT_KEY_INDEX)

/*! Certification key */
#define ZCL_ZLL_SECURITY_KEY "\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf"

/******************************************************************************
                    Prototypes section
******************************************************************************/

/**************************************************************************//**
\brief Encrypt security key
\param[in] text - key to be encrypted
\param[in] callback - function to be called after encryption is finished
******************************************************************************/
void zclZllEncryptKey(uint8_t text[SECURITY_BLOCK_SIZE], void (*callback)(void));

/**************************************************************************//**
\brief Decrypt security key
\param[in] text - key to be decrypted
\param[in] callback - function to be called after decryption is finished
******************************************************************************/
void zclZllDecryptKey(uint8_t text[SECURITY_BLOCK_SIZE], void (*callback)(void));

#endif // _ZCLZLLSECURITY_H

// eof zclZllSecurity.h
