#include "main.h"

/* The interrupt number to use for the software interrupt generation.  This
could be any unused number.  In this case the first chip level (non system)
interrupt is used, which happens to be the watchdog on the LPC1768. */
#define mainSW_INTERRUPT_ID		( ( IRQn_Type ) 0 )

/* Macro to force an interrupt. */
#define mainTRIGGER_INTERRUPT()	NVIC_SetPendingIRQ( mainSW_INTERRUPT_ID )

/* Macro to clear the same interrupt. */
#define mainCLEAR_INTERRUPT()	NVIC_ClearPendingIRQ( mainSW_INTERRUPT_ID )

/* The priority of the software interrupt.  The interrupt service routine uses
an (interrupt safe) FreeRTOS API function, so the priority of the interrupt must
be equal to or lower than the priority set by
configMAX_SYSCALL_INTERRUPT_PRIORITY - remembering that on the Cortex M3 high
numeric values represent low priority values, which can be confusing as it is
counter intuitive. */
#define mainSOFTWARE_INTERRUPT_PRIORITY 		( 5 )

/* The tasks to be created. */
static void vIntegerGenerator( void *pvParameters );
static void vStringPrinter( void *pvParameters );

/* Enable the software interrupt and set its priority. */
static void prvSetupSoftwareInterrupt( void );

/* The service routine for the interrupt.  This is the interrupt that the
task will be synchronized with. */
//void vSoftwareInterruptHandler( void );
void WWDG_IRQHandler(void);
/*-----------------------------------------------------------*/

unsigned long ulNext = 0;
unsigned long ulCount;
unsigned long ul[ 100 ];

/* Declare two variables of type xQueueHandle.  One queue will be read from
within an ISR, the other will be written to from within an ISR. */
xQueueHandle xIntegerQueue, xStringQueue;

/*-----------------------------------------------------------*/

int main(void)
{
	SystemInit();
	TM_DISCO_LedInit();
	TM_USART_Init(USART1, TM_USART_PinsPack_1, 115200);
	
	
    /* Before a queue can be used it must first be created.  Create both queues
	used by this example.  One queue can hold variables of type unsigned long,
	the other queue can hold variables of type char*.  Both queues can hold a
	maximum of 10 items.  A real application should check the return values to
	ensure the queues have been successfully created. */
    xIntegerQueue = xQueueCreate( 10, sizeof( unsigned long ) );
	xStringQueue = xQueueCreate( 10, sizeof( char * ) );

   	/* Enable the software interrupt and set its priority. */
   	prvSetupSoftwareInterrupt();

	/* Create the task that uses a queue to pass integers to the interrupt service
	routine.  The task is created at priority 1. */
	xTaskCreate( vIntegerGenerator, "IntGen", 240, NULL, 1, NULL );

	/* Create the task that prints out the strings sent to it from the interrupt
	service routine.  This task is created at the higher priority of 2. */
	xTaskCreate( vStringPrinter, "String", 240, NULL, 2, NULL );

	/* Start the scheduler so the created tasks start executing. */
	vTaskStartScheduler();
	
    /* If all is well we will never reach here as the scheduler will now be
    running the tasks.  If we do reach here then it is likely that there was
    insufficient heap memory available for a resource to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

static void vIntegerGenerator( void *pvParameters )
{
portTickType xLastExecutionTime;
unsigned portLONG ulValueToSend = 0;
int i;

	/* Initialize the variable used by the call to vTaskDelayUntil(). */
	xLastExecutionTime = xTaskGetTickCount();

	for( ;; )
	{
		/* This is a periodic task.  Block until it is time to run again.
		The task will execute every 200ms. */
		vTaskDelayUntil( &xLastExecutionTime, 200 / portTICK_RATE_MS );

		/* Send an incrementing number to the queue five times.  These will be
		read from the queue by the interrupt service routine.  A block time is
		not specified. */
		for( i = 0; i < 5; i++ )
		{
			xQueueSendToBack( xIntegerQueue, &ulValueToSend, 0 );
			ulValueToSend++;
		}

		/* Force an interrupt so the interrupt service routine can read the
		values from the queue. */
		TM_USART_Puts(USART1, "Generator task - About to generate an interrupt.\r\n" );
		mainTRIGGER_INTERRUPT();
		TM_USART_Puts(USART1, "Generator task - Interrupt generated.\r\n\n" );
	}
}
/*-----------------------------------------------------------*/

static void vStringPrinter( void *pvParameters )
{
char *pcString;

	for( ;; )
	{
		/* Block on the queue to wait for data to arrive. */
		xQueueReceive( xStringQueue, &pcString, portMAX_DELAY );

		/* Print out the string received. */
		TM_USART_Puts(USART1, pcString );
	}
}
/*-----------------------------------------------------------*/

static void prvSetupSoftwareInterrupt( void )
{
	/* The interrupt service routine uses an (interrupt safe) FreeRTOS API
	function so the interrupt priority must be at or below the priority defined
	by configSYSCALL_INTERRUPT_PRIORITY. */
	NVIC_SetPriority( mainSW_INTERRUPT_ID, mainSOFTWARE_INTERRUPT_PRIORITY );

	/* Enable the interrupt. */
	NVIC_EnableIRQ( mainSW_INTERRUPT_ID );
}
/*-----------------------------------------------------------*/

void WWDG_IRQHandler(void)
{
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
static unsigned long ulReceivedNumber;

/* The strings are declared static const to ensure they are not allocated to the
interrupt service routine stack, and exist even when the interrupt service routine
is not executing. */
static const char *pcStrings[] =
{
    "String 0\r\n",
    "String 1\r\n",
    "String 2\r\n",
    "String 3\r\n"
};

    /* Loop until the queue is empty. */
    while( xQueueReceiveFromISR( xIntegerQueue, &ulReceivedNumber, &xHigherPriorityTaskWoken ) != errQUEUE_EMPTY )
    {
        /* Truncate the received value to the last two bits (values 0 to 3 inc.), then
        send the string    that corresponds to the truncated value to the other
        queue. */
        ulReceivedNumber &= 0x03;
        xQueueSendToBackFromISR( xStringQueue, &pcStrings[ ulReceivedNumber ], &xHigherPriorityTaskWoken );
    }

    /* Clear the software interrupt bit using the interrupt controllers
    Clear Pending register. */
    mainCLEAR_INTERRUPT();

    /* xHigherPriorityTaskWoken was initialised to pdFALSE.  It will have then
    been set to pdTRUE only if reading from or writing to a queue caused a task
    of equal or greater priority than the currently executing task to leave the
    Blocked state.  When this is the case a context switch should be performed.
    In all other cases a context switch is not necessary.

    NOTE: The syntax for forcing a context switch within an ISR varies between
    FreeRTOS ports.  The portEND_SWITCHING_ISR() macro is provided as part of
    the Cortex M3 port layer for this purpose.  taskYIELD() must never be called
    from an ISR! */
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
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

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName )
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
/*-----------------------------------------------------------*/

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
