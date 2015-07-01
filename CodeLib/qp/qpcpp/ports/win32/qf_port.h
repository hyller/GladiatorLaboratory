//****************************************************************************
// Product: QF/C++  port to Win32
// Last updated for version 5.3.0
// Last updated on  2014-03-04
//
//                    Q u a n t u m     L e a P s
//                    ---------------------------
//                    innovating embedded systems
//
// Copyright (C) Quantum Leaps, www.state-machine.com.
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
// Web:   www.state-machine.com
// Email: info@state-machine.com
//****************************************************************************
#ifndef qf_port_h
#define qf_port_h

// Win32 event queue and thread types
#define QF_EQUEUE_TYPE     QEQueue
#define QF_OS_OBJECT_TYPE  void*
#define QF_THREAD_TYPE     void*

// The maximum number of active objects in the application
#define QF_MAX_ACTIVE               63

// The number of system clock tick rates
#define QF_MAX_TICK_RATE            2

// various QF object sizes configuration for this port
#define QF_EVENT_SIZ_SIZE           4
#define QF_EQUEUE_CTR_SIZE          4
#define QF_MPOOL_SIZ_SIZE           4
#define QF_MPOOL_CTR_SIZE           4
#define QF_TIMEEVT_CTR_SIZE         4

// Win32 critical section, see NOTE01
// QF_CRIT_STAT_TYPE not defined
#define QF_CRIT_ENTRY(dummy)        (QP::QF_enterCriticalSection())
#define QF_CRIT_EXIT(dummy)         (QP::QF_leaveCriticalSection())

#include "qep_port.h"  // QEP port
#include "qequeue.h"   // Win32 needs event-queue
#include "qmpool.h"    // Win32 needs memory-pool
#include "qpset.h"     // Win32 needs priority-set
#include "qf.h"        // QF platform-independent public interface

namespace QP {

void QF_enterCriticalSection(void);
void QF_leaveCriticalSection(void);
void QF_setTickRate(uint32_t ticksPerSec); // set clock tick rate
void QF_onClockTick(void); // clock tick callback (provided in the app)

} // namespace QP

//****************************************************************************
// interface used only inside QF, but not in applications

#ifdef QP_IMPL
    // Win32-specific event queue customization
    #define QACTIVE_EQUEUE_WAIT_(me_) \
        while ((me_)->m_eQueue.m_frontEvt == static_cast<QEvt const *>(0)) { \
            QF_CRIT_EXIT_(); \
            (void)WaitForSingleObject((me_)->m_osObject, (DWORD)INFINITE); \
            QF_CRIT_ENTRY_(); \
        }

    #define QACTIVE_EQUEUE_SIGNAL_(me_) \
        (void)SetEvent((me_)->m_osObject)

    #define QACTIVE_EQUEUE_ONEMPTY_(me_) ((void)0)

    // Win32-specific event pool operations
    #define QF_EPOOL_TYPE_            QMPool
    #define QF_EPOOL_INIT_(p_, poolSto_, poolSize_, evtSize_) \
        (p_).init(poolSto_, poolSize_, evtSize_)
    #define QF_EPOOL_EVENT_SIZE_(p_)  ((p_).getBlockSize())
    #define QF_EPOOL_GET_(p_, e_, m_) \
        ((e_) = static_cast<QEvt *>((p_).get((m_))))
    #define QF_EPOOL_PUT_(p_, e_)     ((p_).put(e_))

    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>  // Win32 API
#endif  // QP_IMPL

//****************************************************************************
// NOTE01:
// QF, like all real-time frameworks, needs to execute certain sections of
// code indivisibly to avoid data corruption. The most straightforward way of
// protecting such critical sections of code is disabling and enabling
// interrupts, which Win32 does not allow.
//
// This QF port uses therefore a single package-scope Win32 critical section
// object QF_win32CritSect_ to protect all critical sections.
//
// Using the single critical section object for all crtical section guarantees
// that only one thread at a time can execute inside a critical section. This
// prevents race conditions and data corruption.
//
// Please note, however, that the Win32 critical section implementation
// behaves differently than interrupt locking. A common Win32 critical section
// ensures that only one thread at a time can execute a critical section, but
// it does not guarantee that a context switch cannot occur within the
// critical section. In fact, such context switches probably will happen, but
// they should not cause concurrency hazards because the critical section
// eliminates all race conditionis.
//
// Unlinke simply disabling and enabling interrupts, the critical section
// approach is also subject to priority inversions. Various versions of
// Windows handle priority inversions differently, but it seems that most of
// them recognize priority inversions and dynamically adjust the priorities of
// threads to prevent it. Please refer to the MSN articles for more
// information.
//

#endif // qf_port_h
