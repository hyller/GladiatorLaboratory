//****************************************************************************
// Model: game.qm
// File:  ./tunnel.cpp
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
//${.::tunnel.cpp} ...........................................................
#include "qp_port.h"
#include "bsp.h"
#include "game.h"
#include <string.h> // for memmove() and memcpy()

Q_DEFINE_THIS_FILE

// local objects -------------------------------------------------------------
namespace GAME {

//${AOs::Tunnel} .............................................................
class Tunnel : public QP::GuiQMActive {
private:
    QP::QTimeEvt m_blinkTimeEvt;
    QP::QTimeEvt m_screenTimeEvt;
    QP::QMsm * m_mines[GAME_MINES_MAX];
    QP::QMsm * m_mine1_pool[GAME_MINES_MAX];
    QP::QMsm * m_mine2_pool[GAME_MINES_MAX];
    uint8_t m_blink_ctr;
    uint8_t m_last_mine_x;
    uint8_t m_last_mine_y;
    uint8_t m_wall_thickness_top;
    uint8_t m_wall_thickness_bottom;
    uint8_t m_minimal_gap;

public:
    Tunnel();

private:
    void advance();
    void plantMine();
    void addImageAt(
        uint8_t bmp,
        uint8_t x_pos,
        int8_t y_pos);
    void dispatchToAllMines(QP::QEvt const * e);
    bool isWallHit(
        uint8_t bmp,
        uint8_t x_pos,
        uint8_t y_pos);

protected:
    static QP::QState initial(Tunnel * const me, QP::QEvt const * const e);
    static QP::QState active  (Tunnel * const me, QP::QEvt const * const e);
    static QP::QMState const active_s;
    static QP::QState show_logo  (Tunnel * const me, QP::QEvt const * const e);
    static QP::QState show_logo_e(Tunnel * const me);
    static QP::QState show_logo_x(Tunnel * const me);
    static QP::QMState const show_logo_s;
    static QP::QState demo  (Tunnel * const me, QP::QEvt const * const e);
    static QP::QState demo_e(Tunnel * const me);
    static QP::QState demo_x(Tunnel * const me);
    static QP::QMState const demo_s;
    static QP::QState game_over  (Tunnel * const me, QP::QEvt const * const e);
    static QP::QState game_over_e(Tunnel * const me);
    static QP::QState game_over_x(Tunnel * const me);
    static QP::QMState const game_over_s;
    static QP::QState screen_saver  (Tunnel * const me, QP::QEvt const * const e);
    static QP::QState screen_saver_i(Tunnel * const me);
    static QP::QMState const screen_saver_s;
    static QP::QState screen_saver_hide  (Tunnel * const me, QP::QEvt const * const e);
    static QP::QState screen_saver_hide_e(Tunnel * const me);
    static QP::QState screen_saver_hide_x(Tunnel * const me);
    static QP::QMState const screen_saver_hide_s;
    static QP::QState screen_saver_show  (Tunnel * const me, QP::QEvt const * const e);
    static QP::QState screen_saver_show_e(Tunnel * const me);
    static QP::QState screen_saver_show_x(Tunnel * const me);
    static QP::QMState const screen_saver_show_s;
    static QP::QState playing  (Tunnel * const me, QP::QEvt const * const e);
    static QP::QState playing_e(Tunnel * const me);
    static QP::QState playing_x(Tunnel * const me);
    static QP::QMState const playing_s;
    static QP::QState final  (Tunnel * const me, QP::QEvt const * const e);
    static QP::QState final_e(Tunnel * const me);
    static QP::QMState const final_s;
};

} // namespace GAME

namespace GAME {

static Tunnel l_tunnel;       // the sole instance of the Tunnel active object

static uint32_t l_rnd;                                          // random seed
static uint8_t l_walls[GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8U];
static uint8_t l_frame[GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8U];

// local helper functions ----------------------------------------------------
static void randomSeed(uint32_t seed);
static uint32_t random(void);

// Public-scope objects ------------------------------------------------------
QP::QActive * const AO_Tunnel = &l_tunnel;  // opaque pointer

// helper functions ----------------------------------------------------------
//
// The bitmap for the "Press Button" text:
//
//     xxx.........................xxx........x...x...........
//     x..x........................x..x.......x...x...........
//     x..x.x.xx..xx...xxx..xxx....x..x.x..x.xxx.xxx..xx..xxx.
//     xxx..xx...x..x.x....x.......xxx..x..x..x...x..x..x.x..x
//     x....x....xxxx..xx...xx.....x..x.x..x..x...x..x..x.x..x
//     x....x....x.......x....x....x..x.x..x..x...x..x..x.x..x
//     x....x.....xxx.xxx..xxx.....xxx...xxx...x...x..xx..x..x
//     .......................................................
///
static uint8_t const press_button_bits[] = {
    0x7F, 0x09, 0x09, 0x06, 0x00, 0x7C, 0x08, 0x04, 0x04, 0x00,
    0x38, 0x54, 0x54, 0x58, 0x00, 0x48, 0x54, 0x54, 0x24, 0x00,
    0x48, 0x54, 0x54, 0x24, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x49,
    0x49, 0x36, 0x00, 0x3C, 0x40, 0x40, 0x7C, 0x00, 0x04, 0x3F,
    0x44, 0x00, 0x04, 0x3F, 0x44, 0x00, 0x38, 0x44, 0x44, 0x38,
    0x00, 0x7C, 0x04, 0x04, 0x78
};

// bitmap of the Ship:
//
//     x....
//     xxx..
//     xxxxx
///
static uint8_t const ship_bits[] = {
    0x07, 0x06, 0x06, 0x04, 0x04
};

// bitmap of the Missile:
//
//     xxx
///
static uint8_t const missile_bits[] = {
    0x01, 0x01, 0x01
};

// bitmap of the Mine type-1:
//
//     .x.
//     xxx
//     .x.
///
static uint8_t const mine1_bits[] = {
    0x02, 0x07, 0x02
};

// bitmap of the Mine type-2:
//
//     x..x
//     .xx.
//     .xx.
//     x..x
///
static uint8_t const mine2_bits[] = {
    0x09, 0x06, 0x06, 0x09
};

// Mine type-2 is nastier than Mine type-1. The type-2 mine can
// hit the Ship with any of its "tentacles". However, it can be
// destroyed by the Missile only by hitting its center, defined as
// the following bitmap:
//
//     ....
//     .xx.
//     .xx.
//     ....
///
static uint8_t const mine2_missile_bits[] = {
    0x00, 0x06, 0x06, 0x00
};

// The bitmap of the explosion stage 0:
//
//     .......
//     .......
//     ...x...
//     ..x.x..
//     ...x...
//     .......
//     .......
///
static uint8_t const explosion0_bits[] = {
    0x00, 0x00, 0x08, 0x14, 0x08, 0x00, 0x00
};

// The bitmap of the explosion stage 1:
//
//     .......
//     .......
//     ..x.x..
//     ...x...
//     ..x.x..
//     .......
//     .......
///
static uint8_t const explosion1_bits[] = {
    0x00, 0x00, 0x14, 0x08, 0x14, 0x00, 0x00
};

// The bitmap of the explosion stage 2:
//
//     .......
//     .x...x.
//     ..x.x..
//     ...x...
//     ..x.x..
//     .x...x.
//     .......
///
static uint8_t const explosion2_bits[] = {
    0x00, 0x22, 0x14, 0x08, 0x14, 0x22, 0x00
};

// The bitmap of the explosion stage 3:
//
//     x..x..x
//     .x.x.x.
//     ..x.x..
//     xx.x.xx
//     ..x.x..
//     .x.x.x.
//     x..x..x
///
static uint8_t const explosion3_bits[] = {
    0x49, 0x2A, 0x14, 0x6B, 0x14, 0x2A, 0x49
};

struct Bitmap {               // the auxiliary structure to hold const bitmaps
    uint8_t const *bits;                             // the bits in the bitmap
    uint8_t width;                                  // the width of the bitmap
};

static Bitmap const l_bitmap[MAX_BMP] = {
    { press_button_bits,  Q_DIM(press_button_bits)  },
    { ship_bits,          Q_DIM(ship_bits)          },
    { missile_bits,       Q_DIM(missile_bits)       },
    { mine1_bits,         Q_DIM(mine1_bits)         },
    { mine2_bits,         Q_DIM(mine2_bits)         },
    { mine2_missile_bits, Q_DIM(mine2_missile_bits) },
    { explosion0_bits,    Q_DIM(explosion0_bits)    },
    { explosion1_bits,    Q_DIM(explosion1_bits)    },
    { explosion2_bits,    Q_DIM(explosion2_bits)    },
    { explosion3_bits,    Q_DIM(explosion3_bits)    }
};

//............................................................................
uint32_t random(void) {         // a very cheap pseudo-random-number generator
    // "Super-Duper" Linear Congruential Generator (LCG)
    // LCG(2^32, 3*7*11*13*23, 0, seed)
    //
    l_rnd = l_rnd * (3*7*11*13*23);
    return l_rnd >> 8;
}
//............................................................................
void randomSeed(uint32_t seed) {
    l_rnd = seed;
}
//............................................................................
bool do_bitmaps_overlap(uint8_t bmp_id1, uint8_t x1, uint8_t y1,
                        uint8_t bmp_id2, uint8_t x2, uint8_t y2)
{
    uint8_t x;
    uint8_t x0;
    uint8_t w;
    uint32_t bits1;
    uint32_t bits2;
    Bitmap const *bmp1;
    Bitmap const *bmp2;

    Q_REQUIRE((bmp_id1 < Q_DIM(l_bitmap)) && (bmp_id2 < Q_DIM(l_bitmap)));

    bmp1 = &l_bitmap[bmp_id1];
    bmp2 = &l_bitmap[bmp_id2];

    // is the incoming object starting to overlap the Mine bitmap?
    if ((x1 <= x2) && (x1 + bmp2->width > x2)) {
        x0 = x2 - x1;
        w  = x1 + bmp2->width - x2;
        if (w > bmp1->width) {
            w = bmp1->width;
        }
        for (x = 0; x < w; ++x) { // scan over the overlapping columns
            bits1 = ((uint32_t)bmp2->bits[x + x0] << y2);
            bits2 = ((uint32_t)bmp1->bits[x] << y1);
            if ((bits1 & bits2) != 0) { // do the bits overlap?
                return true;   // yes!
            }
        }
    }
    else {
        if ((x1 > x2) && (x2 + bmp1->width > x1)) {
            x0 = x1 - x2;
            w  = x2 + bmp1->width - x1;
            if (w > bmp2->width) {
                w = bmp2->width;
            }
            for (x = 0; x < w; ++x) {  // scan over the overlapping columns
                bits1 = ((uint32_t)bmp1->bits[x + x0] << y1);
                bits2 = ((uint32_t)bmp2->bits[x] << y2);
                if ((bits1 & bits2) != 0U) { // do the bits overlap?
                    return true; // yes!
                }
            }
        }
    }
    return false; // the bitmaps do not overlap
}

} // namespace GAME

// Active object definition ==================================================
namespace GAME {

//${AOs::Tunnel} .............................................................
//${AOs::Tunnel::Tunnel} .....................................................
Tunnel::Tunnel()
 : GuiQMActive(Q_STATE_CAST(&Tunnel::initial)),
   m_blinkTimeEvt(BLINK_TIMEOUT_SIG),
   m_screenTimeEvt(SCREEN_TIMEOUT_SIG),
   m_last_mine_x(0U), m_last_mine_y(0U)
{
    for (uint8_t n = 0U; n < GAME_MINES_MAX; ++n) {
        m_mine1_pool[n] = Mine1_getInst(n); // initialize mine1-type pool
        m_mine2_pool[n] = Mine2_getInst(n); // initialize mine2-type pool
        m_mines[n] = (QHsm *)0;             // mine 'n' is unused
    }
}

//${AOs::Tunnel::advance} ....................................................
void Tunnel::advance() {
    uint32_t bmp1; // bimap representing 1 column of the image

    uint32_t rnd = (random() & 0xFFU);

    // reduce the top wall thickness 18.75% of the time
    if ((rnd < 48U) && (m_wall_thickness_top > 0U)) {
        --m_wall_thickness_top;
    }

    // reduce the bottom wall thickness 18.75% of the time
    if ((rnd > 208U) && (m_wall_thickness_bottom > 0U)) {
        --m_wall_thickness_bottom;
    }

    rnd = (random() & 0xFFU);

    // grow the top wall thickness 18.75% of the time
    if ((rnd < 48U)
        && ((GAME_SCREEN_HEIGHT
             - m_wall_thickness_top
             - m_wall_thickness_bottom) > m_minimal_gap)
        && ((m_last_mine_x < (GAME_SCREEN_WIDTH - 5U))
            || (m_last_mine_y > (m_wall_thickness_top + 1U))))
    {
        ++m_wall_thickness_top;
    }

    // grow the bottom wall thickness 18.75% of the time
    if ((rnd > 208U)
        && ((GAME_SCREEN_HEIGHT
             - m_wall_thickness_top
             - m_wall_thickness_bottom) > m_minimal_gap)
        && ((m_last_mine_x < (GAME_SCREEN_WIDTH - 5U))
             || (m_last_mine_y + 1U
                < (GAME_SCREEN_HEIGHT - m_wall_thickness_bottom))))
    {
        ++m_wall_thickness_bottom;
    }

    // advance the Tunnel by 1 game step to the left
    memmove(l_walls, l_walls + GAME_SPEED_X,
            (GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8U) - GAME_SPEED_X);

    bmp1 = (~(~0U << m_wall_thickness_top))
            | (~0U << (GAME_SCREEN_HEIGHT
                        - m_wall_thickness_bottom));

    l_walls[GAME_SCREEN_WIDTH - 1] = (uint8_t)bmp1;
    l_walls[GAME_SCREEN_WIDTH + GAME_SCREEN_WIDTH - 1] = (uint8_t)(bmp1 >> 8);

    // copy the Tunnel layer to the main frame buffer
    memcpy(l_frame, l_walls, (GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8U));
}
//${AOs::Tunnel::plantMine} ..................................................
void Tunnel::plantMine() {
    uint32_t rnd = (random() & 0xFFU);

    if (m_last_mine_x > 0U) {
        --m_last_mine_x; // shift the last Mine 1 position to the left
    }
    // last mine far enough?
    if ((m_last_mine_x + GAME_MINES_DIST_MIN < GAME_SCREEN_WIDTH)
        && (rnd < 8U)) // place the mines only 5% of the time
    {
        uint8_t n;
        for (n = 0U; n < Q_DIM(m_mines); ++n) { // look for disabled mines
            if (m_mines[n] == (QHsm *)0) {
                break;
            }
        }
        if (n < Q_DIM(m_mines)) { // a disabled Mine found?
            rnd = (random() & 0xFFFFU);

            if ((rnd & 1U) == 0U) { // choose the type of the mine
                m_mines[n] = m_mine1_pool[n];
            }
            else {
                m_mines[n] = m_mine2_pool[n];
            }

            // new Mine is planted in the last column of the tunnel
            m_last_mine_x = GAME_SCREEN_WIDTH;

            // choose a random y-position for the Mine in the Tunnel
            rnd %= (GAME_SCREEN_HEIGHT
                    - m_wall_thickness_top
                    - m_wall_thickness_bottom - 4U);
            m_last_mine_y = (uint8_t)(m_wall_thickness_top + 2U + rnd);

            // event to dispatch to the Mine
            ObjectPosEvt ope(MINE_PLANT_SIG,
                             m_last_mine_x, m_last_mine_y);
            m_mines[n]->dispatch(&ope); // direct dispatch
        }
    }
}
//${AOs::Tunnel::addImageAt} .................................................
void Tunnel::addImageAt(
    uint8_t bmp,
    uint8_t x_pos,
    int8_t y_pos)
{
    Q_REQUIRE(bmp < Q_DIM(l_bitmap));

    uint8_t w = l_bitmap[bmp].width;
    if (w > GAME_SCREEN_WIDTH - x_pos) {
        w = GAME_SCREEN_WIDTH - x_pos;
    }
    for (uint8_t x = 0U; x < w; ++x) {
        uint32_t bmp1;
        if (y_pos >= 0) {
            bmp1 = (l_bitmap[bmp].bits[x] << (uint8_t)y_pos);
        }
        else {
            bmp1 = (l_bitmap[bmp].bits[x] >> (uint8_t)(-y_pos));
        }
        l_frame[x_pos + x] |= (uint8_t)bmp1;
        l_frame[x_pos + x + GAME_SCREEN_WIDTH] |= (uint8_t)(bmp1 >> 8);
    }
}
//${AOs::Tunnel::dispatchToAllMin~} ..........................................
void Tunnel::dispatchToAllMines(QP::QEvt const * e) {
    for (uint8_t n = 0U; n < GAME_MINES_MAX; ++n) {
        if (m_mines[n] != (QHsm *)0) { // is the mine used?
            m_mines[n]->dispatch(e);
        }
    }
}
//${AOs::Tunnel::isWallHit} ..................................................
bool Tunnel::isWallHit(
    uint8_t bmp,
    uint8_t x_pos,
    uint8_t y_pos)
{
    Q_REQUIRE(bmp < Q_DIM(l_bitmap));

    uint8_t w = l_bitmap[bmp].width;
    if (w > GAME_SCREEN_WIDTH - x_pos) {
        w = GAME_SCREEN_WIDTH - x_pos;
    }
    for (uint8_t x = 0U; x < w; ++x) {
        uint32_t bmp1 = ((uint32_t)l_bitmap[bmp].bits[x] << y_pos);
        if (((l_walls[x_pos + x] & (uint8_t)bmp1) != 0U)
            || ((l_walls[x_pos + x + GAME_SCREEN_WIDTH]
                 & (uint8_t)(bmp1 >> 8)) != 0))
        {
            return true;
        }
    }
    return false;
}
//${AOs::Tunnel::SM} .........................................................
QP::QState Tunnel::initial(Tunnel * const me, QP::QEvt const * const e) {
    static struct {
        QP::QMState const *target;
        QP::QActionHandler act[2];
    } const tatbl_ = { // transition-action table
        &show_logo_s,
        {
            Q_ACTION_CAST(&show_logo_e), // entry
            Q_ACTION_CAST(0)  // zero terminator
        }
    };
    // ${AOs::Tunnel::SM::initial}
    for (uint8_t n = 0; n < GAME_MINES_MAX; ++n) {
       me->m_mine1_pool[n]->init();       // take the initial tran. for Mine1
       me->m_mine2_pool[n]->init();       // take the initial tran. for Mine2
    }

    randomSeed(1234);                      // seed the pseudo-random generator

    me->subscribe(TIME_TICK_SIG);
    me->subscribe(PLAYER_TRIGGER_SIG);
    me->subscribe(PLAYER_QUIT_SIG);

    QS_OBJ_DICTIONARY(&l_tunnel);       // object dictionary for Tunnel object
    QS_OBJ_DICTIONARY(&l_tunnel.m_blinkTimeEvt);
    QS_OBJ_DICTIONARY(&l_tunnel.m_screenTimeEvt);

    QS_FUN_DICTIONARY(&Tunnel::initial);   // fun. dictionaries for Tunnel HSM
    QS_FUN_DICTIONARY(&Tunnel::final);
    QS_FUN_DICTIONARY(&Tunnel::active);
    QS_FUN_DICTIONARY(&Tunnel::playing);
    QS_FUN_DICTIONARY(&Tunnel::demo);
    QS_FUN_DICTIONARY(&Tunnel::game_over);
    QS_FUN_DICTIONARY(&Tunnel::screen_saver);
    QS_FUN_DICTIONARY(&Tunnel::screen_saver_hide);
    QS_FUN_DICTIONARY(&Tunnel::screen_saver_show);

    QS_SIG_DICTIONARY(BLINK_TIMEOUT_SIG,  &l_tunnel);         // local signals
    QS_SIG_DICTIONARY(SCREEN_TIMEOUT_SIG, &l_tunnel);
    QS_SIG_DICTIONARY(SHIP_IMG_SIG,       &l_tunnel);
    QS_SIG_DICTIONARY(MISSILE_IMG_SIG,    &l_tunnel);
    QS_SIG_DICTIONARY(MINE_IMG_SIG,       &l_tunnel);
    QS_SIG_DICTIONARY(MINE_DISABLED_SIG,  &l_tunnel);
    QS_SIG_DICTIONARY(EXPLOSION_SIG,      &l_tunnel);
    QS_SIG_DICTIONARY(SCORE_SIG,          &l_tunnel);
    return QM_TRAN_INIT(&tatbl_);
}
//${AOs::Tunnel::SM::active} .................................................
QP::QMState const Tunnel::active_s = {
    static_cast<QP::QMState const *>(0), // superstate (top)
    Q_STATE_CAST(&active),
    Q_ACTION_CAST(0), // no entry action
    Q_ACTION_CAST(0), // no exit action
    Q_ACTION_CAST(0)  // no intitial tran.
};
// ${AOs::Tunnel::SM::active}
QP::QState Tunnel::active(Tunnel * const me, QP::QEvt const * const e) {
    QP::QState status_;
    switch (e->sig) {
        // ${AOs::Tunnel::SM::active::MINE_DISABLED}
        case MINE_DISABLED_SIG: {
            Q_ASSERT((Q_EVT_CAST(MineEvt)->id < GAME_MINES_MAX)
                     && (me->m_mines[Q_EVT_CAST(MineEvt)->id] != (QHsm *)0));
            me->m_mines[Q_EVT_CAST(MineEvt)->id] = (QHsm *)0;
            status_ = QM_HANDLED();
            break;
        }
        // ${AOs::Tunnel::SM::active::PLAYER_QUIT}
        case PLAYER_QUIT_SIG: {
            static struct {
                QP::QMState const *target;
                QP::QActionHandler act[2];
            } const tatbl_ = { // transition-action table
                &final_s,
                {
                    Q_ACTION_CAST(&final_e), // entry
                    Q_ACTION_CAST(0)  // zero terminator
                }
            };
            status_ = QM_TRAN(&tatbl_);
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}
//${AOs::Tunnel::SM::active::show_logo} ......................................
QP::QMState const Tunnel::show_logo_s = {
    &Tunnel::active_s, // superstate
    Q_STATE_CAST(&show_logo),
    Q_ACTION_CAST(&show_logo_e),
    Q_ACTION_CAST(&show_logo_x),
    Q_ACTION_CAST(0)  // no intitial tran.
};
// ${AOs::Tunnel::SM::active::show_logo}
QP::QState Tunnel::show_logo_e(Tunnel * const me) {
    me->m_blinkTimeEvt.postEvery(me, BSP_TICKS_PER_SEC/2U); // 1/2 sec
    me->m_screenTimeEvt.postIn(me, BSP_TICKS_PER_SEC*5U); // 5 sec timeout
    me->m_blink_ctr = 0U;
    BSP_drawNString(0U, 0U, " Quantum  LeAps ");
    BSP_drawNString(0U, 1U, "state-machine.co");
    return QM_ENTRY(&show_logo_s);
}
// ${AOs::Tunnel::SM::active::show_logo}
QP::QState Tunnel::show_logo_x(Tunnel * const me) {
    me->m_blinkTimeEvt.disarm();
    me->m_screenTimeEvt.disarm();
    return QM_EXIT(&show_logo_s);
}
// ${AOs::Tunnel::SM::active::show_logo}
QP::QState Tunnel::show_logo(Tunnel * const me, QP::QEvt const * const e) {
    QP::QState status_;
    switch (e->sig) {
        // ${AOs::Tunnel::SM::active::show_logo::SCREEN_TIMEOUT}
        case SCREEN_TIMEOUT_SIG: {
            static struct {
                QP::QMState const *target;
                QP::QActionHandler act[3];
            } const tatbl_ = { // transition-action table
                &demo_s,
                {
                    Q_ACTION_CAST(&show_logo_x), // exit
                    Q_ACTION_CAST(&demo_e), // entry
                    Q_ACTION_CAST(0)  // zero terminator
                }
            };
            status_ = QM_TRAN(&tatbl_);
            break;
        }
        // ${AOs::Tunnel::SM::active::show_logo::BLINK_TIMEOUT}
        case BLINK_TIMEOUT_SIG: {
            me->m_blink_ctr ^= 1U; // toggle the blink couner
            // ${AOs::Tunnel::SM::active::show_logo::BLINK_TIMEOUT::[me->m_blink_ctr~}
            if (me->m_blink_ctr == 0U) {
                BSP_drawNString(6U*9U, 0U,         " LeAps");
                BSP_drawNString(0U,    1U, "state-machine.co");
                status_ = QM_HANDLED();
            }
            // ${AOs::Tunnel::SM::active::show_logo::BLINK_TIMEOUT::[else]}
            else {
                BSP_drawNString(6U*9U, 0U,         "LeaPs ");
                BSP_drawNString(0U,    1U, "tate-machine.com");
                status_ = QM_HANDLED();
            }
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    (void)me; /* avoid compiler warning in case 'me' is not used */
    return status_;
}
//${AOs::Tunnel::SM::active::demo} ...........................................
QP::QMState const Tunnel::demo_s = {
    &Tunnel::active_s, // superstate
    Q_STATE_CAST(&demo),
    Q_ACTION_CAST(&demo_e),
    Q_ACTION_CAST(&demo_x),
    Q_ACTION_CAST(0)  // no intitial tran.
};
// ${AOs::Tunnel::SM::active::demo}
QP::QState Tunnel::demo_e(Tunnel * const me) {
    me->m_last_mine_x = 0U; // last mine at right edge of the tunnel
    me->m_last_mine_y = 0U;
    // set the tunnel properties...
    me->m_wall_thickness_top = 0U;
    me->m_wall_thickness_bottom = 0U;
    me->m_minimal_gap = GAME_SCREEN_HEIGHT - 3U;

    // erase the tunnel walls
    memset(l_walls, (uint8_t)0,
          (GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8U));

    me->m_blinkTimeEvt.postEvery(me, BSP_TICKS_PER_SEC/2U); // 1/2 sec
    me->m_screenTimeEvt.postIn(me, BSP_TICKS_PER_SEC*20U);  // 20 sec

    me->m_blink_ctr = 0U; // init the blink counter
    return QM_ENTRY(&demo_s);
}
// ${AOs::Tunnel::SM::active::demo}
QP::QState Tunnel::demo_x(Tunnel * const me) {
    me->m_blinkTimeEvt.disarm();
    me->m_screenTimeEvt.disarm();
    return QM_EXIT(&demo_s);
}
// ${AOs::Tunnel::SM::active::demo}
QP::QState Tunnel::demo(Tunnel * const me, QP::QEvt const * const e) {
    QP::QState status_;
    switch (e->sig) {
        // ${AOs::Tunnel::SM::active::demo::BLINK_TIMEOUT}
        case BLINK_TIMEOUT_SIG: {
            me->m_blink_ctr ^= 1U; /* toggle the blink cunter */
            status_ = QM_HANDLED();
            break;
        }
        // ${AOs::Tunnel::SM::active::demo::SCREEN_TIMEOUT}
        case SCREEN_TIMEOUT_SIG: {
            static struct {
                QP::QMState const *target;
                QP::QActionHandler act[3];
            } const tatbl_ = { // transition-action table
                &screen_saver_s,
                {
                    Q_ACTION_CAST(&demo_x), // exit
                    Q_ACTION_CAST(&screen_saver_i), // initial tran.
                    Q_ACTION_CAST(0)  // zero terminator
                }
            };
            status_ = QM_TRAN(&tatbl_);
            break;
        }
        // ${AOs::Tunnel::SM::active::demo::TIME_TICK}
        case TIME_TICK_SIG: {
            me->advance();
            if (me->m_blink_ctr != 0U) {
                // add the text bitmap into the frame buffer
                me->addImageAt(PRESS_BUTTON_BMP,
                    (GAME_SCREEN_WIDTH - 55U)/2U,
                    (GAME_SCREEN_HEIGHT - 8U)/2U);
            }
            BSP_drawBitmap(l_frame);
            status_ = QM_HANDLED();
            break;
        }
        // ${AOs::Tunnel::SM::active::demo::PLAYER_TRIGGER}
        case PLAYER_TRIGGER_SIG: {
            static struct {
                QP::QMState const *target;
                QP::QActionHandler act[3];
            } const tatbl_ = { // transition-action table
                &playing_s,
                {
                    Q_ACTION_CAST(&demo_x), // exit
                    Q_ACTION_CAST(&playing_e), // entry
                    Q_ACTION_CAST(0)  // zero terminator
                }
            };
            status_ = QM_TRAN(&tatbl_);
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}
//${AOs::Tunnel::SM::active::game_over} ......................................
QP::QMState const Tunnel::game_over_s = {
    &Tunnel::active_s, // superstate
    Q_STATE_CAST(&game_over),
    Q_ACTION_CAST(&game_over_e),
    Q_ACTION_CAST(&game_over_x),
    Q_ACTION_CAST(0)  // no intitial tran.
};
// ${AOs::Tunnel::SM::active::game_over}
QP::QState Tunnel::game_over_e(Tunnel * const me) {
    me->m_blinkTimeEvt.postEvery(me, BSP_TICKS_PER_SEC/2U); // 1/2 sec
    me->m_screenTimeEvt.postIn(me, BSP_TICKS_PER_SEC*5U); // 5 sec timeout
    me->m_blink_ctr = 0U;
    BSP_drawNString((GAME_SCREEN_WIDTH - 6U*9U)/2U, 0U, "Game Over");
    return QM_ENTRY(&game_over_s);
}
// ${AOs::Tunnel::SM::active::game_over}
QP::QState Tunnel::game_over_x(Tunnel * const me) {
    me->m_blinkTimeEvt.disarm();
    me->m_screenTimeEvt.disarm();
    BSP_updateScore(0U); // update the score on the display
    return QM_EXIT(&game_over_s);
}
// ${AOs::Tunnel::SM::active::game_over}
QP::QState Tunnel::game_over(Tunnel * const me, QP::QEvt const * const e) {
    QP::QState status_;
    switch (e->sig) {
        // ${AOs::Tunnel::SM::active::game_over::BLINK_TIMEOUT}
        case BLINK_TIMEOUT_SIG: {
            me->m_blink_ctr ^= 1U; // toggle the blink couner
            BSP_drawNString((GAME_SCREEN_WIDTH - 6U*9U)/2U, 0U,
                            ((me->m_blink_ctr == 0U)
                                ? "Game Over"
                                : "         "));
            status_ = QM_HANDLED();
            break;
        }
        // ${AOs::Tunnel::SM::active::game_over::SCREEN_TIMEOUT}
        case SCREEN_TIMEOUT_SIG: {
            static struct {
                QP::QMState const *target;
                QP::QActionHandler act[3];
            } const tatbl_ = { // transition-action table
                &demo_s,
                {
                    Q_ACTION_CAST(&game_over_x), // exit
                    Q_ACTION_CAST(&demo_e), // entry
                    Q_ACTION_CAST(0)  // zero terminator
                }
            };
            status_ = QM_TRAN(&tatbl_);
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}
//${AOs::Tunnel::SM::active::screen_saver} ...................................
QP::QMState const Tunnel::screen_saver_s = {
    &Tunnel::active_s, // superstate
    Q_STATE_CAST(&screen_saver),
    Q_ACTION_CAST(0), // no entry action
    Q_ACTION_CAST(0), // no exit action
    Q_ACTION_CAST(&screen_saver_i)
};
// ${AOs::Tunnel::SM::active::screen_saver::initial}
QP::QState Tunnel::screen_saver_i(Tunnel * const me) {
    static struct {
        QP::QMState const *target;
        QP::QActionHandler act[2];
    } const tatbl_ = { // transition-action table
        &screen_saver_hide_s,
        {
            Q_ACTION_CAST(&screen_saver_hide_e), // entry
            Q_ACTION_CAST(0)  // zero terminator
        }
    };
    // ${AOs::Tunnel::SM::active::screen_saver::initial}
    return QM_TRAN_INIT(&tatbl_);
}
// ${AOs::Tunnel::SM::active::screen_saver}
QP::QState Tunnel::screen_saver(Tunnel * const me, QP::QEvt const * const e) {
    QP::QState status_;
    switch (e->sig) {
        // ${AOs::Tunnel::SM::active::screen_saver::PLAYER_TRIGGER}
        case PLAYER_TRIGGER_SIG: {
            static struct {
                QP::QMState const *target;
                QP::QActionHandler act[2];
            } const tatbl_ = { // transition-action table
                &demo_s,
                {
                    Q_ACTION_CAST(&demo_e), // entry
                    Q_ACTION_CAST(0)  // zero terminator
                }
            };
            status_ = QM_TRAN(&tatbl_);
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}
//${AOs::Tunnel::SM::active::screen_saver::screen_saver_hid~} ................
QP::QMState const Tunnel::screen_saver_hide_s = {
    &Tunnel::screen_saver_s, // superstate
    Q_STATE_CAST(&screen_saver_hide),
    Q_ACTION_CAST(&screen_saver_hide_e),
    Q_ACTION_CAST(&screen_saver_hide_x),
    Q_ACTION_CAST(0)  // no intitial tran.
};
// ${AOs::Tunnel::SM::active::screen_saver::screen_saver_hid~}
QP::QState Tunnel::screen_saver_hide_e(Tunnel * const me) {
    BSP_displayOff(); // power down the display
    me->m_screenTimeEvt.postIn(me, BSP_TICKS_PER_SEC*3U); // 3s timeout
    return QM_ENTRY(&screen_saver_hide_s);
}
// ${AOs::Tunnel::SM::active::screen_saver::screen_saver_hid~}
QP::QState Tunnel::screen_saver_hide_x(Tunnel * const me) {
    me->m_screenTimeEvt.disarm();
    BSP_displayOn(); // power up the display
    return QM_EXIT(&screen_saver_hide_s);
}
// ${AOs::Tunnel::SM::active::screen_saver::screen_saver_hid~}
QP::QState Tunnel::screen_saver_hide(Tunnel * const me, QP::QEvt const * const e) {
    QP::QState status_;
    switch (e->sig) {
        // ${AOs::Tunnel::SM::active::screen_saver::screen_saver_hid~::SCREEN_TIMEOUT}
        case SCREEN_TIMEOUT_SIG: {
            static struct {
                QP::QMState const *target;
                QP::QActionHandler act[3];
            } const tatbl_ = { // transition-action table
                &screen_saver_show_s,
                {
                    Q_ACTION_CAST(&screen_saver_hide_x), // exit
                    Q_ACTION_CAST(&screen_saver_show_e), // entry
                    Q_ACTION_CAST(0)  // zero terminator
                }
            };
            status_ = QM_TRAN(&tatbl_);
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}
//${AOs::Tunnel::SM::active::screen_saver::screen_saver_sho~} ................
QP::QMState const Tunnel::screen_saver_show_s = {
    &Tunnel::screen_saver_s, // superstate
    Q_STATE_CAST(&screen_saver_show),
    Q_ACTION_CAST(&screen_saver_show_e),
    Q_ACTION_CAST(&screen_saver_show_x),
    Q_ACTION_CAST(0)  // no intitial tran.
};
// ${AOs::Tunnel::SM::active::screen_saver::screen_saver_sho~}
QP::QState Tunnel::screen_saver_show_e(Tunnel * const me) {
    uint32_t rnd = random();
    // clear the screen frame buffer
    memset(l_frame, (uint8_t)0,
           (GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8U));
    me->addImageAt(PRESS_BUTTON_BMP,
            (uint8_t)(rnd % (GAME_SCREEN_WIDTH - 55U)),
            (int8_t) (rnd % (GAME_SCREEN_HEIGHT - 8U)));
    BSP_drawBitmap(l_frame);
    me->m_screenTimeEvt.postIn(me, BSP_TICKS_PER_SEC/3U); // 1/3 sec timeout
    return QM_ENTRY(&screen_saver_show_s);
}
// ${AOs::Tunnel::SM::active::screen_saver::screen_saver_sho~}
QP::QState Tunnel::screen_saver_show_x(Tunnel * const me) {
    me->m_screenTimeEvt.disarm();
    // clear the screen frame buffer
    memset(l_frame, (uint8_t)0,
           (GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8U));
    BSP_drawBitmap(l_frame);
    return QM_EXIT(&screen_saver_show_s);
}
// ${AOs::Tunnel::SM::active::screen_saver::screen_saver_sho~}
QP::QState Tunnel::screen_saver_show(Tunnel * const me, QP::QEvt const * const e) {
    QP::QState status_;
    switch (e->sig) {
        // ${AOs::Tunnel::SM::active::screen_saver::screen_saver_sho~::SCREEN_TIMEOUT}
        case SCREEN_TIMEOUT_SIG: {
            static struct {
                QP::QMState const *target;
                QP::QActionHandler act[3];
            } const tatbl_ = { // transition-action table
                &screen_saver_hide_s,
                {
                    Q_ACTION_CAST(&screen_saver_show_x), // exit
                    Q_ACTION_CAST(&screen_saver_hide_e), // entry
                    Q_ACTION_CAST(0)  // zero terminator
                }
            };
            status_ = QM_TRAN(&tatbl_);
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}
//${AOs::Tunnel::SM::active::playing} ........................................
QP::QMState const Tunnel::playing_s = {
    &Tunnel::active_s, // superstate
    Q_STATE_CAST(&playing),
    Q_ACTION_CAST(&playing_e),
    Q_ACTION_CAST(&playing_x),
    Q_ACTION_CAST(0)  // no intitial tran.
};
// ${AOs::Tunnel::SM::active::playing}
QP::QState Tunnel::playing_e(Tunnel * const me) {
    static QP::QEvt const takeoff(TAKE_OFF_SIG);
    me->m_minimal_gap = GAME_SCREEN_HEIGHT - 3U;
    // erase the walls
    memset(l_walls, (uint8_t)0,
          (GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8U));
    AO_Ship->POST(&takeoff, me); // post the TAKEOFF sig
    return QM_ENTRY(&playing_s);
}
// ${AOs::Tunnel::SM::active::playing}
QP::QState Tunnel::playing_x(Tunnel * const me) {
    static QP::QEvt const recycle(MINE_RECYCLE_SIG);
    me->dispatchToAllMines(&recycle); // recycle all Mines
    return QM_EXIT(&playing_s);
}
// ${AOs::Tunnel::SM::active::playing}
QP::QState Tunnel::playing(Tunnel * const me, QP::QEvt const * const e) {
    QP::QState status_;
    switch (e->sig) {
        // ${AOs::Tunnel::SM::active::playing::TIME_TICK}
        case TIME_TICK_SIG: {
            // render this frame on the display
            BSP_drawBitmap(l_frame);
            me->advance();
            me->plantMine();
            me->dispatchToAllMines(e);
            status_ = QM_HANDLED();
            break;
        }
        // ${AOs::Tunnel::SM::active::playing::SHIP_IMG}
        case SHIP_IMG_SIG: {
            uint8_t x   = Q_EVT_CAST(ObjectImageEvt)->x;
            int8_t  y   = Q_EVT_CAST(ObjectImageEvt)->y;
            uint8_t bmp = Q_EVT_CAST(ObjectImageEvt)->bmp;

            // did the Ship/Missile hit the tunnel wall?
            if (me->isWallHit(bmp, x, y)) {
                static QP::QEvt const hit(HIT_WALL_SIG);
                AO_Ship->POST(&hit, me);
            }
            me->addImageAt(bmp, x, y);
            me->dispatchToAllMines(e); // let Mines check for hits
            status_ = QM_HANDLED();
            break;
        }
        // ${AOs::Tunnel::SM::active::playing::MISSILE_IMG}
        case MISSILE_IMG_SIG: {
            uint8_t x   = Q_EVT_CAST(ObjectImageEvt)->x;
            int8_t  y   = Q_EVT_CAST(ObjectImageEvt)->y;
            uint8_t bmp = Q_EVT_CAST(ObjectImageEvt)->bmp;

            // did the Ship/Missile hit the tunnel wall?
            if (me->isWallHit(bmp, x, y)) {
                static QP::QEvt const hit(HIT_WALL_SIG);
                AO_Missile->POST(&hit, me);
            }
            me->addImageAt(bmp, x, y);
            me->dispatchToAllMines(e); // let Mines check for hits
            status_ = QM_HANDLED();
            break;
        }
        // ${AOs::Tunnel::SM::active::playing::MINE_IMG}
        case MINE_IMG_SIG: {
            me->addImageAt(Q_EVT_CAST(ObjectImageEvt)->bmp,
                           Q_EVT_CAST(ObjectImageEvt)->x,
                           Q_EVT_CAST(ObjectImageEvt)->y);
            status_ = QM_HANDLED();
            break;
        }
        // ${AOs::Tunnel::SM::active::playing::EXPLOSION}
        case EXPLOSION_SIG: {
            me->addImageAt(Q_EVT_CAST(ObjectImageEvt)->bmp,
                           Q_EVT_CAST(ObjectImageEvt)->x,
                           Q_EVT_CAST(ObjectImageEvt)->y);
            status_ = QM_HANDLED();
            break;
        }
        // ${AOs::Tunnel::SM::active::playing::SCORE}
        case SCORE_SIG: {
            BSP_updateScore(Q_EVT_CAST(ScoreEvt)->score);
            // increase difficulty of the game:
            // the tunnel gets narrower as the score goes up
            //
            me->m_minimal_gap = (uint8_t)(GAME_SCREEN_HEIGHT - 3U
                              - Q_EVT_CAST(ScoreEvt)->score/2000U);
            status_ = QM_HANDLED();
            break;
        }
        // ${AOs::Tunnel::SM::active::playing::GAME_OVER}
        case GAME_OVER_SIG: {
            static struct {
                QP::QMState const *target;
                QP::QActionHandler act[3];
            } const tatbl_ = { // transition-action table
                &game_over_s,
                {
                    Q_ACTION_CAST(&playing_x), // exit
                    Q_ACTION_CAST(&game_over_e), // entry
                    Q_ACTION_CAST(0)  // zero terminator
                }
            };
            uint16_t score = Q_EVT_CAST(ScoreEvt)->score;
            BSP_updateScore(score);

            // clear the screen
            memset(l_frame, (uint8_t)0,
                   (GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8U));
            BSP_drawBitmap(l_frame);
            // Output the final score to the screen
            BSP_drawNString((GAME_SCREEN_WIDTH - 6U*10U)/2U, 1U, "Score:");
            char str[5];
            str[4] = '\0'; // zero-terminate the string
            str[3] = '0' + (score % 10U); score /= 10U;
            str[2] = '0' + (score % 10U); score /= 10U;
            str[1] = '0' + (score % 10U); score /= 10U;
            str[0] = '0' + (score % 10U);
            BSP_drawNString((GAME_SCREEN_WIDTH - 6U*10U)/2U + 6U*6U, 1U, str);
            status_ = QM_TRAN(&tatbl_);
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}
//${AOs::Tunnel::SM::final} ..................................................
QP::QMState const Tunnel::final_s = {
    static_cast<QP::QMState const *>(0), // superstate (top)
    Q_STATE_CAST(&final),
    Q_ACTION_CAST(&final_e),
    Q_ACTION_CAST(0), // no exit action
    Q_ACTION_CAST(0)  // no intitial tran.
};
// ${AOs::Tunnel::SM::final}
QP::QState Tunnel::final_e(Tunnel * const me) {
    // clear the screen
    memset(l_frame, (uint8_t)0,
          (GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8U));
    BSP_drawBitmap(l_frame);
    QP::QF::stop(); /* stop QF and cleanup */
    (void)me; // avoid compiler warning in case 'me' is not used
    return QM_ENTRY(&final_s);
}
// ${AOs::Tunnel::SM::final}
QP::QState Tunnel::final(Tunnel * const me, QP::QEvt const * const e) {
    QP::QState status_;
    switch (e->sig) {
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    (void)me; /* avoid compiler warning in case 'me' is not used */
    return status_;
}

} // namespace GAME


