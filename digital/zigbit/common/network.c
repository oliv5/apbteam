/* network.c */
/* Beacon network management. {{{
 *
 * Copyright (C) 2012 Florent Duchon
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

#include <types.h>
#include <configServer.h>
#include <zdo.h>

#include "configuration.h"
#include "network.h"
#include "network_specific.h"
#include "print.h"
#include "led.h"
#include "uid.h"

/* Endpoint parameters */
SimpleDescriptor_t simpleDescriptor = { APP_ENDPOINT, APP_PROFILE_ID, 1, 1, 0, 0 , NULL, 0, NULL };
APS_RegisterEndpointReq_t endpointParams;

/* Network related variables */
APS_DataReq_t network_config;
static AppState_t appState = APP_INITIAL_STATE;  // application state
static DeviceType_t deviceType;
static AppDataTransmissionState_t appDataTransmissionState;
static ZDO_StartNetworkReq_t networkParams; // request params for ZDO_StartNetworkReq

/* Leave request, used for router to leave the network when communication was interrupted */
static ZDO_ZdpReq_t leaveReq;


/* This function intializes network parameters */
void network_init(uint16_t uid)
{	
	uprintf("\r\n##########################################\r\n");
	uprintf("Network init called with UID = %d\r\n",uid);
	
	deviceType = get_device_type(uid);
	
	if(deviceType == DEVICE_TYPE_COORDINATOR)
	{
		uprintf("I'm a Cooridinator\r\n");
		bool rx_on_idle = true;
		CS_WriteParameter(CS_RX_ON_WHEN_IDLE_ID, &rx_on_idle);
	}
	if(deviceType == DEVICE_TYPE_END_DEVICE)
	{
		uprintf("I'm an End-Device\r\n");
		bool rx_on_idle = false;
		CS_WriteParameter(CS_RX_ON_WHEN_IDLE_ID, &rx_on_idle);		
	}
	
	/*False = random ID */
	/* True = static short address */
	bool unique_addr = true;
	CS_WriteParameter(CS_NWK_UNIQUE_ADDR_ID,&unique_addr);
	
	ExtAddr_t ownExtAddr = 0x1LL;
	CS_WriteParameter(CS_UID_ID, &ownExtAddr);

  	
  	CS_WriteParameter(CS_NWK_ADDR_ID, &uid);
	
	// Set the deviceType value to Config Server	
	CS_WriteParameter(CS_DEVICE_TYPE_ID, &deviceType);

	network_set_state(APP_NETWORK_JOIN_REQUEST);
}


/* This function starts the network according to the defined configuraiton*/
void network_start(void)
{
	networkParams.ZDO_StartNetworkConf = ZDO_StartNetworkConf;
	// start network
	ZDO_StartNetworkReq(&networkParams);
}

/* ZDO_StartNetwork primitive confirmation callback */
void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t* confirmInfo)
{
	if (confirmInfo->status == ZDO_SUCCESS_STATUS)
	{
		network_set_state(APP_NETWORK_JOINED_STATE);
		
		/*  Set application endpoint properties */
		endpointParams.simpleDescriptor = &simpleDescriptor;
		endpointParams.APS_DataInd = APS_DataIndication;
		
		/* Register endpoint */
		APS_RegisterEndpointReq(&endpointParams);
		
		/* Update status : ready for transfert */
		network_set_transmission_state(APP_DATA_TRANSMISSION_READY_STATE);
		
		/* Display useful informations */
		uprintf("Connection etablished\r\n");
		uprintf("	> Parent : %d\r\n'",confirmInfo->parentAddr);
		uprintf("	> Channel : %d\r\n",confirmInfo->activeChannel);
		uprintf("	> Short Address : %d\r\n",confirmInfo->shortAddr);
		uprintf("	> PANid : 0x%x\r\n",(uint16_t)confirmInfo->PANId);
		uprintf("	> RSSI = %d dBm (-91 dBm to -7 dBm)\r\n",(int8_t)network_get_rssi(confirmInfo->parentAddr));
		uprintf("	> LQI = %d (0 to 255)\r\n",(uint8_t)network_get_lqi(confirmInfo->parentAddr));
		uprintf("##########################################\r\n");

	}
	else
	{
 		uprintf("Connection failed ! returned status  = 0x%x\n\r",confirmInfo->status);
		uprintf("##########################################\r\n");
		
		/* if communication is failed, try to rejoin */
		network_set_state(APP_NETWORK_JOIN_REQUEST);
	}
}


/* This function sets the network status */
void network_set_state(AppState_t state)
{
	appState = state;
	return;
}


/* This function returns the network status */
AppState_t network_get_state(void)
{
	return appState;
}


/* Update network status event handler */
void ZDO_MgmtNwkUpdateNotf(ZDO_MgmtNwkUpdateNotf_t *nwkParams)
{
	switch (nwkParams->status)
	{
		case ZDO_NETWORK_STARTED_STATUS:
			break;
		case ZDO_NETWORK_LOST_STATUS:
			network_leave();
			network_set_state(APP_NETWORK_JOIN_REQUEST);
			break;
		case ZDO_NWK_UPDATE_STATUS:
			break;
		case ZDO_CHILD_JOINED_STATUS:
			uprintf("Child %d joined\r\n",nwkParams->childInfo.shortAddr);
			uprintf("	> RSSI = %d dBm (-91 dBm to -7 dBm)\r\n",(int8_t)network_get_rssi(nwkParams->childInfo.shortAddr));
			uprintf("	> LQI = %d (0 to 255)\r\n",(uint8_t)network_get_lqi(nwkParams->childInfo.shortAddr));
			break;
		case ZDO_CHILD_REMOVED_STATUS:
			uprintf("Child %d quit\r\n",nwkParams->childInfo.shortAddr);
			break;
		default:
			uprintf("Network status = 0x%x\r\n",nwkParams->status);
			break;
	}
}


/* This function returns the LQI of the joined network */
uint8_t network_get_lqi(ShortAddr_t nodeAddr)
{
	ZDO_GetLqiRssi_t lqiRssi;

	lqiRssi.nodeAddr = nodeAddr;
	ZDO_GetLqiRssi(&lqiRssi);
	
	return lqiRssi.lqi;
}


/* This function returns the RSSI of the joined network */
int8_t network_get_rssi(ShortAddr_t nodeAddr)
{
	ZDO_GetLqiRssi_t lqiRssi;

	lqiRssi.nodeAddr = nodeAddr;
	ZDO_GetLqiRssi(&lqiRssi);
	
	return lqiRssi.rssi;
}


/* APS data indication handler */
void APS_DataIndication(APS_DataInd_t* indData)
{
	led_inverse(NETWORK_ACTIVITY_LED);
	network_specific_DataIndicationcallback(indData);
}


/* brief Handler of aps data sent confirmation */
void APS_DataConf(APS_DataConf_t* confInfo)
{
	static int retryCounter = 0;
	
	if (APS_SUCCESS_STATUS != confInfo->status)
	{
		retryCounter++;
// 		uprintf("!!! Last transfert was failed. Reset it (retry = %d)\r\n",retryCounter);
		
		if (MAX_RETRIES_BEFORE_REJOIN == retryCounter)
		{
			network_leave();
			network_set_transmission_state(APP_DATA_TRANSMISSION_READY_STATE);
			network_set_state(APP_NETWORK_JOIN_REQUEST);
		}
		else
		{
			/*  Data not delivered, resend.*/
			network_start_transmission();
		}
		return;
	}
	else
	{
		network_set_transmission_state(APP_DATA_TRANSMISSION_READY_STATE);
		network_specific_DataConfcallback();
	}
	retryCounter = 0;
}


/* This function starts the transmission of a configured packet */
void network_start_transmission(void)
{
	/* Check the transmission state and send if ready */
	if(network_get_transmission_state() != APP_DATA_TRANSMISSION_BUSY_STATE)
	{ 
		network_set_transmission_state(APP_DATA_TRANSMISSION_BUSY_STATE);
		APS_DataReq(&network_config);
	}
	/* TODO: Resent after few ms */
}


/* This function sets the transmission state s*/
void network_set_transmission_state(AppDataTransmissionState_t state)
{
	appDataTransmissionState = state;
	
	/* Modify the LED in accordance with the transmission state */
	if(state == APP_DATA_TRANSMISSION_BUSY_STATE)
		led_on(NETWORK_ACTIVITY_LED);
	else
		led_off(NETWORK_ACTIVITY_LED);
}


/* This function returns the transmission state */
AppDataTransmissionState_t network_get_transmission_state(void)
{
	return appDataTransmissionState;
}


/* This function quits the joined network */
void network_leave(void)
{
	ZDO_MgmtLeaveReq_t *zdpLeaveReq = &leaveReq.req.reqPayload.mgmtLeaveReq;
	APS_UnregisterEndpointReq_t unregEndpoint;

	unregEndpoint.endpoint = endpointParams.simpleDescriptor->endpoint;
	APS_UnregisterEndpointReq(&unregEndpoint);

	leaveReq.ZDO_ZdpResp =  zdpLeaveResp;
	leaveReq.reqCluster = MGMT_LEAVE_CLID;
	leaveReq.dstAddrMode = EXT_ADDR_MODE;
	leaveReq.dstExtAddr = 0;
	zdpLeaveReq->deviceAddr = 0;
	zdpLeaveReq->rejoin = 0;
	zdpLeaveReq->removeChildren = 1;
	zdpLeaveReq->reserved = 0;
	ZDO_ZdpReq(&leaveReq);
}
 

/* Leave network response */
void zdpLeaveResp(ZDO_ZdpResp_t *zdpResp)
{
	(void)zdpResp;
}


/* Wakeup event handler (dummy) */
void ZDO_WakeUpInd(void)
{
}


/* Stub for ZDO Binding Indication */
void ZDO_BindIndication(ZDO_BindInd_t *bindInd) 
{
	(void)bindInd;
}


/* Stub for ZDO Unbinding Indication */
void ZDO_UnbindIndication(ZDO_UnbindInd_t *unbindInd)
{
	(void)unbindInd;
}
