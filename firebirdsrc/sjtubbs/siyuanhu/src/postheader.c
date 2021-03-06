/*
$Id: postheader.c,v 1.2 2004/06/29 14:59:21 rwzmm Exp $
*/

#include "bbs.h"

extern int numofsig;
extern int noreply;
extern int mailtoauther;

struct shortfile *getbcache ();

void
check_title (char *title)
{
    if (killwordsp (title) == 0)
        return;
    if (strncmp (title, "Re: ", 4) != 0)
        return;
    title[0] = 'r';
}

int
post_header (header)
struct postheader *header;
{
    int anonyboard = 0;
	int positiveanonyboard = 0;
	int model=0;
    char r_prompt[20], mybuf[256], ans[5];
    char titlebuf[STRLEN];
	char fname[80];
    struct shortfile *bp;

#ifdef RNDSIGN

    int oldset, rnd_sign = 0;
#endif

    if (currentuser.signature > numofsig || currentuser.signature < 0)
        currentuser.signature = 1;
	/*add by pax 判断是否存在模版文件*/
	setbfile (fname, currboard, "board.model");
	if(dashf(fname))
	{
		model=1;
	}
	/*end by pax*/
#ifdef RNDSIGN

    if (numofsig > 0)
    {
        if (DEFINE (DEF_RANDSIGN))
        {
            oldset = currentuser.signature;
            srand ((unsigned) time (0));
            currentuser.signature = (rand () % numofsig) + 1;
            rnd_sign = 1;
        }
        else
        {
            rnd_sign = 0;
        }
    }
#endif

    if (header->reply_mode)
    {
        strcpy (titlebuf, header->title);
        header->include_mode = 'S';
    }
    else
        titlebuf[0] = '\0';
    bp = getbcache (currboard);
    if (header->postboard)      //modified by littlesword@sjtubbs
	{
		positiveanonyboard = bp->flag & POSITIVE_ANONY_FLAG;
        anonyboard = bp->flag & ANONY_FLAG || positiveanonyboard;
	}
    header->chk_anony = ((anonyboard) ? 1 : 0)  &&  ((positiveanonyboard) ? 0 : 1);
    while (1)
    {
        if (header->reply_mode)
            sprintf (r_prompt, "引言模式 [[1m%c[m]", header->include_mode);
        move (t_lines - 4, 0);
        clrtobot ();
        prints ("[m%s [1m%s[m      %s    %s    %s%s\n",
                (header->postboard) ? "发表文章于" : "收信人：", header->ds,
                (anonyboard) ? (header->chk_anony ==
                                1 ? "[1m要[m使用匿名" :
                                "\033[1m不[m使用匿名") : "",
               (model>0) ? (model == 2 ? "[1m要[m使用模版" :
                                "\033[1m不[m使用模版") : "",	

                        (header->
                         postboard) ? ((noreply) ? "[本文[1;33m不可以[m回复" :
                                       "[本文[1;33m可以[m回复") : "",
                        (header->postboard
                         && header->
                         reply_mode) ? ((mailtoauther) ?
                                        ",且[1;33m发送[m本文至作者信箱]" :
                                        ",且[1;33m不发送[m本文至作者信箱]")
                        : (header->postboard) ? "]" : "");
        prints ("使用标题: [1m%-50s[m\n",
                (header->title[0] == '\0') ? "[正在设定主题]" : header->title);
#ifdef RNDSIGN

        prints ("使用第 [1m%d[m 个签名档     %s %s",
                currentuser.signature, (header->reply_mode) ? r_prompt : "",
                (rnd_sign == 1) ? "[随机签名档]" : "");
#else

        prints ("使用第 [1m%d[m 个签名档     %s", currentuser.signature,
                (header->reply_mode) ? r_prompt : "");
#endif

        if (titlebuf[0] == '\0')
{
            move (t_lines - 1, 0);
            if (header->postboard == YEA || strcmp (header->title, "没主题"))
                strcpy (titlebuf, header->title);
            getdata (t_lines - 1, 0, "标题: ", titlebuf, 50, DOECHO, NA);
            check_title (titlebuf);
            if (titlebuf[0] == '\0')
            {
                if (header->title[0] != '\0')
                {
                    titlebuf[0] = ' ';
                    continue;
                }
                else
                    return NA;
            }
            strcpy (header->title, titlebuf);
            continue;
        }
        move (t_lines - 1, 0);
        sprintf (mybuf,
                 "[1;32m0[m~[1;32m%d V[m看签名档%s [1;32mX[m随机签名档,[1;32mT[m标题%s%s%s%s,[1;32mQ[m放弃:",
                 numofsig,
                 (header->
                  reply_mode) ?
                 ",[1;32mS[m/[1;32mY[m/[1;32mN[m/[1;32mR[m/[1;32mA[m 引言模式"
                 : "", (anonyboard) ? "，[1;32mL[m匿名" : "",
				 (model>0) ? ",[1;32mO[m模版" : "",
                 (header->postboard) ? ",[1;32mU[m属性" : "",
                 (header->postboard
                  && header->reply_mode) ? ",[1;32mM[m寄信" : "");
        getdata (t_lines - 1, 0, mybuf, ans, 3, DOECHO, YEA);
        ans[0] = toupper (ans[0]);
        if ((ans[0] - '0') >= 0 && ans[0] - '0' <= 9)
        {
            if (atoi (ans) <= numofsig)
                currentuser.signature = atoi (ans);
        }
        else if (ans[0] == 'Q' || ans[0] == 'q')
        {
            return -1;
        }
        else if (header->reply_mode &&
                 (ans[0] == 'Y' || ans[0] == 'N' || ans[0] == 'A'
                  || ans[0] == 'R' || ans[0] == 'S'))
        {
            header->include_mode = ans[0];
        }
        else if (ans[0] == 'T')
        {
            titlebuf[0] = '\0';
        }
        else if (ans[0] == 'L' && anonyboard)
        {
            header->chk_anony = (header->chk_anony == 1) ? 0 : 1;
        }
		else if (ans[0] == 'O' && (model>0))
        {
			model=(model==1)?2:1;
        }
        else if (ans[0] == 'U' && header->postboard)
        {
            noreply = ~noreply;
        }
        else if (ans[0] == 'M' && header->postboard && header->reply_mode)
        {
            mailtoauther = ~mailtoauther;
        }
        else if (ans[0] == 'V')
        {
            setuserfile (mybuf, "signatures");
            if (askyn ("预设显示前三个签名档, 要显示全部吗", NA, YEA) == YEA)
                ansimore (mybuf);
            else
            {
                clear ();
                ansimore2 (mybuf, NA, 0, 18);
            }
#ifdef RNDSIGN

        }
        else if (ans[0] == 'X')
        {
            if (rnd_sign == 0 && numofsig != 0)
            {
                oldset = currentuser.signature;
                srand ((unsigned) time (0));
                currentuser.signature = (rand () % numofsig) + 1;
                rnd_sign = 1;
            }
            else if (rnd_sign == 1 && numofsig != 0)
            {
                rnd_sign = 0;
                currentuser.signature = oldset;
            }
            ans[0] = ' ';
#endif

        }
        else
        {
            if (header->title[0] == '\0')
                return NA;
            else if(model>0)
                return model;
			else
				return YEA;
        }
    }
}

int
check_anonyboard (currboard)    //?????
{
    struct shortfile *bp;
    int anonyboard = 0;

    bp = getbcache (currboard);
    anonyboard = bp->flag & ANONY_FLAG;
    if (anonyboard == 8)
    {
        return YEA;
        //this is because anony_flag = 8 = 100 in binary
    }
    else
    {
        return NA;
    }

}
