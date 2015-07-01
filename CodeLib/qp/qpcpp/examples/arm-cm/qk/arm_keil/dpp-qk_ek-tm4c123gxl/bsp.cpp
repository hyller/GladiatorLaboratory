//****************************************************************************
// Product: "Dining Philosophers Problem" example, preemptive QK kernel
// Last Updated for Version: 5.2.0
// Date of the Last Update:  Dec 28, 2013
//
//                    Q u a n t u m     L e a P s
//                    ---------------------------
//                    innovating embedded systems
//
// Copyright (C) 2002-2013 Quantum Leaps, LLC. All rights reserved.
//
// This program is open source software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Alternatively, this program may be distributed and modified under the
// terms of Quantum Leaps commercial licenses, which expressly supersede
// the GNU General Public License and are specifically designed for
// licensees interested in retaining the proprietary status of their code.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Contact information:
// Quantum Leaps Web sites: http://www.quantum-leaps.com
//                          http://www.state-machine.com
// e-mail:                  info@quantum-leaps.com
//****************************************************************************
#include "qp_port.h"
#include "dpp.h"
#include "bsp.h"

extern "C" {
    #include "tm4c_cmsis.h"
    #include "sysctl.h"
    #include "gpio.h"
    #include "rom.h"
}

//****************************************************************************
namespace DPP {

Q_DEFINE_THIS_FILE

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
// DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
//
enum KernelUnawareISRs {                                         // see NOTE00
    // ...
    MAX_KERNEL_UNAWARE_CMSIS_PRI                           // keep always last
};
// "kernel-unaware" interrupts can't overlap "kernel-aware" interrupts
Q_ASSERT_COMPILE(MAX_KERNEL_UNAWARE_CMSIS_PRI <= QF_AWARE_ISR_CMSIS_PRI);

enum KernelAwareISRs {
    GPIOPORTA_PRIO = QF_AWARE_ISR_CMSIS_PRI,                     // see NOTE00
    SYSTICK_PRIO,
    // ...
    MAX_KERNEL_AWARE_CMSIS_PRI                             // keep always last
};
// "kernel-aware" interrupts should not overlap the PendSV priority
Q_ASSERT_COMPILE(MAX_KERNEL_AWARE_CMSIS_PRI <= (0xFF >>(8-__NVIC_PRIO_BITS)));

// ISRs defined in this BSP --------------------------------------------------
extern "C" void SysTick_Handler(void);
extern "C" void GPIOPortA_IRQHandler(void);

// Local-scope objects -------------------------------------------------------
static uint32_t l_rnd;                                          // random seed

#define LED_RED     (1U << 1)
#define LED_GREEN   (1U << 3)
#define LED_BLUE    (1U << 2)

#define USR_SW1     (1U << 4)
#define USR_SW2     (1U << 0)

#ifdef Q_SPY

    QP::QSTimeCtr QS_tickTime_;
    QP::QSTimeCtr QS_tickPeriod_;
    static uint8_t l_SysTick_Handler;
    static uint8_t l_GPIOPortA_IRQHandler;

    uint32_t const UART_BAUD_RATE    = static_cast<uint32_t>(115200U);
    uint32_t const UART_FR_TXFE      = static_cast<uint32_t>(0x80U);
    uint16_t const UART_TXFIFO_DEPTH = static_cast<uint16_t>(16U);

    enum AppRecords {                    // application-specific trace records
        PHILO_STAT = QP::QS_USER
    };

#endif

//............................................................................
extern "C" void SysTick_Handler(void) {

    QK_ISR_ENTRY();                         // infrom QK about entering an ISR

#ifdef Q_SPY
    {
        uint32_t dummy = SysTick->CTRL;        // clear SysTick_CTRL_COUNTFLAG
        QS_tickTime_ += QS_tickPeriod_;      // account for the clock rollover
    }
#endif

    QP::QF::TICK_X(0U, &l_SysTick_Handler);   // process time events at rate 0

    static uint32_t btn_debounced  = USR_SW1;
    static uint8_t  debounce_state = 0U;
    uint32_t btn = GPIOF->DATA_Bits[USR_SW1];             // read the user sw1
    switch (debounce_state) {
        case 0:
            if (btn != btn_debounced) {
                debounce_state = 1U;           // transition to the next state
            }
            break;
        case 1:
            if (btn != btn_debounced) {
                debounce_state = 2U;           // transition to the next state
            }
            else {
                debounce_state = 0U;             // transition back to state 0
            }
            break;
        case 2:
            if (btn != btn_debounced) {
                debounce_state = 3U;           // transition to the next state
            }
            else {
                debounce_state = 0U;             // transition back to state 0
            }
            break;
        case 3:
            if (btn != btn_debounced) {
                btn_debounced = btn;        // save the debounced button value

                if (btn == 0U) {                   // is the button depressed?
                    static QP::QEvt const pauseEvt =
                        QEVT_INITIALIZER(PAUSE_SIG);
                    QP::QF::PUBLISH(&pauseEvt, &l_SysTick_Handler);
                }
                else {
                    static QP::QEvt const pauseEvt =
                        QEVT_INITIALIZER(PAUSE_SIG);
                    QP::QF::PUBLISH(&pauseEvt, &l_SysTick_Handler);
                }
            }
            debounce_state = 0U;                 // transition back to state 0
            break;
        default:
            Q_ERROR();
            break;
    }
    QK_ISR_EXIT();                           // inform QK about exiting an ISR
}
//............................................................................
extern "C" void GPIOPortA_IRQHandler(void) {
    QK_ISR_ENTRY();                         // infrom QK about entering an ISR

    DPP::AO_Table->POST(Q_NEW(QP::QEvt, DPP::MAX_PUB_SIG),      // for testing
                   &l_GPIOPortA_IRQHandler);

    QK_ISR_EXIT();                           // infrom QK about exiting an ISR
}

//............................................................................
void BSP_init(void) {
                                             // Enable the floating-point unit
    SCB->CPACR |= (0xFU << 20);

    // Enable lazy stacking for interrupt handlers. This allows FPU
    // instructions to be used within interrupt handlers, but at the
    // expense of extra stack and CPU usage.
    //
    FPU->FPCCR |= (1U << FPU_FPCCR_ASPEN_Pos) | (1U << FPU_FPCCR_LSPEN_Pos);

    // Set the clocking to run directly from the crystal
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC
                       | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // enable clock to the peripherals used by the application
    SYSCTL->RCGC2 |= (1U << 5);                       // enable clock to GPIOF
    __NOP();                                     // wait after enabling clocks
    __NOP();
    __NOP();

    // configure the LEDs and push buttons
    GPIOF->DIR |= (LED_RED | LED_GREEN | LED_BLUE);   // set direction: output
    GPIOF->DEN |= (LED_RED | LED_GREEN | LED_BLUE);          // digital enable
    GPIOF->DATA_Bits[LED_RED]   = 0;                       // turn the LED off
    GPIOF->DATA_Bits[LED_GREEN] = 0;                       // turn the LED off
    GPIOF->DATA_Bits[LED_BLUE]  = 0;                       // turn the LED off

    // configure the User Switches
    GPIOF->DIR &= ~(USR_SW1 | USR_SW2);               //  set direction: input
    ROM_GPIOPadConfigSet(GPIO_PORTF_BASE, (USR_SW1 | USR_SW2),
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    BSP_randomSeed(1234U);

    Q_ALLEGE(QS_INIT(static_cast<void *>(0)));
    QS_OBJ_DICTIONARY(&l_SysTick_Handler);
    QS_OBJ_DICTIONARY(&l_GPIOPortA_IRQHandler);
    QS_USR_DICTIONARY(PHILO_STAT);
}
//............................................................................
void BSP_displayPhilStat(uint8_t const n, char_t const * const stat) {
    GPIOF->DATA_Bits[LED_BLUE] = ((stat[0] == 'e') ? LED_BLUE : 0U);


    QS_BEGIN(PHILO_STAT, AO_Philo[n])     // application-specific record begin
        QS_U8(1U, n);                                    // Philosopher number
        QS_STR(stat);                                    // Philosopher status
    QS_END()
}
//............................................................................
void BSP_displayPaused(uint8_t const paused) {
    GPIOF->DATA_Bits[LED_RED] = ((paused != 0U) ? LED_RED : 0U);
}
//............................................................................
uint32_t BSP_random(void) {     // a very cheap pseudo-random-number generator
    // code for testing the hardware FPU...
    float volatile x = 3.1415926F;
    x = x + 2.7182818F;

    // "Super-Duper" Linear Congruential Generator (LCG)
    // LCG(2^32, 3*7*11*13*23, 0, seed)
    //
    l_rnd = l_rnd * (3U*7U*11U*13U*23U);
    return l_rnd >> 8;
}
//............................................................................
void BSP_randomSeed(uint32_t const seed) {
    l_rnd = seed;
}
//............................................................................
void BSP_terminate(int16_t const result) {
    (void)result;
}

}                                                             // namespace DPP

//............................................................................
extern "C" void Q_onAssert(char const Q_ROM * const file, int_t line) {
    assert_failed(file, line);
}
//............................................................................
// error routine that is called if the CMSIS library encounters an error
extern "C" void assert_failed(char const *file, int line) {
    (void)file;                                      // avoid compiler warning
    (void)line;                                      // avoid compiler warning
    QF_INT_DISABLE();            // make sure that all interrupts are disabled
    ROM_SysCtlReset();                                     // reset the system
}

//****************************************************************************
namespace QP {

//............................................................................
void QF::onStartup(void) {
                 // set up the SysTick timer to fire at BSP_TICKS_PER_SEC rate
    (void)SysTick_Config(ROM_SysCtlClockGet() / DPP::BSP_TICKS_PER_SEC);

    // assing all priority bits for preemption-prio. and none to sub-prio.
    NVIC_SetPriorityGrouping(0U);

    // set priorities of ALL ISRs used in the system, see NOTE00
    //
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
    // DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
    //
    NVIC_SetPriority(SysTick_IRQn,   DPP::SYSTICK_PRIO);
    NVIC_SetPriority(GPIOPortA_IRQn, DPP::GPIOPORTA_PRIO);

                                                             // enable IRQs...
    NVIC_EnableIRQ(GPIOPortA_IRQn);
    NVIC_EnableIRQ(GPIOPortA_IRQn);
}
//............................................................................
void QF::onCleanup(void) {
}
//............................................................................
void QK::onIdle(void) {

    // toggle the User LED on and then off, see NOTE01
    QF_INT_DISABLE();
    GPIOF->DATA_Bits[LED_GREEN] = LED_GREEN;         // turn the Green LED on
    GPIOF->DATA_Bits[LED_GREEN] = 0;                 // turn the Green LED off
    QF_INT_ENABLE();

#ifdef Q_SPY
    if ((UART0->FR & DPP::UART_FR_TXFE) != 0U) {                   // TX done?
        uint16_t fifo = DPP::UART_TXFIFO_DEPTH;     // max bytes we can accept

        QF_INT_DISABLE();
        uint8_t const *block = QS::getBlock(&fifo);   // try to get next block
        QF_INT_ENABLE();

        while (fifo-- != 0) {                       // any bytes in the block?
            UART0->DR = *block++;                         // put into the FIFO
        }
    }
#elif defined NDEBUG
    // put the CPU and peripherals to the low-power mode
    // you might need to customize the clock management for your application,
    // see the datasheet for your particular Cortex-M3 MCU.
    __WFI();                                             // Wait-For-Interrupt
#endif
}

//----------------------------------------------------------------------------
#ifdef Q_SPY
//............................................................................
bool QS::onStartup(void const *) {
    static uint8_t qsBuf[6*256];                     // buffer for Quantum Spy
    uint32_t tmp;
    initBuf(qsBuf, sizeof(qsBuf));

                                   // enable the peripherals used by the UART0
    SYSCTL->RCGC1 |= (1U << 0);                       // enable clock to UART0
    SYSCTL->RCGC2 |= (1U << 0);                       // enable clock to GPIOA
    __NOP();                                     // wait after enabling clocks
    __NOP();
    __NOP();

                                    // configure UART0 pins for UART operation
    tmp = (1 << 0) | (1 << 1);
    GPIOA->DIR   &= ~tmp;
    GPIOA->AFSEL |= tmp;
    GPIOA->DR2R  |= tmp;           // set 2mA drive, DR4R and DR8R are cleared
    GPIOA->SLR   &= ~tmp;
    GPIOA->ODR   &= ~tmp;
    GPIOA->PUR   &= ~tmp;
    GPIOA->PDR   &= ~tmp;
    GPIOA->DEN   |= tmp;

              // configure the UART for the desired baud rate, 8-N-1 operation
    tmp = (((ROM_SysCtlClockGet() * 8U) / DPP::UART_BAUD_RATE) + 1U) / 2U;
    UART0->IBRD   = tmp / 64U;
    UART0->FBRD   = tmp % 64U;
    UART0->LCRH   = 0x60U;                         // configure 8-N-1 operation
    UART0->LCRH  |= 0x10U;
    UART0->CTL   |= (1U << 0) | (1U << 8) | (1U << 9);

    DPP::QS_tickPeriod_ = ROM_SysCtlClockGet() / DPP::BSP_TICKS_PER_SEC;
    DPP::QS_tickTime_ = DPP::QS_tickPeriod_; // to start the timestamp at zero

                                                    // setup the QS filters...
    QS_FILTER_ON(QS_ALL_RECORDS);

//    QS_FILTER_OFF(QS_QEP_STATE_EMPTY);
//    QS_FILTER_OFF(QS_QEP_STATE_ENTRY);
//    QS_FILTER_OFF(QS_QEP_STATE_EXIT);
//    QS_FILTER_OFF(QS_QEP_STATE_INIT);
//    QS_FILTER_OFF(QS_QEP_INIT_TRAN);
//    QS_FILTER_OFF(QS_QEP_INTERN_TRAN);
//    QS_FILTER_OFF(QS_QEP_TRAN);
//    QS_FILTER_OFF(QS_QEP_IGNORED);

//    QS_FILTER_OFF(QS_QF_ACTIVE_ADD);
//    QS_FILTER_OFF(QS_QF_ACTIVE_REMOVE);
//    QS_FILTER_OFF(QS_QF_ACTIVE_SUBSCRIBE);
//    QS_FILTER_OFF(QS_QF_ACTIVE_UNSUBSCRIBE);
//    QS_FILTER_OFF(QS_QF_ACTIVE_POST_FIFO);
//    QS_FILTER_OFF(QS_QF_ACTIVE_POST_LIFO);
//    QS_FILTER_OFF(QS_QF_ACTIVE_GET);
//    QS_FILTER_OFF(QS_QF_ACTIVE_GET_LAST);
//    QS_FILTER_OFF(QS_QF_EQUEUE_INIT);
//    QS_FILTER_OFF(QS_QF_EQUEUE_POST_FIFO);
//    QS_FILTER_OFF(QS_QF_EQUEUE_POST_LIFO);
//    QS_FILTER_OFF(QS_QF_EQUEUE_GET);
//    QS_FILTER_OFF(QS_QF_EQUEUE_GET_LAST);
//    QS_FILTER_OFF(QS_QF_MPOOL_INIT);
//    QS_FILTER_OFF(QS_QF_MPOOL_GET);
//    QS_FILTER_OFF(QS_QF_MPOOL_PUT);
//    QS_FILTER_OFF(QS_QF_PUBLISH);
//    QS_FILTER_OFF(QS_QF_NEW);
//    QS_FILTER_OFF(QS_QF_GC_ATTEMPT);
//    QS_FILTER_OFF(QS_QF_GC);
//    QS_FILTER_OFF(QS_QF_TICK);
//    QS_FILTER_OFF(QS_QF_TIMEEVT_ARM);
//    QS_FILTER_OFF(QS_QF_TIMEEVT_AUTO_DISARM);
//    QS_FILTER_OFF(QS_QF_TIMEEVT_DISARM_ATTEMPT);
//    QS_FILTER_OFF(QS_QF_TIMEEVT_DISARM);
//    QS_FILTER_OFF(QS_QF_TIMEEVT_REARM);
//    QS_FILTER_OFF(QS_QF_TIMEEVT_POST);
    QS_FILTER_OFF(QS_QF_CRIT_ENTRY);
    QS_FILTER_OFF(QS_QF_CRIT_EXIT);
    QS_FILTER_OFF(QS_QF_ISR_ENTRY);
    QS_FILTER_OFF(QS_QF_ISR_EXIT);

    return true;                                             // return success
}
//............................................................................
void QS::onCleanup(void) {
}
//............................................................................
QSTimeCtr QS::onGetTime(void) {            // invoked with interrupts disabled
    QSTimeCtr ret = DPP::QS_tickTime_ - static_cast<QSTimeCtr>(SysTick->VAL);
    if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U) {     // flag set?
        ret += DPP::QS_tickPeriod_;
    }
    return ret;
}
//............................................................................
void QS::onFlush(void) {
    uint16_t fifo = DPP::UART_TXFIFO_DEPTH;                   // Tx FIFO depth
    uint8_t const *block;
    while ((block = getBlock(&fifo)) != static_cast<uint8_t *>(0)) {
                                              // busy-wait until TX FIFO empty
        while ((UART0->FR & DPP::UART_FR_TXFE) == 0U) {
        }

        while (fifo-- != 0U) {                      // any bytes in the block?
            UART0->DR = *block++;                      // put into the TX FIFO
        }
        fifo = DPP::UART_TXFIFO_DEPTH;            // re-load the Tx FIFO depth
    }
}
#endif                                                                // Q_SPY
//----------------------------------------------------------------------------

}                                                              // namespace QP

//****************************************************************************
// NOTE01:
// The User LED is used to visualize the idle loop activity. The brightness
// of the LED is proportional to the frequency of invcations of the idle loop.
// Please note that the LED is toggled with interrupts locked, so no interrupt
// execution time contributes to the brightness of the User LED.
//
