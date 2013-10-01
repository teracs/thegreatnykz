#include "www.h"
const int gifsize;
extern void captcha(unsigned char im[70*200], unsigned char l[6]);
extern void makegif(unsigned char im[70*200], unsigned char gif[gifsize]);

int bbsvcode_main()
{
    char file[32];
    char buf1[1024], buf2[1024];
    unsigned int r;
    int rfd, ifd, n_read;
    char l[6];
    unsigned char im[70*200];
    unsigned char gif[gifsize];

    captcha(im,l);
    makegif(im,gif);

    silence = 1;
    srandom(time(0) + getpid());
    r = random();
    r = r % 9000 + 1000;
    strsncpy(file, getparm("file"), 30);
    if (strlen(file) == 0)
        return 0;
    snprintf(buf1, 1023, "httpd/vcode/%u.gif", r);
    snprintf(buf2, 1023, "httpd/vcode-tmp/%s", file);
    rfd = open(buf2, O_WRONLY | O_CREAT, 0600);
    if (-1 == rfd)
        return 0;
    write(rfd, l, 6);
    close(rfd);

    cgiHeaderString("Cache-Control: no-cache\r\n");
    cgiHeaderString("Pragma: no-cache\r\n");
    cgiHeaderString("Expires: Thu, 01 Jan 1970 00:00:00 GMT\r\n");
    cgiHeaderContentType("image/gif");


    fwrite(gif, gifsize, 1, stdout);

    return 1;
}
