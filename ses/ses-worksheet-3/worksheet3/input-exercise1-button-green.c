#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

GPIO_InitTypeDef GPIO_InitStructure;

int main(void)
{
    int GreenOn = 1;
    int YellowOn = 0;
    int previousButtonState = 0;
    int currentButtonState;

    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC,
        ENABLE
    );

    /* PC6 and PC7 outputs */
    GPIO_InitStructure.GPIO_Pin =
        GPIO_Pin_6 | GPIO_Pin_7;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* PA0 WKUP input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_WriteBit(GPIOC, GPIO_Pin_6, Bit_SET);
    GPIO_WriteBit(GPIOC, GPIO_Pin_7, Bit_RESET);

    while (1)
    {
        currentButtonState =
            GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);

        if ((currentButtonState == 1) &&
            (previousButtonState == 0))
        {
            GreenOn = 1 - GreenOn;
            YellowOn = 1 - YellowOn;

            GPIO_WriteBit(
                GPIOC,
                GPIO_Pin_6,
                (GreenOn) ? Bit_SET : Bit_RESET
            );

            GPIO_WriteBit(
                GPIOC,
                GPIO_Pin_7,
                (YellowOn) ? Bit_SET : Bit_RESET
            );
        }

        previousButtonState = currentButtonState;
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
