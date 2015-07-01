//****************************************************************************
// Model: dpp.qm
// File:  ./dpp.h
//
// This code has been generated by QM tool (see state-machine.com/qm).
// DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
//
// This program is open source software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
// for more details.
//****************************************************************************
//${.::dpp.h} ................................................................
#ifndef dpp_h
#define dpp_h

namespace DPP {

enum DPPSignals {
    EAT_SIG = QP::Q_USER_SIG, // published by Table to let a philosopher eat
    DONE_SIG,                 // published by Philosopher when done eating
    PAUSE_SIG,                // published by BSP to pause the application
    TERMINATE_SIG,            // published by BSP to terminate the application
    MAX_PUB_SIG,              // the last published signal

    HUNGRY_SIG,               // posted direclty to Table from hungry Philo
    MAX_SIG                   // the last signal
};

} // namespace DPP

namespace DPP {

//${Events::TableEvt} ........................................................
class TableEvt : public QP::QEvt {
public:
    uint8_t philoNum;
};

} // namespace DPP

// number of philosophers
#define N_PHILO ((uint8_t)5)

namespace DPP {

extern QP::QActive * const AO_Philo[N_PHILO];

} // namespace DPP

namespace DPP {

extern QP::QActive * const AO_Table;

} // namespace DPP

#endif // dpp_h
