//****************************************************************************
// Product: QF/C++ port to 80x86, QK, Open Watcom
// Last Updated for Version: 4.4.00
// Date of the Last Update:  Apr 19, 2012
//
//                    Q u a n t u m     L e a P s
//                    ---------------------------
//                    innovating embedded systems
//
// Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
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
#ifndef qf_port_h
#define qf_port_h
                    // The maximum number of active objects in the application
#define QF_MAX_ACTIVE               63

                                               // DOS interrupt disable/enable
#define QF_INT_DISABLE()            _disable()
#define QF_INT_ENABLE()             _enable()

                                            // DOS critical section entry/exit
// QF_CRIT_STAT_TYPE not defined
#define QF_CRIT_ENTRY(dummy)        _disable()
#define QF_CRIT_EXIT(dummy)         _enable()

#include <i86.h>                                   // for _disable()/_enable()

#include "qep_port.h"                                              // QEP port
#include "qk_port.h"                                                // QK port
#include "qf.h"                    // QF platform-independent public interface

#endif                                                            // qf_port_h
