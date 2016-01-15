#ifndef _TCP_FALLBACK_H_
#define _TCP_FALLBACK_H_

#include <unabto_platform_types.h>
#include <unabto/unabto_context.h>

#ifdef WIN32
#include <winsock2.h>
#else
#include <pthread.h>
#endif

#endif
