/*****************************************************************************
* Product: Simple Blinky example, Vanilla kernel
* Last Updated for Version: 5.2.0
* Date of the Last Update:  Dec 29, 2013
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2013 Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* Quantum Leaps Web sites: http://www.quantum-leaps.com
*                          http://www.state-machine.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#include "qpn_port.h"
#include "bsp.h"

#include "tm4c_cmsis.h"
#include "sysctl.h"
#include "gpio.h"
#include "rom.h"

//Q_DEFINE_THIS_FILE

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
* Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
* DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
*/
enum KernelUnawareISRs {                                      /* see NOTE00 */
    /* ... */
    MAX_KERNEL_UNAWARE_CMSIS_PRI                        /* keep always last */
};
/* "kernel-unaware" interrupts can't overlap "kernel-aware" interrupts */
Q_ASSERT_COMPILE(MAX_KERNEL_UNAWARE_CMSIS_PRI <= QF_AWARE_ISR_CMSIS_PRI);

enum KernelAwareISRs {
    GPIOPORTA_PRIO = QF_AWARE_ISR_CMSIS_PRI,                  /* see NOTE00 */
    SYSTICK_PRIO,
    /* ... */
    MAX_KERNEL_AWARE_CMSIS_PRI                          /* keep always last */
};
/* "kernel-aware" interrupts should not overlap the PendSV priority */
Q_ASSERT_COMPILE(MAX_KERNEL_AWARE_CMSIS_PRI <= (0xFF >>(8-__NVIC_PRIO_BITS)));

/* ISRs defined in this BSP ------------------------------------------------*/
void SysTick_Handler(void);
void GPIOPortA_IRQHandler(void);

/* Local-scope objects -----------------------------------------------------*/
#define LED_RED     (1U << 1)
#define LED_GREEN   (1U << 3)
#define LED_BLUE    (1U << 2)

#define USR_SW1     (1U << 4)
#define USR_SW2     (1U << 0)

/*..........................................................................*/
void SysTick_Handler(void) {
    QF_tickXISR(0U);                       /* process time events at rate 0 */
}
/*..........................................................................*/
void GPIOPortA_IRQHandler(void) {
    /* for testing... */
}

/*..........................................................................*/
void BSP_init(void) {
                                          /* Enable the floating-point unit */
    SCB->CPACR |= (0xFU << 20);

    /* Enable lazy stacking for interrupt handlers. This allows FPU
    * instructions to be used within interrupt handlers, but at the
    * expense of extra stack usage.
    */
    ROM_FPULazyStackingEnable();

    /* Set the clocking to run directly from the crystal */
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC
                       | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    /* enable clock to the peripherals used by the application */
    SYSCTL->RCGC2 |= (1U << 5);                   /* enable clock to GPIOF  */
    __NOP();                                  /* wait after enabling clocks */
    __NOP();
    __NOP();

    /* configure the LEDs */
    GPIOF->DIR |= (LED_RED | LED_GREEN | LED_BLUE);/* set direction: output */
    GPIOF->DEN |= (LED_RED | LED_GREEN | LED_BLUE);       /* digital enable */
    GPIOF->DATA_Bits[LED_RED]   = 0U;                   /* turn the LED off */
    GPIOF->DATA_Bits[LED_GREEN] = 0U;                   /* turn the LED off */
    GPIOF->DATA_Bits[LED_BLUE]  = 0U;                   /* turn the LED off */

    /* configure the User Switches */
    GPIOF->DIR &= ~(USR_SW1 | USR_SW2);            /*  set direction: input */
    ROM_GPIOPadConfigSet(GPIO_PORTF_BASE, (USR_SW1 | USR_SW2),
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}
/*..........................................................................*/
void BSP_ledOff() {
    GPIOF->DATA_Bits[LED_GREEN] = 0U;
}
/*..........................................................................*/
void BSP_ledOn() {
    GPIOF->DATA_Bits[LED_GREEN] = LED_GREEN;
}

/*..........................................................................*/
void QF_onStartup(void) {
              /* set up the SysTick timer to fire at BSP_TICKS_PER_SEC rate */
    SysTick_Config(ROM_SysCtlClockGet() / BSP_TICKS_PER_SEC);

    /* assing all priority bits for preemption-prio. and none to sub-prio. */
    NVIC_SetPriorityGrouping(0U);

    /* set priorities of ALL ISRs used in the system, see NOTE00
    *
    * !!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    * Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
    * DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
    */
    NVIC_SetPriority(SysTick_IRQn,   SYSTICK_PRIO);
    NVIC_SetPriority(GPIOPortA_IRQn, GPIOPORTA_PRIO);
    /* ... */
                                                          /* enable IRQs... */
    NVIC_EnableIRQ(GPIOPortA_IRQn);
}
/*..........................................................................*/
void QF_onIdle(void) {       /* called with interrupts disabled, see NOTE01 */

    //GPIOF->DATA_Bits[LED_RED] = LED_RED;          /* turn the Red LED on  */
    //GPIOF->DATA_Bits[LED_RED] = 0;                /* turn the Red LED off */

#ifdef NDEBUG
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular Cortex-M3 MCU.
    */
    QF_CPU_SLEEP();         /* atomically go to sleep and enable interrupts */
#else
    QF_INT_ENABLE();                              /* just enable interrupts */
#endif
}

/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const file, int line) {
    assert_failed(file, line);
}
/*..........................................................................*/
/* error routine that is called if the CMSIS library encounters an error    */
void assert_failed(char const *file, int line) {
    (void)file;                                   /* avoid compiler warning */
    (void)line;                                   /* avoid compiler warning */
    QF_INT_DISABLE();         /* make sure that all interrupts are disabled */
    ROM_SysCtlReset();                                  /* reset the system */
}

/*****************************************************************************
* NOTE00:
* The QF_AWARE_ISR_CMSIS_PRI constant from the QF port specifies the highest
* ISR priority that is disabled by the QF framework. The value is suitable
* for the NVIC_SetPriority() CMSIS function.
*
* Only ISRs prioritized at or below the QF_AWARE_ISR_CMSIS_PRI level (i.e.,
* with the numerical values of priorities equal or higher than
* QF_AWARE_ISR_CMSIS_PRI) are allowed to call any QF services. These ISRs
* are "QF-aware".
*
* Conversely, any ISRs prioritized above the QF_AWARE_ISR_CMSIS_PRI priority
* level (i.e., with the numerical values of priorities less than
* QF_AWARE_ISR_CMSIS_PRI) are never disabled and are not aware of the kernel.
* Such "QF-unaware" ISRs cannot call any QF services. The only mechanism
* by which a "QF-unaware" ISR can communicate with the QF framework is by
* triggering a "QF-aware" ISR, which can post/publish events.
*
* NOTE01:
* The QF_onIdle() callback is called with interrupts disabled, because the
* determination of the idle condition might change by any interrupt posting
* an event. QF_onIdle() must internally re-enable interrupts, which is
* here done atomically with putting the CPU to the power-saving mode.
*/
