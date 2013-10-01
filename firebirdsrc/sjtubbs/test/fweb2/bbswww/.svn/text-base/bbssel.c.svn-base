#include "www.h"
#define MAXRESULT  50
int bbssel_main()
{
    char *board, buf[256], *board1, *title, *keywords;

    char *t;
    FILE *fp;

    int i, total = 0;
    int possibleBoards[MAXRESULT];

    cache_age = 60 * 60;
    init_all();
    board = trim(nohtml(getparm("board")));
    if (board[0] == 0)
    {
        printf("<font class=\"title\">%s - 选择讨论区</font><hr>\n",
               BBSNAME);
        printf("<form action=bbssel>\n");
        printf("讨论区名称: <input type=text name=board>");
        printf(" <input type=submit value=确定>");
        printf("</form>\n");
        printf("</html>");
        return 0;
    }
    else
    {
        for (i = 0; i < MAXBOARD; i++)
        {
            board1 = shm_bcache->bcache[i].filename;
            title = shm_bcache->bcache[i].title;

            if (shm_bcache->bcache[i].flag2[0] & BOARD_DIR_FLAG)
                continue;

            if (!has_read_perm(&currentuser, board1))
                continue;
            if (!strcasecmp(board, board1))
            {
                sprintf(buf, "bbsdoc?board=%s", board1);
                redirect(buf);
                http_quit();
            }
        }
        for (i = 0; i < MAXBOARD; i++)
        {
            board1 = shm_bcache->bcache[i].filename;
            title = shm_bcache->bcache[i].title;

            if (shm_bcache->bcache[i].flag2[0] & BOARD_DIR_FLAG)
                continue;

            if (!has_read_perm(&currentuser, board1))
                continue;
            if (strcasestr(board1, board) || strcasestr(title, board))
            {
                possibleBoards[total++] = i;
                if (total > MAXRESULT - 1)
                    break;
            }
            else
            {
                sprintf(buf, "vote/%s/keyword", shm_bcache->bcache[i].filename);
    	    	fp = fopen(buf, "r+");
    			if (fp != 0)
				{
					fgets(buf, 254, fp);
					fclose(fp);
					t = strchr(buf, '\n');
					if (t)
						*t = '\0';
					keywords = strtok(buf, ",: ;&()/\n");
					while (keywords)
					{
						if (!strcasecmp(keywords, board))
						{
							possibleBoards[total++] = i;
							break;
						}
						keywords = strtok(0, ",: ;&()/\n");
					}
					if (total > MAXRESULT - 1)
					{
						break;
					}
				}
			}
        }
        if (total == 1)
        {
            board1 = shm_bcache->bcache[possibleBoards[0]].filename;
            sprintf(buf, "bbsdoc?board=%s", board1);
            redirect(buf);
            http_quit();
        }
        printf("<font class=\"title\">%s - 选择讨论区</font><hr>\n",
               BBSNAME);
        printf("找不到讨论区'%s'<br/>", board);
        printf("标题或关键字中含有'%s'的讨论区有: <br><br>\n", board);
        printf("<table>");
        for (i = 0; i < total;)
        {
            board1 = shm_bcache->bcache[possibleBoards[i]].filename;
            title = shm_bcache->bcache[possibleBoards[i]].title;
            printf("<tr><td>%d", ++i);
            printf("<td><a href=bbsdoc?board=%s>%s</a><td>%s<br>\n",
                   board1, board1, title + 7);
        }
        printf("</table><br>\n");
        printf("共找到%d个符合条件的讨论区.\n", total);
        printf("</html>");
        return 0;
    }
}
