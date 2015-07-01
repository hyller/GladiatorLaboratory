/*****************************************************************************
* Product: PELICAN crossing example, QK-nano kernel, LPCXpresso-1343, GNU
* Last Updated for Version: 5.1.1
* Date of the Last Update:  Oct 11, 2013
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
#include "pelican.h"

#include "LPC13xx.h"                                 /* LPC13xx definitions */
#include "timer16.h"
#include "clkconfig.h"
#include "gpio.h"

#define LED_PORT    0
#define LED_BIT     7
#define LED_ON      1
#define LED_OFF     0

enum ISR_Priorities {   /* ISR priorities starting from the highest urgency */
    PIOINT0_PRIO,
    SYSTICK_PRIO,
    /* ... */
};

void SysTick_Handler(void) __attribute__((__interrupt__));
void PIOINT0_IRQHandler(void) __attribute__((__interrupt__));

/*..........................................................................*/
void SysTick_Handler(void) {
    QK_ISR_ENTRY();                       /* inform QK-nano about ISR entry */
    QF_tickISR();
    QK_ISR_EXIT();                         /* inform QK-nano about ISR exit */
}
/*..........................................................................*/
void PIOINT0_IRQHandler(void) {
    QK_ISR_ENTRY();                       /* inform QK-nano about ISR entry */
    QActive_postISR((QActive *)&AO_Pelican, PEDS_WAITING_SIG, 0);
    QK_ISR_EXIT();                         /* inform QK-nano about ISR exit */
}
/*..........................................................................*/
void QF_onStartup(void) {
    /* Set up and enable the SysTick timer. It will be used as a reference
    * for delay loops in the interrupt handlers. The SysTick timer period
    * will be set up for BSP_TICKS_PER_SEC.
    */
    SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);

           /* enable EINT0 interrupt, which is used for testing preemptions */
    NVIC_EnableIRQ(EINT0_IRQn);

                       /* set priorities of all interrupts in the system... */
    NVIC_SetPriority(SysTick_IRQn, SYSTICK_PRIO);
    NVIC_SetPriority(EINT0_IRQn,   PIOINT0_PRIO);
}
/*..........................................................................*/
void QK_onIdle(void) {
                         /* toggle the User LED on and then off, see NOTE01 */
    //QF_INT_DISABLE();
    //GPIOSetValue(LED_PORT, LED_BIT, LED_ON);                     /* LED on  */
    //GPIOSetValue(LED_PORT, LED_BIT, LED_OFF);                    /* LED off */
    //QF_INT_ENABLE();

#ifdef NDEBUG
    /* put the CPU and peripherals to the low-power mode */
    __WFI();                /* stop clocking the CPU and wait for interrupt */
#endif
}
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const file, int line) {
    (void)file;                                   /* avoid compiler warning */
    (void)line;                                   /* avoid compiler warning */
    QF_INT_DISABLE();         /* make sure that all interrupts are disabled */
    for (;;) {       /* NOTE: replace the loop with reset for final version */
    }
}
/*..........................................................................*/
/* error routine that is called if the STM32 library encounters an error    */
void assert_failed(char_t const *file, int_t line) {
    Q_onAssert(file, line);
}

/*..........................................................................*/
void BSP_init(void) {
    SystemInit();                         /* initialize the clocking system */
    GPIOInit();                                          /* initialize GPIO */
    GPIOSetDir(LED_PORT, LED_BIT, 1);         /* set port for LED to output */
}
/*..........................................................................*/
void BSP_terminate(int16_t result) {
    (void)result;
}
/*..........................................................................*/
void BSP_showState(char_t const *state) {
    (void)state;
}
/*..........................................................................*/
void BSP_signalCars(enum BSP_CarsSignal sig) {
    (void)sig;
}
/*..........................................................................*/
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    if (sig == PEDS_DONT_WALK) {
        GPIOSetValue(LED_PORT, LED_BIT, LED_ON);                 /* LED on  */
    }
    else {
        GPIOSetValue(LED_PORT, LED_BIT, LED_OFF);                /* LED off */
    }
}

/*****************************************************************************
* NOTE01:
* The User LED is used to visualize the idle loop activity. The brightness
* of the LED is proportional to the frequency of invcations of the idle loop.
* Please note that the LED is toggled with interrupts disabled, so no
* interrupt execution time contributes to the brightness of the User LED.
*/
