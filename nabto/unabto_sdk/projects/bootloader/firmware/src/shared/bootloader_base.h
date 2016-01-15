/*
 *  To use application data define the data area in a similar fashion as shown below.
 *
 * PIC18:
 *   #pragma romdata configuration_defaults_section = 0x1f000
 *   __ROM application_configuration defaultApplicationConfiguration = { ... };
 *
 *   #pragma romdata application_data_section = 0x1f400
 *   const rom uint8_t applicationData[APPLICATION_DATA_SIZE];

 * Coldfire V2:
 *   #pragma define_section application_data_section ".application_data" far_absolute R
 *   __declspec(application_data_section) const uint8_t applicationData[APPLICATION_DATA_SIZE]; 
 */

#ifndef _BOOTLOADER_BASE_H_
#define _BOOTLOADER_BASE_H_

#if NABTO_BUILDING_BOOTLOADER // when building the bootloader also include platform specific definitions
#include <platform/platform.h>
#endif

#include <unabto_platform_types.h>
#include <unabto_version.h>

#if __18CXX // PIC18 MPLAB C18 specific definitions

// detect flash size based on which microcontroller we are building for 
#if defined(__18F66J60) || defined(__18F86J60) || defined(__18F96J60)
#define FLASH_SIZE                                                                      (64ul * 1024ul)
#elif defined(__18F66J65) || defined(__18F86J65) || defined(__18F96J65)
#define FLASH_SIZE                                                                      (96ul * 1024ul)
#elif defined(__18F67J60) || defined(__18F87J60) || defined(__18F97J60)
#define FLASH_SIZE                                                                      (128ul * 1024ul)
#else
#error "Unsupported processor specified!"
#endif

#define BOOTLOADER_SIZE                                                                 (24ul * 1024ul)
#define BOOTLOADER_DATA_SIZE                                                            (2ul * 1024ul)
#define APPLICATION_DATA_SIZE                                                           (2ul * 1024ul)
#define FLASH_RESERVE_TOP_SIZE                                                          (1024ul) // we can't use the last block of flash (the last 1 kB) since the configuration words (8 bytes) reside here

#elif _WIN32

// detect flash size based on which microcontroller we are building for 
#define FLASH_SIZE                                                                      (1024ul * 1024ul)

#define BOOTLOADER_SIZE                                                                 (24ul * 1024ul)
#define BOOTLOADER_DATA_SIZE                                                            (2ul * 1024ul)
#define APPLICATION_DATA_SIZE                                                           (2ul * 1024ul)
#define FLASH_RESERVE_TOP_SIZE                                                          (1024ul) // we can't use the last block of flash (the last 1 kB) since the configuration words (8 bytes) reside here

#elif __GNUC__

#define BOOTLOADER_SIZE                                                                 (44ul * 1024ul)
#define BOOTLOADER_DATA_SIZE                                                            (4ul * 1024ul)
#define APPLICATION_INFO_SIZE															(4ul * 1024ul)
#define APPLICATION_DATA_SIZE                                                           (0)


#define BOOTLOADER_APPLICTION_SECTION ".application_information"
#define BOOTLOADER_DATA_SECTION ".bootloader_data"

#else // Coldfire V2 specific definitions (the compiler does not set any flags so this is the default)

// detect flash size based on which microcontroller we are building for 
#if defined(__MCF52255_H__) || defined(__MCF52259_H__)
#define FLASH_SIZE                                                                      (512ul * 1024ul)
#else
#error "Unsupported processor specified!"
#endif

#define BOOTLOADER_SIZE                                                                 (40ul * 1024ul)
#define BOOTLOADER_DATA_SIZE                                                            (4ul * 1024ul)
#define APPLICATION_DATA_SIZE                                                           (32ul * 1024ul)
#endif

// from here on everything is computed by the preprocessor

// reserve anything at top of flash?
#ifndef FLASH_RESERVE_TOP_SIZE
#define FLASH_RESERVE_TOP_SIZE                                                          0ul
#endif

#define BOOTLOADER_CODE_START                                                           0ul
#define BOOTLOADER_CODE_SIZE                                                            (BOOTLOADER_SIZE - BOOTLOADER_DATA_SIZE)
#define BOOTLOADER_CODE_END                                                             (BOOTLOADER_CODE_START + BOOTLOADER_CODE_SIZE - 1ul)
#define BOOTLOADER_DATA_START                                                           (BOOTLOADER_CODE_END + 1ul)
#define BOOTLOADER_DATA_END                                                             (BOOTLOADER_DATA_START + BOOTLOADER_DATA_SIZE - 1ul)
#define APPLICATION_CODE_START                                                          (BOOTLOADER_DATA_END + 1ul)
#define APPLICATION_CODE_SIZE                                                           (FLASH_SIZE - BOOTLOADER_SIZE - APPLICATION_DATA_SIZE - FLASH_RESERVE_TOP_SIZE)
#define APPLICATION_CODE_END                                                            (APPLICATION_CODE_START + APPLICATION_CODE_SIZE - 1ul)
#define APPLICATION_DATA_START                                                          (APPLICATION_CODE_END + 1ul)
#define APPLICATION_DATA_END                                                            (APPLICATION_DATA_START + APPLICATION_DATA_SIZE - 1ul)
#define APPLICATION_CODE_()

#if __18CXX != 1 // pack structs on Coldfire V2 devices
#pragma pack(1)
#endif

typedef struct
{
  const uint32_t checksum;
  const uint32_t majorVersion;
  const uint32_t minorVersion;
} application_information;

// boot loader data
// contents of base data is read-only (i.e. can not be overwritten via TFTP)

typedef struct
{
  const uint32_t bootloaderDataVersion; // 0x00000000
  const uint32_t buildVersion; // 0x00000004
  const uint32_t flashSize; // 0x00000008
  const uint32_t writeBlockSize; // 0x0000000c
  const uint32_t eraseBlockSize; // 0x00000010
  const uint32_t bootloaderCodeSize; // 0x00000014
  const uint32_t bootloaderDataSize; // 0x00000018
  const uint32_t applicationCodeSize; // 0x0000001c
  const uint32_t applicationDataSize; // 0x00000020 - 0x00000023
} bootloader_data_base;

typedef struct
{
  const bootloader_data_base base; // 0x00000000 - 0x00000023
  const uint8_t mac[6]; // 0x00000024 - 0x00000029
  const char serialNumber[100]; // 0x0000002a - 0x0000008d
} bootloader_data_version1;

typedef struct
{
  const bootloader_data_base base; // 0x00000000 - 0x00000023
  const uint8_t mac[6]; // 0x00000024 - 0x00000029
  const char serialNumber[100]; // 0x0000002a - 0x0000008d
  const uint32_t hardwareVersionMajor; // 0x0000008e - 0x00000091
  const uint32_t hardwareVersionMinor; // 0x00000092 - 0x00000095
  const uint8_t sharedSecret[32]; // 0x00000096 - 0x000000b5
  const char deviceId[100]; // 0x000000b6 - 0x00000119
  const char productDomain[100]; // 0x0000011a - 0x0000017d
} bootloader_data_version2;

typedef union
{
  const bootloader_data_version2 version2;
  const bootloader_data_version1 version1;
  const bootloader_data_base base;
  const uint8_t placeholder[BOOTLOADER_DATA_SIZE];
} bootloader_data;

typedef struct
{
  const uint16_t endiannessIndicator; // 0x00000000
  const uint32_t bootloaderVersion; // 0x00000002
  const uint32_t bootloaderDataVersion; // 0x00000006
  const uint32_t buildVersion; // 0x0000000a
  const uint32_t flashSize; // 0x0000000e
  const uint32_t writeBlockSize; // 0x00000012
  const uint32_t eraseBlockSize; // 0x00000016
  const uint32_t bootloaderCodeSize; // 0x0000001a
  const uint32_t bootloaderDataSize; // 0x0000001e
  const uint32_t applicationCodeSize; // 0x00000022
  const uint32_t applicationDataSize; // 0x00000026
  const uint32_t applicationInformationLocation; // 0x0000002a
} system_information;

#if __18CXX != 1
#pragma pack()
#endif

extern const __ROM bootloader_data bootloaderData;
extern const __ROM application_information applicationInformation;

#endif
