#include <FreeRTOS.h>
#include <stm32f10x.h>
#include <partest.h>
#include <task.h>
#define mainGREEN_LED_PRIORITY  (tskIDLE_PRIORITY + 1)
#define mainORANGE_LED_PRIORITY (tskIDLE_PRIORITY)
extern int __io_putchar(int);
extern void COMPortInit(void);
void vGreenStarvationTask(void *pvParameters)
{
    volatile unsigned long i;
    (void)pvParameters;
    for (;;)
    {
        vParTestToggleLED(0);
        __io_putchar('G');
        for (i = 0; i < 3000000; i++)
        {
        }
    }
}
void vOrangeVictimTask(void *pvParameters)
{
    (void)pvParameters;
    for (;;)
    {
        vParTestToggleLED(1);
        __io_putchar('O');
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    (void)pxTask;
    (void)pcTaskName;
    for (;;)
    {
    }
}
int main(void)
{
    vParTestInitialise();
    COMPortInit();
    xTaskCreate(
        vGreenStarvationTask,
        "GreenStarvation",
        128,
        NULL,
        mainGREEN_LED_PRIORITY,
        NULL
    );
    xTaskCreate(
        vOrangeVictimTask,
        "OrangeVictim",
        128,
        NULL,
        mainORANGE_LED_PRIORITY,
        NULL
    );
    vTaskStartScheduler();
    for (;;)
    {
    }
    return 0;
}
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file;
    (void)line;
    while (1)
    {
    }
}
#endif
