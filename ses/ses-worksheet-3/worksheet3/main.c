#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

GPIO_InitTypeDef GPIO_InitStructure;

#define DEBOUNCE_THRESHOLD 50000

int main(void)
{
    int GreenOn = 1;
    int YellowOn = 0;

    int rawButtonState;
    int stableButtonState = 0;
    int lastRawState = 0;
    int debounceCounter = 0;

    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC,
        ENABLE
    );

    /* LEDs: PC6 and PC7 */
    GPIO_InitStructure.GPIO_Pin =
        GPIO_Pin_6 | GPIO_Pin_7;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* WKUP button: PA0 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_WriteBit(GPIOC, GPIO_Pin_6, Bit_SET);
    GPIO_WriteBit(GPIOC, GPIO_Pin_7, Bit_RESET);

    while (1)
    {
        rawButtonState =
            GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);

        if (rawButtonState == lastRawState)
        {
            if (debounceCounter < DEBOUNCE_THRESHOLD)
            {
                debounceCounter++;
            }
        }
        else
        {
            debounceCounter = 0;
            lastRawState = rawButtonState;
        }

        if (debounceCounter == DEBOUNCE_THRESHOLD)
        {
            if (stableButtonState != rawButtonState)
            {
                stableButtonState = rawButtonState;

                if (stableButtonState == 1)
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
            }
        }
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
