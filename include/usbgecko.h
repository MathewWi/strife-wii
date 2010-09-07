#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <gccore.h>
#include <malloc.h>
#include <wiiuse/wpad.h>

void gcprintf(const char *fmt, ...);
        void gprintf(const char *fmt, ...);
        void InitGecko();
		
static bool geckoinit = false;