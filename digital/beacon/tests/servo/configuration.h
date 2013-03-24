#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#include "apsCommon.h"
#include <BoardConfig.h>
#include <usart.h>

/*----------------------------------------------- */
/* Disables board-specific peripherals support */
/*----------------------------------------------- */
#define APP_DISABLE_BSP 1

#define AC_FREQ 8000000

/* -----------------------------------------------------------  	*/
/* 			USART CONFIGURATION 				*/
/* -----------------------------------------------------------  	*/
#define APP_INTERFACE_USART 			0x01
#define APP_INTERFACE_VCP 			0x02
#define APP_INTERFACE_SPI 				0x03
#define APP_INTERFACE_UART 			0x04
#define APP_INTERFACE_USBFIFO 		0x05
#define APP_USART_RX_BUFFER_SIZE          100 		/* Receive buffer size for USART. */
#define APP_USART_TX_BUFFER_SIZE          500		/* Transmit buffer size for USART */
#define APP_INTERFACE APP_INTERFACE_USART	/* Defines primary serial interface type to be used by application */
#define APP_USART_CHANNEL USART_CHANNEL_0	/* Defines USART interface name to be used by application.*/

/* -----------------------------------------------------------  	*/
/* 			TWI  CONFIGURATION 				*/
/* -----------------------------------------------------------  	*/
#define AC_BEACON_TWI_ADDRESS 		10		/* TWI address */
#define AC_TWI_DRIVER HARD					/* Driver to implement TWI: HARD, SOFT, or USI. */
#define AC_TWI_NO_INTERRUPT 			0		/* Do not use interrupts. */	
#define AC_TWI_FREQ 					100000	/* TWI frequency, should really be 100 kHz. */
#define AC_TWI_SLAVE_ENABLE 			1		/* Enable slave part. */
#define AC_TWI_MASTER_ENABLE 		0		/* Enable master part. */
#define AC_TWI_SLAVE_POLLED 			1		/* Use polled slave mode: received data is stored in a buffer which can be polled using twi_slave_poll. */
#undef AC_TWI_SLAVE_RECV					/* Slave reception callback to be defined by the user when not in polled mode. */
#define AC_TWI_PULL_UP 				0		/* Use internal pull up. */
#define AC_TWI_SLAVE_RECV_BUFFER_SIZE 16		/* Slave reception buffer size. */
#define AC_TWI_SLAVE_SEND_BUFFER_SIZE 32		/* Slave transmission buffer size. */


/* -----------------------------------------------------------  	*/
/* 			ZIGBEE  CONFIGURATION 			*/
/* -----------------------------------------------------------  	*/
#define AT25F2048  					0x01
#define AT45DB041  					0x02
#define AT25DF041A 					0x03



#define APP_FRAGMENTATION 			0		/* Enables or disables APS Fragmentation support. */
#define APP_DETECT_LINK_FAILURE 		1		/* Enable or disable link failure detection */
#define APP_ENDPOINT                      		1		/* Endpoint will be useed */
#define APP_PROFILE_ID                    		1		/* Profile Id will be used */
#define APP_CLUSTER_ID                    		1		/* Cluster Id will be used */
#define APP_JOINING_INDICATION_PERIOD	500L	/* Period of blinking during starting network */



// 32-bit mask of channels to be scanned before network is started. Channels that
// should be used are marked with logical 1 at corresponding bit location.
//  Valid channel numbers for 2.4 GHz band are 0x0b - 0x1a
//  Valid channel numbers for 900 MHz band are 0x00 - 0x0a
// 
//  Notes:
//  1. for small amount of enabled channels it is more convinient to specify list
// of channels in the form of '(1ul << 0x0b)'
//  2. For 900 MHz band you also need to specify channel page
// 
//  Value range: 32-bit values:
//  Valid channel numbers for 2.4 GHz band are 0x0b - 0x1a
//  Valid channel numbers for 900 MHz band are 0x00 - 0x0a
// 
//  C-type: uint32_t
//  Can be set: at any time before network start
#define CS_CHANNEL_MASK (1L<<0x0f)


// The parameter specifies the predefined extended PANID of the network to be
// formed (for the coordinator) or joined (for a router or an end device). For a
// router or an end device the parameter can equal 0 allowing them to join the
// first suitable network that they discover.
#define CS_EXT_PANID 0xAAAAAAAAAAAA1337LL

// The maximum number of direct children that a given device (the coordinator or a
// router) can have.
// 
//  The parameter is only enabled for routers and the coordinator. An end device
// can not have children. If an actual number of children reaches a parameter's
// value, the node will have not been able to accept any more children joining the
// network. The parameter can be set to 0 on a router thus preventing it from
// accepting any children and can be help form a desired network topology. For
// example, if the parameter is set to 0 on all routers, then the coordinator will
// be the only device that can have children and the network will have star
// topology.
#define CS_MAX_CHILDREN_AMOUNT 8

// The maximum number of routers among the direct children of the device
// 
//  The parameter determines how many routers the device can have as children. Note
// that the maximum number of end devices is equal to CS_MAX_CHILDREN_AMOUNT -
// CS_MAX_CHILDREN_ROUTER_AMOUNT.
#define CS_MAX_CHILDREN_ROUTER_AMOUNT 0

// Network depht limits amount of hops that packet may travel in the network.
// Actual maximum number of hops is network depth multiplied by 2.
// 
//  The parameter determines the maximum depth of a network tree formed by
// child-parent relationships between nodes.
// 
//  While joining the network the node receives beacon responses from potential
// parents containing their actual network depth and declines those which show
// values not less than the maximum network depth on the joining device. A
// potential parent will also reject a beacon from the joining device and will not
// sent a response if the joining device shows the network depth greater than it is
// allowed on the potential parent. This logic is enabled if the parameter value is
// not greater than 15. If its value is greater than 15, then device does not
// perform any checkings of the network depth, neither when joining a network nor
// when accepting other nodes as children. This allows forming long chains of
// devices across considerable distances.
#define CS_MAX_NETWORK_DEPTH 1

// Maximum amount of records in the Neighbor Table.
// 
//  The parameter determines the size of the neighbor table which is used to store
// beacon responses from nearby devices. The parameter puts an upper bound over the
// amount of child devices possible for the node.
#define CS_NEIB_TABLE_SIZE 10

// Maximum amount of records in the network Route Table.
// 
//  The parameter sets the maximum number of records that can be kept in the NWK
// route table. The table is used by NWK to store information about established
// routes. Each table entry specifies the next-hop short address for a route from
// the current node to a given destination node. The table is being filled
// automatically during route discovery. An entry is added when a route is
// discovered.
#define CS_ROUTE_TABLE_SIZE 8

// The parameter specifies the TX power of the transceiver device, is measured in
// dBm(s). After the node has entered the network the value can only be changed via
// the ZDO_SetTxPowerReq() function.
// 
//  Value range: depends on the hardware. Transmit power must be in the range from
// -17 to 3 dBm for AT86RF231, AT86RF230 and AT86RF230B. For AT86RF212 transmit
// power must be in the range from -11 to 11 dBm.
#define CS_RF_TX_POWER 3

//-----------------------------------------------
//STANDARD_SECURITY_MODE
//-----------------------------------------------
#ifdef STANDARD_SECURITY_MODE
  // The parameter is used to determine the security type.
  // 
  //  Value range: 0,3 - for standard security; 1,2 - for high security.
  //  0 - network key is preconfigured ;
  //  1 - network join without master key, but with a trust center link key, which
  // must be set via APS_SetLinkKey();
  //  2 - network join employs a master key, which must be set APS_SetMasterKey();
  //  3 - network key is no preconfigured, but rather received from the trust center
  // in an unencrypted frame. <br.
  #define CS_ZDO_SECURITY_STATUS 0
  //#define CS_ZDO_SECURITY_STATUS 3
  
  // Depending on security key type and security mode this is either network key,
  // master key, link key or initial link key.
  // 
  //  Network key is used to encrypt a part of a data frame occupied by the NWK
  // payload. This type of encryption is applied in both the standard and high
  // security mode. The high security mode also enables encryption of the APS payload
  // with a link key, but if the txOptions.useNwkKey field in APS request parameters
  // is set to 0, the APS payload is ecrypted with the network key.
  // 
  //  The network key must be predefined if standard security is used with
  // CS_ZDO_SECURITY_STATUS set to 0. For all other values of CS_ZDO_SECURITY_STATUS
  // the network key is received from the trust center during device authentication.
  // Note that in the standard security mode with CS_ZDO_SECURITY_STATUS equal to 3
  // the network key is trasferred to the joining device in an unencrypted frame.
  #define CS_NETWORK_KEY {0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC}
  
  // The maximum number of network keys that can be stored on the device
  // 
  //  A device in a secured network can keep several network keys up to the value of
  // this parameter. Upon frame reception the device extracts key sequence number
  // from the auxiliary header of the frame and decrypts the message with the network
  // key corresponding to this sequence number. Besides, one key is considered active
  // for each device; this is the key that is used for encrypting outgoing frames.
  // The keys are distributed by the trust center with the help of the
  // APS_TransportKeyReq() command. The trust center can also change the active key
  // of a remote node via a call to APS_SwitchKeyReq().
  #define CS_NWK_SECURITY_KEYS_AMOUNT 1
  
  // Address of device responsible for authentication and key distribution (Trust
  // Center).
  // 
  //  The parameter specifies the trust center extended address. The stack makes use
  // of the parameter to support various opertaions in networks with security
  // enabled. For correct network operation a parameter's value must coincide with
  // the actual trust center address.
  // 
  //  In case the trust center extended address is unknown, for example, for testing
  // purposes, the parameter can be assigned to the universal trust center address
  // which equals APS_SM_UNIVERSAL_TRUST_CENTER_EXT_ADDRESS.
  #define CS_APS_TRUST_CENTER_ADDRESS 0xAAAAAAAAAAAAAAAALL
  
  // Security information waiting timeout before secure network join considered
  // failed.
  // 
  //  A timeout is started when connection with a parent is established. If the
  // security related procedures that are performed after this will not be completed
  // before the timeout exceeds, the device will fail joining the network. A value is
  // measured in milliseconds.
  #define CS_APS_SECURITY_TIMEOUT_PERIOD 10000
#endif

//-----------------------------------------------
//APP_FRAGMENTATION == 1
//-----------------------------------------------
#if (APP_FRAGMENTATION == 1)
  // This parameter limits the number of pieces to which the data sent with one APS
  // request can be split i f the fragmentation feature is applied. Thus it also
  // limits the maximum amount of data sent by the application with a single request:
  //  maximum data length = CS_APS_MAX_BLOCKS_AMOUNT * CS_APS_BLOCK_SIZE if the
  // latter parameter is not 0, else
  //  maximum data length = CS_APS_MAX_BLOCKS_AMOUNT * APS_MAX_ASDU_SIZE.
  #define CS_APS_MAX_BLOCKS_AMOUNT 4
  
  // Maximim size of a single fragment during fragmented transmission.
  // 
  //  If the value is 0, blocks will be of the maximum possilbe size; that is the
  // size will equal the value of CS_APS_MAX_ASDU_SIZE. The parameter and
  // CS_APS_MAX_BLOCKS_AMOUNT are also used to determine an amount of memory
  // allocated for a special buffer that keeps parts of incoming fragmented message
  // until all of them are received.
  #define CS_APS_BLOCK_SIZE 0
#endif


/*
  Parameters that affect data transfer:

  APP_TRANSMISSION_DELAY, ms
    Delay beetween transmitted packets.
    This delay may help in reducing amount of collisions thus increasing throughput.
    A value of 0 means no dealy is needed.

  APP_MAX_PACKET_SIZE, bytes
    Maximum amount of useful data that will be sent in one packet

  APP_DELAY_BEFORE_SEND, ms
    Delay beetween reception of first byte of current packet from UART and
    actual transmission of current packet over the air.
    Larger values of this parameter will result in higher throughput but
    also may lead to higher latencies.
    A value of 0 means no dealy is needed.
*/
#define APP_TRANSMISSION_DELAY            20

#if APP_FRAGMENTATION
  #define  APP_MAX_PACKET_SIZE            150
  // Relatively large value is used here to let application receive enough data to
  // make fragmentation feature work.
  #define  APP_DELAY_BEFORE_SEND          3000
#else // No fragmentation is alowed
  #define  APP_MAX_PACKET_SIZE            60
  #define  APP_DELAY_BEFORE_SEND          0
#endif // APP_FRAGMENTATION




// Size of APS payload (user data + message ID)
#define APP_APS_PAYLOAD_SIZE              (APP_MAX_PACKET_SIZE + 1) //61

// Size of temporary buffer (FIFO) to store data received from air.
// This size should be larger than APP_MAX_PACKET_SIZE.
#define APP_DATA_IND_BUFFER_SIZE          APP_APS_PAYLOAD_SIZE


#if APP_DETECT_LINK_FAILURE == 1
// Maximum amount of retries before rejoin is attempted
#define MAX_RETRIES_BEFORE_REJOIN         3
#endif // APP_DETECT_LINK_FAILURE


// Some sanity checks
#if APP_DATA_IND_BUFFER_SIZE < APP_MAX_PACKET_SIZE
  #error APP_DATA_IND_BUFFER_SIZE must be larger or equal to APP_MAX_PACKET_SIZE
#endif

#if APP_FRAGMENTATION
  #ifndef _APS_FRAGMENTATION_
    #error BitCloud must be built with fragmentation feature enabled
  #endif

  #if APP_APS_PAYLOAD_SIZE > (CS_APS_MAX_BLOCKS_AMOUNT * APS_MAX_ASDU_SIZE)
    #error APP_MAX_PACKET_SIZE  must be less or equal to (CS_APS_MAX_BLOCKS_AMOUNT * APS_MAX_ASDU_SIZE)
  #endif
#else
  #if APP_APS_PAYLOAD_SIZE > APS_MAX_ASDU_SIZE
    #error APP_APS_PAYLOAD_SIZE must be less or equal to APS_MAX_ASDU_SIZE
  #endif
#endif


// Common application state definition
typedef enum
{
  APP_INITIAL_STATE,                           // Initial state
  APP_NETWORK_JOIN_REQUEST,
  APP_NETWORK_JOINING_STATE,                   // Attempting join the network
  APP_NETWORK_JOINED_STATE,                    // Successfully joined
  APP_NETWORK_LEAVING_STATE,                   // Leaving from the network
  APP_ERROR_STATE                              // Error state (runtime error occured)
} AppState_t;

// Network data transmission state
typedef enum
{
  APP_DATA_TRANSMISSION_SENDING_STATE,         // APS Data Request was not sent yet
  APP_DATA_TRANSMISSION_BUSY_STATE,            // APS Data Request was sent (confirm waiting)
  APP_DATA_TRANSMISSION_WAIT_STATE,            // Waiting a data block from USART
  APP_DATA_TRANSMISSION_READY_STATE,           // Ready to send new APS Data Request
  APP_DATA_TRANSMISSION_STOP_STATE             // Inter-frame delay
} AppDataTransmissionState_t;


#endif // _CONFIGURATION_H_
