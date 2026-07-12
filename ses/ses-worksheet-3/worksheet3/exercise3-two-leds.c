#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

GPIO_InitTypeDef GPIO_InitStructure;

int main(void)
{
    volatile uint32_t i;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    while (1)
    {
        /* Green ON, Yellow OFF */
        GPIO_WriteBit(GPIOC, GPIO_Pin_6, Bit_SET);
        GPIO_WriteBit(GPIOC, GPIO_Pin_7, Bit_RESET);

        i = 8000000;
        while (--i);

        /* Green OFF, Yellow ON */
        GPIO_WriteBit(GPIOC, GPIO_Pin_6, Bit_RESET);
        GPIO_WriteBit(GPIOC, GPIO_Pin_7, Bit_SET);

        i = 8000000;
        while (--i);
    }
}
#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{
    while (1)
    {
    }
}

#endif
