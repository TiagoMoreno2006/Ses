# Secure Embedded Systems — Worksheet 7

## Programming Interrupts and Using Timers

**Student ID:** 24042392  
**Worksheet:** 7  
**Platform:** STM32 / Olimex STM32-P107  
**Toolchain:** `arm-none-eabi-gcc`, OpenOCD, Telnet, Minicom  
**UART configuration used:** USART2, 9600 baud, 8N1  

---

## 1. Overview

Worksheet 7 develops interrupt-driven programming on the ARM Cortex-M3 using the STM32 platform. The work progresses in the same order as the worksheet:

1. Timer-generated interrupts controlling LEDs.
2. External button interrupts controlling the green LED.
3. Interrupt-driven UART input and output.
4. Pulse Width Modulation (PWM) to fade and brighten the green LED.

The two credit exercises were not attempted.

The main objective was to move away from continuous hardware polling and instead allow peripherals to notify the processor when service is required.

---

## 2. Exercise 1 — Timer Interrupts

### Requirement

The first exercise asks for the supplied `TimerInt.c` program to be built and executed. The timer values must then be changed so that the LEDs flash at a visibly controlled rate of approximately one second.

### Implementation

The program uses:

- `TIM2` as the hardware timer.
- `TIM2_IRQn` in the NVIC.
- `TIM2_IRQHandler()` as the interrupt service routine.
- GPIO outputs on `PC6` and `PC7`.

The main program performs the initial configuration and then remains in an empty loop. The LED changes are therefore caused by the timer interrupt rather than by polling logic in `main()`.

The timer interrupt handler follows this logic:

```c
void TIM2_IRQHandler()
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

        GPIO_WriteBit(
            GPIOC,
            GPIO_Pin_6,
            (greenledval) ? Bit_SET : Bit_RESET
        );

        GPIO_WriteBit(
            GPIOC,
            GPIO_Pin_7,
            (greenledval) ? Bit_SET : Bit_RESET
        );

        greenledval = 1 - greenledval;
    }
}
```

### Result

Two versions were demonstrated:

- the original fast-blinking version;
- a modified controlled-blinking version with a much slower and clearly visible interval.

### Evidence

- [Fast blink demonstration](evidence/fast%20blink%20exercise%201(1).mp4)
- [Controlled blink demonstration](evidence/controlled%20blink%20exercise%201(1).mp4)

**Status: Complete**

---

## 3. Exercise 2 — External Button Interrupt

### Requirement

The second exercise asks for a button on GPIOC to be connected to an external interrupt and for the interrupt service routine to control the green LED.

### Implementation

The implementation uses:

- `PC13` as the input button.
- `EXTI_Line13` as the external interrupt line.
- `EXTI15_10_IRQn` in the NVIC.
- `EXTI15_10_IRQHandler()` as the interrupt service routine.
- `PC6` as the green LED output.

The button configuration connects GPIOC pin 13 to EXTI line 13:

```c
GPIO_EXTILineConfig(
    GPIO_PortSourceGPIOC,
    GPIO_PinSource13
);

EXTI_InitStructure.EXTI_Line = EXTI_Line13;
EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
EXTI_InitStructure.EXTI_LineCmd = ENABLE;
EXTI_Init(&EXTI_InitStructure);
```

The NVIC is configured for the grouped EXTI interrupt:

```c
NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStructure);
```

The ISR clears the pending interrupt and toggles the green LED:

```c
void EXTI15_10_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line13) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line13);

        GPIO_WriteBit(
            GPIOC,
            GPIO_Pin_6,
            greenledval ? Bit_SET : Bit_RESET
        );

        greenledval = 1 - greenledval;
    }
}
```

The `while (1)` loop in `main()` contains no button polling. The button event is handled asynchronously by the ISR.

### Result

Each button press changes the state of the green LED:

- first press: LED changes state;
- next press: LED changes back;
- subsequent presses continue toggling the LED.

### Evidence

- [Button interrupt source code](evidence/button%20code.png)

**Status: Complete**

> Note: the source-code evidence is present. A short physical runtime video of the button toggling the green LED would make the evidence set stronger, but the exercise implementation itself is complete.

---

## 4. Exercise 3 — Interrupt-Driven UART Input and Output

### Requirement

The worksheet first introduces interrupt-driven UART receive operation and then asks for output to be made interrupt-driven as well.

The USART interrupt routine must therefore check both:

- `USART_IT_RXNE` for received data;
- `USART_IT_TXE` for transmission readiness.

### Implementation

The implementation uses USART2 with shared state between the main program and the interrupt handler:

```c
volatile int gotit = 0;
volatile int rxdata = 0;

volatile int tx_pending = 0;
volatile int txdata = 0;
```

The `volatile` qualifier is required because these values can change asynchronously inside an interrupt service routine.

### Interrupt-Driven Transmission

The transmission function does not directly send the byte. Instead, it stores the byte and enables the USART transmit interrupt:

```c
void outbyte_interrupt(int c)
{
    while (tx_pending)
    {
    }

    txdata = c;
    tx_pending = 1;

    USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}
```

### Combined RX/TX Interrupt Service Routine

The same ISR checks both RX and TX conditions.

Receive side:

```c
if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
{
    rxdata = (int)(USART_ReceiveData(USART2) & 0xFF);
    gotit = 1;

    USART_ClearITPendingBit(USART2, USART_IT_RXNE);
}
```

Transmit side:

```c
if (USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
{
    if (tx_pending)
    {
        USART_SendData(
            USART2,
            (uint16_t)(txdata & 0xFF)
        );

        tx_pending = 0;
    }

    USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
}
```

Disabling `USART_IT_TXE` after the pending character has been sent prevents the processor from continuously receiving transmit-empty interrupts when there is no data to send.

### Main Program

The main program checks only the software flag set by the ISR:

```c
int main(void)
{
    COMPortInit();

    while (1)
    {
        if (gotit == 1)
        {
            outbyte_interrupt(rxdata);
            gotit = 0;
        }
    }
}
```

The hardware receive flag is therefore handled by the interrupt routine rather than polled directly by `main()`.

### Result

Characters typed in Minicom were:

1. received by USART2;
2. captured by the `RXNE` interrupt;
3. passed to the main program through `rxdata` and `gotit`;
4. queued for transmission;
5. returned through the `TXE` interrupt.

The UART test was performed using:

```bash
sudo minicom -o -D /dev/ttyUSB1 -b 9600 -8
```

The serial device mapping used during this worksheet was:

```text
/dev/ttyUSB0  -> Olimex FTDI JTAG/OpenOCD
/dev/ttyUSB1  -> CH341 USB-to-serial adapter
```

### Evidence

- [USART interrupt source code](evidence/usart%20code.png)
- [USART interrupt runtime demonstration](evidence/usart.mp4)

**Status: Complete**

---

## 5. Exercise 4 — PWM LED Brightness Control

### Requirement

The final standard exercise asks for PWM to be used so that the green LED gradually fades and brightens.

The implementation must configure the timer, GPIO alternate function and PWM channel, then repeatedly change the duty cycle.

### Implementation

The solution uses:

- `TIM3`;
- PWM channel 1;
- `PC6`;
- `GPIO_FullRemap_TIM3`;
- `TIM_OCMode_PWM1`;
- `TIM3->CCR1` to update the duty cycle.

The timer and PWM configuration includes:

```c
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

RCC_APB2PeriphClockCmd(
    RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO,
    ENABLE
);

GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
GPIO_Init(GPIOC, &GPIO_InitStructure);

GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
```

The timer is configured with:

```c
TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV4;
TIM_TimeBaseInitStruct.TIM_Period = 1000 - 1;
TIM_TimeBaseInitStruct.TIM_Prescaler = 240 - 1;
```

PWM mode is enabled with:

```c
TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
TIM_OCInitStruct.TIM_Pulse = 0;

TIM_OC1Init(TIM3, &TIM_OCInitStruct);
TIM_Cmd(TIM3, ENABLE);
```

### Brightness Control

The program changes the PWM duty cycle between minimum and maximum values:

```c
while (1)
{
    if (brightness >= 999)
    {
        direction = -1;
    }

    if (brightness <= 0)
    {
        direction = 1;
    }

    brightness += direction;
    TIM3->CCR1 = brightness;

    for (i = 0; i < 0x4000; i++)
    {
    }
}
```

This creates a repeating sequence:

```text
minimum brightness
        ↓
gradual increase
        ↓
maximum brightness
        ↓
gradual decrease
        ↓
minimum brightness
        ↓
repeat
```

### Result

The green LED was observed gradually increasing and decreasing in brightness under PWM control.

### Evidence

- [PWM source code](evidence/led%20code.png)
- [PWM LED brightness demonstration](evidence/ledbrightness.mp4)

**Status: Complete**

---

## 6. Build and Flash Workflow

The general build process used throughout the worksheet was:

```bash
make clean
make
```

The generated executable was:

```text
demo.elf
```

OpenOCD was started with:

```bash
openocd -f openocd.cfg
```

A Telnet session was then opened:

```bash
telnet localhost 4444
```

Typical programming commands were:

```text
reset halt
flash write_image erase demo.elf 0
reset run
```

For exercises where separate executable names were preserved, the corresponding `.elf` file was flashed instead.

---

## 7. Problems Encountered and Resolved

### 7.1 Missing STM32 Header Includes

During Exercise 2, the compiler initially reported errors such as:

```text
unknown type name 'GPIO_InitTypeDef'
'RCC_APB2Periph_GPIOC' undeclared
'EXTI_Line13' undeclared
unknown type name 'NVIC_InitTypeDef'
```

This was caused by missing STM32 header includes.

The required headers were added:

```c
#include <stdint.h>
#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_exti.h>
#include <misc.h>
```

After this correction, the program compiled successfully.

### 7.2 Accidental JTAG Disconnection

At one stage, OpenOCD failed with messages such as:

```text
Error: no device found
Error: unable to open ftdi device
LIBUSB_ERROR_NO_DEVICE
```

The Olimex JTAG adapter had been accidentally unplugged. Reconnecting it restored OpenOCD operation.

### 7.3 USB Device Enumeration

After reconnecting USB devices, Linux reported:

```text
FTDI USB Serial Device converter now attached to ttyUSB0
ch341-uart converter now attached to ttyUSB1
```

This confirmed:

```text
/dev/ttyUSB0 = Olimex JTAG/OpenOCD
/dev/ttyUSB1 = UART/Minicom
```

### 7.4 UART Local Echo

During the UART exercise, repeated characters could be caused by both local terminal echo and the STM32 echoing the character back.

Disabling local echo in Minicom allowed the returned data from the board to be distinguished from locally displayed keyboard input.

---

## 8. Requirements and Evidence Matrix

| Worksheet requirement | Implementation | Evidence | Status |
|---|---|---|---|
| Build and run timer interrupt program | TIM2 interrupt toggles PC6 and PC7 | Two runtime videos | Complete |
| Change timer behaviour for controlled flashing | Modified slower timer behaviour demonstrated | Controlled blink video | Complete |
| Configure GPIOC button interrupt | PC13 mapped to EXTI line 13 | Button source screenshot | Complete |
| Use ISR to control green LED | `EXTI15_10_IRQHandler()` toggles PC6 | Button source screenshot | Complete |
| Build interrupt-driven UART input | RXNE interrupt receives data | USART source + video | Complete |
| Make UART output interrupt-driven | TXE interrupt sends queued byte | USART source + video | Complete |
| ISR checks both RX and TX flags | Combined `USART2_IRQHandler()` | USART source screenshot | Complete |
| Implement PWM LED fade | TIM3 PWM changes `CCR1` duty cycle | PWM source + video | Complete |
| Credit: buffered UART I/O | Not attempted | — | Optional |
| Credit: two LEDs fading oppositely | Not attempted | — | Optional |

---

## 9. Evidence Index

The following evidence files are used by this README:

```text
evidence/
├── fast blink exercise 1(1).mp4
├── controlled blink exercise 1(1).mp4
├── button code.png
├── usart code.png
├── usart.mp4
├── led code.png
└── ledbrightness.mp4
```

---

## 10. Final Status

All four standard Worksheet 7 exercises were completed:

- **Exercise 1:** Timer interrupt and controlled LED flashing — complete.
- **Exercise 2:** External button interrupt controlling the green LED — complete.
- **Exercise 3:** Interrupt-driven UART receive and transmit — complete.
- **Exercise 4:** PWM-based LED fading and brightening — complete.

The optional credit exercises were intentionally not attempted.

The worksheet demonstrates the progression from simple interrupt handling to timer interrupts, external GPIO interrupts, interrupt-driven serial I/O and PWM-based hardware control.
