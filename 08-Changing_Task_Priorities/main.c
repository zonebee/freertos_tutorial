#include "main.h"
/******************************************
*Note:khi set muc do uu tien se chay qua task uu tien khac lien 



*****************/




/* The two task functions. */
void vTask1( void *pvParameters );
void vTask2( void *pvParameters );

/* Used to hold the handle of Task2. */
xTaskHandle xTask2Handle;

/*-----------------------------------------------------------*/

int main(void)
{
	SystemInit();
	TM_DISCO_LedInit();
	TM_USART_Init(USART1, TM_USART_PinsPack_1, 115200);
	/* Create the first task at priority 1... */
	xTaskCreate( vTask1, "Task 1", 240, NULL, 2, NULL );
         /* The task is created at priority 2 ^. */

	/* Create the second task at priority 1 - which is lower than the priority
	given to Task1.  Again the task parameter is not used so is set to NULL -
	BUT this time we want to obtain a handle to the task so pass in the address
	of the xTask2Handle variable. */
	xTaskCreate( vTask2, "Task 2", 240, NULL, 1, &xTask2Handle );
        /* The task handle is the last parameter ^^^^^^^^^^^^^ */

	/* Start the scheduler so our tasks start executing. */
	vTaskStartScheduler();	
}

/*-----------------------------------------------------------*/

void vTask1( void *pvParameters )
{
unsigned portBASE_TYPE uxPriority;

	/* This task will always run before Task2 as it has the higher priority.
	Neither Task1 nor Task2 ever block so both will always be in either the
	Running or the Ready state.

	Query the priority at which this task is running - passing in NULL means
	"return our own priority". */
	uxPriority = uxTaskPriorityGet( NULL );
char buffer [50];
	for( ;; )
	{
		/* Print out the name of this task. */
		TM_USART_Puts(USART1, "Task1 is running\n");
		/* Setting the Task2 priority above the Task1 priority will cause
		Task2 to immediately start running (as then Task2 will have the higher 
		priority of the    two created tasks). */
		TM_USART_Puts(USART1, "About to raise the Task2 priority\n" );
		
		vTaskPrioritySet( xTask2Handle, ( uxPriority + 1 ) );
		sprintf (buffer, "task1 z%ld\n", uxPriority);
		TM_USART_Puts(USART1, buffer);

		/* Task1 will only run when it has a priority higher than Task2.
		Therefore, for this task to reach this point Task2 must already have
		executed and set its priority back down to 0. */
	}
}

/*-----------------------------------------------------------*/

void vTask2( void *pvParameters )
{
unsigned portBASE_TYPE uxPriority;

	/* Task1 will always run before this task as Task1 has the higher priority.
	Neither Task1 nor Task2 ever block so will always be in either the
	Running or the Ready state.

	Query the priority at which this task is running - passing in NULL means
	"return our own priority". */
	uxPriority = uxTaskPriorityGet( NULL );
char buffer [50];
	for( ;; )
	{
		/* For this task to reach this point Task1 must have already run and
		set the priority of this task higher than its own.

		Print out the name of this task. */
		TM_USART_Puts(USART1,"Task2 is running\n" );

		/* Set our priority back down to its original value.  Passing in NULL
		as the task handle means "change our own priority".  Setting the
		priority below that of Task1 will cause Task1 to immediately start
		running again. */
		TM_USART_Puts(USART1,"About to lower the Task2 priority\n" );
		sprintf (buffer, "task2 z%ld\n", uxPriority);
		TM_USART_Puts(USART1, buffer);
		vTaskPrioritySet( NULL, ( uxPriority - 2 ) );//do hien tai task2 nen de nulll
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

