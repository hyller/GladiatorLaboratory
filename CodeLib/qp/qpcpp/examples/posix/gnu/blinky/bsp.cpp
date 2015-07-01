//****************************************************************************
// Product: Simple Blinky example, POSIX
// Last Updated for Version: 5.1.0
// Date of the Last Update:  Oct 01, 2013
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
#include "bsp.h"

#include <iostream>
#include <stdlib.h>
#include <string.h>                               // for memcpy() and memset()
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

using namespace std;
using namespace QP;

Q_DEFINE_THIS_FILE



//............................................................................
static struct termios l_tsav;      // structure with saved terminal attributes

//............................................................................
void BSP_init(void) {
    cout << "Simple Blinky example" << endl
         << "QP version: " << QP_VERSION_STR << endl
         << "Press ESC to quit..." << endl;
}
//............................................................................
void BSP_ledOff(void) {
    cout << "OFF" << endl;
}
//............................................................................
void BSP_ledOn(void) {
    cout << "ON" << endl;
}

//............................................................................
void QF::onStartup(void) {
    struct termios tio;                        // modified terminal attributes

    tcgetattr(0, &l_tsav);             // save the current terminal attributes
    tcgetattr(0, &tio);              // obtain the current terminal attributes
    tio.c_lflag &= ~(ICANON | ECHO);      // disable the canonical mode & echo
    tcsetattr(0, TCSANOW, &tio);                     // set the new attributes

    QF_setTickRate(BSP_TICKS_PER_SEC);            // set the desired tick rate
}
//............................................................................
void QF::onCleanup(void) {
    cout << endl << "Bye Bye!!!" << endl;
    tcsetattr(0, TCSANOW, &l_tsav);   // restore the saved terminal attributes
}
//............................................................................
void QP::QF_onClockTick(void) {
    struct timeval timeout = { 0 };                    // timeout for select()
    fd_set con;                             // FD set representing the console

    QF::TICK_X(0U, (void *)0);         // perform the QF clock tick processing

    FD_ZERO(&con);
    FD_SET(0, &con);
    // check if a console input is available, returns immediately
    if (0 != select(1, &con, 0, 0, &timeout)) {         // any descriptor set?
        char ch;
        read(0, &ch, 1);
        if (ch == '\33') {                                     // ESC pressed?
            QF::stop();
        }
    }
}
//............................................................................
extern "C" void Q_onAssert(char const Q_ROM * const file, int line) {
    cout << "Assertion failed in " << file
              << "line " << line << endl;
    QF::onCleanup();
    exit(-1);
}
