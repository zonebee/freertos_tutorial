#include "main.h"

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
	
	/* Note that this project uses the heap_2.c sample memory allocator, as
	this implements vPortFree() as well as pvPortMalloc().  The free function
	is required to release heap memory when a task is deleted.

	Create the first task at priority 1.  This time the task parameter is
	not used and is set to NULL.  The task handle is also not used so likewise 
	is also set to NULL. */
	xTaskCreate( vTask1, "Task 1", 240, NULL, 1, NULL );
         /* The task is created at priority 1 ^. */

	/* Start the scheduler so our tasks start executing. */
	vTaskStartScheduler();	
	
	for( ;; );
}

/*-----------------------------------------------------------*/
void vTask1( void *pvParameters )
{
const portTickType xDelay100ms = 100 / portTICK_RATE_MS;

	for( ;; )
	{
		/* Print out the name of this task. */
		TM_USART_Puts(USART1, "Task1 is running\n" );

		/* Create task 2 at a higher priority.  Again the task parameter is not 
		used so is set to NULL - BUT this time we want to obtain a handle to the 
		task so pass in the address of the xTask2Handle variable. */
		xTaskCreate( vTask2, "Task 2", 240, NULL, 2, &xTask2Handle );
			/* The task handle is the last parameter ^^^^^^^^^^^^^ */

		/* Task2 has/had the higher priority, so for Task1 to reach here Task2
		must have already executed and deleted itself.  Delay for 100 
		milliseconds. */
		vTaskDelay( xDelay100ms );
	}
}

/*-----------------------------------------------------------*/

void vTask2( void *pvParameters )
{
	/* Task2 does nothing but delete itself.  To do this it could call vTaskDelete()
	using a NULL parameter, but instead and purely for demonstration purposes it
	instead calls vTaskDelete() with its own task handle. */
	TM_USART_Puts(USART1, "Task2 is running and about to delete itself\n" );
	vTaskDelete( xTask2Handle );
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
