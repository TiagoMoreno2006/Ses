#include <FreeRTOS.h>
#include <task.h>
#include <partest.h>
#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
volatile int green_enabled = 1;
volatile int orange_enabled = 1;
void configure_buttons(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC,
        ENABLE
    );
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}
void vGreenToggleTask(void *pvParameters)
{
    (void)pvParameters;
    for (;;)
    {
        if (green_enabled)
        {
            vParTestToggleLED(0);
        }
        else
        {
            vParTestSetLED(0, pdFALSE);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
void vOrangeToggleTask(void *pvParameters)
{
    (void)pvParameters;
    for (;;)
    {
        if (orange_enabled)
        {
            vParTestToggleLED(1);
        }
        else
        {
            vParTestSetLED(1, pdFALSE);
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
void vButtonGreenTask(void *pvParameters)
{
    BitAction previous_state = Bit_RESET;
    BitAction current_state;
    (void)pvParameters;
    for (;;)
    {
        current_state = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
        if ((current_state == Bit_SET) &&
            (previous_state == Bit_RESET))
        {
            green_enabled = !green_enabled;
        }
        previous_state = current_state;
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}
void vButtonOrangeTask(void *pvParameters)
{
    BitAction previous_state = Bit_RESET;
    BitAction current_state;
    (void)pvParameters;
    for (;;)
    {
        current_state = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13);
        if ((current_state == Bit_SET) &&
            (previous_state == Bit_RESET))
        {
            orange_enabled = !orange_enabled;
        }
        previous_state = current_state;
        vTaskDelay(50 / portTICK_PERIOD_MS);
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
    configure_buttons();
    xTaskCreate(
        vGreenToggleTask,
        "GreenLED",
        128,
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL
    );
    xTaskCreate(
        vOrangeToggleTask,
        "OrangeLED",
        128,
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL
    );
    xTaskCreate(
        vButtonGreenTask,
        "ButtonGreen",
        128,
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL
    );
    xTaskCreate(
        vButtonOrangeTask,
        "ButtonOrange",
        128,
        NULL,
        tskIDLE_PRIORITY + 1,
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
