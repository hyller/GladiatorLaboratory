//****************************************************************************
// Product: DPP example
// Last updated for version 5.3.0
// Last updated on  2014-06-27
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
#include "qp_port.h"
#include "dpp.h"
#include "bsp.h"

/*..........................................................................*/
int main() {
    /* stacks */
    static OS_STACKPTR int tableStack[128];
    static OS_STACKPTR int philoStack[N_PHILO][128];

    static QP::QEvt const *tableQueueSto[N_PHILO];
    static QP::QEvt const *philoQueueSto[N_PHILO][N_PHILO];
    static QP::QSubscrList subscrSto[DPP::MAX_PUB_SIG];
    static QF_MPOOL_EL(DPP::TableEvt) smlPoolSto[2*N_PHILO]; // small pool
    uint8_t n;

    QP::QF::init();  // initialize the framework and the underlying RT kernel
    DPP::BSP_init(); // initialize the Board Support Package

    /* object dictionaries... */
    QS_OBJ_DICTIONARY(smlPoolSto);
    QS_OBJ_DICTIONARY(tableQueueSto);
    QS_OBJ_DICTIONARY(philoQueueSto[0]);
    QS_OBJ_DICTIONARY(philoQueueSto[1]);
    QS_OBJ_DICTIONARY(philoQueueSto[2]);
    QS_OBJ_DICTIONARY(philoQueueSto[3]);
    QS_OBJ_DICTIONARY(philoQueueSto[4]);

    // init publish-subscribe
    QP::QF::psInit(subscrSto, Q_DIM(subscrSto));

    // initialize event pools...
    QP::QF::poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    // start the active objects...
    for (n = 0U; n < N_PHILO; ++n) {
        DPP::AO_Philo[n]->start((uint8_t)(n + 1),
                      philoQueueSto[n], Q_DIM(philoQueueSto[n]),
                      philoStack[n], sizeof(philoStack[n]));
    }
    DPP::AO_Table->start((uint8_t)(N_PHILO + 1),
                  tableQueueSto, Q_DIM(tableQueueSto),
                  tableStack, sizeof(tableStack));

    return QP::QF::run(); // run the QF application
}

