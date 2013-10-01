#include "www.h"
static int time1;

//Bug
//Bug-description:	if anyone downloads a file from showfile.c,
//			his broswer such as IE cannot get the ext-filename
//Bug-solution:		Add two http-headers as pigworlds suggested,
//			one is Content-Length, and the other is Content-Disposition
//			printf("Content-Disposition: null; filename=%s\n\n", name)
//Bug-fixer:		hongliang
//Date:			Nov. 20th, 2003 5:40

//set this file-name as global variables by hongliang


/* use gdbm for quickness */
char name[256];

int showfile_main()
{
    struct dir *dp;
	GDBM_FILE dbf;
	datum key,data;


    time1 = times(0);
    silence = 1;
	init_all();
    strsncpy(name, getparm("name"), 40);
	dbf = gdbm_open(UPLOAD_PATH"/.DB",0,GDBM_READER,0644,0);
    if (dbf == 0)
    {
        cgi_head();
        http_fatal("文件不存在1");
    }
	key.dptr = name;
	key.dsize = strlen(name);
	data = gdbm_fetch(dbf,key);
	gdbm_close(dbf);
	if(data.dptr != 0 && data.dsize == sizeof(struct dir))
	{
		dp = (struct dir *)data.dptr;
		cat(dp->filename);
		free(data.dptr);
		exit(0);
	}    
    cgi_head();
    //printf("name=%s\n", name);
    http_fatal("文件不存在3");
}

int cat(int fid)
{
    char file[80], buf[4000];
    FILE *fp;
    int r;

    sprintf(file, UPLOAD_PATH "/%d", fid);
    fp = fopen(file, "r");
    if (fp == 0)
    {
        cgi_head();
        http_fatal("文件不存在(error_code 4)");
    }
    //deleted by hongliang
    //printf("Content-type: application/octet-stream;\n\n");

    //start-hongliang
    printf("Content-Length: %u\n", file_size(file));
    printf("Content-Type: application/octet-stream;\n");
    printf("Content-Disposition: null; filename=%s\n\n", name);
    //end-hongliang
    while (1)
    {
        r = fread(buf, 1, 4000, fp);
        if (r <= 0)
            break;
        fwrite(buf, 1, r, stdout);
    }
}
