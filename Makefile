##
## This file is part of the libopencm3 project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
##
## This library is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with this library.  If not, see <http://www.gnu.org/licenses/>.
##

CFLAGS = -Wall $(INCLUDE_DIRS)
BINARY = mainbms
 
OBJS = ./sources/gpio.o ./sources/rcc.o ./sources/usart.o ./sources/adc.o ./sources/timers.o CANopen.o ./stack/CO_driver.o ./stack/crc16-ccitt.o ./stack/CO_SDO.o ./stack/CO_Emergency.o  ./stack/CO_NMT_Heartbeat.o ./stack/CO_SYNC.o  ./stack/CO_PDO.o  ./stack/CO_HBconsumer.o  ./stack/CO_SDOmaster.o  ./stack/CO_trace.o ./stack/CO_OD.o
 ## CSRC	= mainbms.c gpio.c rcc.c
##SOURCES= mainbms.c gpio.c rcc.c

+OPENCM3_DIR=./libopencm3
 
 LDSCRIPT = $(OPENCM3_DIR)/lib/stm32/f1/stm32f103x8.ld

include ./libopencm3.target.mk
