# Secure Embedded Systems — Worksheet 4

## Serial Data Communication on the STM32F107

**Student ID:** 24042392  
**Worksheet:** 4 — Serial Data Communication  
**Target board:** Olimex STM32-P107  
**Microcontroller:** STM32F107VCT6 / ARM Cortex-M3  
**Serial peripheral used:** USART2  

---

## 1. Overview

This worksheet investigates serial communication on the STM32F107 using USART2. The work covered the two mandatory exercises from Worksheet 4:

1. changing the USART baud rate to **9600 baud**, observing the result of a baud-rate mismatch in Minicom, and then receiving the expected output at the correct speed;
2. implementing an `inbyte()` receive routine so that characters typed in Minicom are received by the STM32 and echoed back to the terminal.

The optional credit exercise involving full USART error handling for overrun, noise, framing and parity errors was not attempted.

---

## 2. Hardware and Software

### Hardware

- Olimex STM32-P107 development board
- STM32F107VCT6 ARM Cortex-M3 microcontroller
- Olimex ARM-USB-TINY-H JTAG adapter
- Serial connection exposed as `/dev/ttyUSB0`
- Linux workstation

### Software and tools

- `arm-none-eabi-gcc`
- `make`
- OpenOCD
- Telnet
- Minicom 2.7.1
- STM32F10x Standard Peripheral Library V3.5.0

Because `/dev/ttyUSB0` belonged to the `dialout` group and the current user did not have access to it, Minicom was started using `sudo`.

---

## 3. Project Structure

The relevant project files are:

```text
worksheet4/
├── Makefile
├── com_port.h
├── main.c
├── mainoutonly.c
├── openocd.cfg
├── startup_stm32f10x.c
├── stm32f100.ld
├── stm32f10x_conf.h
└── evidence/
    ├── code Exercise 2.png
    ├── elf exists and arm-none(1).png
    ├── exercise 2 arm-none-eabi-size demo.elf.png
    ├── hellohelloOUTPUT(1).png
    ├── minicom working on 115200(2).png
    ├── p1 bad minicom(2).png
    ├── p2 bad minicom(2).png
    ├── Screenshot 2 — OpenOCD detects the board(1).png
    └── SES WORKSHEET 4 ECHO TEST 24042392.png
```

---

## 4. USART2 Configuration

USART2 was configured with the following settings:

| Setting | Value |
|---|---|
| USART | USART2 |
| TX pin | PD5 |
| RX pin | PD6 |
| Baud rate | 9600 |
| Word length | 8 bits |
| Parity | None |
| Stop bits | 1 |
| Hardware flow control | None |
| Mode | TX and RX |

The baud-rate definition in `com_port.h` was changed to:

```c
#define mainCOM_PORT_BAUD_RATE    ( 9600 )
```

USART2 is remapped to pins PD5 and PD6. The GPIO and USART clocks are enabled before the peripheral is initialised.

Relevant configuration code:

```c
RCC_APB2PeriphClockCmd(
    RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO,
    ENABLE
);

RCC_APB1PeriphClockCmd(
    RCC_APB1Periph_USART2,
    ENABLE
);

GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);

GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
GPIO_Init(GPIOD, &GPIO_InitStructure);

GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
GPIO_Init(GPIOD, &GPIO_InitStructure);

USART_InitStructure.USART_BaudRate = mainCOM_PORT_BAUD_RATE;
USART_InitStructure.USART_WordLength = USART_WordLength_8b;
USART_InitStructure.USART_StopBits = USART_StopBits_1;
USART_InitStructure.USART_Parity = USART_Parity_No;
USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

USART_Init(USART2, &USART_InitStructure);
USART_Cmd(USART2, ENABLE);
```

---

## 5. Building the Firmware

The firmware was built using:

```bash
make clean
make
```

The generated executable was checked with:

```bash
ls -lh demo.elf
arm-none-eabi-size demo.elf
```

For Exercise 1, the executable size shown in the evidence was:

```text
text    data    bss    dec    hex
15516   40      256    15812  3dc4
```

For Exercise 2, after adding the receive and echo functionality, the executable size was:

```text
text    data    bss    dec    hex
15464   40      256    15760  3d90
```

### Evidence

![Exercise 1 ELF size](evidence/elf%20exists%20and%20arm-none(1).png)

![Exercise 2 ELF size](evidence/exercise%202%20arm-none-eabi-size%20demo.elf.png)

---

## 6. Connecting to the STM32 with OpenOCD

OpenOCD was started with:

```bash
openocd -f openocd.cfg
```

The output confirmed that the STM32 target was detected through JTAG, including the CPU and boundary-scan TAPs.

### Evidence

![OpenOCD detects the STM32 board](evidence/Screenshot%202%20%E2%80%94%20OpenOCD%20detects%20the%20board(1).png)

---

## 7. Flashing and Running the Firmware

A Telnet connection was opened to the OpenOCD server:

```bash
telnet localhost 4444
```

At the OpenOCD prompt, the firmware was written using:

```text
reset halt
flash write_image erase demo.elf 0
```

The board was then started with:

```text
reset run
```

The flashing output confirmed that `demo.elf` was successfully written to the STM32 flash memory.

---

# Exercise 1 — Baud-Rate Test

## 8. Objective

The first exercise required the program baud rate to be changed to **9600**, the effect of using the wrong Minicom speed to be observed, and Minicom then to be changed to the correct 9600 baud setting.

The STM32 program transmitted ten occurrences of `hello`, followed by a newline and `bye`.

Expected output:

```text
hellohellohellohellohellohellohellohellohellohello
bye
```

---

## 9. Wrong Baud-Rate Test: STM32 at 9600, Minicom at 115200

The STM32 remained configured for **9600 baud**, while Minicom was deliberately opened at **115200 baud**:

```bash
sudo minicom -o -D /dev/ttyUSB0 -b 115200 -8
```

The board was then restarted with:

```text
reset halt
reset run
```

No readable `hello...bye` output appeared in Minicom. This demonstrated that a baud-rate mismatch prevents the receiver from correctly decoding the transmitted serial data.

### Evidence

The first screenshot shows Minicom being opened at 115200 baud:

![Minicom at 115200 baud](evidence/p1%20bad%20minicom(2).png)

The second screenshot shows `reset halt` and `reset run` being executed while the Minicom output remains unreadable/blank:

![Wrong baud-rate result](evidence/p2%20bad%20minicom(2).png)

An additional screenshot shows Minicom running on `/dev/ttyUSB0` with the 115200-baud command visible:

![Minicom 115200 setup](evidence/minicom%20working%20on%20115200(2).png)

---

## 10. Correct Baud-Rate Test: STM32 at 9600, Minicom at 9600

Minicom was then reopened at the correct speed:

```bash
sudo minicom -o -D /dev/ttyUSB0 -b 9600 -8
```

After running the program, the expected serial output was received correctly:

```text
hellohellohellohellohellohellohellohellohellohello
bye
```

This confirmed that the transmitter and receiver were using matching serial settings.

### Evidence

![Correct hello and bye output at 9600 baud](evidence/hellohelloOUTPUT(1).png)

---

## 11. Exercise 1 Result

Exercise 1 was completed successfully:

- USART baud rate changed to 9600;
- a deliberate 9600/115200 mismatch was tested;
- no readable output was produced at the wrong receive speed;
- Minicom was changed to 9600;
- the expected `hello...bye` output was received correctly.

---

# Exercise 2 — Implementing `inbyte()` and Echoing Characters

## 12. Objective

The second exercise required an `inbyte()` routine to be written, followed by a `main()` function that accepts characters from the keyboard and writes them back to the screen.

---

## 13. `inbyte()` Implementation

The receive routine waits until the USART receive-data register contains a character by polling `USART_FLAG_RXNE`. The received value is then returned from `USART_ReceiveData()`.

```c
int inbyte(void)
{
    while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
    {
    }

    return (int)(USART_ReceiveData(USART2) & 0xFF);
}
```

The `main()` loop receives one character and immediately transmits the same character back using `__io_putchar()`:

```c
int main(void)
{
    int c;

    COMPortInit();

    __io_putchar('>');
    __io_putchar(' ');

    while (1)
    {
        c = inbyte();
        __io_putchar(c);

        if (c == '\r')
        {
            __io_putchar('\n');
        }
    }
}
```

### Evidence

![Exercise 2 source code](evidence/code%20Exercise%202.png)

---

## 14. Exercise 2 Build

After adding `inbyte()` and the echo loop, the project was rebuilt:

```bash
make clean
make
ls -lh demo.elf
arm-none-eabi-size demo.elf
```

The resulting firmware size was:

```text
text    data    bss    dec    hex
15464   40      256    15760  3d90
```

### Evidence

![Exercise 2 build size](evidence/exercise%202%20arm-none-eabi-size%20demo.elf.png)

---

## 15. Exercise 2 Test

The new firmware was flashed to the STM32 and Minicom was opened at 9600 baud.

The following test string was typed:

```text
SES WORKSHEET 4 ECHO TEST 24042392
```

The same characters were received by USART2, processed by `inbyte()`, transmitted back using `__io_putchar()`, and displayed in Minicom.

The data path was:

```text
Keyboard
   ↓
USART2 RX on PD6
   ↓
inbyte()
   ↓
__io_putchar()
   ↓
USART2 TX on PD5
   ↓
Minicom
```

### Evidence

The screenshot below shows the echoed test string together with the Telnet/OpenOCD commands used to flash and run the program:

![USART2 echo test](evidence/SES%20WORKSHEET%204%20ECHO%20TEST%2024042392.png)

---

## 16. Exercise 2 Result

Exercise 2 was completed successfully:

- an `inbyte()` receive routine was implemented;
- the routine polls `USART_FLAG_RXNE`;
- received data is read with `USART_ReceiveData()`;
- the main loop sends the character back with `__io_putchar()`;
- the STM32 successfully echoed `SES WORKSHEET 4 ECHO TEST 24042392` to Minicom.

---

## 17. Requirements-to-Evidence Matrix

| Worksheet requirement | Implementation / test | Evidence | Status |
|---|---|---|---|
| Configure USART2 serial communication | USART2 initialised in `COMPortInit()` | Source code screenshot | Complete |
| Use PD5 as USART2 TX | PD5 configured as alternate-function push-pull | Source code screenshot | Complete |
| Use PD6 as USART2 RX | PD6 configured as floating input | Source code screenshot | Complete |
| Use 8N1 serial format | 8-bit word, no parity, one stop bit | Source code screenshot | Complete |
| Exercise 1: change baud rate to 9600 | `mainCOM_PORT_BAUD_RATE` set to 9600 | Code/configuration and successful output | Complete |
| Test wrong Minicom baud rate | STM32 at 9600, Minicom at 115200 | Wrong-baud screenshots | Complete |
| Change Minicom to 9600 | Minicom reopened at 9600 | Successful output screenshot | Complete |
| Receive `hello...bye` correctly | Correct output displayed in Minicom | `hellohelloOUTPUT(1).png` | Complete |
| Exercise 2: implement `inbyte()` | RXNE polling and `USART_ReceiveData()` | `code Exercise 2.png` | Complete |
| Accept keyboard characters | Main loop calls `inbyte()` | Source code and echo test | Complete |
| Write received characters back to screen | `__io_putchar(c)` echoes each character | Echo-test screenshot | Complete |
| Credit exercise: full USART error handling | Not attempted | Not applicable | Optional / not completed |

---

## 18. Conclusion

Worksheet 4 was completed successfully for all mandatory exercises.

The first exercise demonstrated the importance of matching transmitter and receiver baud rates. With the STM32 transmitting at 9600 baud and Minicom receiving at 115200 baud, no readable output was obtained. Once Minicom was changed to 9600 baud, the expected `hello...bye` message was displayed correctly.

The second exercise added receive functionality through an `inbyte()` routine. The STM32 successfully received keyboard input through USART2 RX and transmitted the same characters back through USART2 TX, demonstrating bidirectional serial communication.

The final echo test successfully displayed:

```text
SES WORKSHEET 4 ECHO TEST 24042392
```

This confirms that the USART2 receive and transmit paths were both working correctly on the STM32F107 target.

---

## 19. Reproduction Commands

### Build

```bash
make clean
make
ls -lh demo.elf
arm-none-eabi-size demo.elf
```

### Start OpenOCD

```bash
openocd -f openocd.cfg
```

### Connect with Telnet

```bash
telnet localhost 4444
```

### Flash firmware

```text
reset halt
flash write_image erase demo.elf 0
```

### Start Minicom at the wrong baud rate

```bash
sudo minicom -o -D /dev/ttyUSB0 -b 115200 -8
```

### Start Minicom at the correct baud rate

```bash
sudo minicom -o -D /dev/ttyUSB0 -b 9600 -8
```

### Run the program

```text
reset run
```

