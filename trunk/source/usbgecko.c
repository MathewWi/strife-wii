#include "usbgecko.h"  

      //using the gprintf from crediar because it is smaller than mine
        void gprintf(const char *fmt, ...)
        {
                if (!(geckoinit)) return;
                char astr[4096];
                va_list ap;
                va_start(ap,fmt);
                vsprintf( astr, fmt, ap );
                va_end(ap);
                usb_sendbuffer_safe( 1, astr, strlen(astr) );
                memset(astr, 0, sizeof(astr));
        } 

        void gcprintf(const char *fmt, ...)
        {
                char astr[4096];
                va_list ap;
                va_start(ap,fmt);
                vsprintf(astr, fmt, ap);
                va_end(ap);

                gprintf(astr);  
                printf(astr);   
                memset(astr, 0, sizeof(astr));
        }


        void InitGecko()
        {
                if (usb_isgeckoalive(EXI_CHANNEL_1))
                {
                        usb_flush(EXI_CHANNEL_1);
                        geckoinit = true;
                }
        }