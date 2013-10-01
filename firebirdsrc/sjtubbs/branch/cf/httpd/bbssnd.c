#include "www.h"

int bbssnd_main()
{
    char filename[80], dir[80], board[80], title[80], buf[80], *content;
	char reid_str[16];
    int autocr = 0, r, i, sig;
    int anony;
    int noip = 0;

	FILE *fp;
	char keyfile[80];
	char keywords[64][64];
	int ii=0,iii,len,key=0;
	char *pos;
    struct shortfile *brd;

	time_t now = 0;
	unsigned int currid = 0;
	unsigned int reid = 0;

    init_all();
    fflush(stdout);
    if (!loginok)
    {
        strcpy(currentuser.userid, "guest");
        strcpy(currentuser.username, "匆匆过客");
    }
    strsncpy(board, getparm("board"), 18);
    strsncpy(title, getparm("title"), 50);

//	strsncpy(is_new_post, getparm("post_status"), 1);
	strsncpy(reid_str, getparm("reidstr"), 16);
	if(reid_str[0])	//reply
	{
		reid = atoi(reid_str);
		currid = 0;
	}
	else	//new article
	{
		now = time(0);
		reid = now;
		currid = now;
	}
		
    if (!loginok )
        http_fatal("匆匆过客不能发表文章，请先登录");
    if (strlen(getparm("autocr")) > 0)
        autocr = 1;
    brd = getbcache(board);
    if (brd == 0)
        http_fatal("错误的讨论区名称");
    strcpy(board, brd->filename);
    for (i = 0; i < strlen(title); i++)
        if (title[i] <= 27 && title[i] >= -1)
            title[i] = ' ';
    sig = atoi(getparm("signature"));
    anony = atoi(getparm("anony"));
    content = getparm("text");
    sprintf(keyfile,"%s","etc/keywords");
	fp=fopen(keyfile,"r");
	while(1)
		{if(fgets(buf,64,fp) == 0)
			break;
		sprintf(keywords[ii],buf);
		ii++;
		}
	ii=0;
while(1) {
       len=strlen(keywords[ii]);
       if(len <= 1)
             break;
       keywords[ii][len-1]='\0';
       while(pos=strstr(title,keywords[ii])) {
	       key=1;
	       break;
	       /*
       for(iii=0;iii < (len-1);iii++) {
                 *(pos+iii) = '*';
              }*/
                        }
                ii++;
                }

ii=0;	
	while(1) {
		len=strlen(keywords[ii]);
		if(len <= 1)
			break;
//		keywords[ii][len-1]='\0';
		while(pos=strstr(content,keywords[ii])) {
			key=1;
			break;
		/*	for(iii=0;iii < (len);iii++) {
				*(pos+iii) = '*';
				}*/
			}
		ii++;

		}
    if (title[0] == 0)
        http_fatal("文章必须要有标题");
    sprintf(dir, "boards/%s/.DIR", board);
    if (!has_post_perm(&currentuser, board))
        http_fatal("此讨论区是唯读的, 或是您尚无权限在此发表文章.");
#ifdef	POST_AFTER_ONE_DAY
	if(too_young(&currentuser, board))
		http_fatal("您注册时间还不满24小时，暂时不能在本板发表文章.如果有疑问请在 BBSHelp 或者 sysop 板发文咨询。");
#endif
    sprintf(filename, "boards/%s/deny_users", board);
    if (file_has_word(filename, currentuser.userid))
        http_fatal("很抱歉, 你被板务人员停止了本板的post权利.");
    if (loginok && abs(time(0) - *(int *) (u_info->from + 36)) < 6)
    {
        *(int *) (u_info->from + 36) = time(0);
        http_fatal("两次发文间隔过密, 请休息几秒后再试");
    }
    if (loginok)
        *(int *) (u_info->from + 36) = time(0);
    sprintf(filename, "tmp/%d.tmp", getpid());
    unlink(filename);
    f_append(filename, content);
    if (brd->flag & ANONY_FLAG || brd->flag & POSITIVE_ANONY_FLAG)
    {
        noip = anony;
    }
	else
		noip = 0;
    if(key) {
	    r = post_article("junk", title, filename, currentuser.userid,currentuser.username, 
			fromhost, sig - 1, autocr, currid, reid);
	    http_fatal("文章的标题或内容中包含了本站不允许的关键字！");
	}
    else {
    if (noip)
        r = post_article(board, title, filename, board, "我是匿名天使",
                         "匿名天使的家", -1, 1, currid, reid);
    else
        r = post_article(board, title, filename, currentuser.userid,
                         currentuser.username, fromhost, sig - 1, autocr, currid, reid);
    }
    
    if (r <= 0)
        http_fatal("内部错误，无法发文");
    sprintf(buf, "M.%d.A", r);
    if (loginok)
    {
        brc_init(currentuser.userid, board);
        brc_add_read(buf);
        brc_update(currentuser.userid, board);
    }
    unlink(filename);
    sprintf(buf, "bbsdoc?board=%s", board);
    if (loginok)
    {
        if (!junkboard(brd->filename) && !is_hide_board(brd->level))
        {
            currentuser.numposts++;
            save_user_data(&currentuser);
            write_posts(currentuser.userid, board, title);
        }
        else
        {
            save_user_data(&currentuser);
        }
    }
    f_append("trace.post", "b");
    if (loginok && has_BM_perm(&currentuser, board))
    {
        char buf[256];

        sprintf(buf, "%24.24s %s %s W_P\n", Ctime(time(0)),
                currentuser.userid, board);
        f_append("bm2.log", buf);
    }
    redirect(buf);
}

int is_hide_board(int level)
{
    if (level == 0 || level & (PERM_POSTMASK | PERM_NOZAP))
        return 0;
    return 1;
}

int write_posts(char *id, char *board, char *title)
{
    FILE *fp;
    struct post_log x;

    strcpy(x.author, id);
    strcpy(x.board, board);
    if (!strncmp(title, "Re: ", 4))
        title += 4;
    strsncpy(x.title, title, 61);
    if (title[0] == 0)
        return;
    if (if_exist_title(title))
        return;
    x.date = time(0);
    x.number = 1;
    fp = fopen(".post", "a");
    fwrite(&x, sizeof(struct post_log), 1, fp);
    fclose(fp);
}

int if_exist_title(char *title)
{
    static struct
    {
        int hash_ip;
        char title[64][60];
    }
    my_posts;
    char buf1[256];
    int n;
    FILE *fp;

    sethomefile(buf1, currentuser.userid, "my_posts");
    fp = fopen(buf1, "r+");
    if (fp == NULL)
        fp = fopen(buf1, "w+");
    fread(&my_posts, sizeof(my_posts), 1, fp);
    for (n = 0; n < 64; n++)
        if (!strncmp(my_posts.title[n], title, 50))
        {
            fclose(fp);
            return 1;
        };
    my_posts.hash_ip = (my_posts.hash_ip + 1) & 63;
    strncpy(my_posts.title[my_posts.hash_ip], title, 50);
    fseek(fp, 0, SEEK_SET);
    fwrite(&my_posts, sizeof(my_posts), 1, fp);
    fclose(fp);
    return 0;
}
