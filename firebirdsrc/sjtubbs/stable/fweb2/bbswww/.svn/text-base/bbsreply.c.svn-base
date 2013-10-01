#include "www.h"

int bbsreply_main()
{
    FILE *fp;
    int filetime, i, start, total;
    int ret, max = 0, size = 0;
    char *ptr, buf[512], dir[80];
    struct fileheader *data;

    init_all();
    if (!loginok)
        http_quit_print("ÄúÉĞÎ´µÇÂ¼, ÇëÏÈµÇÂ¼");
    printf("<script type=\"text/javascript\" src=\"/app/reply/reply.js\"></script>\n");
    printf("<script type=\"text/javascript\" src=\"/app/reply/base64.js\"></script>\n");
    printf("<script type=\"text/javascript\">\n");
    printf("var data =");
    get_reply_handle(currentuser.userid);
    printf(";");
    printf("</script>\n");
    printf("<body onload=\"show_reply(data)\">"
           "<table id=\"replys\">"
           "</table>"
           "</body>");
    http_quit();
    return 0;
}

