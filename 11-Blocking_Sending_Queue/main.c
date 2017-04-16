#include "main.h"

#define mainSENDER_1		1
#define mainSENDER_2		2

/* The tasks to be created.  Two instances are created of the sender task while
only a single instance is created of the receiver task. */
static void vSenderTask( void *pvParameters );
static void vReceiverTask( void *pvParameters );

/*-----------------------------------------------------------*/

/* Declare a variable of type xQueueHandle.  This is used to store the queue
that is accessed by all three tasks. */
xQueueHandle xQueue;

/* Define the structure type that will be passed on the queue. */
typedef struct
{
	unsigned char ucValue;
	unsigned char ucSource;
} xData;

/* Declare two variables of type xData that will be passed on the queue. */
static const xData xStructsToSend[ 2 ] =
{
	{ 100, mainSENDER_1 }, /* Used by Sender1. */
	{ 200, mainSENDER_2 }  /* Used by Sender2. */
};
/*-----------------------------------------------------------*/
int main(void)
{
	SystemInit();
	TM_DISCO_LedInit();
	TM_USART_Init(USART1, TM_USART_PinsPack_1, 115200);
	
	/* The queue is created to hold a maximum of 3 structures of type xData. */
    xQueue = xQueueCreate( 3, sizeof( xData ) );

	if( xQueue != NULL )
	{
		/* Create two instances of the task that will write to the queue.  The
		parameter is used to pass the structure that the task should write to the 
		queue, so one task will continuously send xStructsToSend[ 0 ] to the queue
		while the other task will continuously send xStructsToSend[ 1 ].  Both 
		tasks are created at priority 2 which is above the priority of the receiver. */
		xTaskCreate( vSenderTask, "Sender1", 240, ( void * ) &( xStructsToSend[ 0 ] ), 2, NULL );
		xTaskCreate( vSenderTask, "Sender2", 240, ( void * ) &( xStructsToSend[ 1 ] ), 2, NULL );

		/* Create the task that will read from the queue.  The task is created with
		priority 1, so below the priority of the sender tasks. */
		xTaskCreate( vReceiverTask, "Receiver", 240, NULL, 1, NULL );

		/* Start the scheduler so the created tasks start executing. */
		vTaskStartScheduler();
	}
	else
	{
		/* The queue could not be created. */
	}
		
    /* If all is well we will never reach here as the scheduler will now be
    running the tasks.  If we do reach here then it is likely that there was 
    insufficient heap memory available for a resource to be created. */
	for( ;; );
}

/*-----------------------------------------------------------*/

static void vSenderTask( void *pvParameters )
{
portBASE_TYPE xStatus;
const portTickType xTicksToWait = 100 / portTICK_RATE_MS;

	/* As per most tasks, this task is implemented within an infinite loop. */
	for( ;; )
	{
		/* The first parameter is the queue to which data is being sent.  The 
		queue was created before the scheduler was started, so before this task
		started to execute.

		The second parameter is the address of the structure being sent.  The
		address is passed in as the task parameter. 

		The third parameter is the Block time - the time the task should be kept
		in the Blocked state to wait for space to become available on the queue
		should the queue already be full.  A block time is specified as the queue
		will become full.  Items will only be removed from the queue when both
		sending tasks are in the Blocked state.. */
		xStatus = xQueueSendToBack( xQueue, pvParameters, xTicksToWait );

		if( xStatus != pdPASS )
		{
			/* We could not write to the queue because it was full - this must
			be an error as the receiving task should make space in the queue
			as soon as both sending tasks are in the Blocked state. */
			TM_USART_Puts(USART1, "Could not send to the queue.\n" );
		}

		/* Allow the other sender task to execute. */
		taskYIELD();
	}
}
/*-----------------------------------------------------------*/

static void vReceiverTask( void *pvParameters )
{
/* Declare the structure that will hold the values received from the queue. */
xData xReceivedStructure;
portBASE_TYPE xStatus;
	char buffer[50];

	/* This task is also defined within an infinite loop. */
	for( ;; )
	{
		/* As this task only runs when the sending tasks are in the Blocked state, 
		and the sending tasks only block when the queue is full, this task should
		always find the queue to be full.  3 is the queue length. */
		if( uxQueueMessagesWaiting( xQueue ) != 3 )
		{
			TM_USART_Puts(USART1, "Queue should have been full!\n" );
		}

		/* The first parameter is the queue from which data is to be received.  The
		queue is created before the scheduler is started, and therefore before this
		task runs for the first time.

		The second parameter is the buffer into which the received data will be
		placed.  In this case the buffer is simply the address of a variable that
		has the required size to hold the received structure. 

		The last parameter is the block time - the maximum amount of time that the
		task should remain in the Blocked state to wait for data to be available 
		should the queue already be empty.  A block time is not necessary as this
		task will only run when the queue is full so data will always be available. */
		xStatus = xQueueReceive( xQueue, &xReceivedStructure, 0 );

		if( xStatus == pdPASS )
		{
			/* Data was successfully received from the queue, print out the received
			value and the source of the value. */
			if( xReceivedStructure.ucSource == mainSENDER_1 )
			{
				sprintf(buffer, "From Sender 1 = %d\n", xReceivedStructure.ucValue );
				TM_USART_Puts(USART1, buffer );
			}
			else
			{
				sprintf(buffer, "From Sender 2 = %d\n", xReceivedStructure.ucValue );
				TM_USART_Puts(USART1, buffer );
			}
		}
		else
		{
			/* We did not receive anything from the queue.  This must be an error 
			as this task should only run when the queue is full. */
			TM_USART_Puts(USART1, "Could not receive from the queue.\n" );
		}
	}
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* This function will only be called if an API call to create a task, queue
	or semaphore fails because there is too little heap RAM remaining - and
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName)
{
	/* This function will only be called if a task overflows its stack.  Note
	that stack overflow checking does slow down the context switch
	implementation and will only be performed if configCHECK_FOR_STACK_OVERFLOW
	is set to either 1 or 2 in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* This example does not use the idle hook to perform any processing.  The
	idle hook will only be called if configUSE_IDLE_HOOK is set to 1 in 
	FreeRTOSConfig.h. */
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	/* This example does not use the tick hook to perform any processing.   The
	tick hook will only be called if configUSE_TICK_HOOK is set to 1 in
	FreeRTOSConfig.h. */
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif
