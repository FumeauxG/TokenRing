//////////////////////////////////////////////////////////////////////////////////
/// \file mac_receiver.c
/// \brief MAC receiver thread
/// \author Pascal Sartoretti (sap at hevs dot ch)
/// \version 1.0 - original
/// \date  2018-02
//////////////////////////////////////////////////////////////////////////////////
#include "stm32f7xx_hal.h"

#include <stdio.h>
#include <string.h>
#include "main.h"

#include "cmsis_os2.h"


//////////////////////////////////////////////////////////////////////////////////
// THREAD MAC RECEIVER
//////////////////////////////////////////////////////////////////////////////////
void MacReceiver(void *argument)
{
	struct queueMsg_t msg;		// Message from PHY
	osStatus_t statusMsg;			// Status of queue get
	
	for(;;)
	{
		statusMsg = osMessageQueueGet(queue_macR_id,&msg,NULL,0);		// Get the queue of macR
		if(statusMsg == osOK)			// if the get is ok
		{
			if(msg.type == FROM_PHY)		// if the message is from the PHY
			{
				if(*(uint8_t*)msg.anyPtr == 0xFF)		// if it is a token
				{
					msg.type = TO_PHY;
					osMessageQueuePut(queue_phyS_id,&msg,1,0);	// Pour l'instant renvoie token sans rien faire
				}
			}
		}
	}
}


