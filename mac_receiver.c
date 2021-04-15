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
					queueMsg.type = TOKEN;
					osMessageQueuePut(queue_macS_id, &queueMsg, NULL, 0);
				}
				else
				{
					if((qPtr[0] >> 3) == MYADDRESS)	// if we are the source
					{
						if((qPtr[3+length]&2) == 2) // if R = 1
						{
							if((qPtr[3+length]&1) == 1) // if A = 1
							{
								//DATABACK
								queueMsg.type = DATABACK;
								osMessageQueuePut(queue_macS_id, &queueMsg, NULL, 0);
							}
							else
							{
								qPtr[3+length] = qPtr[3+length] - 2;	// Set read and ack to 0
								//DATA 
								queueMsg.type = TO_PHY;
								osMessageQueuePut(queue_phyS_id, &queueMsg, NULL, 0);
							}
						}
						else
						{
							//MAC_ERROR_INDICATION
							//DATABACK
							queueMsg.type = DATABACK;
							osMessageQueuePut(queue_macS_id, &queueMsg, NULL, 0);
						}
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
							
							if((qPtr[3+length])>>2 == checksum)
							{
								qPtr[3+length] = qPtr[3+length] + 3;	// Set read and ack to 1
								//DATA 
								queueMsg.type = TO_PHY;
								osMessageQueuePut(queue_phyS_id, &queueMsg, NULL, 0);
								
								//MAC_DATA_INDICATION
								msg = osMemoryPoolAlloc(memPool,osWaitForever);	//MALLOC
								queueMsg.type = DATA_IND;
								memcpy(msg, &qPtr[3], qPtr[2]);		// Copy the whole message
								msg[qPtr[2]] = '\0';	// Add \0 at the end
								queueMsg.anyPtr = msg;
								queueMsg.addr = (qPtr[0] >> 3);	//Source of message
								queueMsg.sapi = qPtr[0]&7;	// Source sapi
								
								if(queueMsg.sapi == TIME_SAPI)
								{
									osMessageQueuePut(queue_timeR_id, &queueMsg, NULL, 0);
								}
								else if(queueMsg.sapi == CHAT_SAPI)
								{
									osMessageQueuePut(queue_chatR_id, &queueMsg, NULL, 0);
								}
							}
							else
							{
								//R=1,A=0,
								qPtr[3+length] = qPtr[3+length] + 2;	// Set read to 1 and ack to 0
								//DATA
								queueMsg.type = TO_PHY;
								osMessageQueuePut(queue_phyS_id, &queueMsg, NULL, 0);
							}
						}
						else
						{
							//DATA
							queueMsg.type = TO_PHY;
							osMessageQueuePut(queue_phyS_id, &queueMsg, NULL, 0);
						}
					}
				}
					
				//queueMsg.type = TO_PHY;
				//osMessageQueuePut(queue_phyS_id, &queueMsg,NULL, 0);
			}
			
		}
		
	}
}


