//////////////////////////////////////////////////////////////////////////////////
/// \file mac_sender.c
/// \brief MAC sender thread
/// \author Pascal Sartoretti (pascal dot sartoretti at hevs dot ch)
/// \version 1.0 - original
/// \date  2018-02
//////////////////////////////////////////////////////////////////////////////////
#include "stm32f7xx_hal.h"

#include <stdio.h>
#include <string.h>
#include "main.h"
#include "ext_led.h"

#include "cmsis_os2.h"


//////////////////////////////////////////////////////////////////////////////////
// THREAD MAC RECEIVER
//////////////////////////////////////////////////////////////////////////////////
void MacSender(void *argument)
{
	struct queueMsg_t msg;		// Message from PHY
	osStatus_t statusMsg;			// Status of queue get
	
	for(;;)
	{
		statusMsg = osMessageQueueGet(queue_macS_id,&msg,NULL,0);		// Get the queue of macR
		if(statusMsg == osOK)			// if the get is ok
		{
			if(msg.type == NEW_TOKEN)		// if we want a new token
			{
				osMessageQueuePut(queue_phyS_id, &gTokenInterface,NULL, 0);
				Ext_LED_PWM(4,100);	
			}
		}
	}
}


