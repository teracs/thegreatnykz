/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu
    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@bbs.cs.ccu.edu.tw
                        Peng Piaw Foong, ppfoong@csie.ncu.edu.tw
    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@bbs.cs.ccu.edu.tw
                        Peng Piaw Foong, ppfoong@csie.ncu.edu.tw 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/

#include "bbs.h"
#include "vote.h"

extern int page, range;
extern struct shortfile *bcache;
extern struct BCACHE *brdshm;
static char *vote_type[] = { "ÊÇ·Ç", "µ¥Ñ¡", "¸´Ñ¡", "Êı×Ö", "ÎÊ´ğ", "ÌØÊâ" };
struct votebal currvote;
char controlfile[STRLEN];
unsigned int result[33];
int vnum;
int voted_flag;
FILE *sug;
int makevote(struct votebal *ball, char *bname);
int setvoteperm(struct votebal *ball);

int cmpvuid(char *userid, void *vuv)
{
    struct ballot *uv = (struct ballot *) vuv;

    return !strcmp(userid, uv->uid);
}

int setvoteflag(char *bname, int flag)
{
    int pos;
    struct boardheader fh;

    pos = search_record(BOARDS, &fh, sizeof(fh), cmpbnames, bname);
    if (((fh.flag & VOTE_FLAG) && (flag != 0))
        || (((fh.flag & VOTE_FLAG) == 0) && (flag == 0)))
        return 0;
    if (flag == 0)
        fh.flag = fh.flag & ~VOTE_FLAG;
    else
        fh.flag = fh.flag | VOTE_FLAG;
    if (substitute_record(BOARDS, &fh, sizeof(fh), pos) == -1)
        prints("Error updating BOARDS file...\n");
    return 1;
}

void b_report(char *str)
{
    char buf[STRLEN];

    sprintf(buf, "%s %s", currboard, str);
    report(buf);
}

void makevdir(char *bname)
{
    struct stat st;
    char buf[STRLEN];

    sprintf(buf, "vote/%s", bname);
    if (stat(buf, &st) != 0)
        mkdir(buf, 0755);
}

void setvfile(char *buf, char *bname, char *filename)
{
    sprintf(buf, "vote/%s/%s", bname, filename);
}

void setcontrolfile(void)
{
    setvfile(controlfile, currboard, "control");
}

int b_notes_edit(void)
{
    char buf[STRLEN], buf2[STRLEN];
    char ans[4];
    int aborted, notetype;

    if (!chk_currBM(currBM))
        return 0;
    clear();
    move(1, 0);
    prints("±à¼­/É¾³ı±¾°å±¸ÍüÂ¼");
    getdata(3, 0, "±à¼­»òÉ¾³ı (0)Àë¿ª (1)Ò»°ã±¸ÍüÂ¼ (2)ÃØÃÜ±¸ÍüÂ¼ (3)°åÃæ¹Ø¼ü×Ö? [1] ",
            ans, 2, DOECHO, YEA);
    if (ans[0] == '0')
        return FULLUPDATE;
    if(ans[0] == '3')
	{
        setvfile(buf, currboard, "keyword");
        notetype = 3;
	}
    else if (ans[0] != '2')
    {
        setvfile(buf, currboard, "notes");
        notetype = 1;
    }
    else
    {
        setvfile(buf, currboard, "secnotes");
        notetype = 2;
    }
    sprintf(buf2, "[%s] (E)±à¼­ (D)É¾³ı (A)È¡Ïû [E]: ",
            (notetype == 1) ? "Ò»°ã±¸ÍüÂ¼" : (notetype == 2 ? "ÃØÃÜ±¸ÍüÂ¼" : "°åÃæ¹Ø¼ü×Ö"));
    getdata(5, 0, buf2, ans, 2, DOECHO, YEA);
    if (ans[0] == 'A' || ans[0] == 'a')
    {
        aborted = -1;
    }
    else if (ans[0] == 'D' || ans[0] == 'd')
    {
        move(6, 0);
        sprintf(buf2, "ÕæµÄÒªÉ¾³ı[%s]",
                (notetype == 1) ? "Ò»°ã±¸ÍüÂ¼" : (notetype == 2 ? "ÃØÃÜ±¸ÍüÂ¼" : "°åÃæ¹Ø¼ü×Ö"));
        if (askyn(buf2, NA, NA) == YEA)
        {
            move(7, 0);
            unlink(buf);
            prints("[%s]ÒÑ¾­É¾³ı...\n",
                   (notetype == 1) ? "Ò»°ã±¸ÍüÂ¼" : (notetype == 2 ? "ÃØÃÜ±¸ÍüÂ¼" : "°åÃæ¹Ø¼ü×Ö"));
            pressanykey();
            aborted = 1;

            log_bm_ops(currboard, "°åÃæ", "É¾³ı",
                       (notetype == 1) ? "Ò»°ã±¸ÍüÂ¼" : (notetype == 2 ? "ÃØÃÜ±¸ÍüÂ¼" : "°åÃæ¹Ø¼ü×Ö"));
        }
        else
            aborted = -1;
    }
    else
    {
        aborted = vedit(buf, NA, YEA);
        if (aborted != -1)
            log_bm_ops(currboard, "°åÃæ", "ĞŞ¸Ä",
                       (notetype == 1) ? "Ò»°ã±¸ÍüÂ¼" : (notetype == 2 ? "ÃØÃÜ±¸ÍüÂ¼" : "°åÃæ¹Ø¼ü×Ö"));
    }
    if (aborted == -1)
    {
        pressreturn();
    }
    else
    {
        if (notetype == 1)
        {
            setvfile(buf, currboard, "noterec");
			unlink(buf);
        }
        else if (notetype == 2)
        {
            setvfile(buf, currboard, "notespasswd");
            unlink(buf);
        }
    }
    return FULLUPDATE;
}

int b_notes_passwd(void)
{
    FILE *pass;
    char passbuf[20], prepass[20];
    char buf[STRLEN];

    if (!chk_currBM(currBM))
    {
        return 0;
    }
    clear();
    move(1, 0);
    prints("Éè¶¨/¸ü¸Ä/È¡Ïû¡¸ÃØÃÜ±¸ÍüÂ¼¡¹ÃÜÂë...");
    setvfile(buf, currboard, "secnotes");
    if (!dashf(buf))
    {
        move(3, 0);
        prints("±¾ÌÖÂÛÇøÉĞÎŞ¡¸ÃØÃÜ±¸ÍüÂ¼¡¹¡£\n\n");
        prints("ÇëÏÈÓÃ W ±àºÃ¡¸ÃØÃÜ±¸ÍüÂ¼¡¹ÔÙÀ´Éè¶¨ÃÜÂë...");
        pressanykey();
        return FULLUPDATE;
    }
    if (!check_notespasswd())
        return FULLUPDATE;
    getdata(3, 0, "ÇëÊäÈëĞÂµÄÃØÃÜ±¸ÍüÂ¼ÃÜÂë(Enter È¡ÏûÃÜÂë): ", passbuf,
            19, NOECHO, YEA);
    if (passbuf[0] == '\0')
    {
        setvfile(buf, currboard, "notespasswd");
        unlink(buf);
        prints("ÒÑ¾­È¡Ïû±¸ÍüÂ¼ÃÜÂë¡£");
        pressanykey();
        log_bm_ops(currboard, "°åÃæ", "", "È¡ÏûÃØÃÜ±¸ÍüÂ¼ÃÜÂë");
        return FULLUPDATE;
    }
    getdata(4, 0, "È·ÈÏĞÂµÄÃØÃÜ±¸ÍüÂ¼ÃÜÂë: ", prepass, 19, NOECHO, YEA);
    if (strcmp(passbuf, prepass))
    {
        prints("\nÃÜÂë²»Ïà·û, ÎŞ·¨Éè¶¨»ò¸ü¸Ä....");
        pressanykey();
        return FULLUPDATE;
    }
    setvfile(buf, currboard, "notespasswd");
    if ((pass = fopen(buf, "w")) == NULL)
    {
        move(5, 0);
        prints("±¸ÍüÂ¼ÃÜÂëÎŞ·¨Éè¶¨....");
        pressanykey();
        return FULLUPDATE;
    }
    fprintf(pass, "%s\n", genpasswd(passbuf));
    fclose(pass);
    move(5, 0);
    prints("ÃØÃÜ±¸ÍüÂ¼ÃÜÂëÉè¶¨Íê³É....");
    pressanykey();
    log_bm_ops(currboard, "°åÃæ", "", "Éè¶¨ÃØÃÜ±¸ÍüÂ¼ÃÜÂë");
    return FULLUPDATE;
}

int b_suckinfile(FILE * fp, char *fname)
{
    char inbuf[256];
    FILE *sfp;

    if ((sfp = fopen(fname, "r")) == NULL)
        return -1;
    while (fgets(inbuf, sizeof(inbuf), sfp) != NULL)
        fputs(inbuf, fp);
    fclose(sfp);
    return 0;
}

int catnotepad(FILE * fp, char *fname)
{
    char inbuf[256];
    FILE *sfp;
    int count;

    count = 0;
    if ((sfp = fopen(fname, "r")) == NULL)
    {
        fprintf(fp,
                "[1;34m  ¡õ[44m__________________________________________________________________________[m \n\n");
        return -1;
    }
    while (fgets(inbuf, sizeof(inbuf), sfp) != NULL)
    {
        if (count != 0)
            fputs(inbuf, fp);
        else
            count++;
    }
    fclose(sfp);
    return 0;
}

int b_closepolls(void)
{
    char buf[80];
    time_t now, nextpoll;
    int i, end;

    now = time(0);
    resolve_boards();

    if (now < brdshm->pollvote)
    {
        return 1;
    }
    move(t_lines - 1, 0);
    prints("¶Ô²»Æğ£¬ÏµÍ³¹Ø±ÕÍ¶Æ±ÖĞ£¬ÇëÉÔºò...");
    refresh();
    nextpoll = now + 7 * 3600;
    brdshm->pollvote = nextpoll;
    strcpy(buf, currboard);
    for (i = 0; i < brdshm->number; i++)
    {
        strcpy(currboard, (&bcache[i])->filename);
        setcontrolfile();
        end = get_num_records(controlfile, sizeof(currvote));
        for (vnum = end; vnum >= 1; vnum--)
        {
            time_t closetime;

            get_record(controlfile, &currvote, sizeof(currvote), vnum);
            closetime = currvote.opendate + currvote.maxdays * 86400;
            if (now > closetime)
                mk_result(vnum);
            else if (nextpoll > closetime)
            {
                nextpoll = closetime + 300;
                brdshm->pollvote = nextpoll;
            }
        }
    }
    strcpy(currboard, buf);

    return 0;
}

int count_result(void *vptr)
{
    struct ballot *ptr = (struct ballot *) vptr;
    int i;

    if (ptr->msg[0][0] != '\0')
    {
        if (currvote.type == VOTE_ASKING)
        {
            fprintf(sug, "[1m%s [mµÄ×÷´ğÈçÏÂ£º\n", ptr->uid);
        }
        else
            fprintf(sug, "[1m%s [mµÄ½¨ÒéÈçÏÂ£º\n", ptr->uid);
        for (i = 0; i < 3; i++)
            fprintf(sug, "%s\n", ptr->msg[i]);
    }
    result[32]++;
    if (currvote.type == VOTE_ASKING)
    {
        return 0;
    }
    if (currvote.type != VOTE_VALUE)
    {
        for (i = 0; i < 32; i++)
        {
            if ((ptr->voted >> i) & 1)
                (result[i])++;
        }

    }
    else
    {
        result[31] += ptr->voted;
        result[(ptr->voted * 10) / (currvote.maxtkt + 1)]++;
    }
    return 0;
}

void get_result_title(void)
{
    char buf[STRLEN];

    getdatestring(currvote.opendate, NA);
    fprintf(sug, "¡Ñ Í¶Æ±¿ªÆôÓÚ£º[1m%s[m  Àà±ğ£º[1m%s[m\n",
            datestring, vote_type[currvote.type - 1]);
    fprintf(sug, "¡Ñ Ö÷Ìâ£º[1m%s[m\n", currvote.title);
    if (currvote.type == VOTE_VALUE)
        fprintf(sug, "¡Ñ ´Ë´ÎÍ¶Æ±µÄÖµ²»¿É³¬¹ı£º[1m%d[m\n\n",
                currvote.maxtkt);
    fprintf(sug, "¡Ñ Æ±Ñ¡ÌâÄ¿ÃèÊö£º\n\n");
    sprintf(buf, "vote/%s/desc.%ld", currboard, currvote.opendate);
    b_suckinfile(sug, buf);
}

int HiddenBoard(char *name)
{
    struct shortfile *bptr = NULL;

    int i = 0;

    for (i = 0; i < MAXBOARD; i++)
    {
        if (!strcmp(bcache[i].filename, name))
            break;
    }
    if (i == MAXBOARD)
        return 0;
    bptr = &bcache[i];
    if ((bptr->level != 0) && (!(bptr->level & PERM_POSTMASK)))
        return 1;
    else
        return 0;
}

int getVoterIP(char *file, struct voter_info v_info[MAXUSERS])
{
    int nums = 0;
    int fd;

    if ((fd = open(file, O_RDONLY, 0)) == -1)
        return 0;
    while (read(fd, &v_info[nums], sizeof(v_info[nums])) ==
           sizeof(v_info[nums]))
    {
        nums++;
    }
    return nums;
}

int writeVoterIP(char *file, char *board,
                 struct voter_info v_info[MAXUSERS], int nums)
{
    FILE *fd;
    int i;

    if ((fd = fopen(file, "w")) == NULL)
        return 0;
    fprintf(fd, "[¹«¸æ] %s °åµÄÍ¶Æ±²ÎÓëÇé¿ö\n\n", board);
    fprintf(fd, "¡¾´ËÆªÎÄÕÂÊÇÓÉ×Ô¶¯·¢ĞÅÏµÍ³ËùÕÅÌù¡¿\n\n");
    fprintf(fd,
            "		  ID    		   IP				        Í¶Æ±Ê±¼ä\n");
    for (i = 0; i < nums; i++)
    {
        fprintf(fd, "%12s  ", v_info[i].uid);
        fprintf(fd, "%15s  ", v_info[i].uip);
        getdatestring(v_info[i].vote_time, 0);
        fprintf(fd, "%s\n", datestring);
    }
    fclose(fd);
    return 0;
}

int compare_ip(struct voter_info a, struct voter_info b)
{
    return strcmp(a.uip, b.uip);
}

int mk_result(int num)
{
    char fname[STRLEN], nname[STRLEN];
    char sugname[STRLEN];
    char title[STRLEN];
    char title2[STRLEN];
    char ipname[STRLEN];
    struct voter_info vinfo[MAXUSERS];
    int votenums;
    char ipname2[STRLEN];
    int i;
    unsigned int total = 0;

    setcontrolfile();
    sprintf(fname, "vote/%s/flag.%ld", currboard, currvote.opendate);
    sprintf(ipname, "vote/%s/uip.%ld", currboard, currvote.opendate);
    sprintf(ipname2, "vote/%s/uip2.%ld", currboard, currvote.opendate);

    sug = NULL;
    sprintf(sugname, "vote/%s/tmp.%d", currboard, uinfo.pid);
    if ((sug = fopen(sugname, "w")) == NULL)
    {
        report("open vote tmp file error");
        prints("Error: ½áÊøÍ¶Æ±´íÎó...\n");
        pressanykey();
    }
    (void) memset(result, 0, sizeof(result));
    if (apply_record(fname, count_result, sizeof(struct ballot)) == -1)
    {
        report("Vote apply flag error");
    }
    fprintf(sug,
            "[1;44;36m¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª©ÈÊ¹ÓÃÕß%s©À¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª[m\n\n\n",
            (currvote.type != VOTE_ASKING) ? "½¨Òé»òÒâ¼û" : "´Ë´ÎµÄ×÷´ğ");
    fclose(sug);
    sprintf(nname, "vote/%s/results", currboard);
    if ((sug = fopen(nname, "w")) == NULL)
    {
        report("open vote newresult file error");
        prints("Error: ½áÊøÍ¶Æ±´íÎó...\n");
    }
    get_result_title();

    fprintf(sug, "** Í¶Æ±½á¹û:\n\n");
    if (currvote.type == VOTE_VALUE)
    {
        total = result[32];
        for (i = 0; i < 10; i++)
        {
            fprintf(sug,
                    "[1m  %4d[m µ½ [1m%4d[m Ö®¼äÓĞ [1m%4d[m Æ±  Ô¼Õ¼ [1m%d%%[m\n",
                    (i * currvote.maxtkt) / 10 + ((i == 0) ? 0 : 1),
                    ((i + 1) * currvote.maxtkt) / 10, result[i],
                    (result[i] * 100) / ((total <= 0) ? 1 : total));
        }
        fprintf(sug, "´Ë´ÎÍ¶Æ±½á¹ûÆ½¾ùÖµÊÇ: [1m%d[m\n",
                result[31] / ((total <= 0) ? 1 : total));
    }
    else if (currvote.type == VOTE_ASKING)
    {
        total = result[32];
    }
    else
    {
        for (i = 0; i < currvote.totalitems; i++)
        {
            total += result[i];
        }
        for (i = 0; i < currvote.totalitems; i++)
        {
            fprintf(sug, "(%c) %-40s  %4d Æ±  Ô¼Õ¼ [1m%d%%[m\n", 'A' + i,
                    currvote.items[i], result[i],
                    (result[i] * 100) / ((total <= 0) ? 1 : total));
        }
    }
    fprintf(sug, "\nÍ¶Æ±×ÜÈËÊı = [1m%d[m ÈË\n", result[32]);
    fprintf(sug, "Í¶Æ±×ÜÆ±Êı =[1m %d[m Æ±\n\n", total);
    if (currvote.type == VOTE_SPECIAL)
    {
        if (strcmp(currvote.items[30], "Anony_ID") == 0)
        {
            if (result[0] >= 3 && result[1] == 0)
                fprintf(sug, "Í¶Æ±ÒÑÍ¨¹ı£¬´Ë´Î²éÑ¯½á¹û: %s\n", currvote.items[29]);
            else
                fprintf(sug, "Í¶Æ±Î´Í¨¹ı£¬´Ë´Î²éÑ¯ÎŞĞ§¡£\n");
        }
    }
    fprintf(sug,
            "[1;44;36m¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª©ÈÊ¹ÓÃÕß%s©À¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª[m\n\n\n",
            (currvote.type != VOTE_ASKING) ? "½¨Òé»òÒâ¼û" : "´Ë´ÎµÄ×÷´ğ");
    b_suckinfile(sug, sugname);
    unlink(sugname);
    fclose(sug);

    sprintf(title, "[¹«¸æ] %s °åµÄÍ¶Æ±½á¹û", currboard);

    if (currvote.showip)
    {
        votenums = getVoterIP(ipname, vinfo);
        writeVoterIP(ipname2, currboard, vinfo, votenums);
        sprintf(title2, "[¹«¸æ] %s °åµÄÍ¶Æ±²ÎÓëÇé¿ö", currboard);
    }

    if (currvote.type == VOTE_SPECIAL)
    {
        Postfile(nname, "syssecurity", title, 1);
        
        if (currvote.showip)
            Postfile(ipname2, "syssecurity", title2, 1);
    }
    else
    {
        if (strncmp(currboard, "BMTraining", 10) == 0)
        {
            Postfile(nname, "TrainingAdmin", title, 1);

            if (currvote.showip)
                Postfile(ipname2, "TrainingAdmin", title2, 1);
        }

        else if (!HiddenBoard(currboard))
        {
            Postfile(nname, "vote", title, 1);

            if (currvote.showip)
                Postfile(ipname2, "vote", title2, 1);
        }
    }
    if (strncmp(currboard, "vote", 4))
    {
        Postfile(nname, currboard, title, 1);

        if (currvote.showip)
            Postfile(ipname2, currboard, title2, 1);
    }
    dele_vote(num);
    return 0;
}

int get_vitems(struct votebal *bal)
{
    int num;
    char buf[STRLEN];

    move(3, 0);
    prints("ÇëÒÀĞòÊäÈë¿ÉÑ¡ÔñÏî, °´ ENTER Íê³ÉÉè¶¨.\n");
    num = 0;
    for (num = 0; num < 32; num++)
    {
        sprintf(buf, "%c) ", num + 'A');
        getdata((num % 16) + 4, (num / 16) * 40, buf, bal->items[num], 36,
                DOECHO, YEA);
        if (strlen(bal->items[num]) == 0)
        {
            if (num != 0)
                break;
            num = -1;
        }
    }
    bal->totalitems = num;
    return num;
}

int vote_maintain(char *bname)
{
    char buf[STRLEN], ans[2];
    struct votebal *ball = &currvote;

    setcontrolfile();
    if (!HAS_PERM(PERM_OVOTE))
        if (!chk_currBM(currBM))
            return 0;
    stand_title("¿ªÆôÍ¶Æ±Ïä");
    makevdir(bname);
    for (;;)
    {
        getdata(2, 0,
                "(1)ÊÇ·Ç (2)µ¥Ñ¡ (3)¸´Ñ¡ (4)ÊıÖµ (5)ÎÊ´ğ (6)È¡Ïû ? [6]",
                ans, 2, DOECHO, YEA);
        ans[0] -= '0';
        if (ans[0] < 1 || ans[0] > 5)
        {
            prints("È¡Ïû´Ë´ÎÍ¶Æ±\n");
            return FULLUPDATE;
        }
        ball->type = (int) ans[0];
        break;
    }
    ball->opendate = time(0);
    if (makevote(ball, bname))
        return FULLUPDATE;
    setvoteflag(currboard, 1);
    clear();
    strcpy(ball->userid, currentuser.userid);
    if (append_record(controlfile, ball, sizeof(*ball)) == -1)
    {
        prints("·¢ÉúÑÏÖØµÄ´íÎó£¬ÎŞ·¨¿ªÆôÍ¶Æ±£¬ÇëÍ¨¸æÕ¾³¤");
        b_report("Append Control file Error!!");
    }
    else
    {
        char votename[STRLEN];
        int i;

        b_report("OPEN");
        prints("Í¶Æ±Ïä¿ªÆôÁË£¡\n");
        range++;
        sprintf(votename, "tmp/votetmp.%s.%05d", currentuser.userid,
                uinfo.pid);
        if ((sug = fopen(votename, "w")) != NULL)
        {
            sprintf(buf, "[Í¨Öª] %s ¾Ù°ìÍ¶Æ±£º%s", currboard, ball->title);
            get_result_title();
            if (ball->type != VOTE_ASKING && ball->type != VOTE_VALUE)
            {
                fprintf(sug, "\n¡¾[1mÑ¡ÏîÈçÏÂ[m¡¿\n");
                for (i = 0; i < ball->totalitems; i++)
                {
                    fprintf(sug, "([1m%c[m) %-40s\n", 'A' + i,
                            ball->items[i]);
                }
            }
            fclose(sug);
            if (strncmp(currboard, "BMTraining", 10) == 0)
            {
                Postfile(votename, "TrainingAdmin", buf, 1);
            }
            else if (!HiddenBoard(currboard))
            {
                Postfile(votename, "vote", buf, 1);
            }

            if (strncmp(currboard, "vote", 4))
            {
                Postfile(votename, currboard, buf, 1);
            }

            unlink(votename);
        }
    }
    pressreturn();
    return FULLUPDATE;
}

int makevote(struct votebal *ball, char *bname)
{
    char buf[STRLEN];
    int aborted;

    prints("Çë°´ÈÎºÎ¼ü¿ªÊ¼±à¼­´Ë´Î [Í¶Æ±µÄÃèÊö]: \n");
    igetkey();
    setvfile(genbuf, bname, "desc");
    sprintf(buf, "%s.%ld", genbuf, ball->opendate);
    aborted = vedit(buf, NA, YEA);
    if (aborted)
    {
        clear();
        prints("È¡Ïû´Ë´ÎÍ¶Æ±Éè¶¨\n");
        pressreturn();
        return 1;
    }
    clear();
    getdata(0, 0, "´Ë´ÎÍ¶Æ±ËùĞëÌìÊı[1]: ", buf, 3, DOECHO, YEA);
    if (*buf == '\n' || atoi(buf) == 0 || *buf == '\0')
        ball->maxdays = 1;
    else
        ball->maxdays = atoi(buf);
    while (1)
    {
        getdata(1, 0, "Í¶Æ±ÏäµÄ±êÌâ: ", ball->title, 51, DOECHO, YEA);
        if (killwordsp(ball->title) != 0)
            break;
        bell();
    }
    switch (ball->type)
    {
    case VOTE_YN:
        ball->maxtkt = 0;
        strcpy(ball->items[0], "ÔŞ³É  £¨ÊÇµÄ£©");
        strcpy(ball->items[1], "²»ÔŞ³É£¨²»ÊÇ£©");
        strcpy(ball->items[2], "Ã»Òâ¼û£¨²»Çå³ş£©");
        ball->maxtkt = 1;
        ball->totalitems = 3;
        break;
    case VOTE_SINGLE:
        get_vitems(ball);
        ball->maxtkt = 1;
        break;
    case VOTE_MULTI:
        get_vitems(ball);
        for (;;)
        {
            getdata(21, 0, "Ò»¸öÈË×î¶à¼¸Æ±? [1]: ", buf, 5, DOECHO, YEA);
            ball->maxtkt = atoi(buf);
            if (ball->maxtkt <= 0)
                ball->maxtkt = 1;
            if (ball->maxtkt > ball->totalitems)
                continue;
            break;
        }
        break;
    case VOTE_VALUE:
        for (;;)
        {
            getdata(3, 0, "ÊäÈëÊıÖµ×î´ó²»µÃ³¬¹ı [100] : ", buf, 4, DOECHO,
                    YEA);
            ball->maxtkt = atoi(buf);
            if (ball->maxtkt <= 0)
                ball->maxtkt = 100;
            break;
        }
        break;
    case VOTE_ASKING:
        ball->maxtkt = 0;
        currvote.totalitems = 0;
        break;
    default:
        ball->maxtkt = 1;
        break;
    }
    if (askyn("±¾´ÎÍ¶Æ±ĞèÒªÏÔÊ¾Í¶Æ±ÈËĞÅÏ¢Âğ?", NA, NA))
    {
        ball->showip = 1;
    }
    else
    {
        ball->showip = 0;
    }
    return 0;
}

int setvoteperm(struct votebal *ball)
{
    int flag = NA, changeit = YEA;
    char buf[6], msgbuf[STRLEN];

    if (!HAS_PERM(PERM_OVOTE))
        return 0;
    clear();
    prints("µ±Ç°Í¶Æ±±êÌâ£º[\033[1;33m%s\033[m]", ball->title);
    prints("\nÄúÊÇ±¾Õ¾µÄÍ¶Æ±¹ÜÀíÔ±£¬Äú¿ÉÒÔ¶ÔÍ¶Æ±½øĞĞÍ¶Æ±ÊÜÏŞÉè¶¨\n");
    prints("\nÍ¶Æ±ÊÜÏŞÉè¶¨¹²ÓĞÈıÖÖÏŞÖÆ£¬ÕâÈıÖÖÏŞÖÆ¿ÉÒÔ×ö×ÛºÏÏŞ¶¨¡£\n");
    prints
        ("    (1) È¨ÏŞÏŞÖÆ [\033[32mÓÃ»§±ØĞëÓµÓĞÄ³Ğ©È¨ÏŞ²Å¿ÉÒÔÍ¶Æ±\033[m]\n");
    prints
        ("    (2) Í¶Æ±Ãûµ¥ [\033[35m´æÔÚÓÚÍ¶Æ±Ãûµ¥ÖĞµÄÈË²Å¿ÉÒÔÍ¶Æ±\033[m]\n");
    prints
        ("    (3) Ìõ¼şÉóºË [\033[36mÓÃ»§±ØĞëÂú×ãÒ»¶¨µÄÌõ¼ş²Å¿ÉÍ¶Æ±\033[m]\n");
    prints("\nÏÂÃæÏµÍ³½«Ò»²½Ò»²½Òıµ¼ÄúÀ´ÉèÖÃÍ¶Æ±ÊÜÏŞÉè¶¨¡£\n\n");
    if (askyn("ÄúÈ·¶¨Òª¿ªÊ¼¶Ô¸ÃÍ¶Æ±½øĞĞÍ¶Æ±ÊÜÏŞÉè¶¨Âğ", NA, NA) == NA)
        return 0;
    flag = (ball->level & ~(LISTMASK | VOTEMASK)) ? YEA : NA;
    prints
        ("\n\(\033[36m1\033[m) È¨ÏŞÏŞÖÆ  [Ä¿Ç°×´Ì¬: \033[32m%s\033[m]\n\n",
         flag ? "ÓĞÏŞÖÆ" : "ÎŞÏŞÖÆ");
    if (flag == YEA)
    {
        if (askyn("ĞèÒªÈ¡ÏûÍ¶Æ±ÕßµÄÈ¨ÏŞÏŞÖÆÂğ", NA, NA) == YEA)
            flag = NA;
        else
            changeit = askyn("ÄúĞèÒªĞŞ¸ÄÍ¶Æ±ÕßµÄÈ¨ÏŞÏŞÖÆÂğ", NA, NA);
    }
    else
        flag = askyn("ÄúÏ£ÍûÍ¶Æ±Õß±ØĞë¾ß±¸Ä³ÖÖÈ¨ÏŞÂğ", NA, NA);
    if (flag == NA)
    {
        ball->level = ball->level & (LISTMASK | VOTEMASK);
        prints("\n\nµ±Ç°Í¶Æ±\033[32mÎŞÈ¨ÏŞÏŞÖÆ\033[m£¡");
    }
    else if (changeit == NA)
    {
        prints("\nµ±Ç°Í¶Æ±\033[32m¡¾±£Áô¡¿È¨ÏŞÏŞÖÆ\033[m£¡");
    }
    else
    {
        clear();
        prints("\nÉè¶¨\033[32mÍ¶Æ±Õß±ØĞè\033[mµÄÈ¨ÏŞ¡£");
        ball->level =
            setperms(ball->level, "Í¶Æ±È¨ÏŞ", NUMPERMS, showperminfo);
        move(1, 0);
        if (ball->level & ~(LISTMASK | VOTEMASK))
            prints
                ("ÄúÒÑ¾­\033[32mÉè¶¨ÁË\033[mÍ¶Æ±µÄ±ØĞèÈ¨ÏŞ£¬ÏµÍ³½ÓÊÜÄúµÄÉè¶¨");
        else
            prints
                ("ÄúÏÖÔÚ\033[32mÈ¡ÏûÁË\033[mÍ¶Æ±µÄÈ¨ÏŞÏŞ¶¨£¬ÏµÍ³½ÓÊÜÄúµÄÉè¶¨");
    }
    prints("\n\n\n\033[33m¡¾ÏÂÒ»²½¡¿\033[m½«¼ÌĞø½øĞĞÍ¶Æ±ÏŞÖÆµÄÉè¶¨\n");
    pressanykey();
    clear();
    flag = (ball->level & LISTMASK) ? YEA : NA;
    prints
        ("\n(\033[36m2\033[m) Í¶Æ±Ãûµ¥  [Ä¿Ç°×´Ì¬: \033[32m%s\033[m]\n\n",
         flag ? "ÔÚÃûµ¥ÖĞµÄ ID ²Å¿ÉÍ¶Æ±" : "²»ÊÜÃûµ¥ÏŞÖÆ");
    if (askyn("ÄúÏë¸Ä±ä±¾ÏîÉè¶¨Âğ", NA, NA) == YEA)
    {
        if (flag)
        {
            ball->level &= ~LISTMASK;
            prints("\nÄúµÄÉè¶¨£º\033[32m¸ÃÍ¶Æ±²»ÊÜÍ¶Æ±Ãûµ¥µÄÏŞ¶¨\033[m\n");
        }
        else
        {
            ball->level |= LISTMASK;
            prints
                ("\nÄúµÄÉè¶¨£º\033[32mÖ»ÓĞÔÚÃûµ¥ÖĞµÄ ID ²Å¿ÉÒÔÍ¶Æ±\033[m\n");
            prints
                ("[\033[33mÌáÊ¾\033[m]Éè¶¨Íê±Ïºó£¬ÓÃ Ctrl+E À´±à¼­Í¶Æ±Ãûµ¥\n");
        }
    }
    else
    {
        prints
            ("\nÄúµÄÉè¶¨£º\033[32m[±£ÁôÉè¶¨]ÔÚÃûµ¥ÖĞµÄ ID ²Å¿ÉÒÔÍ¶Æ±\033[m\n");
    }
    flag = (ball->level & VOTEMASK) ? YEA : NA;
    prints
        ("\n(\033[36m3\033[m) Ìõ¼şÉóºË  [Ä¿Ç°×´Ì¬: \033[32m%s\033[m]\n\n",
         flag ? "ÊÜÌõ¼şÏŞÖÆ" : "²»ÊÜÌõ¼şÏŞÖÆ");
    if (flag == YEA)
    {
        if (askyn("ÄúÏëÈ¡ÏûÍ¶Æ±ÕßµÄÉÏÕ¾´ÎÊı¡¢ÉÏÕ¾Ê±¼äµÈµÄÏŞÖÆÂğ", NA, NA)
            == YEA)
            flag = NA;
        else
            changeit = askyn("ÄúÏëÖØĞÂÉè¶¨Ìõ¼şÏŞÖÆÂğ", NA, NA);
    }
    else
    {
        changeit = YEA;
        flag = askyn("Í¶Æ±ÕßÊÇ·ñĞèÒªÊÜµ½ÉÏÕ¾´ÎÊıµÈÌõ¼şµÄÏŞÖÆ", NA, NA);
    }
    if (flag == NA)
    {
        ball->level &= ~VOTEMASK;
        prints
            ("\nÄúµÄÉè¶¨£º\033[32m²»ÊÜÌõ¼şÏŞ¶¨\033[m\n\nÍ¶Æ±ÊÜÏŞÉè¶¨Íê±Ï\n\n");
    }
    else if (changeit == NA)
    {
        prints
            ("\nÄúµÄÉè¶¨£º\033[32m±£ÁôÌõ¼şÏŞ¶¨\033[m\n\nÍ¶Æ±ÊÜÏŞÉè¶¨Íê±Ï\n\n");
    }
    else
    {
        ball->level |= VOTEMASK;
        sprintf(msgbuf, "ÉÏÕ¾´ÎÊıÖÁÉÙ´ïµ½¶àÉÙ´Î£¿[%d]", ball->x_logins);
        getdata(11, 4, msgbuf, buf, 5, DOECHO, YEA);
        if (buf[0] != '\0')
            ball->x_logins = atoi(buf);
        sprintf(msgbuf, "·¢±íµÄÎÄÕÂÖÁÉÙÓĞ¶àÉÙÆª£¿[%d]", ball->x_posts);
        getdata(12, 4, msgbuf, buf, 5, DOECHO, YEA);
        if (buf[0] != '\0')
            ball->x_posts = atoi(buf);
        sprintf(msgbuf, "ÔÚ±¾Õ¾µÄÀÛ¼ÆÉÏÕ¾Ê±¼äÖÁÉÙÓĞ¶àÉÙĞ¡Ê±£¿[%d]",
                ball->x_stay);
        getdata(13, 4, msgbuf, buf, 5, DOECHO, YEA);
        if (buf[0] != '\0')
            ball->x_stay = atoi(buf);
        sprintf(msgbuf, "¸ÃÕÊºÅ×¢²áÊ±¼äÖÁÉÙÓĞ¶àÉÙÌì£¿[%d]", ball->x_live);
        getdata(14, 4, msgbuf, buf, 5, DOECHO, YEA);
        if (buf[0] != '\0')
            ball->x_live = atoi(buf);
        prints("\nÍ¶Æ±ÊÜÏŞÉè¶¨Íê±Ï\n\n");
    }
    if (askyn("ÄúÈ·¶¨ÒªĞŞ¸ÄÍ¶Æ±ÊÜÏŞÂğ", NA, NA) == YEA)
    {
        sprintf(msgbuf, "ĞŞ¸Ä %s °åÍ¶Æ±[Í¶Æ±ÊÜÏŞ]", currboard);
        securityreport(msgbuf);
        return 1;
    }
    else
        return 0;
}

int vote_flag(char *bname, char val, int mode)
{
    char buf[STRLEN], flag;
    int fd, num, size;

    num = usernum - 1;
    switch (mode)
    {
    case 2:
        sprintf(buf, "Welcome.rec");
        break;
    case 1:
        setvfile(buf, bname, "noterec");
        break;
    default:
        return -1;
    }
    if (num >= MAXUSERS)
    {
        report("Vote Flag, Out of User Numbers");
        return -1;
    }
    if ((fd = open(buf, O_RDWR | O_CREAT, 0600)) == -1)
    {
        return -1;
    }
    flock(fd, LOCK_EX);
    size = (int) lseek(fd, 0, SEEK_END);
    memset(buf, 0, sizeof(buf));
    while (size <= num)
    {
        write(fd, buf, sizeof(buf));
        size += sizeof(buf);
    }
    lseek(fd, (off_t) num, SEEK_SET);
    read(fd, &flag, 1);
    if ((flag == 0 && val != 0))
    {
        lseek(fd, (off_t) num, SEEK_SET);
        write(fd, &val, 1);
    }
    flock(fd, LOCK_UN);
    close(fd);
    return flag;
}

int vote_check(int bits)
{
    int i, count;

    for (i = count = 0; i < 32; i++)
    {
        if ((bits >> i) & 1)
            count++;
    }
    return count;
}

int showvoteitems(unsigned int pbits, int i, int flag)
{
    char buf[STRLEN];
    int count;

    if (flag == YEA)
    {
        count = vote_check(pbits);
        if (count > currvote.maxtkt)
            return NA;
        move(2, 0);
        clrtoeol();
        prints("ÄúÒÑ¾­Í¶ÁË [1m%d[m Æ±", count);
    }
    sprintf(buf, "%c.%2.2s%-36.36s", 'A' + i,
            ((pbits >> i) & 1 ? "¡Ì" : "  "), currvote.items[i]);
    move(i + 6 - ((i > 15) ? 16 : 0), 0 + ((i > 15) ? 40 : 0));
    prints(buf);
    refresh();
    return YEA;
}

void show_voteing_title(void)
{
    time_t closedate;
    char buf[STRLEN];

    if (currvote.type != VOTE_VALUE && currvote.type != VOTE_ASKING)
        sprintf(buf, "¿ÉÍ¶Æ±Êı: [1m%d[m Æ±", currvote.maxtkt);
    else
        buf[0] = '\0';
    closedate = currvote.opendate + currvote.maxdays * 86400;
    getdatestring(closedate, NA);
    prints("Í¶Æ±½«½áÊøÓÚ: [1m%s[m  %s  %s\n",
           datestring, buf, (voted_flag) ? "([5;1mĞŞ¸ÄÇ°´ÎÍ¶Æ±[m)" : "");
    prints("Í¶Æ±Ö÷ÌâÊÇ: [1m%-50s[mÀàĞÍ: [1m%s[m \n", currvote.title,
           vote_type[currvote.type - 1]);
    if (currvote.showip)
        prints("[1m×¢Òâ£º±¾´ÎÍ¶Æ±½«¹«²¼Í¶Æ±ÈËIDºÍIPĞÅÏ¢[m\n");
}

int getsug(struct ballot *uv)
{
    int i, line;

    move(0, 0);
    clrtobot();
    if (currvote.type == VOTE_ASKING)
    {
        show_voteing_title();
        line = 3;
        prints("ÇëÌîÈëÄúµÄ×÷´ğ(ÈıĞĞ):\n");
    }
    else
    {
        line = 1;
        prints("ÇëÌîÈëÄú±¦¹óµÄÒâ¼û(ÈıĞĞ):\n");
    }
    move(line, 0);
    for (i = 0; i < 3; i++)
    {
        prints(": %s\n", uv->msg[i]);
    }
    for (i = 0; i < 3; i++)
    {
        getdata(line + i, 0, ": ", uv->msg[i], STRLEN - 2, DOECHO, NA);
        if (uv->msg[i][0] == '\0')
            break;
    }
    return i;
}

int multivote(struct ballot *uv)
{
    unsigned int i;

    i = uv->voted;
    move(0, 0);
    show_voteing_title();
    uv->voted =
        setperms(uv->voted, "Ñ¡Æ±", currvote.totalitems, showvoteitems);
    if (uv->voted == i)
        return -1;
    return 1;
}

int valuevote(struct ballot *uv)
{
    unsigned int chs;
    char buf[10];

    chs = uv->voted;
    move(0, 0);
    show_voteing_title();
    prints("´Ë´Î×÷´ğµÄÖµ²»ÄÜ³¬¹ı [1m%d[m", currvote.maxtkt);
    if (uv->voted != 0)
        sprintf(buf, "%d", uv->voted);
    else
        memset(buf, 0, sizeof(buf));
    do
    {
        getdata(3, 0, "ÇëÊäÈëÒ»¸öÖµ? [0]: ", buf, 5, DOECHO, NA);
        uv->voted = abs(atoi(buf));
    }
    while ((int) uv->voted > currvote.maxtkt && buf[0] != '\n'
           && buf[0] != '\0');
    if (buf[0] == '\n' || buf[0] == '\0' || uv->voted == chs)
        return -1;
    return 1;
}

int user_vote(int page, int num)
{
    char fname[STRLEN], bname[STRLEN], ipfname[STRLEN], emailfname[STRLEN];
    char buf[STRLEN];
    char uipname[STRLEN];
    char real_mail[STRLEN];
    struct ballot uservote, tmpbal;
    struct voter_info v_info;
    int votevalue, result = NA;
    int aborted = NA, pos;

    move(t_lines - 2, 0);
    get_record(controlfile, &currvote, sizeof(struct votebal), num);
    sprintf(ipfname, "vote/%s/ip.%ld", currboard, currvote.opendate);
    sprintf(emailfname, "vote/%s/mail.%ld", currboard, currvote.opendate);
    sprintf(uipname, "vote/%s/uip.%ld", currboard, currvote.opendate);
    if (!haspostperm(currboard))
    {
        prints("¶Ô²»Æğ£¬ÄúÔÚ±¾°åÃ»ÓĞ·¢ÎÄÈ¨ÏŞ£¬ËùÒÔ²»ÄÜÍ¶Æ±\n");
    }
    else if (currentuser.firstlogin > currvote.opendate)
    {
        prints("¶Ô²»Æğ, ±¾Í¶Æ±ÔÚÄúÕÊºÅÉêÇëÖ®Ç°¿ªÆô£¬Äú²»ÄÜÍ¶Æ±\n");
    }
    else if (!HAS_PERM(currvote.level & ~(LISTMASK | VOTEMASK)))
    {
        prints("¶Ô²»Æğ£¬ÄúÄ¿Ç°ÉĞÎŞÈ¨ÔÚ±¾Æ±ÏäÍ¶Æ±\n");
    }
    else if (currvote.level & LISTMASK)
    {
        char listfilename[STRLEN];

        setvfile(listfilename, currboard, "vote.list");
        if (!dashf(listfilename))
            prints("¶Ô²»Æğ£¬±¾Æ±ÏäĞèÒªÉè¶¨ºÃÍ¶Æ±Ãû²á·½¿É½øĞĞÍ¶Æ±\n");
        else if (!seek_in_file(listfilename, currentuser.userid))
            prints("¶Ô²»Æğ, Í¶Æ±Ãû²áÉÏÕÒ²»µ½ÄúµÄ´óÃû\n");
        else
            result = YEA;
    }
    else if (currvote.level & VOTEMASK)
    {
        if (currentuser.numlogins < currvote.x_logins
            || currentuser.numposts < currvote.x_posts
            || (unsigned int) currentuser.stay < currvote.x_stay * 3600
            || (unsigned int) currentuser.firstlogin >
            (currvote.opendate - currvote.x_live * 86400))
            prints("¶Ô²»Æğ£¬ÄúÄ¿Ç°ÉĞ²»¹»×Ê¸ñÔÚ±¾Æ±ÏäÍ¶Æ±\n");
        else
            result = YEA;
    }
    else
        result = YEA;
    if (result == NA)
    {
        pressanykey();
        return 0;
    }
    sprintf(fname, "vote/%s/flag.%ld", currboard, currvote.opendate);
    if ((pos = search_record(fname, &uservote, sizeof(uservote),
                             cmpvuid, currentuser.userid)) <= 0)
    {
        (void) memset(&uservote, 0, sizeof(uservote));
        voted_flag = NA;
    }
    else
    {
        voted_flag = YEA;
    }
    strcpy(uservote.uid, currentuser.userid);
    strcpy(v_info.uid, currentuser.userid);
    strcpy(v_info.uip, currentuser.lasthost);
    v_info.vote_time = time(0);
    sprintf(bname, "desc.%ld", currvote.opendate);
    setvfile(buf, currboard, bname);
    ansimore(buf, YEA);
    move(0, 0);
    clrtobot();
    switch (currvote.type)
    {
    case VOTE_SINGLE:
    case VOTE_MULTI:
    case VOTE_YN:
    case VOTE_SPECIAL:
        votevalue = multivote(&uservote);
        if (votevalue == -1)
            aborted = YEA;
        break;
    case VOTE_VALUE:
        votevalue = valuevote(&uservote);
        if (votevalue == -1)
            aborted = YEA;
        break;
    case VOTE_ASKING:
        uservote.voted = 0;
        aborted = !getsug(&uservote);
        break;
    }
    clear();
    if (aborted == YEA)
    {
        prints("±£Áô ¡¾[1m%s[m¡¿Ô­À´µÄµÄÍ¶Æ±¡£\n", currvote.title);
    }
    else
    {
        if (currvote.type != VOTE_ASKING)
            getsug(&uservote);
        pos = search_record(fname, &tmpbal, sizeof(tmpbal),
                            cmpvuid, currentuser.userid);
        if (pos)
        {
            substitute_record(fname, &uservote, sizeof(uservote), pos);
            append_record(uipname, &v_info, sizeof(v_info));
        }
        else
        {
	    if (get_real_mail(toupper(currentuser.userid[0]),currentuser.userid,real_mail) >= 0 && search_mail(emailfname, real_mail) >= MAXVOTEPEREMAIL)
	    {
               	move(2, 0);
               	clrtoeol();
               	prints("\n¶Ô²»Æğ£¬ÄúµÄemail Í¶Æ±´ÎÊıÒÑ³¬¹ıÏŞÖÆ\n");
               	pressreturn();

               	return 0;
	    }
	    else if (search_ip(ipfname, currentuser.lasthost) >= MAXVOTEPERIP)
            {
                move(2, 0);
                clrtoeol();
                prints("\n¶Ô²»Æğ£¬ÄúµÄip Í¶Æ±´ÎÊıÒÑ³¬¹ıÏŞÖÆ\n");
                pressreturn();

                return 0;
            }
            else
            {
                if (append_record(fname, &uservote, sizeof(uservote)) ==
                    -1)
                {
                    move(2, 0);
                    clrtoeol();
                    prints("Í¶Æ±Ê§°Ü! ÇëÍ¨ÖªÕ¾³¤²Î¼ÓÄÇÒ»¸öÑ¡ÏîÍ¶Æ±\n");
                    pressreturn();
                }
                else
                {
                    if (append_record(uipname, &v_info, sizeof(v_info)) ==
                        -1)
                    {
                        move(2, 0);
                        clrtoeol();
                        prints("Í¶Æ±Ê§°Ü! ÇëÍ¨ÖªÕ¾³¤²Î¼ÓÄÇÒ»¸öÑ¡ÏîÍ¶Æ±\n");
                        pressreturn();
                    }
                    else
                    {
                        append_ip(ipfname, currentuser.lasthost);
	    		if (get_real_mail(toupper(currentuser.userid[0]),currentuser.userid,real_mail) >= 0)
			{
                        	append_mail(emailfname, real_mail);
			}
                    }
                }
            }

        }

        prints("\nÒÑ¾­°ïÄúÍ¶ÈëÆ±ÏäÖĞ...\n");
    }
    pressanykey();
    return 0;
}

int voteexp(void)
{
    clrtoeol();
    prints("[1;44m±àºÅ ¿ªÆôÍ¶Æ±ÏäÕß ¿ªÆôÈÕ %-40sÀà±ğ ÌìÊı ÈËÊı[m\n",
           "Í¶Æ±Ö÷Ìâ");
    return 0;
}

int printvote(void *vent)
{
    struct votebal *ent = (struct votebal *) vent;
    static int i;
    struct ballot uservote;
    char buf[STRLEN + 10];
    char flagname[STRLEN];
    int num_voted;

    if (ent == NULL)
    {
        move(2, 0);
        voteexp();
        i = 0;
        return 0;
    }
    i++;
    if (i > page + 20 || i > range)
        return QUIT;
    else if (i <= page)
        return 0;
    sprintf(buf, "flag.%ld", ent->opendate);
    setvfile(flagname, currboard, buf);
    if (search_record(flagname, &uservote, sizeof(uservote), cmpvuid,
                      currentuser.userid) <= 0)
    {
        voted_flag = NA;
    }
    else
        voted_flag = YEA;
    num_voted = get_num_records(flagname, sizeof(struct ballot));
    getdatestring(ent->opendate, NA);
    sprintf(buf, " %s%3d %-12.12s %6.6s%s%-40.40s%-4.4s %3d  %4d[m\n",
            (voted_flag == NA) ? "[1m" : "", i, ent->userid,
            datestring + 6, ent->level ? "\033[33ms\033[37m" : " ",
            ent->title, vote_type[ent->type - 1], ent->maxdays, num_voted);
    prints("%s", buf);
    return 0;
}

int dele_vote(int num)
{
    char buf[STRLEN];

    sprintf(buf, "vote/%s/ip.%ld", currboard, currvote.opendate);
    unlink(buf);
    sprintf(buf, "vote/%s/mail.%ld", currboard, currvote.opendate);
    unlink(buf);
    sprintf(buf, "vote/%s/flag.%ld", currboard, currvote.opendate);
    unlink(buf);
    sprintf(buf, "vote/%s/desc.%ld", currboard, currvote.opendate);
    unlink(buf);
    sprintf(buf, "vote/%s/uip.%ld", currboard, currvote.opendate);
    unlink(buf);
    sprintf(buf, "vote/%s/uip2.%ld", currboard, currvote.opendate);
    unlink(buf);

    if (delete_record(controlfile, sizeof(currvote), num) == -1)
    {
        prints("·¢Éú´íÎó£¬ÇëÍ¨ÖªÕ¾³¤....");
        pressanykey();
    }
    range--;
    if (get_num_records(controlfile, sizeof(currvote)) == 0)
    {
        setvoteflag(currboard, 0);
    }
    return 0;
}

int vote_results(char *bname)
{
    char buf[STRLEN];

    setvfile(buf, bname, "results");
    if (ansimore(buf, YEA) == -1)
    {
        move(3, 0);
        prints("Ä¿Ç°Ã»ÓĞÈÎºÎÍ¶Æ±µÄ½á¹û¡£\n");
        clrtobot();
        pressreturn();
    }
    else
        clear();
    return FULLUPDATE;
}

int b_vote_maintain(void)
{
    return vote_maintain(currboard);
}

int vote_title(void)
{

    docmdtitle("[Í¶Æ±ÏäÁĞ±í]",
               "[[1;32m¡û[m,[1;32me[m] Àë¿ª [[1;32mh[m] ÇóÖú [[1;32m¡ú[m,[1;32mr <cr>[m] ½øĞĞÍ¶Æ± [[1;32m¡ü[m,[1;32m¡ı[m] ÉÏ,ÏÂÑ¡Ôñ [1m¸ßÁÁ¶È[m±íÊ¾ÉĞÎ´Í¶Æ±");
    update_endline();
    return 0;
}

#define maxrecp 300
int ListFileEdit(char *fname, char *msg, int response)
{
    char listfile[STRLEN], uident[13], ans[3];
    int cnt, i, n, fmode = NA;

    char mail_buffer[1024] = { 0 };
    char mail_title[STRLEN] = { 0 };

    if (fname == NULL)
        return 0;
    strcpy(listfile, fname);
    clear();
    cnt = listfilecontent(listfile, 3);
    while (1)
    {
        if (msg != NULL)
        {
            move(0, 0);
            prints("\033[0;1;47;31m%s\033[m", msg);
        }
        if (cnt > maxrecp - 10)
        {
            move(2, 0);
            prints("Ä¿Ç°ÏŞÖÆ±¾Ãûµ¥×î¶à ^[[1m%d^[[m ÈË", maxrecp);
        }
        getdata(1, 0,
                "(A)Ôö¼Ó (D)É¾³ı (I)ÒıÈëºÃÓÑ (C)Çå³ıÄ¿Ç°Ãûµ¥ (E)È·¶¨ [E]: ",
                ans, 2, DOECHO, YEA);
        if (ans[0] != 'a' && ans[0] != 'A' && ans[0] != 'd'
            && ans[0] != 'D' && ans[0] != 'i' && ans[0] != 'I'
            && ans[0] != 'c' && ans[0] != 'C')
        {
            clear();
            return cnt;
        }
        if (ans[0] == 'a' || ans[0] == 'd' || ans[0] == 'A'
            || ans[0] == 'D')
        {
            move(2, 0);
            if (ans[0] == 'a' || ans[0] == 'A')
                usercomplete("ÇëÒÀ´ÎÊäÈëÊ¹ÓÃÕß´úºÅ(Ö»°´ ENTER ½áÊøÊäÈë): ",
                             uident);
            else
                namecomplete("ÇëÒÀ´ÎÊäÈëÊ¹ÓÃÕß´úºÅ(Ö»°´ ENTER ½áÊøÊäÈë): ",
                             uident);
            move(2, 0);
            clrtoeol();
            if (uident[0] == '\0')
                continue;
        }
        switch (ans[0])
        {
        case 'A':
        case 'a':
            if (!getuser(uident))
            {
                move(2, 0);
                prints("Õâ¸öÊ¹ÓÃÕß´úºÅÊÇ´íÎóµÄ.");
                break;
            }
            if (!(lookupuser.userlevel & PERM_READMAIL))
            {
                move(2, 0);
                prints("[%s] Ä¿Ç°±»Í£È¨\n", uident);
                break;
            }
            if (SeekInNameList(uident))
            {
                move(2, 0);
                prints("[%s] ÒÑ¾­ÔÚÃûµ¥ÖĞ\n", uident);
                break;
            }
            add_to_file(listfile, uident);
            cnt++;

            if (response)
            {
                memset(mail_buffer, 0x00, 1024);
                memset(mail_title, 0x00, STRLEN);
                sprintf(mail_title, "%s ¼ÓÈë %s °å¿É¶ÁĞ´Ãûµ¥",
                        uident, currboard);
                sprintf(mail_buffer,
                        "\n  %s ÍøÓÑ: \n\n"
                        "\t Äú±»¼ÓÈë %s °åµÄ¡º¿É¶ÁĞ´Ãûµ¥¡»\n\n"
                        "\t ´ÓÏÖÔÚ¿ªÊ¼£¬Äú»ñµÃÔÚ %s °åä¯ÀÀºÍ·¢ÎÄµÄÈ¨Àû\n\n"
                        "\t\t\t\t\t\t\t\t\t\t\t\tBy %s",
                        uident, currboard, currboard, currentuser.userid);
                autoreport(mail_title, mail_buffer, uident, NULL);
            }

            break;
        case 'D':
        case 'd':
            if (seek_in_file(listfile, uident))
            {
                del_from_file(listfile, uident);
                DelFromNameList(uident);
                cnt--;

                if ((response) && (getuser(uident)))

                {
                    memset(mail_buffer, 0x00, 1024);
                    memset(mail_title, 0x00, STRLEN);
                    sprintf(mail_title,
                            "%s ²»ÔÙÊôÓÚ %s °å¿É¶ÁĞ´Ãûµ¥",
                            uident, currboard);
                    sprintf(mail_buffer,
                            "\n  %s ÍøÓÑ: \n\n"
                            "\t Äú±»²»ÔÙÊôÓÚ %s °åµÄ¡º¿É¶ÁĞ´Ãûµ¥¡»\n\n"
                            "\t ´ÓÏÖÔÚ¿ªÊ¼£¬Äú½«Ê§È¥ÔÚ %s °åä¯ÀÀºÍ·¢ÎÄµÄÈ¨Àû\n\n"
                            "\t\t\t\t\t\t\t\t\t\t\t\tBy %s",
                            uident, currboard, currboard,
                            currentuser.userid);
                    autoreport(mail_title, mail_buffer, uident, NULL);
                }

            }
            break;
        case 'I':
        case 'i':
            n = 0;
            clear();
            for (i = cnt; i < maxrecp && n < uinfo.fnum; i++)
            {
                int key;

                move(2, 0);
                getuserid(uident, uinfo.friends[n]);
                prints("%s\n", uident);
                move(3, 0);
                n++;
                prints("(A)È«²¿¼ÓÈë (Y)¼ÓÈë (N)²»¼ÓÈë (Q)½áÊø? [Y]:");
                if (!fmode)
                    key = igetkey();
                else
                    key = 'Y';
                if (key == 'q' || key == 'Q')
                    break;
                if (key == 'A' || key == 'a')
                {
                    fmode = YEA;
                    key = 'Y';
                }
                if (key == '\0' || key == '\n' || key == 'y' || key == 'Y')
                {
                    if (!getuser(uident))
                    {
                        move(4, 0);
                        prints("Õâ¸öÊ¹ÓÃÕß´úºÅÊÇ´íÎóµÄ.\n");
                        i--;
                        continue;
                    }
                    else if (!(lookupuser.userlevel & PERM_READMAIL))
                    {
                        move(4, 0);
                        prints("[%s] Ä¿Ç°±»Í£È¨\n", uident);
                        i--;
                        continue;
                    }
                    else if (SeekInNameList(uident))
                    {
                        move(4, 0);
                        prints("[%s] ÒÑ¾­ÔÚÃûµ¥ÖĞ\n", uident);
                        i--;
                        continue;
                    }
                    add_to_file(listfile, uident);
                    cnt++;
                    if (response)
                    {
                        memset(mail_buffer, 0x00, 1024);
                        memset(mail_title, 0x00, STRLEN);
                        sprintf(mail_title,
                                "%s ¼ÓÈë %s °å¿É¶ÁĞ´Ãûµ¥",
                                uident, currboard);
                        sprintf(mail_buffer,
                                "\n  %s ÍøÓÑ: \n\n"
                                "\t Äú±»¼ÓÈë %s °åµÄ¡º¿É¶ÁĞ´Ãûµ¥¡»\n\n"
                                "\t ´ÓÏÖÔÚ¿ªÊ¼£¬Äú»ñµÃÔÚ %s °åä¯ÀÀºÍ·¢ÎÄµÄÈ¨Àû\n\n"
                                "\t\t\t\t\t\t\t\t\t\t\t\tBy %s",
                                uident, currboard, currboard,
                                currentuser.userid);
                        autoreport(mail_title, mail_buffer, uident, NULL);
                    }

                }
            }
            fmode = NA;
            clear();
            break;
        case 'C':
        case 'c':
            unlink(listfile);
            cnt = 0;
        }
        move(5, 0);
        clrtobot();
        move(3, 0);
        clrtobot();
        cnt = listfilecontent(listfile, 3);
    }
}

int vote_key(int ch, int allnum, int pagenum)
{
    int deal = 0;
    char buf[STRLEN];

    switch (ch)
    {
    case 'v':
    case 'V':
    case '\n':
    case '\r':
    case 'r':
    case KEY_RIGHT:
        user_vote(0, allnum + 1);
        deal = 1;
        break;
    case 'R':
        vote_results(currboard);
        deal = 1;
        break;
    case 'H':
    case 'h':
        show_help("help/votehelp");
        deal = 1;
        break;
    case 'A':
    case 'a':
        if (!chk_currBM(currBM))
            return YEA;
        vote_maintain(currboard);
        deal = 1;
        break;
    case 'O':
    case 'o':
        if (!chk_currBM(currBM))
            return YEA;
        clear();
        deal = 1;
        get_record(controlfile, &currvote, sizeof(struct votebal),
                   allnum + 1);
        prints("[5;1;31m¾¯¸æ!![m\nÍ¶Æ±Ïä±êÌâ£º[1m%s[m\n",
               currvote.title);
        if (askyn("ÄãÈ·¶¨ÒªÌáÔç½áÊøÕâ¸öÍ¶Æ±Âğ", NA, NA) != YEA)
        {
            move(2, 0);
            prints("È¡ÏûÌáÔç½áÊøÍ¶Æ±ĞĞ¶¯\n");
            pressreturn();
            clear();
            break;
        }
        mk_result(allnum + 1);
        sprintf(buf, "ÌáÔç½áÊøÍ¶Æ± %s", currvote.title);
        securityreport(buf);
        break;
    case 'T':
        if (!chk_currBM(currBM))
            return YEA;
        get_record(controlfile, &currvote, sizeof(struct votebal),
                   allnum + 1);
        if (currvote.type == VOTE_SPECIAL)
            return YEA;
        deal = 1;
        getdata(t_lines - 1, 0, "ĞÂÍ¶Æ±Ö÷Ìâ£º", currvote.title, 51, DOECHO,
                YEA);
        if (currvote.title[0] != '\0')
        {
            substitute_record(controlfile, &currvote,
                              sizeof(struct votebal), allnum + 1);
        }
        break;
    case 's':
        get_record(controlfile, &currvote, sizeof(struct votebal),
                   allnum + 1);
        if (currvote.level == 0)
            break;
        deal = 1;
        move(t_lines - 8, 0);
        clrtoeol();
        prints("\033[47;30m¡º\033[31m%s\033[30m¡»Í¶Æ±ÊÜÏŞËµÃ÷£º\033[m\n",
               currvote.title);
        if (currvote.level & ~(LISTMASK | VOTEMASK))
        {
            int num, len;

            strcpy(genbuf, "bTCPRD#@XWBA#VS-DOM-F0s23456789t");
            len = strlen(genbuf);
            for (num = 0; num < len; num++)
                if (!(currvote.level & (1 << num)))
                    genbuf[num] = '-';
            prints("È¨ÏŞÊÜÏŞ£º[\033[32m%s\033[m]\n", genbuf);
        }
        else
            prints("È¨ÏŞÊÜÏŞ£º[\033[32m±¾Æ±ÏäÍ¶Æ±²»ÊÜÈ¨ÏŞÏŞÖÆ\033[m]\n");
        if (currvote.level & LISTMASK)
            prints
                ("Ãûµ¥ÊÜÏŞ£º[\033[35mÖ»ÓĞÍ¶Æ±Ãûµ¥ÖĞµÄ ID ²Å¿ÉÍ¶Æ±  \033[m]\n");
        else
            prints
                ("Ãûµ¥ÊÜÏŞ£º[\033[35m±¾Æ±ÏäÍ¶Æ±²»ÊÜÍ¶Æ±Ãûµ¥ÏŞÖÆ    \033[m]\n");
        if (currvote.level & VOTEMASK)
        {
            prints("          ©³ ¢Ù¡¢ÔÚ±¾Õ¾µÄÉÏÕ¾´ÎÊıÖÁÉÙ [%-4d] ´Î\n",
                   currvote.x_logins);
            prints("Ìõ¼şÊÜÏŞ£º©Ç ¢Ú¡¢ÔÚ±¾Õ¾·¢±íÎÄÕÂÖÁÉÙ   [%-4d] Æª\n",
                   currvote.x_posts);
            prints("          ©Ç ¢Û¡¢Êµ¼ÊÀÛ¼ÆÉÏÕ¾Ê±¼äÖÁÉÙ [%-4d] Ğ¡Ê±\n",
                   currvote.x_stay);
            prints("          ©» ¢Ü¡¢¸Ã ID µÄ×¢²áÊ±¼äÖÁÉÙ [%-4d] Ìì\n",
                   currvote.x_live);
        }
        else
            prints
                ("Ìõ¼şÊÜÏŞ£º[\033[36m±¾Æ±ÏäÍ¶Æ±²»ÊÜ¸öÈËÌõ¼şÏŞÖÆ\033[m    ]\n");
        pressanykey();
        break;
    case 'S':
        if (!HAS_PERM(PERM_OVOTE))
            return YEA;
        deal = 1;
        get_record(controlfile, &currvote, sizeof(struct votebal),
                   allnum + 1);
        if (setvoteperm(&currvote) != 0)
        {
            substitute_record(controlfile, &currvote,
                              sizeof(struct votebal), allnum + 1);
        }
        break;
    case Ctrl('E'):/*
        if (!HAS_PERM(PERM_OVOTE))
            return YEA;*/
        if (!chk_currBM(currBM) && !HAS_PERM(PERM_OVOTE))
            return YEA;
        deal = 1;
        setvfile(genbuf, currboard, "vote.list");
        get_record(controlfile, &currvote, sizeof(struct votebal), allnum + 1);
        if (ListFileEdit(genbuf, "±à¼­¡ºÍ¶Æ±Ãûµ¥¡»", 0) == 0){
		currvote.level &= ~LISTMASK;
	}
	else{
		currvote.level |= LISTMASK;
	}
                substitute_record(controlfile, &currvote, sizeof(struct votebal), allnum + 1);
        break;
    case 'E':
        if (!chk_currBM(currBM))
            return YEA;
        get_record(controlfile, &currvote, sizeof(struct votebal),
                   allnum + 1);
        if (currvote.type == VOTE_SPECIAL)
            return YEA;
        deal = 1;
        setvfile(genbuf, currboard, "desc");
        sprintf(buf, "%s.%ld", genbuf, currvote.opendate);
        vedit(buf, NA, YEA);
        break;
    case 'M':
    case 'm':
        if (!chk_currBM(currBM))
            return YEA;
        get_record(controlfile, &currvote, sizeof(struct votebal),
                   allnum + 1);
        if (currvote.type == VOTE_SPECIAL)
            return YEA;
        deal = 1;
        clear();
        prints("[5;1;31m¾¯¸æ!![m\n");
        prints("Í¶Æ±Ïä±êÌâ£º[1m%s[m\n", currvote.title);
        if (askyn("ÄãÈ·¶¨ÒªĞŞ¸ÄÕâ¸öÍ¶Æ±µÄÉè¶¨Âğ", NA, NA) != YEA)
        {
            move(2, 0);
            prints("È¡ÏûĞŞ¸ÄÍ¶Æ±ĞĞ¶¯\n");
            pressreturn();
            clear();
            break;
        }
        makevote(&currvote, currboard);
        substitute_record(controlfile, &currvote,
                          sizeof(struct votebal), allnum + 1);
        sprintf(buf, "ĞŞ¸ÄÍ¶Æ±Éè¶¨ %s", currvote.title);
        securityreport(buf);
        break;
    case 'D':
    case 'd':
        if (!HAS_PERM(PERM_OVOTE))
        {
            if (!chk_currBM(currBM))
                return YEA;
        }
        deal = 1;
        get_record(controlfile, &currvote, sizeof(struct votebal),
                   allnum + 1);
        clear();
        prints("[5;1;31m¾¯¸æ!![m\n");
        prints("Í¶Æ±Ïä±êÌâ£º[1m%s[m\n", currvote.title);
        if (askyn("ÄúÈ·¶¨ÒªÇ¿ÖÆ¹Ø±ÕÕâ¸öÍ¶Æ±Âğ", NA, NA) != YEA)
        {
            move(2, 0);
            prints("È¡ÏûÇ¿ÖÆ¹Ø±ÕĞĞ¶¯\n");
            pressreturn();
            clear();
            break;
        }
        sprintf(buf, "Ç¿ÖÆ¹Ø±ÕÍ¶Æ± %s", currvote.title);
        securityreport(buf);
        dele_vote(allnum + 1);
        break;
    default:
        return 0;
    }
    if (deal)
    {
        Show_Votes();
        vote_title();
    }
    return 1;
}

int Show_Votes(void)
{

    move(3, 0);
    clrtobot();
    printvote(NULL);
    setcontrolfile();
    if (apply_record(controlfile, printvote, sizeof(struct votebal)) == -1)
    {
        prints("´íÎó£¬Ã»ÓĞÍ¶Æ±Ïä¿ªÆô....");
        pressreturn();
        return 0;
    }
    clrtobot();
    return 0;
}

int b_vote(void)
{
    int num_of_vote;
    int voting;

    if (!HAS_PERM(PERM_VOTE)
        || ((currentuser.stay < 1800) && (currentuser.numlogins < 10)))
        return DONOTHING;
    setcontrolfile();
    num_of_vote = get_num_records(controlfile, sizeof(struct votebal));
    if (num_of_vote == 0)
    {
        move(2, 0);
        clrtobot();
        prints("\n±§Ç¸, Ä¿Ç°²¢Ã»ÓĞÈÎºÎÍ¶Æ±¾ÙĞĞ¡£\n");
        pressreturn();
        setvoteflag(currboard, 0);
        return FULLUPDATE;
    }
    setvoteflag(currboard, 1);
    modify_user_mode(VOTING);
    setlistrange(num_of_vote);
    clear();
    voting = choose(NA, 0, vote_title, vote_key, Show_Votes, user_vote);
    clear();
    return FULLUPDATE;
}

int b_results(void)
{
    return vote_results(currboard);
}

int m_vote(void)
{
    char buf[STRLEN];

    strcpy(buf, currboard);
    strcpy(currboard, DEFAULTBOARD);
    modify_user_mode(ADMIN);
    vote_maintain(DEFAULTBOARD);
    strcpy(currboard, buf);
    return 0;
}

int x_vote(void)
{
    char buf[STRLEN];

    modify_user_mode(XMENU);
    strcpy(buf, currboard);
    strcpy(currboard, DEFAULTBOARD);
    b_vote();
    strcpy(currboard, buf);
    return 0;
}

int x_results(void)
{
    modify_user_mode(XMENU);
    return vote_results(DEFAULTBOARD);
}

int check_global_vote_status(char *bname)
{

    FILE *fp_control = NULL;
    FILE *fp_flag = NULL;
    int num_of_vote = 0;

    int callforvoted = 0;
    struct votebal vote_rec;
    struct ballot vote_ballot;
    char path[STRLEN];

    if (!HAS_PERM(PERM_LOGINOK))
        return 0;
    if (currentuser.stay < 1800)
        return 0;
    if ((time(0) - lookupuser.firstlogin) / 86400 < 10)
        return 0;
    memset(&vote_rec, 0, sizeof(vote_rec));
    memset(&vote_ballot, 0, sizeof(vote_ballot));
    strcpy(currboard, DEFAULTBOARD);
    setcontrolfile();
    num_of_vote = get_num_records(controlfile, sizeof(struct votebal));
    if (num_of_vote <= 0)
    {
        return 0;
    }
    fp_control = fopen(controlfile, "r");

    callforvoted = 0;
    while ((fread(&vote_rec, sizeof(struct votebal), 1, fp_control)) > 0)
    {
        sprintf(path, "vote/%s/flag.%ld", currboard, vote_rec.opendate);
        fp_flag = fopen(path, "r");
        if (fp_flag == NULL)
            return 1;
        while ((fread(&vote_ballot, sizeof(struct ballot), 1, fp_flag)) >
               0)
        {
            callforvoted = 1;
            if (!strcmp(vote_ballot.uid, currentuser.userid))
            {
                callforvoted = 0;
                break;
            }
        }
        fclose(fp_flag);
        fp_flag = NULL;
        if (callforvoted == 1)
            return 1;

    }
    fclose(fp_control);
    fp_control = NULL;
    return 0;
}

void show_global_vote(void)
{
    int voted = 0;
    char user_path[STRLEN];

    FILE *fp;

    clear();
    move(23, 0);
    voted = check_global_vote_status(DEFAULTBOARD);
    if (voted == 0)
        return;
    setuserfile(user_path, "novote_sjtubbs");
    if (dashf(user_path))
        return;

    if (askyn("ÕıÔÚ¾ÙĞĞÏµÍ³Í¶Æ±ÖĞ£¬ÒªÈ¥¿´¿´Âğ?", YEA, YEA) == NA)
    {
        clear();
        move(23, 0);
        if (askyn("Ï£ÍûÏÂ´ÎµÇÂ½Ê±ÏµÍ³²»ÔÙÌáÊ¾Âğ?", NA, NA) == YEA)
        {
            fp = fopen(user_path, "w");
            fclose(fp);
            fp = NULL;
        }
        return;
    }
    strcpy(currboard, DEFAULTBOARD);
    b_vote();
    return;
}

int enquire_anonymous(int ent, struct fileheader *fileinfo, char *direct, char *userid)
{
    char bname[STRLEN], descname[STRLEN], origname[STRLEN];
    struct votebal *ball = &currvote;
    char *t = NULL;
    int matching = NA;

    setbfile(genbuf, currboard, "deny_users");
    if (seek_in_denyfile(genbuf, fileinfo->owner) == 0)
    {
        sprintf(genbuf, "%s Î´±»·â½û£¬½«Ö»ÄÜ²éÑéÊÇ·ñÎªÖ¸¶¨ID¡£ĞèÒªÏÖÔÚ·â½ûÂğ", fileinfo->owner);
        if (askyn(genbuf, YEA, YEA) == YEA)
            return 1;
        matching = YEA;
        getdata(t_lines - 1, 0, "ÇëÊäÈë´ıÑéÖ¤µÄID: ", genbuf, 12, DOECHO, YEA);
    }
    if (askyn("ÏÖÔÚ¿ªÆôÍ¶Æ±ÉêÇë²éÑ¯IDÂğ", YEA, YEA) == NA)
        return 0;

    strcpy(ball->userid, currentuser.userid);
    ball->showip = 1;
    if (matching)
        sprintf(ball->title, "²éÑé %s ÊÇ·ñÎª %s", fileinfo->owner, genbuf);
    else
        sprintf(ball->title, "²éÑ¯ %s µÄÕæÊµÉí·İ", fileinfo->owner);
    ball->type = VOTE_SPECIAL;
    strcpy(ball->items[0], "Í¬Òâ²éÑ¯");
    strcpy(ball->items[1], "·´¶Ô²éÑ¯");
    strcpy(ball->items[30], "Anony_ID");
    if (matching)
        sprintf(ball->items[29], "%s %s %s", fileinfo->owner,
                strcmp(genbuf, userid) ? "²»ÊÇ" : "¾ÍÊÇ", genbuf);
    else
        sprintf(ball->items[29], "%s µÄÕæÊµÉí·İÊÇ %s",
                fileinfo->owner, userid);
    ball->maxdays = 7;
    ball->maxtkt = 1;
    ball->totalitems = 2;
    ball->opendate = time(0);
    ball->level = LISTMASK;

    strcpy(bname, currboard);
    strcpy(currboard, COMMENT_ADMIN_BOARDNAME);
    makevdir(currboard);
    setvfile(genbuf, currboard, "desc");
    sprintf(descname, "%s.%ld", genbuf, ball->opendate);
    strcpy(genbuf, direct);
    if ((t = strrchr(genbuf, '/')) != NULL)
        *t = '\0';
    sprintf(origname, "%s/%s/%s", genbuf, getfolderstr(fileinfo->filename), fileinfo->filename);
#if 0
    link(origname, descname);
#endif
    sprintf(genbuf, "cp %s %s", origname, descname);
    system(genbuf);
    
    setcontrolfile();
    setvoteflag(currboard, 1);
    move(t_lines - 1, 0);
    clrtoeol();
    if (append_record(controlfile, ball, sizeof(*ball)) == -1)
    {
        b_report("Append Control file Error!!");
        prints("·¢ÉúÑÏÖØµÄ´íÎó£¬ÎŞ·¨¿ªÆôÍ¶Æ±£¬ÇëÍ¨¸æÕ¾³¤! [1;33mÇë°´ ¡ô[5;36mEnter[m[1;33m¡ô ¼ÌĞø\033[m");
        getdata(t_lines - 1, 78, "", genbuf, 2, NOECHO, YEA);
    }
    else
    {
        char votename[STRLEN];
        int i;

        b_report("OPEN");
        sprintf(votename, "tmp/votetmp.%s.%05d", currentuser.userid, uinfo.pid);
        if ((sug = fopen(votename, "w")) != NULL)
        {
            sprintf(genbuf, "[Í¨Öª] %s ¾Ù°ìÍ¶Æ±£º%s", currboard, ball->title);
            get_result_title();
            fprintf(sug, "\n¡¾[1mÑ¡ÏîÈçÏÂ[m¡¿\n");
            for (i = 0; i < ball->totalitems; i++)
                fprintf(sug, "([1m%c[m) %-40s\n", 'A' + i, ball->items[i]);
            fclose(sug);
            Postfile(votename, currboard, genbuf, 1);
            unlink(votename);
        }
        prints("Í¶Æ±Ïä¿ªÆôÁË£¡ÇëÇ°Íù %s °åÍ¶Æ±...[1;33mÇë°´ ¡ô[5;36mEnter[m[1;33m¡ô ¼ÌĞø\033[m", currboard);
        getdata(t_lines - 1, 78, "", genbuf, 2, NOECHO, YEA);
    }
    strcpy(currboard, bname);
    return 0;
}

