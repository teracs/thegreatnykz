#include "www.h"

int bbsinboardlist_main()
{

	char inboard_file[STRLEN], buf[STRLEN], board[STRLEN];
	FILE *fp;
	char *namep;

	silence = 1;
    init_all();
	strsncpy(board, getparm("board"), 32);
	if (!has_read_perm(&currentuser, board))
	{
        cgi_head();
        html_head();
        http_quit_print("´íÎóµÄÌÖÂÛÇø");
    }
    cgi_head();
    html_head();

	sprintf(inboard_file, "boards/%s/%s", board, INBOARD_FILE);
	if ((fp = fopen(inboard_file, "r")) == NULL)
	{
		printf("</html>");
		return 0;
	}
	printf("<ul class=\"inline\">");
	while (fgets(buf, STRLEN, fp) != NULL)
	{
		namep = (char *) strtok(buf, "\n");
		printf("<li><a href=\"/bbsqry?userid=%s\"target=\"f3\">%s </a></li>", namep, namep);
	}
    printf("</ul></html>");
    return 0;
}
