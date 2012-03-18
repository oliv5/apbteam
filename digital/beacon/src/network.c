#include <types.h>
#include <configServer.h>
#include <zdo.h>
// #include <peer2peer.h>
#include "network.h"
// #include <serialInterface.h>



#define APP_DETECT_LINK_FAILURE 1

// Network related variables
AppMessageBuffer_t buf_to_send;

extern AppState_t appState;
extern DeviceType_t deviceType;
// extern HAL_UsartDescriptor_t appUsartDescriptor;
// extern uint8_t usartTxBuffer[APP_USART_TX_BUFFER_SIZE];


static ZDO_StartNetworkReq_t networkParams; // request params for ZDO_StartNetworkReq
APS_DataReq_t test;


// Endpoint parameters
static SimpleDescriptor_t simpleDescriptor = { APP_ENDPOINT, APP_PROFILE_ID, 1, 1, 0, 0 , NULL, 0, NULL };
static APS_RegisterEndpointReq_t endpointParams;


#if APP_DETECT_LINK_FAILURE == 1
static uint8_t retryCounter = 0;                // Data sending retries counter
// Leave request, used for router to leave the network when communication was interrupted
static ZDO_ZdpReq_t leaveReq;
static void leaveNetwork(void);
static void zdpLeaveResp(ZDO_ZdpResp_t *zdpResp);
#endif // APP_DETECT_LINK_FAILURE

uint16_t petite_adresse = 0;
int lost_packet = 0;
extern int jack;

/**************************************************************************//**
  \brief Intializes network parameters.

  \param  none.
  \return none.
******************************************************************************/
void initNetwork(void)
{
	if(deviceType == DEVICE_TYPE_COORDINATOR)
	{
		bool rx_on_idle = true;
		CS_WriteParameter(CS_RX_ON_WHEN_IDLE_ID, &rx_on_idle);
	}
	if(deviceType == DEVICE_TYPE_END_DEVICE)
	{
		_delay_ms(2000);
		bool rx_on_idle = false;
		CS_WriteParameter(CS_RX_ON_WHEN_IDLE_ID, &rx_on_idle);
		
	}
	
	/*False = random ID */
	bool unique_addr = true;
	CS_WriteParameter(CS_NWK_UNIQUE_ADDR_ID,&unique_addr);

//  	uint16_t nwkAddr=1; 
//  	CS_WriteParameter(CS_NWK_ADDR_ID, &nwkAddr);
	
	// Set the deviceType value to Config Server	
	CS_WriteParameter(CS_DEVICE_TYPE_ID, &deviceType);

	/* Neighbor table size - The size of neighbor table an ED can have */
	int neighbor_table_size = 10;
	CS_WriteParameter(CS_NEIB_TABLE_SIZE_ID, &neighbor_table_size);
	
	/* Max children amount -Maximum number of children that a given device (coordinator or router) may have */
	int max_children = 8;
	CS_WriteParameter(CS_MAX_CHILDREN_AMOUNT_ID, &max_children);
	
	/* Max router children amount - Maximum number of routers among the children of one device */
	int max_children_router = 2;
	CS_WriteParameter(CS_MAX_CHILDREN_ROUTER_AMOUNT_ID, &max_children_router);
	
	CS_WriteParameter(CS_EXT_PANID_ID,&(char){"0xAAAAAAAAAAAAAAAALL"});

	appState = APP_NETWORK_JOINING_STATE;
}


/**************************************************************************//**
  \brief ZDO_StartNetwork primitive confirmation callback.

  \param  confirmInfo - confirmation parametrs.
  \return none.
******************************************************************************/
void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t* confirmInfo)
{
	if (confirmInfo->status == ZDO_SUCCESS_STATUS)
	{
#if APP_DETECT_LINK_FAILURE == 1
		retryCounter = 0;
#endif // APP_DETECT_LINK_FAILURE
		
 		PORTD=0b00100000;
		petite_adresse = confirmInfo->shortAddr;
		
		appState = APP_NETWORK_JOINED_STATE;
		
		// Set application endpoint properties
		endpointParams.simpleDescriptor = &simpleDescriptor;
		endpointParams.APS_DataInd = APS_DataIndication;
		// Register endpoint
		APS_RegisterEndpointReq(&endpointParams);
		
		// Configure the message structure
		test.dstAddrMode = APS_SHORT_ADDRESS;				// Short addressing mode
		test.dstAddress.shortAddress = 0x0000;				// Destination address
		test.profileId = APP_PROFILE_ID;				// Profile ID
		test.dstEndpoint = APP_ENDPOINT;				// Desctination endpoint
		test.clusterId = APP_CLUSTER_ID;				// Desctination cluster ID
		test.srcEndpoint = APP_ENDPOINT;				// Source endpoint
		test.asdu = &buf_to_send.message;				// application message pointer
		test.asduLength = 3 + sizeof(buf_to_send.message.messageId);	// actual application message length
		test.txOptions.acknowledgedTransmission = 0;			// Acknowledged transmission enabled
		test.radius = 0;						// Use maximal possible radius
		test.APS_DataConf = APS_DataConf;				// Confirm handler    Z
	}
	else
	{
 		 PORTD=0b01000000;
	}
	SYS_PostTask(APL_TASK_ID);
}

/**************************************************************************//**
  \brief Starts network.

  \param  none.
  \return none.
******************************************************************************/
void startNetwork(void)
{
  networkParams.ZDO_StartNetworkConf = ZDO_StartNetworkConf;
  // start network
  ZDO_StartNetworkReq(&networkParams);
}


/**************************************************************************//**
  \brief Update network status event handler.

  \param  nwkParams - new network parameters.
  \return none.
******************************************************************************/
void ZDO_MgmtNwkUpdateNotf(ZDO_MgmtNwkUpdateNotf_t *nwkParams)
{
  switch (nwkParams->status)
  {
    case ZDO_NETWORK_STARTED_STATUS:
      break;

    case ZDO_NETWORK_LOST_STATUS:
    {
      APS_UnregisterEndpointReq_t unregEndpoint;

      unregEndpoint.endpoint = endpointParams.simpleDescriptor->endpoint;
      APS_UnregisterEndpointReq(&unregEndpoint);
      
      // try to rejoin the network
      appState = APP_NETWORK_JOINING_STATE;
      SYS_PostTask(APL_TASK_ID);
      break;
    }

    case ZDO_NWK_UPDATE_STATUS:
      break;

    default:
      break;
  }
}


/**************************************************************************//**
  \brief Handler of aps data sent confirmation.

  \param  confInfo - confirmation info
  \return none.
******************************************************************************/
void APS_DataConf(APS_DataConf_t* confInfo)
{
	if (APS_SUCCESS_STATUS != confInfo->status)
	{
#if APP_DETECT_LINK_FAILURE == 1
		retryCounter++;
		if(retryCounter>3)
		{
			lost_packet++;
		}
		if (MAX_RETRIES_BEFORE_REJOIN == retryCounter)
		{
			leaveNetwork();
		}
		else
#endif // APP_DETECT_LINK_FAILURE
		{
			// Data not delivered, resend.
			send_data();
		}
		return;
	}
#if APP_DETECT_LINK_FAILURE == 1
	retryCounter = 0;
#endif // APP_DETECT_LINK_FAILURE
}



/**************************************************************************//**
  \brief APS data indication handler.

  \param  indData - received data pointer.
  \return none.
******************************************************************************/
int received_event=0;
int beacon_number = 0;
int angle_received = 0;
void APS_DataIndication(APS_DataInd_t* indData)
{
	AppMessage_t *appMessage = (AppMessage_t *) indData->asdu;
	// Data received indication
	received_event++;
	
	switch(appMessage->data[0])
	{
		case 42: // COMMANDE JACK
// 			jack = appMessage->data[2];
			break;
		case 43: // Update ANGLE
			beacon_number = appMessage->data[1];
			angle_received = appMessage->data[2];
			update_position(beacon_number,angle_received);
			break;
	}
	
 	/*if(appMessage->data[0]==0x55)
	{
		sprintf(usartTxBuffer,"Trame recue from %d ==> Lost packet = %d\r\n\0",appMessage->data[1],appMessage->data[2]);
		WRITE_USART(&appUsartDescriptor,usartTxBuffer,strlen(usartTxBuffer));	
	}
	else
	{

	}*/
}


#if APP_DETECT_LINK_FAILURE == 1
/**************************************************************************//**
 * \brief Leave network.
 * 
 * \param  none.
 * \return none.
 ******************************************************************************/
 static void leaveNetwork(void)
 {
	 ZDO_MgmtLeaveReq_t *zdpLeaveReq = &leaveReq.req.reqPayload.mgmtLeaveReq;
	 APS_UnregisterEndpointReq_t unregEndpoint;
	 
	 appState = APP_NETWORK_LEAVING_STATE;
	 
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
 
 /**************************************************************************//**
  * \brief Leave network response.
  * 
  * \param  zdpResp - response data
  * \return none.
  ******************************************************************************/
  static void zdpLeaveResp(ZDO_ZdpResp_t *zdpResp)
  {
	  
	  // Try to rejoin the network
	  appState = APP_NETWORK_JOINING_STATE;
	  SYS_PostTask(APL_TASK_ID);
	  
	  (void)zdpResp;
  }
  #endif // APP_DETECT_LINK_FAILURE
  
void send_data(void)
{
// 	PORTD ^= (1<<7);
	buf_to_send.message.data[0]=0x55;
	buf_to_send.message.data[1]=petite_adresse;
	buf_to_send.message.data[2]=lost_packet;
	APS_DataReq(&test);
// 	PORTD ^= (1<<7);
}

void send_angle(int angle_degree)
{
	buf_to_send.message.data[0]=0x43;
	buf_to_send.message.data[1]=petite_adresse;
	buf_to_send.message.data[2]=angle_degree;	
}
  