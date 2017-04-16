#include "main.h"

#define B0 (1 << 0)
#define B1 (1 << 1)

static void write_task_1(void *pvParameters);
static void write_task_2(void *pvParameters);
static void read_task(void *pvParameters);

static EventGroupHandle_t event_group = NULL;

int main(void)
{
	
	SystemInit();
	TM_DISCO_LedInit();
	TM_USART_Init(USART1, TM_USART_PinsPack_1, 115200);
	
	
    event_group = xEventGroupCreate();
    xTaskCreate(write_task_1, "WRITE_TASK_1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(write_task_2, "WRITE_TASK_2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(read_task, "READ_TASK", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
    /* Start scheduling. */
    vTaskStartScheduler();
	
    /* If all is well we will never reach here as the scheduler will now be
    running the tasks.  If we do reach here then it is likely that there was
    insufficient heap memory available for a resource to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/
/*!
 * @brief write_task_1 function
 */
static void write_task_1(void *pvParameters)
{
    while (1)
    {
        xEventGroupSetBits(event_group, B0);
    }
}

/*!
 * @brief write_task_2 function
 */
static void write_task_2(void *pvParameters)
{
    while (1)
    {
        xEventGroupSetBits(event_group, B1);
    }
}

/*!
 * @brief read_task function
 */
static void read_task(void *pvParameters)
{
    EventBits_t event_bits;
    while (1)
    {
        event_bits = xEventGroupWaitBits(event_group,    /* The event group handle. */
                                         B0 | B1,        /* The bit pattern the event group is waiting for. */
                                         pdTRUE,         /* BIT_0 and BIT_4 will be cleared automatically. */
                                         pdFALSE,        /* Don't wait for both bits, either bit unblock task. */
                                         portMAX_DELAY); /* Block indefinitely to wait for the condition to be met. */

        if ((event_bits & (B0 | B1)) == (B0 | B1))
        {
            TM_USART_Puts(USART1,"Both bits are set.");
        }
        else if ((event_bits & B0) == B0)
        {
            TM_USART_Puts(USART1,"Bit B0 is set.\r\n");
        }
        else if ((event_bits & B1) == B1)
        {
            TM_USART_Puts(USART1,"Bit B1 is set.\r\n");
        }
    }
}

void vApplicationTickHook( void )
{
}
/*-----------------------------------------------------------*/


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
