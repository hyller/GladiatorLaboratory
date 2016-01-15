/*-------------------------------------------------------------------------*
 * File:  App_Nabto.c
 *-------------------------------------------------------------------------*
 * Description:
 *     uNabto protocol implementation on Renesas
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <HostApp.h>
#include <unabto/unabto.h>
#include <system/platform.h>
#include <system/console.h>
#include <sensors/Accelerometer.h>
#include <sensors/LightSensor.h>
#include <sensors/Temperature.h>
#include <sensors/Potentiometer.h>
#include <CmdLib/AtCmdLib.h>
#include <CmdLib/GainSpan_SPI.h>
#include <system/mstimer.h>
#include <system/Switch.h>
#include <drv/Glyph/lcd.h>
#include "Apps.h"

#include "unabto/unabto_env_base.h"
#include "unabto/unabto_common_main.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/


static char id[] = "xxxxxx.rl78.u.nabto.net";

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

void App_NabtoMain(void)
{
        static nabto_main_setup* nms;
	char mac[20] = "00:00:00:00:00:00";
	uint8_t id_substring[16] = {0};

	// Get device mac address
	if (AtLibGs_GetMAC(mac)) {
		id[0] = mac[9];
		id[1] = mac[10];
		id[2] = mac[12];
		id[3] = mac[13];
		id[4] = mac[15];
		id[5] = mac[16];
	}

   nms = unabto_init_context();
   nms->id = (const char*)id;
   if (!unabto_init()) return;

   memcpy(id_substring, &id, 7);

	DisplayLCD(LCD_LINE4, "Nabto running      ");
        DisplayLCD(LCD_LINE6, " ID:");
        DisplayLCD(LCD_LINE7, id_substring);
        DisplayLCD(LCD_LINE8, "rl78.u.nabto.net");
        
        
	while (1) {
	  //MSTimerDelay(10);
          unabto_tick();
	}

   unabto_close();
}
