/*
 * Copyright (C) 2008-2013 Nabto - All Rights Reserved.
 */
#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#endif
#include "unabto/unabto_env_base.h"
#include "modules/cli/unabto_args.h"
#include "modules/diagnostics/unabto_diag.h"
#include "unabto/unabto_logging.h"
#include "unabto_version.h"
#include "gopt.h" // http://www.purposeful.co.uk/software/gopt/

#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>


/// Provide the safe version API. @param src source @param format formatstring @param param1 the parameter
/// TODO: use proper linux variant!!!!
#define sscanf_s(src, format, param1) sscanf(src, format, param1)

#else
#define strdup _strdup
#endif

/**
 * @file
 * The uNabto Program Argument Verification (PC Device) Implementation.
 */


/**
 * Print help message
 * @param errmsg  if not 0 this is an error message
 */
static void help(const char* errmsg, const char *progname)
{
    if (errmsg) {
        NABTO_LOG_INFO(("ERROR: %s", errmsg));
    }
    NABTO_LOG_INFO(("Usage: %s -d <host id> [-s [-k <preshared key (hex encoded)>] ] [-A <BS>] [-a <address>] [-p <port>] ", progname));
    NABTO_LOG_INFO(("    -d: the host id to use (e.g. myweatherstation.nabto.net)"));
    NABTO_LOG_INFO(("    -s: use encryption (if no -k parameter specified, a null preshared secret is used)"));
    NABTO_LOG_INFO(("    -k: preshared key to use for encryption"));
    NABTO_LOG_INFO(("    -A: register with specified basestation"));
    NABTO_LOG_INFO(("        - if omitted, the basestation address is resolved using the specified server id"));
    NABTO_LOG_INFO(("    -a: bind to specified local address"));
    NABTO_LOG_INFO(("    -p: bind to specified local port"));
    NABTO_LOG_INFO(("    -i: bind to specified local interface (Unix only)"));
    NABTO_LOG_INFO(("    -V: Print version and exit."));
    NABTO_LOG_INFO(("    -C: Print configuration (unabto_config.h) and exit."));
    NABTO_LOG_INFO(("    -S: Print size (in bytes) of memory usage and exit."));
} /* help(const char* errmsg) */


bool check_args(int argc, char* argv[], nabto_main_setup *nms)
{
    const char *address;
    const char *basestationAddress;
    const char *preSharedKey;
    const char *localPortStr;
    const char *bufferSizeStr;
    const char *idParam;
    const char *interfaceParam;
    const char *htmlddurloverride;
    uint32_t addr;
    const char *progname;
#ifdef WIN32
    char modulename[MAX_PATH];
#endif

    
    const char x0s[] = "h?";     const char* x0l[] = { "help", "HELP", 0 };
    const char x1s[] = "a";      const char* x1l[] = { "localAddress", 0 };
    const char x2s[] = "d";      const char* x2l[] = { "deviceName", 0 };
    const char x3s[] = "l";      const char* x3l[] = { "log", 0 };
    const char x4s[] = "A";      const char* x4l[] = { "controllerAddress", 0 };
    const char x5s[] = "p";      const char* x5l[] = { "localport", 0 };
    const char x6s[] = "b";      const char* x6l[] = { "buffersize", 0 };
    const char x7s[] = "k";      const char* x7l[] = { "presharedkey", 0 };
    const char x8s[] = "s";      const char* x8l[] = { "securedevice", 0 };
    const char x9s[] = "n";      const char* x9l[] = { "datanullencrypted", 0 };
    const char x10s[] = "i";     const char* x10l[] = { "interface", 0 };
    const char x11s[] = "U";     const char* x11l[] = { "htmlddurloverride", 0 };
    const char x12s[] = "V";     const char* x12l[] = { "version", 0 };
    const char x13s[] = "C";     const char* x13l[] = { "config", 0 };
    const char x14s[] = "S";     const char* x14l[] = { "size", 0 };

    const struct { int k; int f; const char *s; const char*const* l; } opts[] = {
        { 'h', 0,           x0s, x0l },
        { 'a', GOPT_ARG,    x1s, x1l },
        { 'd', GOPT_ARG,    x2s, x2l },
        { 'l', GOPT_REPEAT, x3s, x3l },
        { 'A', GOPT_ARG,    x4s, x4l },
        { 'p', GOPT_ARG,    x5s, x5l },
        { 'b', GOPT_ARG,    x6s, x6l },
        { 'k', GOPT_ARG,    x7s, x7l },
        { 's', 0,           x8s, x8l },
        { 'n', 0,           x9s, x9l },
        { 'i', GOPT_ARG,    x10s, x10l },
        { 'U', GOPT_ARG,    x11s, x11l },
        { 'V', GOPT_NOARG,  x12s, x12l },
        { 'C', GOPT_NOARG,  x13s, x13l },
        { 'S', GOPT_NOARG,  x14s, x14l },
        { 0,0,0,0 }
    };

    void *options = gopt_sort( & argc, (const char**)argv, opts);

#ifdef WIN32
    modulename[0] = 0;
    GetModuleFileNameA(NULL, modulename, sizeof(modulename));
    progname = strrchr(modulename, '\\');
    if (!progname)
        progname = modulename;
    else
        progname++;
#else
    progname = strrchr(argv[0], '/');
    if (!progname)
        progname = argv[0];
    else
        progname++;
#endif

    if( gopt( options, 'h')) {
        help("Help", progname);
        return false;
    }
    
    if (gopt(options, 'V')) {
        fprintf(stdout, "%d.%d\n", RELEASE_MAJOR, RELEASE_MINOR);
        return false;
    }

    if( gopt( options, 'C')) {
        unabto_printf_unabto_config(stdout, progname);
        return false;
    }
    
    if (gopt(options, 'S')) {
        unabto_printf_memory_sizes(stdout, progname);
        return false;
    }

    if( gopt_arg( options, 'a', & address ) ){
        addr = inet_addr(address);
        if (addr == INADDR_NONE) {
            help("Illegal local address", progname);
            gopt_free(options);
            return false;
        }
		nms->ipAddress = htonl(addr);
    } 

    if( gopt_arg( options, 'd', &idParam ) ){
        nms->id = strdup(idParam);
    } else {
        help("You must specify an id for your uNabto device", progname);
        gopt_free(options);
        return false;
    }

    if( gopt_arg( options, 'p', &localPortStr) ){
		nms->localPort = atoi(localPortStr);
    }

    if( gopt_arg( options, 'b', &bufferSizeStr) ){
        nms->bufsize = (size_t)atoi(bufferSizeStr);
    }

    if( gopt_arg( options, 'A', & basestationAddress ) ){
        addr = inet_addr(basestationAddress);
        if (addr == INADDR_NONE) {
            help("Illegal basestation address", progname);
            gopt_free(options);
            return false;
        }
        nms->controllerArg.addr = htonl(addr);
    }

    if ( gopt_arg( options, 'k', &preSharedKey)) {
        size_t i;
        size_t pskLen = strlen(preSharedKey);
        // read the pre shared key as a hexadecimal string.
        for (i = 0; i < pskLen/2 && i < 16; i++) {
            sscanf_s(preSharedKey+(2*i), "%02hhx", &nms->presharedKey[i]);
        }
    }

    if (gopt(options, 's')) {
        nms->secureAttach= true;
        nms->secureData = true;
#if NABTO_ENABLE_CONNECTIONS
        nms->cryptoSuite = CRYPT_W_AES_CBC_HMAC_SHA256;
#endif
    }
    if (gopt(options, 'n')) {
        nms->secureData = false;
    }

    if (gopt_arg(options, 'i', &interfaceParam)) {
        nms->interfaceName = strdup(interfaceParam);
    }

    if (gopt_arg(options, 'U', &htmlddurloverride)) {
        nms->url = strdup(htmlddurloverride);
    }

    gopt_free(options);
    
    return true;
}
