#******************************************************************************
#
# Makefile - Rules for building the uart_echo example.
#
# Copyright (c) 2013-2014 Texas Instruments Incorporated.  All rights reserved.
# Software License Agreement
# 
# Texas Instruments (TI) is supplying this software for use solely and
# exclusively on TI's microcontroller products. The software is owned by
# TI and/or its suppliers, and is protected under applicable copyright
# laws. You may not combine this software with "viral" open-source
# software in order to form a larger program.
# 
# THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
# NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
# NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
# CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
# DAMAGES, FOR ANY REASON WHATSOEVER.
# 
# This is part of revision 2.1.0.12573 of the EK-TM4C1294XL Firmware Package.
#
#******************************************************************************

#
# The base directory for TivaWare.
#
ROOT=.

#
# Include the common make definitions.
#
include ${ROOT}/makedefs

#
# Where to find header files that do not live in the source directory.
#
IPATH=./src/CMSIS/Include
IPATH+=./src/Main/inc
IPATH+=./src/STM32_USB_Device_Library/Class/HID/Inc
IPATH+=./src/STM32_USB_Device_Library/Core/Inc
IPATH+=./src/STM32L1xx_HAL_Driver/Inc
IPATH+=./src/cmsis
VPATH=./src/STM32L1xx_HAL_Driver/Src
VPATH+=./src/STM32_USB_Device_Library/Class/HID/Src
VPATH+=./src/cmsis
VPATH+=./src/Main/src
VPATH+=./src/STM32_USB_Device_Library/Core/Src
VPATH+=./src/ARM
#
# The default rule, which causes the uart_echo example to be built.
#
all: ${COMPILER}
all: ${COMPILER}/net.axf

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${COMPILER} ${wildcard *~}
install:
	openocd -f openocd.cfg -c "flash_image"

#
# The rule to create the target directory.
#
${COMPILER}:
	@mkdir -p ${COMPILER}

#
# Rules for building the uart_echo example.
#
${COMPILER}/net.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/net.axf: ${COMPILER}/syscalls.o
${COMPILER}/net.axf: ${COMPILER}/main.o
${COMPILER}/net.axf: ${COMPILER}/stm32l1xx_it.o
${COMPILER}/net.axf: ${COMPILER}/usbd_conf.o
${COMPILER}/net.axf: ${COMPILER}/usbd_desc.o
${COMPILER}/net.axf: ${COMPILER}/stm32l1xx_nucleo.o
${COMPILER}/net.axf: ${COMPILER}/system_stm32l1xx.o
${COMPILER}/net.axf: ${COMPILER}/stm32l1xx_hal.o
${COMPILER}/net.axf: ${COMPILER}/stm32l1xx_hal_cortex.o
${COMPILER}/net.axf: ${COMPILER}/stm32l1xx_hal_gpio.o
${COMPILER}/net.axf: ${COMPILER}/stm32l1xx_hal_pcd.o
${COMPILER}/net.axf: ${COMPILER}/stm32l1xx_hal_pcd_ex.o
${COMPILER}/net.axf: ${COMPILER}/stm32l1xx_hal_pwr.o
${COMPILER}/net.axf: ${COMPILER}/stm32l1xx_hal_pwr_ex.o
${COMPILER}/net.axf: ${COMPILER}/stm32l1xx_hal_rcc.o
${COMPILER}/net.axf: ${COMPILER}/stm32l1xx_hal_rcc_ex.o
${COMPILER}/net.axf: ${COMPILER}/usbd_hid.o
${COMPILER}/net.axf: ${COMPILER}/usbd_core.o
${COMPILER}/net.axf: ${COMPILER}/usbd_ctlreq.o
${COMPILER}/net.axf: ${COMPILER}/usbd_ioreq.o
${COMPILER}/net.axf: stm32l1xx_flash.ld
SCATTERgcc_net=stm32l1xx_flash.ld
ENTRY_net=ResetISR
CFLAGSgcc=-DSTM32L152xE -DUSE_HAL_DRIVER -DUSE_STM32L1xx_NUCLEO

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
