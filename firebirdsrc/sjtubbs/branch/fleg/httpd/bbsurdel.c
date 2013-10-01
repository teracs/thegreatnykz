#include "www.h"

int bbsurdel_main()
{
	struct shortfile *brd;
	char board[40],url[256];
	int i;
	
	init_all();
	strsncpy(board,getparm("board"),40);
	brd = getbcache(board);
    if (brd == 0)
    	http_fatal("内部错误10002");
    if (!has_BM_perm(&currentuser, board))
    {
         http_fatal("你无权进入管理页面");
    }
	for (i = 0; i < parm_num && i < 40; i++)
    {
        if (!strncmp(parm_name[i], "del", 3))
        {
            del_upload_file(parm_name[i] + 3,board);
        }
    }
	sprintf(url,"bbsfddoc?board=%s",board);
	redirect(url);
	
}
int del_upload_file( char * value, char *board)
{
	/* store pre deleted fid, to determine if fid should be decrese 1 */
	static int prefid =-1,count=1;
	int dirsize,fid,ofid;
	char dirbuf[256],buf[7],filename[256];
	FILE *fp;
	struct dir x;
	if(strlen(value)<=10)
		return 1;
	strncpy(buf,value,6);
	buf[6]='\0';
	fid = atoi(buf);
	ofid = fid;
	if( prefid != -1 && prefid < fid)
	{
		fid -= count;
		count++;
	}
	sprintf(dirbuf,UPLOAD_PATH2"/%s",board);
	dirsize = file_size(dirbuf)/sizeof(struct dir);
	if(fid>=0 && fid<dirsize)
	{
		fp = fopen(dirbuf,"r");
		fseek(fp,fid*sizeof(struct dir),SEEK_SET);
		fread(&x,sizeof(struct dir),1,fp);
		fclose(fp);
		if( strcmp(x.showname,value+6) )
		{
			count--;
			return 1;
		}
		sprintf(filename,UPLOAD_FILE_PATH2"/%s/%s",board,x.showname);
		unlink(filename);
		del_record(dirbuf, sizeof(struct dir), fid);
		prefid = ofid;
		return 0;
	}
	count--;
}
