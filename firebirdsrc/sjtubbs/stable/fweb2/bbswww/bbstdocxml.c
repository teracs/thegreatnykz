#include "www.h"

int bbstdocxml_main()
{
    char board[80], page_str[10], post_str[10];
    struct shortfile *x1;
    struct fileheader *fh, **fhs;
    int page = -1, num = 0, from, to, i, num2 = 0, showtop = 0, j, replynum;

    silence = 1;
    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(page_str, getparm("page"), 10);
    strsncpy(post_str, getparm("post"), 10);

    if (strlen(page_str) != 0)
        page = atoi(page_str);

    else if (strlen(post_str) != 0)
        page = (atoi(post_str) - 1) / 20;

    if (!has_read_perm(&currentuser, board))
    {
        http_quit_print("错误的讨论区");
    }
    printf("Connection: close\n");
    printf("Content-type: text/xml;charset=gb2312\n\n");
    x1 = getbcache(board);
    strcpy(board, x1->filename);
    fh = (struct fileheader *) map_board_dir(board, ".DIR");
    if (fh == NULL)
        http_quit_print("目录错误");
    num = get_size_of_mapdir();
    fhs = (struct fileheader **) mp_alloc(sizeof(struct fileheader *) *
                                          num);
    for (i = 0; i < num; ++i)
    {
        if (fh[i].id == fh[i].reid)
        {
            fhs[num2++] = &fh[i];
        }
    }
    from = page * PAGE_SIZE;
    to = from + PAGE_SIZE;
    if (from >= num2 || from < 0)
    {
        from = num2 - PAGE_SIZE;
        to = num2;
    }
    else if (to > num2)
    {
        from -= (to - num2);
        to = num2;
    }
    if (from < 0)
        from = 0;
    if (num2 == 0)
    {
        unmap_board_dir();
        http_quit_print("本讨论区目前没有文章");
    }
    if (to == num2)
        showtop = 1;
    x1 = getbcache(board);
    strcpy(board, x1->filename);
    printf
        ("<?xml version=\"1.0\" encoding=\"gb2312\"?>\n<!-- generator=\"fweb/xml mod 0.2\" -->\n");
    printf("<board>\n");
    printf("<meta>\n");
    printf("<enum>%d</enum>\n<tnum>%d</tnum>\n",num,num2);
    printf("</meta>");
    printf("<posts>\n");
    for (i = from; i < to; i++)
    {
        printf("<post>");
        replynum = 0;
        for (j = num - 1; j >=0; j--)
            if (fh[j].reid == fhs[i]->id)
            {
               if (fh[j].id == fhs[i]->id)
                   break;
               replynum++;
            }    
        printf("<num>%d</num>\n<status>%s</status>\n<author><![CDATA[ %s ]]></author>\n",i+1,flag_str(fhs[i]->accessed[0]),fhs[i]->owner);
	
        printf("<pubdate>%12.12s</pubdate>", Ctime(atoi(fhs[i]->filename + 2)) + 4);
	printf
            ("<title><![CDATA[ %s  ]]></title>\n<postsize><![CDATA[ %d ]]></postsize>\n<postfilename><![CDATA[ %d ]]></postfilename>\n",void1(nohtml2(fhs[i]->title)),replynum,fhs[i]->reid);
	printf("</post>\n");
    }
	printf("</posts>\n");
    unmap_board_dir();
    if (showtop)
    {
	printf("<topposts>");
        fh = (struct fileheader *) map_board_dir(board, ".TOPFILE");
        if (NULL != fh)
        {
            int size = get_size_of_mapdir();
            char *ptr;

            for (i = 0; i < size; i++)
            {
		printf("<toppost>");
                ptr = flag_str2(fh[i].accessed[0]);
                switch (ptr[0])
                {
                case 'M':
                    printf("<flag>[提示]</flag>");
                    break;
                case 'G':
		    printf("<flag>[通知]</flag>\n");
                    break;
                case 'B':
		    printf("<flag>[板规]</flag>\n");
                    break;
                default:
		    printf("<flag>[置底]</flag>\n");
                }
                printf("<author><![CDATA[ %s ]]></author>\n",
                       userid_str(fh[i].owner));
                printf("<pubdate>%12.12s</pubdate>\n",
                       Ctime(atoi(fh[i].filename + 2)) + 4);
				printf("<postfilename><![CDATA[ %s ]]></postfilename>\n",fh[i].filename);
                printf
                    ("<title><![CDATA[ %s%s ]]></title>\n",
                     strncmp(fh[i].title, "Re: ", 4) ? "○ " : "",
                     void1(nohtml2(fh[i].title)));
                printf("</toppost>\n");
            }
            unmap_board_dir();
	    printf("</topposts>\n");
        }
    }
    printf("</board>\n");
    return 0;
}
