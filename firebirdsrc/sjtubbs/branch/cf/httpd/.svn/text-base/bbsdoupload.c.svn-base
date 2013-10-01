#include "www.h"
static struct dir x;
int memstr(char *s, int len, char *s2)
{
    int i, len2 = strlen(s2);

    for (i = 0; i <= len - len2; i++)
        if (!strncmp(s + i, s2, len2))
            return i;
    return -1;
}

int bbsdoupload_main()
{
    int m, n, nn = 0, i;
    char *buf, *p, *filename, buf1[256], ID[280];
    char board[80], level[80], live[80], exp[80];
    FILE *fp;

    chdir(BBSHOME);
    printf("Content-type: text/html; charset=gb2312\n\n");
    n = atoi(getsenv("CONTENT_LENGTH"));
    if (n < 0)
        n = 0;
    if (n > 2000000)
        n = 2000000;
    buf = malloc(n);
    if (buf == 0)
        http_fatal("bbsupload: out of memory");
    fgets(ID, 200, stdin);
    fgets(buf1, 256, stdin);
    nn += strlen(ID) + strlen(buf1);
    filename = strcasestr(buf1, "filename=\"");
    if (filename == 0)
        http_fatal("bbsupload: filename unknown");
    while (1)
    {
        p = strchr(filename, '\\');
        if (!p)
            break;
        filename = p + 1;
    }

	//modified by zsong, debug for auto change name
	//under linux, the seperate character should resort to "/" not "\".
	while (1)
    {
        p = strchr(filename, '/');
        if (!p)
            break;
        filename = p + 1;
    }
	

    p = strstr(filename, "\"");
    if (p)
        p[0] = 0;
    fgets(buf1, 256, stdin);
    nn += strlen(buf1);
    fgets(buf1, 256, stdin);
    nn += strlen(buf1);
    n -= nn;
    if (n < 0)
        n = 0;
    m = fread(buf, 1, n, stdin);
    buf[m] = 0;
    i = memstr(buf, m, ID);
    if (i >= 0)
    {
        int ii;
        char tmpfilename[100], tmp2[100];

        sprintf(tmpfilename, "tmp/%d.upload", getpid());
        sprintf(tmp2, "tmp/%d.tmp", getpid());
        fp = fopen(tmp2, "w+");
		unlink(tmp2);
        p = buf + i;
        fprintf(fp, "%s", p);
        rewind(fp);
        for (ii = 1; ii <= 7; ii++)
            fgets(buf1, 256, fp);
        fgets(board, 80, fp);
        for (ii = 1; ii <= 3; ii++)
            fgets(buf1, 256, fp);
        fgets(level, 80, fp);
        for (ii = 1; ii <= 3; ii++)
            fgets(buf1, 256, fp);
        fgets(live, 80, fp);
        for (ii = 1; ii <= 3; ii++)
            fgets(buf1, 256, fp);
        fgets(exp, 80, fp);
        fclose(fp);
        fp = fopen(tmpfilename, "w");
        fwrite(buf, 1, i - 2, fp);
        fclose(fp);

		/* can't execute init_all(), so go to next page to do what we want */
        sprintf(buf,
                "bbsupload2?board=%s&type=data&file=%d&level=%s&name=%s&exp=%s&live=%s",
                board, getpid(), level, filename, exp, live);
        redirect(buf);
        
    }
    else
    {
        http_fatal("upload format error");
    }
}


