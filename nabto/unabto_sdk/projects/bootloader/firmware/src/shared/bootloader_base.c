#include "bootloader_base.h"

#define BOOTLOADER_VERSION                                                              3
// Bootloader version history:
//   1  First release
//   2  Added bootloader data version 2
//   3  Major clean up to ease porting

#define BOOTLOADER_DATA_VERSION                                                         2

#if NABTO_BUILDING_BOOTLOADER
const __ROM system_information systemInformation = {0x00ff, BOOTLOADER_VERSION, BOOTLOADER_DATA_VERSION, RELEASE_MINOR, FLASH_SIZE, FLASH_WRITE_BLOCK, FLASH_ERASE_BLOCK, BOOTLOADER_CODE_SIZE, BOOTLOADER_DATA_SIZE, APPLICATION_CODE_SIZE, APPLICATION_DATA_SIZE, (uint32_t) & applicationInformation};
#endif

#if __18CXX

#if defined(__EXTENDED18__)
#error The bootloader does not support Extended mode!
#endif

#if NABTO_BUILDING_BOOTLOADER

#pragma romdata application_information_section
const __ROM application_information applicationInformation = {0, 0xffffffff, 0xffffffff};

// define bootloader data area
#pragma romdata bootloader_data_section
const __ROM bootloader_data bootloaderData = {BOOTLOADER_DATA_VERSION, RELEASE_MINOR, FLASH_SIZE, FLASH_WRITE_BLOCK, FLASH_ERASE_BLOCK, BOOTLOADER_CODE_SIZE, BOOTLOADER_DATA_SIZE, APPLICATION_CODE_SIZE, APPLICATION_DATA_SIZE,
  { MY_DEFAULT_MAC_BYTE1, MY_DEFAULT_MAC_BYTE2, MY_DEFAULT_MAC_BYTE3, MY_DEFAULT_MAC_BYTE4, MY_DEFAULT_MAC_BYTE5, MY_DEFAULT_MAC_BYTE6},
  { 0}, 0, 0,
  { 0},
  { 0},
  { 0}};

#pragma romdata

#else

#pragma config WDT = OFF
#pragma config STVR = ON
#pragma config DEBUG = OFF
#pragma config CP0 = OFF
#pragma config FOSC = HSPLL
#pragma config FOSC2 = ON
#pragma config FCMEN = OFF
#pragma config IESO = OFF
#pragma config WDTPS = 1
#pragma config ETHLED = ON
#pragma config XINST = OFF

// insert bootloader dummy code to avoid application code being placed here. leave room for the reset vector so the runtime can generate that as usual.
#pragma romdata bootloader_code_section = 0x00006
static const __ROM uint8_t bootloaderCode[BOOTLOADER_CODE_SIZE - 6];

// prevent the application from using the last erase block (because of the configuration fuses placed there).
#if FLASH_SIZE == (128ul * 1024ul)
#pragma romdata bootloader_configuration_fuses_section = 0x1fc00
#else
#error Unhandled flash size!
#endif
static const __ROM uint8_t bootloaderReserved[1024 - 8];

// import bootloader data area defined in bootloader.
#pragma romdata bootloader_data_section = 0x05800
const __ROM bootloader_data bootloaderData;

// allocate space for the application checksum that is patched into the application.
// place it after reset and interrupt vectors.
#pragma romdata application_information_section = 0x06028
const __ROM application_information applicationInformation = {0, RELEASE_MAJOR, RELEASE_MINOR};

#pragma romdata

// c runtime start up.
extern void _startup(void);

// create a new reset vector at the new offset location.
#pragma code application_reset_vector = 0x06000

void ResetVector(void)
{
  _asm
  goto _startup
    _endasm
}
#pragma code

#endif

#elif _WIN32

const application_information applicationInformation = {0, RELEASE_MAJOR, RELEASE_MINOR};

const __ROM bootloader_data bootloaderData = {BOOTLOADER_DATA_VERSION, RELEASE_MINOR, 0, 0, 0, 0, 0, 0, 0,
  { 0, 0, 0, 0, 0, 0},
  { 0}, 0, 0,
  { 0},
  { 0},
  { 0}};

#elif  __GNUC__

#if NABTO_BUILDING_BOOTLOADER == 1


/*
 *  BOOTLOADER_APPLICTION_SECTION and BOOTLOADER_DATA_SECTION
 * is define (for the moment) in the platform header file
 */

const application_information applicationInformation __attribute__((section(BOOTLOADER_APPLICTION_SECTION))) = {0, 0xffffffff, 0xffffffff};

const bootloader_data bootloaderData __attribute__((section(BOOTLOADER_DATA_SECTION))) = {BOOTLOADER_DATA_VERSION, RELEASE_MINOR, FLASH_SIZE, FLASH_WRITE_BLOCK, FLASH_ERASE_BLOCK, BOOTLOADER_CODE_SIZE, BOOTLOADER_DATA_SIZE, APPLICATION_CODE_SIZE, APPLICATION_DATA_SIZE,
 { MY_DEFAULT_MAC_BYTE1, MY_DEFAULT_MAC_BYTE2, MY_DEFAULT_MAC_BYTE3, MY_DEFAULT_MAC_BYTE4, MY_DEFAULT_MAC_BYTE5, MY_DEFAULT_MAC_BYTE6},
 { 0}, 0, 0,
 { 0},
 { 0},
 { 0}};

#else

const application_information applicationInformation __attribute__((section(BOOTLOADER_APPLICTION_SECTION)))  = {0, RELEASE_MAJOR, RELEASE_MINOR};

const bootloader_data bootloaderData __attribute__((section(BOOTLOADER_DATA_SECTION)));

#endif
#else  /* Coldfire */
#if NABTO_BUILDING_BOOTLOADER == 1
#pragma force_active on

#pragma define_section app_info_section ".application_information" far_absolute R
__declspec(app_info_section) const __ROM application_information applicationInformation = {0, 0xffffffff, 0xffffffff};

#pragma define_section bootloader_data_section ".bootloader_data" far_absolute R
__declspec(bootloader_data_section) const __ROM bootloader_data bootloaderData = {BOOTLOADER_DATA_VERSION, RELEASE_MINOR, FLASH_SIZE, FLASH_WRITE_BLOCK, FLASH_ERASE_BLOCK, BOOTLOADER_CODE_SIZE, BOOTLOADER_DATA_SIZE, APPLICATION_CODE_SIZE, APPLICATION_DATA_SIZE,
  { MY_DEFAULT_MAC_BYTE1, MY_DEFAULT_MAC_BYTE2, MY_DEFAULT_MAC_BYTE3, MY_DEFAULT_MAC_BYTE4, MY_DEFAULT_MAC_BYTE5, MY_DEFAULT_MAC_BYTE6},
  { 0}, 0, 0,
  { 0},
  { 0},
  { 0}};

#pragma force_active reset

#else

#pragma force_active on

// import bootloader data area defined in bootloader
#pragma define_section bootloader_data_section ".bootloader_data" far_absolute R
__declspec(bootloader_data_section) const __ROM bootloader_data bootloaderData;

// allocate space for the application checksum that is patched into the application
// place it after reset and interrupt vectors
#pragma define_section app_info_section ".application_information" far_absolute R
__declspec(app_info_section) const __ROM application_information applicationInformation = {0, RELEASE_MAJOR, RELEASE_MINOR};

#pragma force_active reset
#endif

#endif


