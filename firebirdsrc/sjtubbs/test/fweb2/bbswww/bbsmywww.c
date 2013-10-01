#include "www.h"
static int
save_set(int t_lines, int link_mode, int def_mode, int font_mode,
         int fontn_mode, int ansisize_mode, int ansifont_mode,
         int theme_mode)
{
    FILE *fp;
    char path[80];

    sprintf(path, "home/%c/%c/%s/.mywww", toupper(currentuser.userid[0]),
            toupper(currentuser.userid[1]), currentuser.userid);
    if (t_lines < 10 || t_lines > 40)
        http_quit_print("错误的行数");
    if (link_mode < 0 || link_mode > 1)
        http_quit_print("错误的链接识别参数");
    if (def_mode < 0 || def_mode > 1)
        http_quit_print("错误的缺省模式");
    if (font_mode < 0 || font_mode > 1)
        http_quit_print("错误的字体类型");
    if (ansisize_mode < 0 || ansisize_mode > 1)
        http_quit_print("错误的字体类型");
    if (theme_mode < 1 || theme_mode > 2)
        http_quit_print("错误的Web外观主题");
    fp = fopen(path, "w");
    fprintf(fp, "t_lines %d\n", t_lines);
    fprintf(fp, "link_mode %d\n", link_mode);
    fprintf(fp, "def_mode %d\n", def_mode);
    fprintf(fp, "font_mode %d\n", font_mode);
    fprintf(fp, "fontn_mode %d\n", fontn_mode);
    fprintf(fp, "ansisize_mode %d\n", ansisize_mode);
    fprintf(fp, "ansifont_mode %d\n", ansifont_mode);
    fprintf(fp, "theme_mode %d\n", theme_mode);
    fclose(fp);
    printf("WWW定制参数设定成功.<br>\n");
    printf("[<a href='javascript:history.go(-2)'>返回</a>]");
    return 0;
}

int bbsmywww_main()
{
    int t_lines = 20, link_mode = 0, def_mode = 0, font_mode = 0, type;
    int fontn_mode = 1, ansisize_mode = 1, ansifont_mode = 0, theme_mode =
        1;

    init_all();
    if (!loginok)
        http_quit_print("匆匆过客不能定制界面");
    printf
        ("<center><font class=\"title\">%s - WWW个人定制　　</font>[使用者: %s]<hr>",
         BBSNAME, currentuser.userid);
    type = atoi(getparm("type"));
    t_lines = atoi(getparm("t_lines"));
    link_mode = atoi(getparm("link_mode"));
    def_mode = atoi(getparm("def_mode"));
    font_mode = atoi(getparm("font_mode"));
    fontn_mode = atoi(getparm("fontn_mode"));
    ansisize_mode = atoi(getparm("ansisize_mode"));
    ansifont_mode = atoi(getparm("ansifont_mode"));
    theme_mode = atoi(getparm("theme_mode"));
    if (type > 0)
    {
        save_set(t_lines, link_mode, def_mode, font_mode, fontn_mode,
                 ansisize_mode, ansifont_mode, theme_mode);
        http_quit();
    }
    printf("<form action=bbsmywww>\n");
    printf("<table>\n");
    printf("<input type=hidden name=type value=1>");
    printf("<tr><td>一屏显示的行数: &nbsp;");
    printf("<td><input name=t_lines size=8 value=%d> (10-40)\n",
           my_t_lines);
    printf("<tr><td>链接识别: ");
    printf("<td><input type=radio name=link_mode value=0 %s>识别 ",
           my_link_mode == 0 ? "checked" : "");
    printf("<input type=radio name=link_mode value=1 %s>不识别\n",
           my_link_mode == 1 ? "checked" : "");
    printf("<tr><td>缺省模式: ");
    printf("<td><input type=radio name=def_mode value=0 %s>一般 ",
           my_def_mode == 0 ? "checked" : "");
    printf("<input type=radio name=def_mode value=1 %s>主题\n",
           my_def_mode == 1 ? "checked" : "");
    printf("<tr><td>Web 环境字体大小: ");
    printf("<td><input type=radio name=font_mode value=0 %s>小字 ",
           my_font_mode == 0 ? "checked" : "");
    printf("<input type=radio name=font_mode value=1 %s>大字\n",
           my_font_mode == 1 ? "checked" : "");
    printf("<tr><td>Web 环境使用字体: ");
    printf("<td><input type=radio name=fontn_mode value=0 %s>宋体 ",
           my_fontn_mode == 0 ? "checked" : "");
    printf("<input type=radio name=fontn_mode value=1 %s>Verdana\n",
           my_fontn_mode == 1 ? "checked" : "");
    printf("<tr><td>浏览文章文字大小: ");
    printf("<td><input type=radio name=ansisize_mode value=0 %s>小字 ",
           my_ansisize_mode == 0 ? "checked" : "");
    printf("<input type=radio name=ansisize_mode value=1 %s>大字\n",
           my_ansisize_mode == 1 ? "checked" : "");
    printf("<tr><td>浏览文章使用字体: ");
    printf("<td><input type=radio name=ansifont_mode value=0 %s>宋体 ",
           my_ansifont_mode == 0 ? "checked" : "");
    printf("<input type=radio name=ansifont_mode value=1 %s>Verdana\n",
           my_ansifont_mode == 1 ? "checked" : "");
    printf("<tr><td>Web 外观主题: ");
    printf("<td><input type=radio name=theme_mode value=1 %s>宝石蓝 ",
           my_theme_mode == 1 ? "checked" : "");
    printf("<input type=radio name=theme_mode value=2 %s>玛瑙红\n",
           my_theme_mode == 2 ? "checked" : "");
    printf("</table><hr>\n");
    printf
        ("<input type=submit value=确定> <input type=reset value=复原>\n");
    printf
        ("<p> 注意：将“浏览文章使用字体”设为 Verdana 可能会造成阅读的文章板面错乱。\n");
    printf("</form>\n");
    printf("</html>");
    return 0;
}
