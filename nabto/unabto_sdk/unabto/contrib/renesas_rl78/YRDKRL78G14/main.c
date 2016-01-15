/*-------------------------------------------------------------------------*
 * File:  main.c
 *-------------------------------------------------------------------------*
 * Description:
 *     Sets up one of the hardware and drivers and then calls one of the
 *     tests or demos.
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <system/platform.h>
#include <HostApp.h>
#include <init/hwsetup.h>
#include <drv\Glyph\lcd.h>
#include <system\mstimer.h>
#include <system/Switch.h>
#include <sensors\Potentiometer.h>
#include <sensors\Temperature.h>
#include <system\console.h>
#include <drv\UART0.h>
#include <Sensors\LightSensor.h>
#include <drv\SPI.h>
#include <CmdLib\GainSpan_SPI.h>
#include <Apps/NVSettings.h>
#include <Apps/Apps.h>
#include "stdio.h"
#include "string.h"
#include "led.h"
extern void LEDFlash(uint32_t timeout);
extern void led_task(void);
extern void DisplayLCD(uint8_t, const uint8_t *);
extern int16_t *Accelerometer_Get(void);
extern void Accelerometer_Init(void);
extern int16_t	gAccData[3];



/*-------------------------------------------------------------------------*
 * Macros:
 *-------------------------------------------------------------------------*/
/* Set option bytes */
#pragma location = "OPTBYTE"
__root const uint8_t opbyte0 = 0xEFU;
#pragma location = "OPTBYTE"
__root const uint8_t opbyte1 = 0xFFU;
#pragma location = "OPTBYTE"
__root const uint8_t opbyte2 = 0xF8U;
#pragma location = "OPTBYTE"
__root const uint8_t opbyte3 = 0x04U;

/* Set security ID */
#pragma location = "SECUID"
__root const uint8_t secuid[10] = 
    {0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U};

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
/* Application Modes */
typedef enum {
    PROGRAM_MODE,
    UART_PT_MODE,
    RUN_EXOSITE,
    RUN_PROVISIONING,
    RUN_OVER_AIR_DOWNLOAD,
    GAINSPAN_DEMO,
    NABTO_DEMO
}AppMode_T;

/*---------------------------------------------------------------------------*
 * Routine:  main
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the hardware, setup the peripherals, show the startup banner,
 *      wait for the module to power up, run a selected test or app.
 * Inputs:
 *      void
 * Outputs:
 *      int -- always 0.
 *---------------------------------------------------------------------------*/
uint16_t gAmbientLight;
float gTemp_F;
typedef union {
	int16_t		temp;
	uint8_t		T_tempValue[2];
} temp_u;
extern void SPI2_Init();
extern void SPI_Init(uint32_t bitsPerSecond);
extern void App_WebProvisioning(void);
extern void App_OverTheAirProgrammingPushMetheod(void);
int  main(void)
{
    AppMode_T AppMode; APP_STATE_E state=UPDATE_TEMPERATURE;
    char LCDString[30], temp_char[2]; uint16_t temp; float ftemp;
    
    //UART0 will be used for debug messaging (pins 54 and 55 on RDK board)
    UART0_Start(GAINSPAN_CONSOLE_BAUD);
    
    HardwareSetup();
        
    /* Default app mode to nabto */
    AppMode = NABTO_DEMO;
    
    /* Determine if SW1 & SW3 is pressed at power up to enter programming mode */
    if (Switch1IsPressed() && Switch3IsPressed()) {
        AppMode = PROGRAM_MODE;
    }
    else if(Switch1IsPressed())
    {
        AppMode = GAINSPAN_DEMO;
    }
    else if(Switch2IsPressed())
    {
        AppMode = RUN_PROVISIONING;
    }
    else if(Switch3IsPressed())
    {
        AppMode = RUN_OVER_AIR_DOWNLOAD;
    }
    
    /************************initialize LCD module********************************/
    SPI2_Init();
    InitialiseLCD();
    led_init();
    led_all_off();
    MSTimerInit();
#if 1
    if(AppMode == NABTO_DEMO) {
        LCDDisplayLogo();
        LCDSelectFont(FONT_SMALL);
        DisplayLCD(LCD_LINE3, " Demo by Nabto     ");
        DisplayLCD(LCD_LINE4, "                   ");
        DisplayLCD(LCD_LINE5, "Other modes:       ");
        DisplayLCD(LCD_LINE6, " SW1 Gainspan Demo ");
        DisplayLCD(LCD_LINE7, " SW2 Change WiFi AP");
        DisplayLCD(LCD_LINE8, " SW3 Air Download  ");
        MSTimerDelay(3000);
        ClearLCD();
        LCDSelectFont(FONT_SMALL);
    }
#endif
    
    DisplayLCD(LCD_LINE1, "Starting..."); 
    /*****************************************************************************/  
    SPI_Init(GAINSPAN_SPI_RATE);  
    /* Setup LCD SPI channel for Chip Select P10, active low, active per byte  */
    SPI_ChannelSetup(GAINSPAN_SPI_CHANNEL, false, true);
    GainSpan_SPI_Start();

    PM15 &= ~(1 << 2);
    P15 &= ~(1 << 2);
    
    if(AppMode == PROGRAM_MODE) {
      App_ProgramMode();
    }
		
		/**
		 * Nabto Demo
		 */
		else if (AppMode == NABTO_DEMO)
		{
			LCDSelectFont(FONT_LARGE);
			DisplayLCD(LCD_LINE1, "-NABTO DEMO-");
			LCDSelectFont(FONT_SMALL);
			
			// Initialize modules
			Temperature_Init();
			Potentiometer_Init();
			App_InitModule();
			Accelerometer_Init();
			
			// Read wifi parameters from flash memory, if available
			if(NVSettingsLoad(&G_nvsettings)) {
				NVSettingsSave(&G_nvsettings);
			}
			AtLibGs_Init();
			
			while (!AtLibGs_IsNodeAssociated()) {
        
        DisplayLCD(LCD_LINE2, (uint8_t const*)G_nvsettings.webprov.ssid);
        
				ATLIBGS_MSG_ID_E r = App_Connect(&G_nvsettings.webprov);
				if (r != ATLIBGS_MSG_ID_OK) {
					LCDSelectFont(FONT_SMALL);
					DisplayLCD(LCD_LINE1, "WiFi Error!");
					DisplayLCD(LCD_LINE2, " Failed to connect ");
					MSTimerDelay(2000);
					LCDSelectFont(FONT_LARGE);
					DisplayLCD(LCD_LINE1, "Provisioning");
					App_WebProvisioning();
				}
			}
			
      DisplayLCD(LCD_LINE2, "");
      
			// The Nabto main
			App_NabtoMain();
    }
    else if(AppMode == RUN_PROVISIONING)
    {
      ConsolePrintf("Begin App_WebProvisioning();\r\n");
      DisplayLCD(LCD_LINE1, "*SSID CONFIG");
      DisplayLCD(LCD_LINE3, "Initializing");
      App_WebProvisioning();
    }
      else if(AppMode == RUN_OVER_AIR_DOWNLOAD)
    {
      App_OverTheAirProgrammingPushMetheod();
    }
    else{
        ConsolePrintf("\r\nBegin default demo application\r\n");
        // UART2_Start(GAINSPAN_UART_BAUD);

        Temperature_Init();
        Potentiometer_Init();

        // sprintf(LCDString, "RDK Demo %s", VERSION_TEXT);
        // DisplayLCD(LCD_LINE1, (const uint8_t *)LCDString);

        /* Before doing any tests or apps, startup the module */
        /* and nonvolatile stettings */
        App_Startup();
        // Now connect to the system
        //App_Connect(&G_nvsettings.webprov);

        //App_PassThroughSPI();
        
        /******************Start Processing Sensor data******************/
        
        uint32_t start = MSTimerGet();  uint8_t c;
        Accelerometer_Init();
        while(1) 
        { 
          // if (GainSpan_SPI_ReceiveByte(GAINSPAN_SPI_CHANNEL, &c)) 
           if(App_Read(&c, 1, 0)) 
             AtLibGs_ReceiveDataProcess(c);
                   
          /* Timeout? */
          if (MSTimerDelta(start) >= 100)     // every 100 ms, read sensor data
          {  
            led_task();
            switch(state)
            {              
                case UPDATE_TEMPERATURE:         
                // Temperature sensor reading
                  temp = Temperature_Get();

                  temp_char[1] = (temp & 0xFF00)>>8;
                  temp_char[0] = temp & 0xFF;
                  
                  ftemp = *(uint16_t *)temp_char;
                  
                  gTemp_F = ((ftemp/5)*9)/128 + 22;
              
                  // Display the contents of lcd_buffer onto the debug LCD 
                  //sprintf((char *)LCDString, "TEMP: %d.%d C", temp_char[0], temp_char[1]);
                  sprintf((char *)LCDString, "TEMP: %.1fF", gTemp_F);
                  DisplayLCD(LCD_LINE6, (const uint8_t *)LCDString);  
                  state = UPDATE_LIGHT;
                break;
                
                case UPDATE_LIGHT:
                 // Light sensor reading
                  gAmbientLight = LightSensor_Get();
                    // Display the contents of lcd_buffer onto the debug LCD 
                  sprintf((char *)LCDString, "Light: %d ", gAmbientLight);
                  DisplayLCD(LCD_LINE7, (const uint8_t *)LCDString);
                  state = UPDATE_ACCELEROMETER;
                break;
                
                case UPDATE_ACCELEROMETER: 
                 // 3-axis accelerometer reading
                  Accelerometer_Get();
                  sprintf((char *)LCDString, "x%2d y%2d z%2d", gAccData[0], gAccData[1], gAccData[2]);
                  DisplayLCD(LCD_LINE8, (const uint8_t *)LCDString); 
                  state = UPDATE_TEMPERATURE;
                break;
              }
              start = MSTimerGet();
           }
         }          
    }    
    return 0;
}
/*-------------------------------------------------------------------------*
 * End of File:  main.c
 *-------------------------------------------------------------------------*/

