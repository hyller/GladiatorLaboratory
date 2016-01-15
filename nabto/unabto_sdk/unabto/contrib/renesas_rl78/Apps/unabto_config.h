#ifndef UNABTO_CONFIG_H
#define UNABTO_CONFIG_H

/**
 * This header contains the specific Nabto settings for this project.
 * All available settings can be read about in "unabto_config_defaults.h".
 * The default value will be used if a specific setting is not set here.
 */

#define NABTO_DEVICE NABTO_DEVICE_RENESAS_RL78

#define NABTO_ENABLE_STREAM                         0
#define NABTO_CONNECTIONS_SIZE                      4
#define NABTO_ENABLE_UCRYPTO                        0
#define NABTO_ENABLE_INIT_CLOSE_PLATFORM            0

#define NABTO_ENABLE_LOGGING                        0
#define NABTO_LOG_ALL                               0
#define NABTO_LOG_SEVERITY_FILTER NABTO_LOG_SEVERITY_LEVEL_DEBUG

#define NABTO_ENABLE_REMOTE_CONNECTION              1
#define NABTO_ENABLE_LOCAL_CONNECTION               1
#define NABTO_ENABLE_LOCAL_ACCESS_LEGACY_PROTOCOL   1

#define NABTO_SET_TIME_FROM_ALIVE                   0

#define UNABTO_PLATFORM_RENESAS_RL78                1

#endif
