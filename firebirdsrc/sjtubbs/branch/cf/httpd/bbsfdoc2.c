#include "www.h"

int bbsfdoc2_main()
{
    FILE *fp;
    char *ptr, board[30],dirbuf[256];
	int dirsize,n_read,i,start;
    struct dir x[20];
	
	cache_age = 60*60;
    init_all();
    strsncpy(board, getparm("board"), 30);
    if (!has_read_perm(&currentuser, board))
        http_fatal("�����������");
    ptr = getparm("start");
	sprintf(dirbuf,UPLOAD_PATH2"/%s",board);
	dirsize= file_size(dirbuf) / sizeof(struct dir);
	if(0 == dirsize)
		http_fatal("������Ϊ��");
	
	if (ptr[0] == 0)
    {
        start = dirsize-20;
    }
    else
    {
        start = atoi(ptr);
		if(start >= dirsize)
			start = dirsize-20;
    }
	if(start<0)
		start=0;

		
    fp = fopen(dirbuf, "r");
    if (fp == 0)
        http_fatal("û���ҵ��ļ�");
    fseek(fp,start*sizeof(struct dir),SEEK_SET);
	n_read = fread(x,sizeof(struct dir),20,fp);
	
    printf("<center> %s -- ������������ [������: %s]<hr color=green>\n",
           BBSNAME, board);
    printf
    ("<table width=800 border=1><tr><td>���<td>�ļ���<td>������<td>ʱ��<td>��С<td>˵��<td>��ע<td>����\n");
    for (i = 0; i < n_read ; i++)
    {
        printf("<tr><td>%d", start+1+i);
        printf("<td><a href=http://"BBSHOST":"UPLOAD_SERVER_PORT"/%s/%s",x[i].board,x[i].showname);
        printf(" target=_blank>%s </a>",x[i].showname);
        printf("<td>%s", userid_str(x[i].userid));
        printf("<td>%6.6s", Ctime(x[i].date) + 4);
        printf("<td>%d", x[i].size);
        printf("<td><a href=bbsfexp2?fid=%d&board=%s",start+i,x[i].board);
        printf(">more </a>\n");
        printf("<td width='33%'>%s ",nohtml(x[i].exp));
        printf("<td><a onclick='return confirm(\"ȷ��?\")' href=bbsudel2?fid=%d&file=%s&board=%s",start+i,x[i].showname,x[i].board);
		printf(">ɾ��</a>\n");
    }
    printf("</table><br>\n");
    printf
    ("[<a href=bbsfdoc2?board=%s&start=%d>ˢ��</a>] [<a href=bbsdoc?board=%s>��������</a>] [<a href=bbsupload?board=%s>�����ļ�</a>]",
     board,start, board, board);
    printf("[<a href=bbsfdoc2?board=%s&start=0>��ҳ</a>] ", board);
    if ( 0 != start)
        printf("[<a href=bbsfdoc2?board=%s&start=%d>��һҳ</a>] ", board,
               ((start-20)<0)?0:(start-20));
    if (start+20 < dirsize )
        printf("[<a href=bbsfdoc2?board=%s&start=%d>��һҳ</a>] ", board,
               start+20);
    printf("[<a href=bbsfdoc2?board=%s>ĩҳ</a>] ", board);
	printf("[<a href=bbsfddoc?board=%s>����������</a>] ",board);
    printf
    ("<form name=form1 action=bbsfdoc2?board=%s method=post onsubmit='this.start.value-=1' >\n",
     board);
    printf
    ("<input type=submit value=��ת��> �� <input type=text name=start size=4 onkeydown=\"return true;\" >�� ");
    printf("</form>\n");
}
