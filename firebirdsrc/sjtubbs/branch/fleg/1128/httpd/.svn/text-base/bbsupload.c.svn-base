#include "www.h"

int bbsupload_main()
{
    char board[80];

    init_all();
    strsncpy(board, getparm("board"), 30);
    if (!loginok)
        http_fatal("匆匆过客无法上传附件");
    if (!has_read_perm(&currentuser, board))
        http_fatal("错误的讨论区");
    if (!has_post_perm(&currentuser, board))
        http_fatal("无权上载附件至该板");
#ifdef	POST_AFTER_ONE_DAY
	if(too_young(&currentuser, board))
		http_fatal("您注册时间还不满24小时，暂时不能在本板上传附件.如果有疑问请在 BBSHelp 或者 sysop 板发文咨询。");
#endif	
    strsncpy(board, getparm("board"), 80);
    printf
    ("<center><font class=\"title\">%s - 上传附件至%s讨论区　　</font>[使用者: %s] <hr>\n",
     BBSNAME, board, currentuser.userid);
    printf
    ("注意，bbs服务器资源宝贵，为节省空间，请勿上传过大的文件。请勿上传与板面无关的文件。<br>\n");
    printf("目前单个上载文件大小限制为2M字节. <br>\n");
    printf
    ("请遵守国家法律，<font color=red>严禁上载非法资料和可能导致纠纷的资料</font>。<br>\n");
    printf
    ("<form method=post action=bbsdoupload enctype='multipart/form-data'>\n");
    printf("<table>\n");
    printf("<tr><td>上载附件: <td><input type=file name=up>");
    printf("<input type=hidden name=MAX_FILE_SIZE value=5000000>");
    printf("<input type=hidden name=board value='%s'>", board);
    printf("<tr><td>个人推荐评价: <td>");
    printf("<input type=radio name=level value=0 checked>1星 ");
    printf("<input type=radio name=level value=1>2星 ");
    printf("<input type=radio name=level value=2>3星 ");
    printf("<input type=radio name=level value=3>4星 ");
    printf("<input type=radio name=level value=4>5星 ");
    printf
    ("<tr><td>希望保留的时间: <td><input name=live size=4 maxlength=4 value=180>(1-9999)天<br>\n");
    printf
    ("<tr><td>附件的简要说明: <td><input name=exp size=60 maxlength=72>\n");
    printf("</table>\n");
    printf("<hr><br>\n");
    printf("<input type=submit value='开始上传文件'> \n");
    printf
    ("<input type=button onclick='javascript:history.go(-1)' value='返回上一页'> \n");
    printf("</form>");
}
