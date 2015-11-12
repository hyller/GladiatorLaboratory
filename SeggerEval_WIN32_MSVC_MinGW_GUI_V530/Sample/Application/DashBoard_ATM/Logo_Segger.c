/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2015  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.30 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : Logo_Segger.c
Purpose     : Bitmap file
---------------------------END-OF-HEADER------------------------------
*/

#include <stdlib.h>

#include "GUI.h"

#ifndef GUI_CONST_STORAGE
  #define GUI_CONST_STORAGE const
#endif

/*   Palette
The following are the entries of the palette table.
Every entry is a 32-bit value (of which 24 bits are actually used)
the lower   8 bits represent the Red component,
the middle  8 bits represent the Green component,
the highest 8 bits (of the 24 bits used) represent the Blue component
as follows:   0xBBGGRR
*/

static GUI_CONST_STORAGE GUI_COLOR ColorsSeggerLogo[] = {
#if (GUI_USE_ARGB)
     0xFFFF0000,0xFFA0A0FF,0xFF3030D0,0xFF2020A0
    ,0xFF000088,0xFF2020B0,0xFF6060B0,0xFFE6E6FF
    ,0xFFF6F6F6,0xFFFFFFFF,0xFFEEEEFF,0xFF5250FF
    ,0xFF9495FF,0xFF8B89FF,0xFF7371FF,0xFF0000FF
    ,0xFFACAEFF,0xFF4140FF,0xFFF6F6FF,0xFF3130FF
    ,0xFFD5D6FF,0xFF3128FF,0xFF8381EE,0xFF5A59FF
    ,0xFFCDCEFF,0xFF1010FF,0xFF2020FF,0xFFBDBEFF
    ,0xFF6A69FF,0xFFA4A5FF,0xFF4A48FF,0xFF2028FF
    ,0xFFE6E6F6,0xFF1818FF,0xFFACAEF6,0xFF6261FF
    ,0xFF9489FF,0xFF3938FF,0xFFDEDEFF,0xFF0808FF
    ,0xFF2928FF,0xFFA4A5F6,0xFF9C9DFF,0xFF8B81FF
    ,0xFFC5C6FF,0xFFB4B6FF,0xFFE6E6E6,0xFFD5D6D5
    ,0xFFEEEEEE,0xFFD5CECD,0xFFDEDEDE,0xFF7B797B
    ,0xFF393839,0xFF313031,0xFF414041,0xFF9C9D9C
    ,0xFFEEE6E6,0xFF6A696A,0xFFB4B6B4,0xFF837983
    ,0xFF737173,0xFF292829,0xFF393831,0xFF626162
    ,0xFF949594,0xFFC5C6C5,0xFFF6F6EE,0xFF4A484A
    ,0xFF080808,0xFF202020,0xFF000000,0xFFCDCECD
    ,0xFF101010,0xFF5A595A,0xFFDEE6DE,0xFFBDB6B4
    ,0xFFA4A5A4,0xFF4A4841,0xFFBDBEB4,0xFF525052
    ,0xFF94958B,0xFFBDBEBD,0xFFD5D6CD,0xFFACA5A4
    ,0xFFACAEAC,0xFF181818,0xFFCDCEC5,0xFF838183
    ,0xFF52504A,0xFFB4B6AC,0xFF8B898B,0xFF7B7973
    ,0xFF62595A,0xFFACB6AC,0xFFC5BEBD,0xFF525952
    ,0xFF83797B,0xFFACB6FF
#else
     0x0000FF,0xFFA0A0,0xD03030,0xA02020
    ,0x880000,0xB02020,0xB06060,0xFFE6E6
    ,0xF6F6F6,0xFFFFFF,0xFFEEEE,0xFF5052
    ,0xFF9594,0xFF898B,0xFF7173,0xFF0000
    ,0xFFAEAC,0xFF4041,0xFFF6F6,0xFF3031
    ,0xFFD6D5,0xFF2831,0xEE8183,0xFF595A
    ,0xFFCECD,0xFF1010,0xFF2020,0xFFBEBD
    ,0xFF696A,0xFFA5A4,0xFF484A,0xFF2820
    ,0xF6E6E6,0xFF1818,0xF6AEAC,0xFF6162
    ,0xFF8994,0xFF3839,0xFFDEDE,0xFF0808
    ,0xFF2829,0xF6A5A4,0xFF9D9C,0xFF818B
    ,0xFFC6C5,0xFFB6B4,0xE6E6E6,0xD5D6D5
    ,0xEEEEEE,0xCDCED5,0xDEDEDE,0x7B797B
    ,0x393839,0x313031,0x414041,0x9C9D9C
    ,0xE6E6EE,0x6A696A,0xB4B6B4,0x837983
    ,0x737173,0x292829,0x313839,0x626162
    ,0x949594,0xC5C6C5,0xEEF6F6,0x4A484A
    ,0x080808,0x202020,0x000000,0xCDCECD
    ,0x101010,0x5A595A,0xDEE6DE,0xB4B6BD
    ,0xA4A5A4,0x41484A,0xB4BEBD,0x525052
    ,0x8B9594,0xBDBEBD,0xCDD6D5,0xA4A5AC
    ,0xACAEAC,0x181818,0xC5CECD,0x838183
    ,0x4A5052,0xACB6B4,0x8B898B,0x73797B
    ,0x5A5962,0xACB6AC,0xBDBEC5,0x525952
    ,0x7B7983,0xFFB6AC
#endif
};

static GUI_CONST_STORAGE GUI_LOGPALETTE PalSeggerLogo = {
  98,	/* number of entries */
  1, 	/* Has transparency */
  &ColorsSeggerLogo[0]
};

static GUI_CONST_STORAGE unsigned char acSeggerLogo[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 
        0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 
        0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x03, 0x02, 0x01, 0x00,
  0x01, 0x05, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 
        0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 
        0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x01,
  0x02, 0x04, 0x06, 0x07, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 
        0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 
        0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x06, 0x04, 0x02,
  0x03, 0x04, 0x07, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x0A, 0x04, 0x03,
  0x04, 0x04, 0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x09, 0x07, 0x0B, 0x0C, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x0D, 0x0E, 0x07, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x09, 0x0B, 0x0F, 0x0F, 0x10, 0x09, 0x09, 0x09, 0x09, 0x09, 0x10, 0x0F, 0x0F, 0x11, 0x12, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x09, 0x0C, 0x0F, 0x0F, 0x13, 0x14, 0x09, 0x09, 0x09, 0x09, 0x14, 0x15, 0x0F, 0x0F, 0x16, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x09, 0x09, 0x17, 0x0F, 0x0F, 0x11, 0x12, 0x09, 0x09, 0x09, 0x09, 0x18, 0x19, 0x0F, 0x1A, 0x1B, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x09, 0x09, 0x14, 0x15, 0x0F, 0x0F, 0x16, 0x09, 0x09, 0x09, 0x09, 0x09, 0x1C, 0x0F, 0x0F, 0x15, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x09, 0x09, 0x09, 0x18, 0x19, 0x0F, 0x19, 0x1D, 0x09, 0x09, 0x09, 0x09, 0x07, 0x0B, 0x0F, 0x0F, 0x1C, 0x12, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 0x16, 0x0F, 0x0F, 0x1A, 0x07, 0x09, 0x09, 0x09, 0x09, 0x14, 0x0F, 0x0F, 0x19, 0x1D, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 0x12, 0x1C, 0x0F, 0x0F, 0x1E, 0x0A, 0x09, 0x09, 0x09, 0x09, 0x1D, 0x19, 0x0F, 0x1F, 0x07, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x07, 0x1F, 0x0F, 0x19, 0x0D, 0x09, 0x09, 0x09, 0x09, 0x0A, 0x1C, 0x0F, 0x0F, 0x1C, 0x12, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x20, 0x07, 0x09, 0x09, 0x09, 0x09, 0x1B, 0x21, 0x0F, 0x0F, 0x10, 0x09, 0x09, 0x09, 0x09, 0x07, 0x1A, 0x0F, 0x0F, 0x16, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x22, 0x23, 0x09, 0x09, 0x09, 0x09, 0x12, 0x24, 0x0F, 0x0F, 0x13, 0x14, 0x09, 0x09, 0x09, 0x09, 0x1D, 0x19, 0x0F, 0x19, 0x18, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x1D, 0x0F, 0x24, 0x12, 0x09, 0x09, 0x09, 0x09, 0x17, 0x0F, 0x0F, 0x11, 0x12, 0x09, 0x09, 0x09, 0x09, 0x16, 0x0F, 0x0F, 0x25, 0x26, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x1D, 0x0F, 0x21, 0x1B, 0x09, 0x09, 0x09, 0x09, 0x14, 0x15, 0x0F, 0x0F, 0x16, 0x09, 0x09, 0x09, 0x09, 0x12, 0x11, 0x0F, 0x27, 0x16, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x1D, 0x0F, 0x0F, 0x1F, 0x07, 0x09, 0x09, 0x09, 0x09, 0x18, 0x19, 0x0F, 0x19, 0x1D, 0x09, 0x09, 0x09, 0x09, 0x18, 0x28, 0x0F, 0x0F, 0x24, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 
        0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 
        0x29, 0x29, 0x29, 0x1D, 0x14, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x1D, 0x0F, 0x0F, 0x0F, 0x1C, 0x12, 0x09, 0x09, 0x09, 0x09, 0x16, 0x0F, 0x0F, 0x1A, 0x07, 0x09, 0x09, 0x09, 0x12, 0x24, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 
        0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 
        0x0F, 0x0F, 0x0F, 0x0F, 0x15, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x1D, 0x0F, 0x0F, 0x0F, 0x0F, 0x16, 0x09, 0x09, 0x09, 0x09, 0x12, 0x1C, 0x0F, 0x0F, 0x11, 0x07, 0x09, 0x09, 0x09, 0x09, 0x17, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 
        0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 
        0x0F, 0x0F, 0x0F, 0x0F, 0x1A, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x1D, 0x0F, 0x0F, 0x0F, 0x0F, 0x19, 0x18, 0x09, 0x09, 0x09, 0x09, 0x07, 0x1F, 0x0F, 0x27, 0x2A, 0x09, 0x09, 0x09, 0x09, 0x26, 0x0C, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 
        0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 
        0x2B, 0x2B, 0x2B, 0x24, 0x2C, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x1D, 0x0F, 0x0F, 0x0F, 0x0F, 0x19, 0x18, 0x09, 0x09, 0x09, 0x09, 0x07, 0x1F, 0x0F, 0x27, 0x2A, 0x09, 0x09, 0x09, 0x09, 0x07, 0x2A, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 
        0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 
        0x2B, 0x2B, 0x2B, 0x2B, 0x1B, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x1D, 0x0F, 0x0F, 0x0F, 0x0F, 0x16, 0x09, 0x09, 0x09, 0x09, 0x12, 0x1C, 0x0F, 0x0F, 0x11, 0x0A, 0x09, 0x09, 0x09, 0x09, 0x23, 0x27, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 
        0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 
        0x0F, 0x0F, 0x0F, 0x0F, 0x1A, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x1D, 0x0F, 0x0F, 0x0F, 0x1C, 0x12, 0x09, 0x09, 0x09, 0x09, 0x16, 0x0F, 0x0F, 0x1A, 0x07, 0x09, 0x09, 0x09, 0x12, 0x24, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 
        0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 
        0x0F, 0x0F, 0x0F, 0x0F, 0x15, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x1D, 0x0F, 0x0F, 0x1F, 0x07, 0x09, 0x09, 0x09, 0x09, 0x18, 0x19, 0x0F, 0x19, 0x1D, 0x09, 0x09, 0x09, 0x09, 0x18, 0x28, 0x0F, 0x0F, 0x24, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 
        0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 
        0x29, 0x29, 0x29, 0x1D, 0x14, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x1D, 0x0F, 0x21, 0x1B, 0x09, 0x09, 0x09, 0x09, 0x14, 0x15, 0x0F, 0x0F, 0x16, 0x09, 0x09, 0x09, 0x09, 0x12, 0x11, 0x0F, 0x27, 0x16, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x1D, 0x0F, 0x24, 0x12, 0x09, 0x09, 0x09, 0x09, 0x17, 0x0F, 0x0F, 0x11, 0x12, 0x09, 0x09, 0x09, 0x09, 0x16, 0x0F, 0x0F, 0x25, 0x26, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x2D, 0x23, 0x09, 0x09, 0x09, 0x09, 0x12, 0x24, 0x0F, 0x0F, 0x13, 0x14, 0x09, 0x09, 0x09, 0x09, 0x1D, 0x19, 0x0F, 0x19, 0x18, 0x09, 0x09, 0x09, 0x08, 0x2E, 0x2F, 0x2F, 0x2E, 0x09, 0x09, 0x09, 0x09, 0x08, 0x30, 0x30, 0x30, 0x30, 
        0x30, 0x30, 0x09, 0x09, 0x09, 0x09, 0x08, 0x2E, 0x2F, 0x2F, 0x2E, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x2E, 0x31, 0x32, 0x32, 0x09, 0x09, 0x09, 0x09, 0x09, 0x08, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x09, 0x09, 0x08, 0x30, 0x30, 0x30, 0x30, 
        0x30, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x20, 0x07, 0x09, 0x09, 0x09, 0x09, 0x1B, 0x21, 0x0F, 0x0F, 0x10, 0x09, 0x09, 0x09, 0x09, 0x07, 0x1A, 0x0F, 0x0F, 0x16, 0x09, 0x09, 0x09, 0x2E, 0x33, 0x34, 0x35, 0x35, 0x36, 0x37, 0x09, 0x09, 0x38, 0x39, 0x36, 0x36, 0x36, 0x36, 
        0x36, 0x36, 0x3A, 0x09, 0x09, 0x2E, 0x33, 0x34, 0x35, 0x35, 0x36, 0x3B, 0x09, 0x09, 0x09, 0x09, 0x38, 0x3C, 0x36, 0x3D, 0x3E, 0x3E, 0x3C, 0x38, 0x09, 0x09, 0x32, 0x3F, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x2F, 0x09, 0x40, 0x36, 0x36, 0x36, 0x36, 
        0x36, 0x39, 0x41, 0x09, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x07, 0x1F, 0x0F, 0x27, 0x16, 0x09, 0x09, 0x09, 0x09, 0x0A, 0x1C, 0x0F, 0x0F, 0x1C, 0x12, 0x09, 0x09, 0x42, 0x43, 0x44, 0x36, 0x40, 0x40, 0x45, 0x46, 0x3B, 0x09, 0x47, 0x45, 0x44, 0x34, 0x36, 0x36, 
        0x36, 0x36, 0x3A, 0x09, 0x41, 0x45, 0x44, 0x34, 0x40, 0x39, 0x45, 0x46, 0x3C, 0x42, 0x09, 0x38, 0x3D, 0x46, 0x35, 0x33, 0x3B, 0x36, 0x46, 0x3D, 0x08, 0x09, 0x3A, 0x48, 0x48, 0x36, 0x36, 0x36, 0x36, 0x43, 0x2F, 0x09, 0x36, 0x46, 0x34, 0x36, 0x36, 
        0x34, 0x46, 0x46, 0x41, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 0x12, 0x1C, 0x0F, 0x0F, 0x11, 0x07, 0x09, 0x09, 0x09, 0x09, 0x1D, 0x19, 0x0F, 0x1F, 0x07, 0x09, 0x09, 0x09, 0x31, 0x3D, 0x45, 0x41, 0x09, 0x09, 0x41, 0x49, 0x37, 0x09, 0x47, 0x45, 0x35, 0x47, 0x30, 0x30, 
        0x30, 0x30, 0x09, 0x09, 0x49, 0x46, 0x39, 0x4A, 0x09, 0x09, 0x4B, 0x45, 0x39, 0x2E, 0x09, 0x4C, 0x46, 0x3D, 0x32, 0x09, 0x09, 0x2E, 0x4D, 0x3D, 0x30, 0x09, 0x3A, 0x48, 0x36, 0x32, 0x30, 0x30, 0x30, 0x08, 0x09, 0x09, 0x36, 0x46, 0x4E, 0x30, 0x30, 
        0x2E, 0x4D, 0x46, 0x40, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 0x16, 0x0F, 0x0F, 0x1A, 0x07, 0x09, 0x09, 0x09, 0x09, 0x14, 0x0F, 0x0F, 0x19, 0x1D, 0x09, 0x09, 0x09, 0x09, 0x2E, 0x4F, 0x46, 0x3D, 0x3C, 0x3A, 0x31, 0x42, 0x09, 0x09, 0x47, 0x45, 0x45, 0x50, 0x4C, 0x4C, 
        0x4C, 0x51, 0x09, 0x30, 0x46, 0x46, 0x52, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x2E, 0x36, 0x46, 0x40, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x3A, 0x48, 0x45, 0x50, 0x4C, 0x4C, 0x4C, 0x47, 0x09, 0x09, 0x36, 0x46, 0x53, 0x32, 0x32, 
        0x31, 0x3D, 0x46, 0x54, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x09, 0x09, 0x09, 0x18, 0x19, 0x0F, 0x19, 0x1D, 0x09, 0x09, 0x09, 0x09, 0x07, 0x0B, 0x0F, 0x0F, 0x1C, 0x12, 0x09, 0x09, 0x09, 0x09, 0x09, 0x41, 0x4D, 0x46, 0x46, 0x55, 0x3D, 0x43, 0x2F, 0x09, 0x47, 0x45, 0x46, 0x46, 0x46, 0x46, 
        0x46, 0x45, 0x08, 0x56, 0x46, 0x46, 0x08, 0x09, 0x08, 0x57, 0x4F, 0x4F, 0x39, 0x2F, 0x32, 0x3E, 0x55, 0x51, 0x09, 0x09, 0x37, 0x4F, 0x4F, 0x4F, 0x3A, 0x09, 0x3A, 0x48, 0x46, 0x46, 0x46, 0x46, 0x46, 0x58, 0x09, 0x09, 0x36, 0x46, 0x3D, 0x3E, 0x3E, 
        0x3D, 0x46, 0x49, 0x08, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x09, 0x09, 0x14, 0x15, 0x0F, 0x0F, 0x16, 0x09, 0x09, 0x09, 0x09, 0x09, 0x1C, 0x0F, 0x0F, 0x15, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x08, 0x30, 0x2E, 0x59, 0x39, 0x45, 0x46, 0x35, 0x2F, 0x47, 0x45, 0x3D, 0x51, 0x56, 0x56, 
        0x32, 0x32, 0x09, 0x2E, 0x46, 0x46, 0x2F, 0x09, 0x38, 0x39, 0x36, 0x3D, 0x46, 0x5A, 0x2E, 0x34, 0x46, 0x37, 0x09, 0x09, 0x40, 0x36, 0x35, 0x46, 0x4F, 0x09, 0x3A, 0x48, 0x34, 0x4E, 0x32, 0x56, 0x32, 0x30, 0x09, 0x09, 0x36, 0x46, 0x36, 0x4F, 0x4F, 
        0x36, 0x46, 0x4D, 0x30, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x09, 0x09, 0x17, 0x0F, 0x0F, 0x11, 0x12, 0x09, 0x09, 0x09, 0x09, 0x18, 0x19, 0x0F, 0x1A, 0x1B, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x38, 0x39, 0x39, 0x38, 0x09, 0x09, 0x4B, 0x45, 0x45, 0x41, 0x47, 0x45, 0x3E, 0x32, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x3D, 0x46, 0x4C, 0x09, 0x09, 0x08, 0x2E, 0x49, 0x46, 0x5A, 0x08, 0x33, 0x46, 0x49, 0x09, 0x09, 0x08, 0x30, 0x5B, 0x46, 0x4F, 0x09, 0x3A, 0x48, 0x36, 0x30, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x36, 0x46, 0x51, 0x09, 0x09, 
        0x2E, 0x46, 0x46, 0x56, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x09, 0x0C, 0x0F, 0x0F, 0x13, 0x14, 0x09, 0x09, 0x09, 0x09, 0x26, 0x25, 0x0F, 0x0F, 0x16, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x2E, 0x4F, 0x46, 0x4D, 0x3A, 0x3A, 0x3C, 0x55, 0x35, 0x32, 0x47, 0x45, 0x55, 0x5C, 0x39, 0x39, 
        0x39, 0x39, 0x5D, 0x08, 0x54, 0x55, 0x55, 0x5C, 0x4C, 0x4C, 0x4D, 0x46, 0x46, 0x5A, 0x09, 0x2E, 0x45, 0x46, 0x49, 0x37, 0x37, 0x3C, 0x46, 0x46, 0x4F, 0x09, 0x3A, 0x48, 0x45, 0x39, 0x39, 0x39, 0x39, 0x39, 0x41, 0x09, 0x35, 0x46, 0x5E, 0x09, 0x09, 
        0x09, 0x55, 0x46, 0x59, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x09, 0x0B, 0x0F, 0x0F, 0x10, 0x09, 0x09, 0x09, 0x09, 0x09, 0x2C, 0x21, 0x0F, 0x11, 0x12, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x2F, 0x4D, 0x46, 0x44, 0x44, 0x46, 0x3D, 0x54, 0x09, 0x4A, 0x34, 0x46, 0x46, 0x46, 0x46, 
        0x46, 0x46, 0x5F, 0x30, 0x08, 0x54, 0x3D, 0x46, 0x46, 0x46, 0x45, 0x60, 0x46, 0x5A, 0x09, 0x09, 0x4B, 0x58, 0x46, 0x46, 0x46, 0x46, 0x3C, 0x35, 0x4F, 0x09, 0x41, 0x45, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x57, 0x09, 0x35, 0x46, 0x2F, 0x09, 0x09, 
        0x09, 0x5C, 0x46, 0x59, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x09, 0x07, 0x0B, 0x0C, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x61, 0x23, 0x14, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x52, 0x5E, 0x5E, 0x56, 0x08, 0x09, 0x09, 0x09, 0x09, 0x32, 0x32, 0x32, 0x32, 
        0x32, 0x32, 0x09, 0x09, 0x09, 0x09, 0x08, 0x56, 0x5E, 0x5E, 0x09, 0x09, 0x2F, 0x38, 0x09, 0x09, 0x09, 0x09, 0x2F, 0x5E, 0x5E, 0x2E, 0x09, 0x52, 0x2E, 0x09, 0x09, 0x09, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x09, 0x09, 0x30, 0x32, 0x09, 0x09, 0x09, 
        0x09, 0x30, 0x32, 0x09, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x04, 0x04, 0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x04, 0x04,
  0x03, 0x04, 0x07, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
        0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x0A, 0x04, 0x03,
  0x02, 0x04, 0x06, 0x07, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 
        0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 
        0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x06, 0x04, 0x02,
  0x01, 0x05, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 
        0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 
        0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x01,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 
        0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 
        0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x03, 0x02, 0x01, 0x00
};

extern GUI_CONST_STORAGE GUI_BITMAP bmLogo_Segger;

GUI_CONST_STORAGE GUI_BITMAP bmLogo_Segger = {
  93, /* XSize */
  44, /* YSize */
  93, /* BytesPerLine */
  8, /* BitsPerPixel */
  acSeggerLogo,  /* Pointer to picture data (indices) */
  &PalSeggerLogo  /* Pointer to palette */
};

/* *** End of file *** */
