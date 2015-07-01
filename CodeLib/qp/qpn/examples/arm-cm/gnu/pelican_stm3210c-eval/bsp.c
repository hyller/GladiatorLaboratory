/*****************************************************************************
* Product: PELICAN crossing example, STM3210C-EVAL board
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

#include "stm32f10x.h"
#include "stm32_eval.h"
#include "stm3210c_eval_lcd.h"

enum ISR_Priorities {   /* ISR priorities starting from the highest urgency */
    SYSTICK_PRIO,
    /* ... */
};

void SysTick_Handler(void) __attribute__((__interrupt__));

/*..........................................................................*/
void SysTick_Handler(void) {
    QF_tickISR();
}
/*..........................................................................*/
void QF_onStartup(void) {
    /* Set up and enable the SysTick timer.  It will be used as a reference
    * for delay loops in the interrupt handlers.  The SysTick timer period
    * will be set up for BSP_TICKS_PER_SEC.
    */
    SysTick_Config(SystemFrequency_SysClk / BSP_TICKS_PER_SEC);

                       /* set priorities of all interrupts in the system... */
    NVIC_SetPriority(SysTick_IRQn, SYSTICK_PRIO);
    /* ... */
}
/*..........................................................................*/
void QF_onIdle(void) {      /* entered with interrupts DISABLED, see NOTE01 */

    /* toggle the User LED on and then off, see NOTE02 */
    STM_EVAL_LEDOn (LED4);                                  /* blue LED on  */
    STM_EVAL_LEDOff(LED4);                                  /* blue LED off */

#ifdef NDEBUG
    /* put the CPU and peripherals to the low-power mode, see NOTE02 */
    QF_CPU_SLEEP();         /* atomically go to sleep and enable interrupts */
#else
    QF_INT_ENABLE();                              /* just enable interrupts */
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
    SystemInit();         /* initialize STM32 system (clock, PLL and Flash) */

             /* initialize LEDs, Key Button, and LCD on STM3210X-EVAL board */
    STM_EVAL_LEDInit(LED1);
    STM_EVAL_LEDInit(LED2);
    STM_EVAL_LEDInit(LED3);
    STM_EVAL_LEDInit(LED4);

    STM3210C_LCD_Init();                              /* initialize the LCD */
    LCD_Clear(White);                                      /* clear the LCD */
    LCD_SetBackColor(Grey);
    LCD_SetTextColor(Black);
    LCD_DisplayString(Line0, 0, "   Quantum Leaps    ");
    LCD_DisplayString(Line1, 0, "  PELICAN Crossing  ");
    LCD_DisplayString(Line2, 0, "  QP-nano (Vanilla) ");
    LCD_SetBackColor(White);
    LCD_DisplayString(Line4, 0, "stat");
    LCD_DisplayString(Line5, 0, "cars");
    LCD_DisplayString(Line6, 0, "peds");
    LCD_SetBackColor(Black);
    LCD_SetTextColor(Yellow);
    LCD_DisplayString(Line9, 0, "  state-machine.com ");
    LCD_SetBackColor(Blue);
    LCD_SetTextColor(White);
    LCD_DisplayString(Line4, 4*16, "                ");
    LCD_DisplayString(Line5, 4*16, "                ");
    LCD_DisplayString(Line6, 4*16, "                ");
}
/*..........................................................................*/
void BSP_terminate(int16_t result) {
    (void)result;
}
/*..........................................................................*/
void BSP_showState(char_t const *state) {
    LCD_DisplayString(Line4, 4*16, state);
}
/*..........................................................................*/
void BSP_signalCars(enum BSP_CarsSignal sig) {
    switch (sig) {
        case CARS_RED: {
            LCD_DisplayString(Line5, 4*16, "RED   ");
            STM_EVAL_LEDOn (LED3);
            STM_EVAL_LEDOff(LED2);
            STM_EVAL_LEDOff(LED1);
            break;
        }
        case CARS_YELLOW: {
            LCD_DisplayString(Line5, 4*16, "YELLOW");
            STM_EVAL_LEDOff(LED3);
            STM_EVAL_LEDOn (LED2);
            STM_EVAL_LEDOff(LED1);
            break;
        }
        case CARS_GREEN: {
            LCD_DisplayString(Line5, 4*16, "GREEN ");
            STM_EVAL_LEDOff(LED3);
            STM_EVAL_LEDOff(LED2);
            STM_EVAL_LEDOn (LED1);
            break;
        }
        case CARS_BLANK: {
            LCD_DisplayString(Line5, 4*16, "      ");
            STM_EVAL_LEDOff(LED3);
            STM_EVAL_LEDOff(LED2);
            STM_EVAL_LEDOff(LED1);
            break;
        }
    }
}
/*..........................................................................*/
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    switch (sig) {
        case PEDS_DONT_WALK: {
            LCD_DisplayString(Line6, 4*16, "DON'T WALK");
            break;
        }
        case PEDS_BLANK: {
            LCD_DisplayString(Line6, 4*16, "          ");
            break;
        }
        case PEDS_WALK: {
            LCD_DisplayString(Line6, 4*16, "***WALK***");
            break;
        }
    }
}
/*****************************************************************************
* NOTE01:
* The QF_onIdle() callback is called with interrupts locked, because the
* determination of the idle condition might change by any interrupt posting
* an event. QF_onIdle() must internally enable interrupts, ideally atomically
* with putting the CPU to the power-saving mode.
*
* NOTE02:
* The User LED is used to visualize the idle loop activity. The brightness
* of the LED is proportional to the frequency of invcations of the idle loop.
* Please note that the LED is toggled with interrupts disable, so no
* interrupt execution time contributes to the brightness of the User LED.
*/
