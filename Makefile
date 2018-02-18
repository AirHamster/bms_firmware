# Makefile for CANopenNode, basic compile with no CAN device.


STACKDRV_SRC =  stack
STACK_SRC =     stack
BMS_SRC =   .
APPL_SRC =      sources

BINARY = mainbms
#LINK_TARGET  =  canopennode


INCLUDE_DIRS = -I$(STACKDRV_SRC) \
               -I$(STACK_SRC)    \
               -I$(BMS_SRC)  \
               -I$(APPL_SRC)


SOURCES =       $(STACKDRV_SRC)/CO_driver.c     \
                $(STACK_SRC)/crc16-ccitt.c      \
                $(STACK_SRC)/CO_SDO.c           \
                $(STACK_SRC)/CO_Emergency.c     \
                $(STACK_SRC)/CO_NMT_Heartbeat.c \
                $(STACK_SRC)/CO_SYNC.c          \
                $(STACK_SRC)/CO_PDO.c           \
                $(STACK_SRC)/CO_HBconsumer.c    \
                $(STACK_SRC)/CO_SDOmaster.c     \
                $(STACK_SRC)/CO_trace.c         \
                $(BMS_SRC)/CANopen.c            \
                $(APPL_SRC)/CO_OD.c             \
                $(APPL_SRC)/gpio.c              \
                $(APPL_SRC)/rcc.c               \
                $(APPL_SRC)/usart.c             \
                $(APPL_SRC)/adc.c               \
                $(APPL_SRC)/bq76pl455.c         \
                $(APPL_SRC)/timers.c            
                #$(BMS_SRC)/mainbms.c


OBJS = $(SOURCES:%.c=%.o)
+OPENCM3_DIR=./libopencm3
#CFLAGS = -Wall $(INCLUDE_DIRS)

LDSCRIPT = $(OPENCM3_DIR)/lib/stm32/f1/stm32f103x8.ld

include ./libopencm3.target.mk
