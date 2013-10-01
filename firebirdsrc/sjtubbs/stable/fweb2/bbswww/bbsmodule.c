#include "www.h"

static int readmodulename(struct module_index module[3], char *board)
{
    FILE *fp;
    int number = 0;
    char buf[STRLEN];
    int i = 0;

    sprintf(buf, "vote/%s/%s", board, "modulename");
    fp = fopen(buf, "r");
    while (fread(&module[number], sizeof(struct module_index), 1, fp) != 0)
    {
        i = strlen(module[number].filename);
        module[number].filename[i - 2] = '\0';
        number++;
    }
    fclose(fp);
    return number;
}

int bbsmodule_main()
{
    char board[80];
    struct module_index module[3];
    int i = 0;
    int num = 0;
    char check[] = "a";

    init_all();
    strsncpy(board, getparm("board"), 80);
    num = readmodulename(module, board);
    printf
        ("<center><font class=\"title\">%s - 使用模板 %s讨论区　　</font>[使用者: %s] <hr></center>\n",
         BBSNAME, board, currentuser.userid);
    printf("<table>\n");
    printf("<form method=get action='bbspst'>\n");
    printf("<input type=hidden name=board value='%s'>", board);
    printf("<input type=hidden name=file value='%s'>", getparm("file"));
    printf("<input type=hidden name=check value='%s'>", check);
    printf("本板模板如下所示:<br>\n");
    while (i < num)
    {
        printf("\n模板: %s", module[i].filename);
        printf("<input type=radio name=level value=%s checked>选定 <br>",
               module[i].filename);
        i++;
    }
    printf("</table>\n");
    printf("<hr><br>\n");
    printf("<input type=submit value='确定'> \n");
    printf
        ("<input type=button onclick='javascript:history.go(-1)' value='取消并返回'> \n");
    printf("</form>");
    printf("</html>");
    return 0;
}
