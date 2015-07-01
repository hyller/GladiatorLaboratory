/*****************************************************************************
* Product: PELICAN crossing example
* Last updated for version 5.3.0
* Last updated on  2014-04-14
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, www.state-machine.com.
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
* Web:   www.state-machine.com
* Email: info@state-machine.com
*****************************************************************************/
#ifndef qpn_port_h
#define qpn_port_h

#define Q_NMSM
//#define Q_NHSM
#define Q_NFSM
#define Q_PARAM_SIZE            4
#define QF_TIMEEVT_CTR_SIZE     2

/* maximum # active objects--must match EXACTLY the QF_active[] definition  */
#define QF_MAX_ACTIVE           2

/* interrupt locking policy for task level */
#define QF_INT_DISABLE()        _disable()
#define QF_INT_ENABLE()         _enable()

/* interrupt disabling policy for interrupt level */
/* #define QF_ISR_NEST */

/* interrupt entry/exit for QK-nano */
#ifndef QF_ISR_NEST  /* ISR nesting NOT allowed */

    #define QK_ISR_ENTRY()      ((void)0)

    #define QK_ISR_EXIT()  do { \
        outp(0x20, 0x20); \
        QK_SCHEDULE_(); \
    } while (0)

#else /* ISR nesting allowed */

    #define QK_ISR_ENTRY() do { \
        ++QK_intNest_; \
        _enable(); \
    } while (0)

    #define QK_ISR_EXIT()  do { \
        _disable(); \
        outp(0x20, 0x20); \
        --QK_intNest_; \
        QK_SCHEDULE_(); \
    } while (0)

#endif

#include <i86.h>        /* for _disable()/_enable() */
#include <stdint.h>     /* Exact-width types. WG14/N843 C99 Standard */
#include <stdbool.h>    /* Boolean type.      WG14/N843 C99 Standard */

#include "qepn.h"       /* QEP-nano platform-independent public interface */
#include "qfn.h"        /* QF-nano platform-independent public interface */
#include "qkn.h"        /* QK-nano platform-independent header file */
#include "qassert.h"    /* QP-nano assertions header file */

#endif /* qpn_port_h */
