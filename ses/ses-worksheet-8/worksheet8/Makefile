# name of executable - change this to change the executables name
ELF=demo.elf

###################################################################################
# All object files

OBJS=  $(STARTUP) $(RTOSOBJ) $(PERIPHOBJ) partest.o main.o

# Object files with OBJDIR path
OBJSP= $(patsubst %.o, $(OBJDIR)/%.o, $(OBJS))
OBJDIR= ./objects

#FreeRTOS object files
RTOSOBJ= tasks.o queue.o list.o timers.o heap_1.o port.o

#Processor specific object files
STARTUP= startup_stm32f10x.o system_stm32f10x.o core_cm3.o sys_calls.o io.o

#STM Peripheral object files
PERIPHOBJ= stm32f10x_rcc.o stm32f10x_gpio.o stm32f10x_usart.o misc.o

###################################################################################

# All paths

# Library path
LIBROOT=../STM32F10x_StdPeriph_Lib_V3.5.0

#FreeRTOS path
FreeRTOS=../../FreeRTOSv202012.00/FreeRTOS/Source

# Code Paths
DEVICE=$(LIBROOT)/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
CORE=$(LIBROOT)/Libraries/CMSIS/CM3/CoreSupport
PERIPH=$(LIBROOT)/Libraries/STM32F10x_StdPeriph_Driver

# Search paths for perpheral library
vpath %.c $(CORE)
vpath %.c $(PERIPH)/src
vpath %.c $(DEVICE)

#FreeRTOS search paths
vpath %.c $(FreeRTOS)
vpath %.c $(FreeRTOS)/portable/MemMang
vpath %.c $(FreeRTOS)/portable/GCC/ARM_CM3

###################################################################################
# Tools
TOOLROOT=/usr/bin

CC=$(TOOLROOT)/arm-none-eabi-gcc
LD=$(TOOLROOT)/arm-none-eabi-gcc
AR=$(TOOLROOT)/arm-none-eabi-ar
AS=$(TOOLROOT)/arm-none-eabi-as

###################################################################################

# Flags

# Processor specific
PTYPE= STM32F10X_CL
LDSCRIPT= stm32f100.ld

# compilation flags for gdb
CFLAGS  = -O0 -g

# Compilation Flags
FULLASSERT = -DUSE_FULL_ASSERT 
LDFLAGS+= -T$(LDSCRIPT) -mthumb -mcpu=cortex-m3 -nostdlib
LDLIBS+= ../../ws6-newlib-build/newlib-arm-none-eabi/arm-none-eabi/newlib/libc.a
LDLIBS+= $(shell $(CC) -print-libgcc-file-name)
CFLAGS+= -mcpu=cortex-m3 -mthumb 
CFLAGS+= -I. -I$(DEVICE) -I$(CORE) -I$(PERIPH)/inc
CFLAGS+= -D$(PTYPE) -DUSE_STDPERIPH_DRIVER $(FULLASSERT)
CFLAGS+= -I$(FreeRTOS)/include -I$(FreeRTOS)/portable/MemMang -DGCC_ARMCM3
CFLAGS+= -I../../ws6-newlib-build/newlib-cygwin/newlib/libc/include

# Build executable 
$(ELF) : $(OBJSP)
	$(LD) $(LDFLAGS) -o $@ $(OBJSP) $(LDLIBS)

# compile and generate dependency info
$(OBJDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
	$(CC) -MM $(CFLAGS) $< > $(OBJDIR)/$*.d 

$(OBJDIR)/%.o: %.s
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJDIR)/*.o $(OBJSP:.o=.d) $(ELF) *~

debug: $(ELF)
	arm-none-eabi-gdb $(ELF)

#-include $(OBJSP:.o=.d)




