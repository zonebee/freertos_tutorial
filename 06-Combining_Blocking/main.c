#include "main.h"

/* The task functions. */
void vContinuousProcessingTask( void *pvParameters );
void vPeriodicTask( void *pvParameters );

/* Define the strings that will be passed in as the task parameters.  These are
defined const and off the stack to ensure they remain valid when the tasks are
executing. */
const char *pcTextForTask1 = "Continuous task 1 running\n";
const char *pcTextForTask2 = "Continuous task 2 running\n";
const char *pcTextForPeriodicTask = "Periodic task is running\n";

/*-----------------------------------------------------------*/

int main(void)
{
	SystemInit();
	TM_DISCO_LedInit();
	TM_USART_Init(USART1, TM_USART_PinsPack_1, 115200);
	/* Create two instances of the continuous processing task, both at priority	1. */
	xTaskCreate( vContinuousProcessingTask, "Task 1", 240, (void*)pcTextForTask1, 1, NULL );
	xTaskCreate( vContinuousProcessingTask, "Task 2", 240, (void*)pcTextForTask2, 1, NULL );

	/* Create one instance of the periodic task at priority 2. */
	xTaskCreate( vPeriodicTask, "Task 3", 240, (void*)pcTextForPeriodicTask, 2, NULL );

	TM_USART_Puts(USART1,"fished\n" );
	/* Start the scheduler so our tasks start executing. */
	vTaskStartScheduler();
	
}

/*-----------------------------------------------------------*/

void vContinuousProcessingTask( void *pvParameters )
{
char *pcTaskName;
//volatile unsigned long ul;

	/* The string to print out is passed in via the parameter.  Cast this to a
	character pointer. */
	pcTaskName = ( char * ) pvParameters;

	/* As per most tasks, this task is implemented in an infinite loop. */
	for( ;; )
	{
		/* Print out the name of this task.  This task just does this repeatedly
		without ever blocking or delaying. */
		TM_USART_Puts(USART1, pcTaskName);

		/* A null loop has been inserted just to slow down the rate at which
		messages are sent down the debug link to the console.  Without this
		messages print out too quickly for the debugger display and controls
		to keep up.  For clarity this null loop is not shown in the eBook text
		as it is not relevant to the behaviour being demonstrated. */
//		for( ul = 0; ul < 0xfff; ul++ )
//		{
//			__nop();
//		}
	}
}
/*-----------------------------------------------------------*/

void vPeriodicTask( void *pvParameters )
{
portTickType xLastWakeTime;

	/* The xLastWakeTime variable needs to be initialized with the current tick
	count.  Note that this is the only time we access this variable.  From this
	point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
	API function. */
	xLastWakeTime = xTaskGetTickCount();

	/* As per most tasks, this task is implemented in an infinite loop. */
	for( ;; )
	{
		/* Print out the name of this task. */
		TM_USART_Puts(USART1,"Periodic task is running..........\n" );

		/* We want this task to execute exactly every 10 milliseconds. */
		vTaskDelayUntil( &xLastWakeTime, ( 10 / portTICK_RATE_MS ) );
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
