#include "main.h"

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0xfffff )
/* The task functions. */
void vTask1( void *pvParameters );
void vTask2( void *pvParameters );

/*-----------------------------------------------------------*/
int main(void)
{
	SystemInit();
	TM_DISCO_LedInit();
	TM_USART_Init(USART1, TM_USART_PinsPack_1, 115200);
	
	/* Create one of the two tasks. */
	xTaskCreate(	vTask1,		/* Pointer to the function that implements the task. */
					"Task 1",	/* Text name for the task.  This is to facilitate debugging only. */
					200,		/* Stack depth in words. */
					NULL,		/* We are not using the task parameter. */
					1,			/* This task will run at priority 1. */
					NULL );		/* We are not using the task handle. */

	/* Create the other task in exactly the same way. */
	xTaskCreate( vTask2, "Task 2", 200, NULL, 1, NULL );

	/* Start the scheduler so our tasks start executing. */
	vTaskStartScheduler();
}

/*-----------------------------------------------------------*/

void vTask1( void *pvParameters )
{
	char *pcTaskName = "Task 1 is running\r\n";
	volatile unsigned long ul;

	/* As per most tasks, this task is implemented in an infinite loop. */
	for( ;; )
	{
		/* Print out the name of this task. */
		TM_USART_Puts(USART1, pcTaskName );

		/* Delay for a period. */
		for( ul = 0; ul < mainDELAY_LOOP_COUNT; ul++ )
		{
			/* This loop is just a very crude delay implementation.  There is
			nothing to do in here.  Later exercises will replace this crude
			loop with a proper delay/sleep function. */
		}
	}
}
/*-----------------------------------------------------------*/

void vTask2( void *pvParameters )
{
	char *pcTaskName = "Task 2 is running\r\n";
	volatile unsigned long ul;

	/* As per most tasks, this task is implemented in an infinite loop. */
	for( ;; )
	{
		/* Print out the name of this task. */
		TM_USART_Puts(USART1, pcTaskName );

		/* Delay for a period. */
		for( ul = 0; ul < mainDELAY_LOOP_COUNT; ul++ )
		{
			/* This loop is just a very crude delay implementation.  There is
			nothing to do in here.  Later exercises will replace this crude
			loop with a proper delay/sleep function. */
		}
	}
}

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
