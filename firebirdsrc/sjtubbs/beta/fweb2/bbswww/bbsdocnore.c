#include "www.h"

//this function is for un-re
int bbsdocnore_main()
{
    FILE *fp;
    char board[80], dir[80], file[80];
    struct fileheader x, *p;
	struct shortfile *x1;
    int num, total;
//    int start;

    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(file, getparm("file"), 32);
//    start	= atoi(getparm("start"));
    num	= atoi(getparm("num"));
	x1 = getbcache(board);
	if (!x1)
	{
		http_quit_print("板面不存在");
	}
	strcpy(board, x1->filename);
    p  = get_file_ent(board, file, 1);
    if (p == 0)
        http_quit_print("错误的参数");
    if (!has_read_perm(&currentuser, board))
        http_quit_print("错误的讨论区");
    if (strcmp(p->owner, currentuser.userid) && !has_BM_perm(&currentuser, board))
        http_quit_print("你无权设置此文章不可re");

    strcpy(board, getbcache(board)->filename);

    if (strncmp(file, "M.", 2) && strncmp(file, "G.", 2))
        http_quit_print("错误的参数1");
    if (strstr(file, "..") || strstr(file, "/"))
        http_quit_print("错误的参数2");
    sprintf(dir, "boards/%s/.DIR", board);
    total = file_size(dir) / sizeof(x);
    if (total <= 0)
        http_quit_print("此讨论区不存在或者为空");
	

    fp = fopen(dir, "r+");
    if (fp == 0)
        http_quit_print("dir error2");
    if (num >= 0 && num < total)
    {
        fseek(fp, sizeof(x) * num, SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        //Set it to no-re or re
        if ( x.accessed[0] & FILE_NOREPLY )
        {
            x.accessed[0] &= ~FILE_NOREPLY;
        }
        else
        {
            x.accessed[0] |= FILE_NOREPLY;
        }
        fseek(fp, sizeof(x) * num, SEEK_SET);
        fwrite(&x, sizeof(x), 1, fp);
    }
    fclose(fp);
	
    printf("<body>");
    printf("<center>\n");
    printf("设置成功.<br><a href=\"bbsdoc?board=%s&page=%d\">返回本讨论区</a>",
           board, num/PAGE_SIZE);

    printf("\n</html>");
	return 0;
}
