//*****************************************************************************
//
// startup_gcc.c - Startup code for use with GNU tools.
//
// Copyright (c) 2013-2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.0.12573 of the EK-TM4C1294XL Firmware Package.
//
//*****************************************************************************

#include <stdint.h>
#include "system_stm32l1xx.h"
//#include "inc/hw_types.h"

//*****************************************************************************
//
// Forward declaration of the default fault handlers.
//
//*****************************************************************************
void ResetISR(void);
static void IntDefaultHandler(void);

//*****************************************************************************
//
// External declaration for the interrupt handler used by the application.
//
//*****************************************************************************
extern void NMI_Handler(void);
extern void HardFault_Handler();
extern void MemManage_Handler(void);
extern void BusFault_Handler(void);
extern void UsageFault_Handler(void);
extern void SVC_Handler(void);
extern void DebugMon_Handler(void);
extern void PendSV_Handler(void);
extern void SysTick_Handler(void);
extern void USB_LP_IRQHandler(void);
extern void USB_FS_WKUP_IRQHandler(void);
extern void EXTI15_10_IRQHandler(void);
extern void DMA1_Channel4_IRQHandler(void);
extern void DMA1_Channel5_IRQHandler(void);
extern void TIM3_IRQHandler(void);
extern void USART1_IRQHandler(void);
extern void USART2_IRQHandler(void);
extern void USART3_IRQHandler(void);
extern void EXTI0_IRQHandler(void);
extern void EXTI1_IRQHandler(void);
extern void DMA1_Channel2_IRQHandler(void);
extern void DMA1_Channel3_IRQHandler(void);
extern void DMA1_Channel5_IRQHandler(void);
extern void DMA1_Channel6_IRQHandler(void);
extern void DMA1_Channel7_IRQHandler(void);
extern void RTC_Alarm_IRQHandler(void);
//*****************************************************************************
//
// The entry point for the application.
//
//*****************************************************************************
extern int main(void);

//*****************************************************************************
//
// Reserve space for the system stack.
//
//*****************************************************************************
static uint32_t pui32Stack[1024];
#define BootRAM ((void *)(0xF108F85F))
//*****************************************************************************
//
// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000.
//
//*****************************************************************************
__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) =
{
  (void (*)(void))((uint32_t)pui32Stack + sizeof(pui32Stack)),
   // The initial stack pointer
  ResetISR,
  NMI_Handler,
  HardFault_Handler,
  MemManage_Handler,//MemManage_Handler,
  BusFault_Handler,//BusFault_Handler,
  UsageFault_Handler,//UsageFault_Handler,
  0,
  0,
  0,
  0,
  SVC_Handler,//SVC_Handler,
  DebugMon_Handler,//DebugMon_Handler,
  0,
  PendSV_Handler,//PendSV_Handler,
  SysTick_Handler,//SysTick_Handler,
  IntDefaultHandler,//WWDG_IRQHandler,
  IntDefaultHandler,//PVD_IRQHandler,
  IntDefaultHandler,//TAMPER_STAMP_IRQHandler,
  IntDefaultHandler,//RTC_WKUP_IRQHandler,
  IntDefaultHandler,//FLASH_IRQHandler,
  IntDefaultHandler,//RCC_IRQHandler,
  EXTI0_IRQHandler,//EXTI0_IRQHandler,
  EXTI1_IRQHandler,//EXTI1_IRQHandler,
  IntDefaultHandler,//EXTI2_IRQHandler,
  IntDefaultHandler,//EXTI3_IRQHandler,
  IntDefaultHandler,//EXTI4_IRQHandler,
  IntDefaultHandler,//DMA1_Channel1_IRQHandler,
  DMA1_Channel2_IRQHandler,//DMA1_Channel2_IRQHandler,
  DMA1_Channel3_IRQHandler,//DMA1_Channel3_IRQHandler,
  DMA1_Channel4_IRQHandler,//DMA1_Channel4_IRQHandler,
  DMA1_Channel5_IRQHandler,//DMA1_Channel5_IRQHandler,
  DMA1_Channel6_IRQHandler,//DMA1_Channel6_IRQHandler,
  DMA1_Channel7_IRQHandler,//DMA1_Channel7_IRQHandler,
  IntDefaultHandler,//ADC1_IRQHandler,
  IntDefaultHandler,//USB_HP_IRQHandler,
  USB_LP_IRQHandler,//USB_LP_IRQHandler,
  IntDefaultHandler,//DAC_IRQHandler,
  IntDefaultHandler,//COMP_IRQHandler,
  IntDefaultHandler,//EXTI9_5_IRQHandler,
  IntDefaultHandler,//LCD_IRQHandler,
  IntDefaultHandler,//TIM9_IRQHandler,
  IntDefaultHandler,//TIM10_IRQHandler,
  IntDefaultHandler,//TIM11_IRQHandler,
  IntDefaultHandler,//TIM2_IRQHandler,
  TIM3_IRQHandler,//TIM3_IRQHandler,
  IntDefaultHandler,//TIM4_IRQHandler,
  IntDefaultHandler,//I2C1_EV_IRQHandler,
  IntDefaultHandler,//I2C1_ER_IRQHandler,
  IntDefaultHandler,//I2C2_EV_IRQHandler,
  IntDefaultHandler,//I2C2_ER_IRQHandler,
  IntDefaultHandler,//SPI1_IRQHandler,
  IntDefaultHandler,//SPI2_IRQHandler,
  USART1_IRQHandler,//USART1_IRQHandler,
  USART2_IRQHandler,//USART2_IRQHandler,
  USART3_IRQHandler,//USART3_IRQHandler,
  IntDefaultHandler,//EXTI15_10_IRQHandler,
  RTC_Alarm_IRQHandler,//RTC_Alarm_IRQHandler,
  IntDefaultHandler,//USB_FS_WKUP_IRQHandler,
  IntDefaultHandler,//TIM6_IRQHandler,
  IntDefaultHandler,//TIM7_IRQHandler,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  BootRAM
};

//*****************************************************************************
//
// The following are constructs created by the linker, indicating where the
// the "data" and "bss" segments reside in memory.  The initializers for the
// for the "data" segment resides immediately following the "text" segment.
//
//*****************************************************************************
//extern uint32_t _etext;
//extern uint32_t _data;
//extern uint32_t _edata;
//extern uint32_t _bss;
//extern uint32_t _ebss;

extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;




//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
//
//*****************************************************************************
	void
ResetISR(void)
{
	uint32_t *pui32Src, *pui32Dest;

	//
	// Copy the data segment initializers from flash to SRAM.
	//
	pui32Src = &_sidata;
	for(pui32Dest = &_sdata; pui32Dest < &_edata; )
	{
		*pui32Dest++ = *pui32Src++;
	}

	//
	// Zero fill the bss segment.
	//
	for(pui32Dest = &_sbss; pui32Dest < &_ebss; )
	{
		*pui32Dest++ = 0;
	}

	//
	// Call the application's entry point.
	//
	SystemInit();
	//__libc_init_array();
	main();
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
	static void
IntDefaultHandler(void)
{
	//
	// Go into an infinite loop.
	//
	while(1)
	{
	}
}
