#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

GPIO_InitTypeDef GPIO_InitStructure;

int main(void)
{
    volatile uint32_t i;
    static int greenledval = 0;

    /* Enable GPIOC peripheral clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    /* Configure PC6 as output push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    while (1)
    {
        GPIO_WriteBit(
            GPIOC,
            GPIO_Pin_6,
            (greenledval) ? Bit_SET : Bit_RESET
        );

        greenledval = 1 - greenledval;

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
