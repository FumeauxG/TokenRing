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
	struct queueMsg_t queueMsg;		// Message from PHY
	osStatus_t statusMsg;			// Status of queue get
	
	uint8_t* qPtr;
	uint8_t* msg;
	uint16_t checksum = 0;
	uint8_t length;
	
	for(;;)
	{
		//message = osMemoryPoolAlloc(memPool,osWaitForever);
		statusMsg = osMessageQueueGet(queue_macR_id,&queueMsg,NULL,0);		// Get the queue of macR
		//CheckRetCode(statusMsg,__LINE__,__FILE__,CONTINUE);
		
		if(statusMsg == osOK)			// if the get is ok
		{
			if(queueMsg.type == FROM_PHY)		// if the message is from the PHY
			{
				qPtr = queueMsg.anyPtr;
				if (qPtr[0] == TOKEN_TAG)
				{
					//frameType = isTOKEN;
					//i++;
				}
				else
				{
					if((qPtr[1] >> 3) == MYADDRESS) //if we are the destination
					{
						length = qPtr[2];
						//Calculate checksum
						for(int i = 0; i<length; i++)
						{
							checksum = (checksum + qPtr[3+i])%64;
						}
						
						//MALLOC
						msg = osMemoryPoolAlloc(memPool,osWaitForever);
						
						if((qPtr[3+length])>>2 == checksum)
						{
							
						}
						
						if((qPtr[0] >> 3) == MYADDRESS)	// if we are the source
						{
							//DATABACK
						}
						else
						{
							queueMsg.type = TO_PHY;
							osMessageQueuePut(queue_phyS_id, &queueMsg,NULL, 0);	// Send to next
						}					
					}
				}
					
				queueMsg.type = TO_PHY;
				osMessageQueuePut(queue_phyS_id, &queueMsg,NULL, 0);
			}
			
		}
		
	}
}


