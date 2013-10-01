#ifndef _H_WWW
#define _H_WWW

/* include stdio wrapper for fastcgi */
#define FWEB_STDIO_DEFINES
#include "fwebio.h"
#undef FWEB_STDIO_DEFINES

#include "shm.h"
#include "kernel.h"
#include "cgic.h"
#include "mempool.h"

#endif
