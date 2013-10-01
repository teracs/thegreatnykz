#include "www.h"

int bbsdocxml_main()
{
    char board[80], page_str[10], filename[80], dirname[80];
    struct shortfile *x1;
    struct fileheader *fh;
    struct stat dirfile;
    int width = 720, page = -1, num = 0, from, to, showtop = 0, i;

    silence = 1;
    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(page_str, getparm("page"), 10);
    if (strlen(page_str) != 0)
        page = atoi(page_str);

    if (!has_read_perm(&currentuser, board))
    {
        cgi_head();
        http_quit_print("错误的讨论区");
    }

    printf("Connection: close\n");
    printf("Content-type: text/xml;charset=gb2312\n\n");
    x1 = getbcache(board);
    strcpy(board, x1->filename);
    sprintf(dirname, "boards/%s/.DIR", board);
    if (stat(dirname, &dirfile) == -1)
    {
        http_quit_print("fatal error 2");
    }
    fh = (struct fileheader *) map_board_dir(board, ".DIR");
    if (fh == NULL)
        http_quit_print("目录错误");

    num = get_size_of_mapdir();
    from = page * PAGE_SIZE;
    to = from + PAGE_SIZE;
    if (from >= num || from < 0)
    {
        from = num - PAGE_SIZE;
        to = num;
    }
    else if (to > num)
    {
        from -= (to - num);
        to = num;
    }
    if (from < 0)
        from = 0;
    if (num == to)
        showtop = 1;

    if (my_font_mode == 0)
        width = 670;

    printf
        ("<?xml version=\"1.0\" encoding=\"gb2312\"?>\n<!-- generator=\"fweb/0.1\" -->\n");

    printf("<board>\n");
    printf("<meta>\n");
    printf
        ("<boardname>%s</boardname>\n<boarddescription>%s</boarddescription>\n",
         x1->filename, x1->title + 11);
    printf("<timestamp>%lu</timestamp>\n", dirfile.st_mtime);
    printf("<bm><![CDATA[ %s ]]></bm>\n", x1->BM);
    printf("<wishwords>not implement</wishwords>\n");
    printf("</meta>\n");

    printf("<posts>\n");
    for (i = from; i != to; i++)
    {
        printf("<post>");
        sprintf(filename, "boards/%s/%s/%s", board,
                getfolderstr(fh[i].filename), fh[i].filename);

        printf
            ("<num>%d</num>\n<status>%s</status>\n<author><![CDATA[ %s ]]></author>\n",
             i + 1, flag_str2(fh[i].accessed[0]), fh[i].owner);
        printf("<pubdate>%12.12s</pubdate>\n",
               Ctime(atoi(fh[i].filename + 2)) + 4);

        printf
            ("<title><![CDATA[ %s%s%s  ]]></title>\n<postsize><![CDATA[ %s ]]></postsize>\n<postfilename><![CDATA[ %s ]]></postfilename>",
             (fh[i].accessed[1] & FILE_NOREPLY2) ? "<font color=\"red\"><u>x</u></font>" : 
             ((fh[i].accessed[0] & FILE_NOREPLY) ?
             "<font color = blue><u>x</u></font>" : "&nbsp;&nbsp;"),
             strncmp(fh[i].title, "Re: ", 4) ? "○ " : "",
             void1(nohtml2(fh[i].title)), eff_size(filename),
             fh[i].filename);
        printf("</post>\n");

    }
    printf("</posts>");

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
                char filename[80];

                printf("<toppost>");
                ptr = flag_str2(fh[i].accessed[0]);
                sprintf(filename, "boards/%s/%s/%s", board,
                        getfolderstr(fh[i].filename), fh[i].filename);
                switch (ptr[0])
                {
                case 'M':
                    printf("<flag>[提示]</flag>\n");
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

                printf
                    ("<title><![CDATA[ <a href=bbstopcon?board=%s&file=%s>&nbsp;&nbsp;%s%s </a>%s ]]></title>\n",
                     board, fh[i].filename, strncmp(fh[i].title, "Re: ",
                                                    4) ? "○ " : "",
                     void1(nohtml2(fh[i].title)), eff_size(filename));
                printf("</toppost>\n");

            }
        }
        unmap_board_dir();
        printf("</topposts>\n");
    }

    printf("</board>\n");

    unmap_board_dir();

    return 0;
}
