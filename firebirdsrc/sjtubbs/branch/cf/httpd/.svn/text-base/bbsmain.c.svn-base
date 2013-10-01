#include "www.h"

int bbsmain_main()
{
	cache_age = 60*60*24*7;
	last_modified_time = 60*60*24*365*20;/*20 years*/
    cgi_head();
    printf("
           <html>
           <head>
           <meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\">
           <title>%s</title>
           </head>
           <frameset border=0 frameborder=0 framespacing=2 framemargin=0 cols=\"120,*\">
           <frame name=f2 framespcing=2 src=\"bbsleft\">
           <frameset rows=\"18, *, 16\">
           <frame scrolling=no marginwidth=4 marginheight=0 framespacing=0 name=fmsg src=\"bbsgetmsg\">
           <frame framespacing=2 name=f3 src=\"bbssec\">
           <frame scrolling=no marginwidth=4 marginheight=1 framespacing=1 name=f4 src=\"bbsfoot\">
           </frameset>
           </frameset>
           </html>
           ", BBSNAME);
}
