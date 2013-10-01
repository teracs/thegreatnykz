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

#define BBS_PAGESIZE    (t_lines - 4)

#define PUTCURS   move(3+locmem->crs_line-locmem->top_line,0);prints(">");
#define RMVCURS   move(3+locmem->crs_line-locmem->top_line,0);prints(" ");

struct fileheader SR_fptr;
int SR_BMDELFLAG = NA;
char *pnt;

extern int noreply;
extern int local_article;

extern void show_announce_paths();
struct keeploc
{
    char *key;
    int top_line;
    int crs_line;
    struct keeploc *next;
};

char currdirect[STRLEN];
char keyword[STRLEN];
int screen_len;
int last_line;
int from_digestmode;
unsigned int digestmode_fileid;
char digestmode_filename[STRLEN];

struct keeploc *getkeep(char *s, int def_topline, int def_cursline)
{
    static struct keeploc *keeplist = NULL;
    struct keeploc *p;

    for (p = keeplist; p != NULL; p = p->next)
    {
        if (!strcmp(s, p->key))
        {
            if (p->crs_line < 1)
                p->crs_line = 1;
            return p;
        }
    }
    p = (struct keeploc *) malloc(sizeof(*p));
    p->key = (char *) malloc(strlen(s) + 1);
    strcpy(p->key, s);
    p->top_line = def_topline;
    p->crs_line = def_cursline;
    p->next = keeplist;
    keeplist = p;
    return p;
}

int locate_by_filename(struct keeploc *locmem, char * filename)
{
    int fd, pos = 0;
    int ssize = sizeof(struct fileheader);

    if ((fd = open(currdirect, O_RDONLY, 0)) == -1)
        return -1;
    while (read(fd, &SR_fptr, ssize) == ssize)
    {
        pos++;
        if (!strncmp(SR_fptr.filename,filename,STRLEN))
        {
            close(fd);
            return cursor_pos(locmem, pos, 10);
        }
    }
    return 0;
}
    
int locate_by_fileid(struct keeploc *locmem, unsigned int fileid)
{
    int fd, pos = 0;
    int ssize = sizeof(struct fileheader);

    if ((fd = open(currdirect, O_RDONLY, 0)) == -1)
        return -1;
    while (read(fd, &SR_fptr, ssize) == ssize)
    {
        pos++;
        if (SR_fptr.id == fileid)
        {
            close(fd);
            return cursor_pos(locmem, pos, 10);
        }
    }
    return 0;
}

void fixkeep(char *s, int first, int last)
{
    struct keeploc *k;

    k = getkeep(s, 1, 1);
    if (k->crs_line >= first)
    {
        k->crs_line = (first == 1 ? 1 : first - 1);
        k->top_line = (first < 11 ? 1 : first - 10);
    }
}

void modify_locmem(struct keeploc *locmem, int total)
{
    if (locmem->top_line > total)
    {
        locmem->crs_line = total;
        locmem->top_line = total - t_lines / 2;
        if (locmem->top_line < 1)
            locmem->top_line = 1;
    }
    else if (locmem->crs_line > total)
    {
        locmem->crs_line = total;
    }
}

int move_cursor_line(struct keeploc *locmem, int mode)
{

    int top, crs;
    int reload = 0;

    top = locmem->top_line;
    crs = locmem->crs_line;
    if (mode == READ_PREV)
    {
        if (crs <= top)
        {
            top -= screen_len - 1;
            if (top < 1)
                top = 1;
            reload = 1;
        }
        crs--;
        if (crs < 1)
        {
            crs = 1;
            reload = -1;
        }
    }
    else if (mode == READ_NEXT)
    {
        if (crs + 1 >= top + screen_len)
        {
            top += screen_len - 1;
            reload = 1;
        }
        crs++;
        if (crs > last_line)
        {
            crs = last_line;
            reload = -1;
        }
    }
    locmem->top_line = top;
    locmem->crs_line = crs;
    return reload;
}

void draw_title(void (*dotitle) (void))
{
    clear();
    (*dotitle) ();
}

void
draw_entry(char *(*doentry) (int, void *), struct keeploc *locmem, int num,
           int ssize)
{
    char *str;
    int base, i;

    base = locmem->top_line;
    move(3, 0);
    clrtobot();
    for (i = 0; i < num; i++)
    {
        str = (*doentry) (base + i, &pnt[i * ssize]);
        if (!check_stuffmode())
            prints("%s", str);
        else
            showstuff(str);
        prints("\n");
    }
    move(t_lines - 1, 0);
    clrtoeol();
    update_endline();
}

void
i_read(int cmdmode, char *direct, void (*dotitle) (void),
       char *(*doentry) (int, void *), struct one_key *rcmdlist, int ssize)
{
    extern int talkrequest;
    extern int friendflag;
    struct keeploc *locmem;
    char lbuf[11];
    char *ptr;
    int lbc, recbase, mode, ch;
    int num, entries;

    screen_len = t_lines - 4;
    modify_user_mode(cmdmode);
    ptr = pnt = calloc(screen_len, ssize);
    strcpy(currdirect, direct);
    draw_title(dotitle);
    last_line = get_num_records(currdirect, ssize);

    if (last_line == 0)
    {
        if (cmdmode == RMAIL)
        {
            prints("没有任何新信件...");
            pressreturn();
            clear();
        }
        else if (cmdmode == GMENU)
        {
            char desc[5];
            char buf[40];

            if (friendflag)
                strcpy(desc, "好友");
            else
                strcpy(desc, "坏人");
            sprintf(buf, "没有任何%s (A)新增%s (Q)离开？[Q] ", desc, desc);
            getdata(t_lines - 1, 0, buf, genbuf, 4, DOECHO, YEA);
            if (genbuf[0] == 'a' || genbuf[0] == 'A')
                (friendflag) ? friend_add(0, NULL, NULL) : reject_add(0,
                                                                      NULL,
                                                                      NULL);
        }
        else
        {
            getdata(t_lines - 1, 0, "看板新成立 (P)发表文章 (Q)离开？[Q] ",
                    genbuf, 4, DOECHO, YEA);
            if (genbuf[0] == 'p' || genbuf[0] == 'P')
                do_post();
        }
        free(pnt);
        return;
    }
    num = last_line - screen_len + 2;
    locmem = getkeep(currdirect, num < 1 ? 1 : num, last_line);
    modify_locmem(locmem, last_line);
    recbase = locmem->top_line;
    entries = get_records(currdirect, pnt, ssize, recbase, screen_len);
    draw_entry(doentry, locmem, entries, ssize);
    PUTCURS;
    lbc = 0;
    mode = DONOTHING;
    while ((ch = egetch()) != EOF)
    {
        if (talkrequest)
        {
            talkreply();
            mode = FULLUPDATE;
        }
        else if (ch >= '0' && ch <= '9')
        {
            if (lbc < 9)
                lbuf[lbc++] = ch;
        }
        else if (lbc > 0 && (ch == '\n' || ch == '\r'))
        {
            lbuf[lbc] = '\0';
            lbc = atoi(lbuf);
            if (cursor_pos(locmem, lbc, 10))
                mode = PARTUPDATE;
            lbc = 0;
        }
        else
        {
            lbc = 0;

            mode = i_read_key(rcmdlist, locmem, ch, ssize);

            while (mode == READ_NEXT || mode == READ_PREV)
            {
                int reload;

                reload = move_cursor_line(locmem, mode);
                if (reload == -1)
                {
                    mode = FULLUPDATE;
                    break;
                }
                else if (reload)
                {
                    recbase = locmem->top_line;
                    entries = get_records(currdirect, pnt, ssize,
                                          recbase, screen_len);
                    if (entries <= 0)
                    {
                        last_line = -1;
                        break;
                    }
                }
                num = locmem->crs_line - locmem->top_line;
                mode = i_read_key(rcmdlist, locmem, ch, ssize);
            }
            modify_user_mode(cmdmode);
        }
        if (mode == DOQUIT)
            break;
        if (mode == GOTO_NEXT)
        {
            cursor_pos(locmem, locmem->crs_line + 1, 1);
            mode = PARTUPDATE;
        }
        switch (mode)
        {
        case NEWDIRECT:
        case DIRCHANGED:
        case MODECHANGED:
            recbase = -1;

            if (mode == MODECHANGED)
            {

                if (uinfo.mode == GMENU)
                {
                    strcpy(currdirect, direct);
                }
                else
                {
                    setbdir(currdirect, currboard);
                }

                pnt = ptr;
            }
            last_line = get_num_records(currdirect, ssize);
            if (last_line == 0 && digestmode > 0)
            {
                acction_mode(0, NULL, NULL);
            }
            if (mode == NEWDIRECT)
            {

                if (from_digestmode&& uinfo.mode != RMAIL)
                {
                    locmem = getkeep(currdirect, 1, 1);
                    locate_by_fileid(locmem, digestmode_fileid);
                    from_digestmode = NA;
                    num = locmem->crs_line - locmem->top_line;
                }
                else if(from_digestmode)
                {
                    locmem = getkeep(currdirect, 1, 1);
                    locate_by_filename(locmem, digestmode_filename);
                    from_digestmode = NA;
                    num = locmem->crs_line - locmem->top_line;
                }
                else
                {
                    num = last_line - screen_len + 1;
                    locmem =
                        getkeep(currdirect, num < 1 ? 1 : num, last_line);
                }
            }
        case FULLUPDATE:
            draw_title(dotitle);
        case PARTUPDATE:
            if (last_line < locmem->top_line + screen_len)
            {
                num = get_num_records(currdirect, ssize);
                if (last_line != num)
                {
                    last_line = num;
                    recbase = -1;
                }
            }
            if (last_line == 0)
            {
                prints("No Messages\n");
                entries = 0;
            }
            else if (recbase != locmem->top_line)
            {
                recbase = locmem->top_line;
                if (recbase > last_line)
                {
                    recbase = last_line - screen_len / 2;
                    if (recbase < 1)
                        recbase = 1;
                    locmem->top_line = recbase;
                }
                entries = get_records(currdirect, pnt, ssize,
                                      recbase, screen_len);
            }
            if (locmem->crs_line > last_line)
                locmem->crs_line = last_line;
            draw_entry(doentry, locmem, entries, ssize);
            PUTCURS;
            break;
        default:
            break;
        }
        mode = DONOTHING;
        if (entries == 0)
            break;
    }
    clear();
    free(pnt);
}

int
i_read_key(struct one_key *rcmdlist, struct keeploc *locmem, int ch,
           int ssize)
{
    int i, mode = DONOTHING, savemode;
    char tch[4];
    extern int inmarkedmode;
    switch (ch)
    {
    case 'q':
    case 'e':
    case KEY_LEFT:
        if (digestmode && uinfo.mode != RMAIL)
        {
            from_digestmode = YEA;
            digestmode_fileid =
                ((struct fileheader *)
                 &pnt[(locmem->crs_line - locmem->top_line) * ssize])->id;
            return acction_mode(0, NULL, NULL);
        }
        else if(uinfo.mode == RMAIL&&inmarkedmode == YEA)
        {
            from_digestmode = YEA;
            strncpy(digestmode_filename,
                ((struct fileheader *)
                 &pnt[(locmem->crs_line - locmem->top_line) * ssize])->filename,
                 STRLEN); 
        	sprintf(currdirect, "mail/%c/%c/%s/%s", toupper(currentuser.userid[0]),
            toupper(currentuser.userid[1]), currentuser.userid,DOT_DIR);
        	inmarkedmode = NA;
        	return NEWDIRECT;
        }
        else
            return DOQUIT;
        break;
    case Ctrl('L'):
        redoscr();
        break;
    case 'M':
        savemode = uinfo.mode;
        m_new();
        modify_user_mode(savemode);
        return FULLUPDATE;
        break;
    case 'u':
        savemode = uinfo.mode;
        modify_user_mode(QUERY);
        t_query(NULL);
        modify_user_mode(savemode);
        return FULLUPDATE;
        break;
    case 'H':
        show_help("0Announce/bbslist/day");
        move(t_lines - 1, 0);
        tch[0] = '\0';
        getdata(t_lines - 1, 0, "查看分区十大:(1~b)区 0)取消 [0]:", tch, 3,
                DOECHO, YEA);
        char dList[] = "123456789ab";
        char top10file[64];

        while (tch[0] != '\0' && strchr(dList, tch[0]) != NULL)
        {
            sprintf(top10file, "0Announce/bbslist/new%ctop10", tch[0]);
            show_help(top10file);
            move(t_lines - 1, 0);
            tch[0] = '\0';
            getdata(t_lines - 1, 0, "查看分区十大:(1~b)区 0)取消 [0]:",
                    tch, 3, DOECHO, YEA);
        }
        return FULLUPDATE;
        break;
    case 'k':
    case KEY_UP:
        if (cursor_pos(locmem, locmem->crs_line - 1, screen_len - 1))
            return PARTUPDATE;
        break;
    case 'j':
    case KEY_DOWN:
        if (cursor_pos(locmem, locmem->crs_line + 1, 0))
            return PARTUPDATE;
        break;
    case 'l':
        show_allmsgs();
        return FULLUPDATE;
        break;
    case 'L':

        if (uinfo.mode == RMAIL)
            return DONOTHING;
        savemode = uinfo.mode;
        m_read();
        modify_user_mode(savemode);
        return MODECHANGED;
        break;
    case 'N':
    case Ctrl('F'):
    case KEY_PGDN:
    case ' ':
        if (last_line >= locmem->top_line + screen_len)
        {
            locmem->top_line += screen_len - 1;
            locmem->crs_line = locmem->top_line;
            return PARTUPDATE;
        }
        RMVCURS;
        locmem->crs_line = last_line;
        PUTCURS;
        break;
    case 'P':
    case Ctrl('B'):
    case KEY_PGUP:
        if (locmem->top_line > 1)
        {
            locmem->top_line -= screen_len - 1;
            if (locmem->top_line <= 0)
                locmem->top_line = 1;
            locmem->crs_line = locmem->top_line;
            return PARTUPDATE;
        }
        else
        {
            RMVCURS;
            locmem->crs_line = locmem->top_line;
            PUTCURS;
        }
        break;
    case KEY_HOME:
        locmem->top_line = 1;
        locmem->crs_line = 1;
        return PARTUPDATE;
        break;
    case '$':
    case KEY_END:
        i = get_topfile_num(currdirect, sizeof(struct fileheader));
        if (digestmode == 9)
            last_line =
                i + get_return_dir_num(currdirect,
                                       sizeof(struct fileheader));
        else
            last_line =
                i + get_dir_num(currdirect, sizeof(struct fileheader));
        if (last_line >= locmem->top_line + screen_len)
        {
            locmem->top_line = last_line - screen_len + 1;
            if (locmem->top_line <= 0)
                locmem->top_line = 1;
            locmem->crs_line = last_line - i;
            if (locmem->crs_line < locmem->top_line)
                locmem->crs_line = locmem->top_line;

            return PARTUPDATE;
        }
        locmem->crs_line = last_line - i;
        if (locmem->crs_line < locmem->top_line)
            locmem->crs_line = locmem->top_line;
        return PARTUPDATE;
        break;
    case 'S':
        if (HAS_PERM(PERM_MESSAGE))
        {
            s_msg();
            return FULLUPDATE;
        }
        break;
    case 'f':
        if (!HAS_PERM(PERM_BASIC))
            break;
        t_friends();
        return FULLUPDATE;
        break;
    case Ctrl('e'):
        if (!HAS_PERM(PERM_BASIC))
            break;
        t_watchinboard();
        return FULLUPDATE;
        break;
    case Ctrl('o'):
        edit_boardnote();
        return FULLUPDATE;
        break;
    case '!':
        return Goodbye();
        break;
    case Ctrl('v'):
        anywhere_lockscreen();
        return FULLUPDATE;
    case '\n':
    case '\r':
    case KEY_RIGHT:
        ch = 'r';
    default:
        for (i = 0; rcmdlist[i].fptr != NULL; i++)
        {
            if (rcmdlist[i].key == ch)
            {
                mode = (*(rcmdlist[i].fptr)) (locmem->crs_line,
                                              &pnt[(locmem->crs_line -
                                                    locmem->top_line) *
                                                   ssize], currdirect);
                break;
            }
        }
        break;
    }
    return mode;
}

int auth_search_down(int ent, struct fileheader *fileinfo, char *direct)
{
    struct keeploc *locmem;

    locmem = getkeep(direct, 1, 1);
    if (search_author(locmem, 1, fileinfo->owner))
        return PARTUPDATE;
    else
        update_endline();
    return DONOTHING;
}

int auth_search_up(int ent, struct fileheader *fileinfo, char *direct)
{
    struct keeploc *locmem;

    locmem = getkeep(direct, 1, 1);
    if (search_author(locmem, -1, fileinfo->owner))
        return PARTUPDATE;
    else
        update_endline();
    return DONOTHING;
}

int post_search_down(int ent, struct fileheader *fileinfo, char *direct)
{
    struct keeploc *locmem;

    locmem = getkeep(direct, 1, 1);
    if (search_post(locmem, 1))
        return PARTUPDATE;
    else
        update_endline();
    return DONOTHING;
}

int post_search_up(int ent, struct fileheader *fileinfo, char *direct)
{
    struct keeploc *locmem;

    locmem = getkeep(direct, 1, 1);
    if (search_post(locmem, -1))
        return PARTUPDATE;
    else
        update_endline();
    return DONOTHING;
}

int show_author(int ent, struct fileheader *fileinfo, char *direct)
{
    modify_user_mode(QAUTHOR);
    t_query(fileinfo->owner);
    return FULLUPDATE;
}

int SR_BMfunc(int ent, struct fileheader *fileinfo, char *direct)
{
    int i, dotype = 0, result = 0, full = 0;
    int has_yinyan = 0;
    char buf[80], ch[4], BMch, annpath[512];
    struct keeploc *locmem;
    char SR_BMitems[7][8] =
        { "删除", "保留", "文摘", "精华区", "暂存档", "不可RE", "合集" };
    char subBMitems[3][9] = { "相同主题", "相同作者", "相关主题" };

    if (digestmode != 0)
        return DONOTHING;
    if (!strcmp(currboard, "deleted") || !strcmp(currboard, "junk"))
        return DONOTHING;
    if (fileinfo->owner[0] == '-')
        return DONOTHING;
    if (!chk_currBM(currBM))
        return DONOTHING;
    saveline(t_lines - 1, 0);
    move(t_lines - 1, 0);
    clrtoeol();
    ch[0] = '\0';
    getdata(t_lines - 1, 0,
            "执行: 1) 相同主题  2) 相同作者 3) 相关主题 0) 取消 [0]: ",
            ch, 3, DOECHO, YEA);
    dotype = atoi(ch);
    if (dotype < 1 || dotype > 3)
    {
        saveline(t_lines - 1, 1);
        return DONOTHING;
    }
    sprintf(buf, "%s ", subBMitems[dotype - 1]);
    for (i = 0; i < 7; i++)
        sprintf(buf, "%s(%d)%s ", buf, i + 1, SR_BMitems[i]);
    strcat(buf, "? [0]: ");
    getdata(t_lines - 1, 0, buf, ch, 3, DOECHO, YEA);
    BMch = atoi(ch);
    if (BMch <= 0 || BMch > 7 || (digestmode > 1 && BMch == 3)
        || (digestmode == 1 && BMch == 3) || (digestmode > 2 && BMch < 3))
    {
        saveline(t_lines - 1, 1);
        return DONOTHING;
    }
    move(t_lines - 1, 0);
    sprintf(buf, "确定要执行%s[%s]吗", subBMitems[dotype - 1],
            SR_BMitems[BMch - 1]);
    if (askyn(buf, NA, NA) == 0)
    {
        saveline(t_lines - 1, 1);
        return DONOTHING;
    }
    if (BMch == 4)
    {
        FILE *fn;
        int opt;
        char msg[2];
        char fname[STRLEN];

	show_announce_paths();

        getdata(t_lines - 1, 0, "请输入操作丝路编号 1-4(默认为第一丝路):",
                msg, 2, DOECHO, YEA);

        if (msg[0] == '\n' || msg[0] == '\0')
            msg[0] = '1';
        opt = atoi(msg) - 1;
        if (!isdigit(msg[0]) || opt < 0 || opt > 3)
        {
            presskeyfor("输入错误, 请按<Enter>继续...", t_lines - 1);
            saveline(t_lines - 1, 1);
            return FULLUPDATE;
        }
        else
        {
            if (opt == 0)
            {
                sprintf(fname, ".announcepath");
            }
            else
            {
                sprintf(fname, ".announcepath%d", opt);
            }

	    if (!chk_ap(opt)){
	        presskeyfor("您不再拥有该丝路的权限，请重新设定丝路", t_lines - 1);
	        return 1;    
	    }

            sethomefile(annpath, currentuser.userid, fname);
            if ((fn = fopen(annpath, "r")) == NULL)
            {
                presskeyfor("对不起, 你没有设定丝路. 请先用 f 设定丝路.",
                            t_lines - 1);
                saveline(t_lines - 1, 1);
                return DONOTHING;
            }
            fscanf(fn, "%s", annpath);
            fclose(fn);
            if (!dashd(annpath))
            {
                presskeyfor("你设定的丝路已丢失, 请重新用 f 设定.",
                            t_lines - 1);
                saveline(t_lines - 1, 1);
                return DONOTHING;
            }
        }
    }
    else if (BMch == 5)
    {
        if (askyn("[收入暂存档模式: 一般/精简] 采用精简方式吗", YEA, YEA)
            == NA)
            full = 1;
    }

    if (BMch == 7)
    {
        move(t_lines - 1, 0);
        if (askyn("制作的合集需要引言吗？", YEA, NA) == YEA)
            has_yinyan = 1;
    }

    if (dotype == 3)
    {
        strcpy(keyword, "");
        getdata(t_lines - 1, 0, "请输入主题关键字: ", keyword, 50, DOECHO,
                YEA);
        if (keyword[0] == '\0')
        {
            saveline(t_lines - 1, 1);
            return DONOTHING;
        }
    }
    else if (dotype == 1)
    {
        strcpy(keyword, fileinfo->title);
    }
    else
    {
        strcpy(keyword, fileinfo->owner);
    }

    if (BMch == 7)
    {
        sprintf(buf, "tmp/%s.combine", currentuser.userid);
        if (dashf(buf))
            unlink(buf);
    }

    move(t_lines - 1, 0);
    sprintf(buf, "是否从%s第一篇开始%s (Y)第一篇 (N)目前这一篇",
            (dotype == 2) ? "该作者" : "此主题", SR_BMitems[BMch - 1]);
    if (askyn(buf, YEA, NA) == YEA)
    {
        result = locate_the_post(fileinfo, keyword, 5, dotype - 1, 0);
    }
    else if (dotype == 3)
    {
        result = locate_the_post(fileinfo, keyword, 1, 2, 0);
    }
    else
    {
        memcpy(&SR_fptr, fileinfo, sizeof(SR_fptr));
    }
    if (result == -1)
    {
        saveline(t_lines - 1, 1);
        return DONOTHING;
    }
    while (1)
    {
        locmem = getkeep(currdirect, 1, 1);
        switch (BMch)
        {
        case 1:
            SR_BMDELFLAG = YEA;
            result = del_post(locmem->crs_line, &SR_fptr, currdirect);
            SR_BMDELFLAG = NA;
            if (result == -1)
                return DIRCHANGED;
            if (result != DONOTHING)
            {
                last_line--;
                locmem->crs_line--;
            }
            break;
        case 2:
            mark_post(locmem->crs_line, &SR_fptr, currdirect);
            break;
        case 3:
            digest_post(locmem->crs_line, &SR_fptr, currdirect);
            break;
        case 4:
            SR_fptr.accessed[0] |= FILE_VISIT;
            substitute_record(direct, &SR_fptr, sizeof(SR_fptr),
                              locmem->crs_line);
            a_Import("0Announce", currboard, &SR_fptr, annpath, YEA);
            break;
        case 5:
            a_Save("0Announce", currboard, &SR_fptr, YEA, full);
            break;
        case 6:
            underline_post(locmem->crs_line, &SR_fptr, currdirect);
            break;

        case 7:
            Add_Combine(currboard, &SR_fptr, has_yinyan);
            break;

        }
        if (locmem->crs_line <= 0)
        {
            result = locate_the_post(fileinfo, keyword, 5, dotype - 1, 0);
        }
        else
        {
            result = locate_the_post(fileinfo, keyword, 1, dotype - 1, 0);
        }
        if (result == -1)
            break;
    }

    if (BMch == 7)
    {
        if ((dotype == 1 || dotype == 3))
        {
            if (!strncmp(keyword, "Re: ", 4)
                || !strncmp(keyword, "RE: ", 4))
            {
                sprintf(buf, "【合集】%s", keyword + 4);
            }
            else
                sprintf(buf, "【合集】%s", keyword);
        }
        else
        {
            sprintf(buf, "【合集】%s 的个人文集", keyword);
        }
        strcpy(keyword, buf);
        sprintf(buf, "tmp/%s.combine", currentuser.userid);
        Postfile(buf, currboard, keyword, 2);
        unlink(buf);
    }

    return DIRCHANGED;
}

int SR_first_new(int ent, struct fileheader *fileinfo, char *direct)
{
    if (locate_the_post(fileinfo, fileinfo->title, 5, 0, 1) != -1)
    {
        sread(1, 0, &SR_fptr);
        return FULLUPDATE;
    }
    return PARTUPDATE;
}

int SR_last(int ent, struct fileheader *fileinfo, char *direct)
{
    locate_the_post(fileinfo, fileinfo->title, 3, 0, 0);
    return PARTUPDATE;
}

int SR_first(int ent, struct fileheader *fileinfo, char *direct)
{
    locate_the_post(fileinfo, fileinfo->title, 5, 0, 0);
    return PARTUPDATE;
}

int SR_read(int ent, struct fileheader *fileinfo, char *direct)
{
    sread(1, 0, fileinfo);
    return FULLUPDATE;
}

int SR_author(int ent, struct fileheader *fileinfo, char *direct)
{
    sread(1, 1, fileinfo);
    return FULLUPDATE;
}

int search_author(struct keeploc *locmem, int offset, char *powner)
{
    static char author[IDLEN + 1];
    char ans[IDLEN + 1], pmt[STRLEN];
    char currauth[STRLEN];
    char *temp = NULL;
    int length_author = 0;

    strcpy(currauth, powner);

    temp = strstr(currauth, " (");
    if (temp != NULL)
    {
        length_author = temp - currauth;
        memset(temp, 0x00, STRLEN - length_author);
    }

    sprintf(pmt, "%s的文章搜寻作者 [%s]: ",
            offset > 0 ? "往後来" : "往先前", currauth);
    move(t_lines - 1, 0);
    clrtoeol();

    getdata(t_lines - 1, 0, pmt, ans, IDLEN + 1, DOECHO, YEA);
    if (ans[0] != '\0')
        strcpy(author, ans);
    else
        strcpy(author, currauth);

    return search_articles(locmem, author, 0, offset, 1, 0);
}

int auth_post_down(int ent, struct fileheader *fileinfo, char *direct)
{
    struct keeploc *locmem;

    locmem = getkeep(direct, 1, 1);
    if (search_author(locmem, 1, fileinfo->owner))
        return PARTUPDATE;
    else
        update_endline();
    return DONOTHING;
}

int auth_post_up(int ent, struct fileheader *fileinfo, char *direct)
{
    struct keeploc *locmem;

    locmem = getkeep(direct, 1, 1);
    if (search_author(locmem, -1, fileinfo->owner))
        return PARTUPDATE;
    else
        update_endline();
    return DONOTHING;
}

int t_search_down(int ent, struct fileheader *fileinfo, char *direct)
{
    struct keeploc *locmem;

    locmem = getkeep(direct, 1, 1);
    if (search_title(locmem, 1))
        return PARTUPDATE;
    else
        update_endline();
    return DONOTHING;
}

int t_search_up(int ent, struct fileheader *fileinfo, char *direct)
{
    struct keeploc *locmem;

    locmem = getkeep(direct, 1, 1);
    if (search_title(locmem, -1))
        return PARTUPDATE;
    else
        update_endline();
    return DONOTHING;
}

int thread_up(int ent, struct fileheader *fileinfo, char *direct)
{
    struct keeploc *locmem;

    locmem = getkeep(direct, 1, 1);
    if (search_thread(locmem, -1, fileinfo->title, fileinfo->reid))
    {
        update_endline();
        return PARTUPDATE;
    }
    update_endline();
    return DONOTHING;
}

int thread_down(int ent, struct fileheader *fileinfo, char *direct)
{
    struct keeploc *locmem;

    locmem = getkeep(direct, 1, 1);
    if (search_thread(locmem, 1, fileinfo->title, fileinfo->reid))
    {
        update_endline();
        return PARTUPDATE;
    }
    update_endline();
    return DONOTHING;
}

int search_post(struct keeploc *locmem, int offset)
{
    static char query[STRLEN];
    char ans[STRLEN], pmt[STRLEN];

    strcpy(ans, query);
    sprintf(pmt, "搜寻%s的文章 [%s]: ", offset > 0 ? "往後来" : "往先前",
            ans);
    move(t_lines - 1, 0);
    clrtoeol();
    getdata(t_lines - 1, 0, pmt, ans, 50, DOECHO, YEA);
    if (ans[0] != '\0')
        strcpy(query, ans);

    return search_articles(locmem, query, 0, offset, -1, 0);
}

int search_title(struct keeploc *locmem, int offset)
{
    static char title[STRLEN];
    char ans[STRLEN], pmt[STRLEN];

    strcpy(ans, title);
    sprintf(pmt, "%s搜寻标题 [%.16s]: ", offset > 0 ? "往後" : "往前",
            ans);
    move(t_lines - 1, 0);
    clrtoeol();
    getdata(t_lines - 1, 0, pmt, ans, 46, DOECHO, YEA);
    if (*ans != '\0')
        strcpy(title, ans);
    return search_articles(locmem, title, 0, offset, 2, 0);
}

int
search_thread(struct keeploc *locmem, int offset, char *title,
              unsigned int reid)
{

    if (title[0] == 'R' && (title[1] == 'e' || title[1] == 'E')
        && title[2] == ':')
        title += 4;
    setqtitle(title);
    return search_articles(locmem, title, reid, offset, 0, 0);
}

int sread(int readfirst, int auser, struct fileheader *ptitle)
{
    struct keeploc *locmem;
    int rem_top, rem_crs, ch;
    int isend = 0, isstart = 0, isnext = 1;
    char tempbuf[STRLEN], title[STRLEN];

    if (readfirst)
    {
        isstart = 1;
    }
    else
    {
        isstart = 0;
        move(t_lines - 1, 0);
        clrtoeol();
        prints
            ("\033[0;1;44;31m[%8s] \033[33m下一封 <Space>,<Enter>,↓│上一封 ↑,U                              \033[m",
             auser ? "相同作者" : "主题阅读");
        switch (egetch())
        {
        case ' ':
        case '\n':
        case KEY_DOWN:
        case ']':
            isnext = 1;
            break;
        case KEY_UP:
        case 'u':
        case 'U':
        case '[':
            isnext = -1;
            break;
        default:
            isend = 1;
            break;
        }
    }
    locmem = getkeep(currdirect, 1, 1);
    rem_top = locmem->top_line;
    rem_crs = locmem->crs_line;
    if (auser == 0)
    {
        strcpy(title, ptitle->title);

        setqreid(ptitle->reid);
    }
    else
    {
        strcpy(title, ptitle->owner);
        setqtitle(ptitle->title);
    }
    if (!strncmp(title, "Re: ", 4))
    {
        strcpy(title, title + 4);
    }
    memcpy(&SR_fptr, ptitle, sizeof(SR_fptr));
    while (!isend)
    {
        if (!isstart)
        {
            if (search_articles
                (locmem, title, ptitle->reid, isnext, auser, 0) == -1)
                break;
        }
        if (uinfo.mode != RMAIL)
            setbfile(tempbuf, currboard, SR_fptr.filename);
        else
            sprintf(tempbuf, "mail/%c/%c/%s/%s",
                    toupper(currentuser.userid[0]),
                    toupper(currentuser.userid[1]), currentuser.userid,
                    SR_fptr.filename);
        setquotefile(tempbuf);
        if (auser)
            ch = ansimore_withmode(tempbuf, NA, MORE_MODE_AUSER);
        else
            ch = ansimore_withmode(tempbuf, NA, MORE_MODE_SREAD);
        brc_addlist(SR_fptr.filename);
        isstart = 0;
        if (ch == KEY_UP || ch == 'u' || ch == 'U')
        {
            readfirst = (ch == KEY_UP);
            isnext = -1;
            continue;
        }
        move(t_lines - 1, 0);
        clrtoeol();
        prints
            ("\033[0;1;44;31m[%8s] \033[33m回信 R │ 结束 Q,← │下一封 ↓,Enter│上一封 ↑,U │ ^R 回给作者   \033[m",
             auser ? "相同作者" : "主题阅读");
        if (ch == 0)
            ch = egetch();
        switch (ch)
        {
        case KEY_LEFT:
        case 'N':
        case 'Q':
        case 'n':
        case 'q':
            isend = 1;
            break;
        case 'Y':
#if 0

        case 'R':
            {
                struct shortfile *bp;
                extern struct shortfile *getbcache(char *bname);

                bp = getbcache(currboard);
                noreply = ptitle->accessed[0] & FILE_NOREPLY
                    || bp->flag & NOREPLY_FLAG;
                if (!noreply || HAS_PERM(PERM_SYSOP) || chk_currBM(currBM))
                {
                    local_article = !(ptitle->filename[STRLEN - 1] == 'S');

                    do_reply_push(ptitle);

                }
                else
                {
                    clear();
                    move(5, 6);
                    prints
                        ("对不起, 该文章有不可 RE 属性, 你不能回复(RE) 这篇文章.");
                    pressreturn();
                }
                break;
            }
#endif
        case 'y':
        case 'r':
        case 'R':
            {
                struct shortfile *bp;
                extern struct shortfile *getbcache(char *bname);

                bp = getbcache(currboard);
                noreply = ptitle->accessed[0] & FILE_NOREPLY || ptitle->accessed[1] & FILE_NOREPLY2
                    || bp->flag & NOREPLY_FLAG;
                if (!noreply || HAS_PERM(PERM_SYSOP) || chk_currBM(currBM))
                {
                    local_article = !(ptitle->filename[STRLEN - 1] == 'S');
                    do_reply(ptitle->title, ptitle->owner, ptitle->reid);
                }
                else
                {
                    clear();
                    move(5, 6);
                    prints
                        ("对不起, 该文章有不可 RE 属性, 你不能回复(RE) 这篇文章.");
                    pressreturn();
                }
                break;
            }
        case ' ':
        case '\n':
        case KEY_DOWN:
        case ']':
            readfirst = (ch == KEY_DOWN);
            isnext = 1;
            break;
        case Ctrl('A'):
            clear();
            show_author(0, &SR_fptr, currdirect);
            isnext = 1;
            break;
        case KEY_UP:
        case 'u':
        case 'U':
        case '[':
            readfirst = (ch == KEY_UP);
            isnext = -1;
            break;
        case Ctrl('R'):
            post_reply(0, &SR_fptr, (char *) NULL);
            break;
        case 'g':
            digest_post(0, &SR_fptr, currdirect);
            break;
        default:
            break;
        }
    }
    if (readfirst == 0)
    {
        RMVCURS;
        locmem->top_line = rem_top;
        locmem->crs_line = rem_crs;
        PUTCURS;
    }
    return 1;
}

int searchpattern(char *filename, char *query)
{
    FILE *fp;
    char buf[256];

    if ((fp = fopen(filename, "r")) == NULL)
        return 0;
    while (fgets(buf, 256, fp) != NULL)
    {
        if (strstr(buf, query))
        {
            fclose(fp);
            return YEA;
        }
    }
    fclose(fp);
    return NA;
}

int
search_articles(struct keeploc *locmem, char *query, unsigned int reid,
                int offset, int aflag, int newflag)
{
    char *ptr;
    char *temp_ptr = NULL;

    int length_ptr = 0;

    int ent, fd;
    int complete_search, oldent, lastent = 0;
    int ssize = sizeof(struct fileheader);

    int search_auther_in_mail = 0;

    if ((aflag == 1) && (newflag == 0))
    {
        search_auther_in_mail = 1;
    }

    if (*query == '\0')
        return 0;
    if (aflag >= 2)
    {
        complete_search = 0;
        aflag -= 2;

    }
    else if (aflag == 1)
    {
        complete_search = 0;

    }
    else
    {
        complete_search = 1;
    }
    if (aflag != -1 && offset > 2)
    {
        ent = 0;
        oldent = 0;
        offset -= 4;
    }
    else
    {
        ent = locmem->crs_line;
        oldent = locmem->crs_line;
    }
    move(t_lines - 1, 0);
    clrtoeol();
    prints
        ("\033[1;44;33m搜寻中，请稍候....                                                             \033[m");
    refresh();
    if ((fd = open(currdirect, O_RDONLY, 0)) == -1)
        return -1;
    if (aflag != -1 && offset < 0)
        ent = 0;
    if (ent)
    {
        if (aflag == -1 && offset < 0)
            ent -= 2;
        if (ent < 0 || lseek(fd, (off_t) (ent * ssize), SEEK_SET) == -1)
        {
            close(fd);
            return -1;
        }
    }
    if (aflag != -1 && offset > 0)
        ent = oldent;
    if (aflag == -1 && offset < 0)
        ent += 2;
    while (read(fd, &SR_fptr, ssize) == ssize)
    {
        if (aflag == -1 && offset < 0)
            ent--;
        else
            ent++;
        if (aflag != -1 && offset < 0 && oldent > 0 && ent >= oldent)
            break;
        if (newflag && !brc_unread(SR_fptr.filename))
            continue;
        if (aflag == -1)
        {
            char p_name[256];

            if (uinfo.mode != RMAIL)
                setbfile(p_name, currboard, SR_fptr.filename);
            else
                sprintf(p_name, "mail/%c/%c/%s/%s",
                        toupper(currentuser.userid[0]),
                        toupper(currentuser.userid[1]), currentuser.userid,
                        SR_fptr.filename);
            if (searchpattern(p_name, query))
            {
                lastent = ent;
                break;
            }
            else if (offset > 0)
                continue;
            else
            {
                if (lseek(fd, (off_t) (-2 * ssize), SEEK_CUR) == -1)
                {
                    close(fd);
                    return -1;
                }
                continue;
            }
        }
        ptr = aflag ? SR_fptr.owner : SR_fptr.title;

        if (complete_search == 1)
        {

            if ((aflag != 0) || (reid == 0))
            {
                if ((*ptr == 'R' || *ptr == 'r')
                    && (*(ptr + 1) == 'E' || *(ptr + 1) == 'e')
                    && (*(ptr + 2) == ':') && (*(ptr + 3) == ' '))
                {
                    ptr = ptr + 4;
                }

                if (!strncmp(ptr, query, 40))
                {
                    lastent = ent;
                    if (offset > 0)
                        break;
                }
            }

            else
            {
                if (SR_fptr.reid == reid)
                {
                    lastent = ent;
                    if (offset > 0)
                        break;
                }
            }
        }
        else
        {
            char upper_ptr[STRLEN], upper_query[STRLEN];

            strtoupper(upper_ptr, ptr);
            strtoupper(upper_query, query);

            if ((uinfo.mode == RMAIL) && (search_auther_in_mail == 1))
            {
                temp_ptr = strstr(ptr, " (");
                if (temp_ptr != NULL)
                {
                    length_ptr = temp_ptr - ptr;
                    memset(upper_ptr + length_ptr, 0x00,
                           STRLEN - length_ptr);
                }
            }

            if (strstr(upper_ptr, upper_query) != NULL)
            {
                if (aflag)
                {
                    if (strcmp(upper_ptr, upper_query))
                        continue;
                }
                lastent = ent;
                if (offset > 0)
                    break;
            }
        }
    }
    move(t_lines - 1, 0);
    clrtoeol();
    close(fd);
    if (lastent == 0)
        return -1;
    get_record(currdirect, &SR_fptr, sizeof(SR_fptr), lastent);
    last_line = get_num_records(currdirect, ssize);
    return (cursor_pos(locmem, lastent, 10));
}

int
locate_the_post(struct fileheader *fileinfo, char *query, int offset,
                int aflag, int newflag)
{
    struct keeploc *locmem;

    locmem = getkeep(currdirect, 1, 1);

    setqreid(fileinfo->reid);

    if (query[0] == 'R' && (query[1] == 'e' || query[1] == 'E')
        && query[2] == ':')
        query += 4;

    if ((aflag != 0) || (fileinfo->reid == 0))
    {
        setqtitle(query);
        return search_articles(locmem, query, fileinfo->reid, offset,
                               aflag, newflag);
    }
    else
    {
        setqreid(fileinfo->reid);
        return search_articles(locmem, query, fileinfo->reid, offset,
                               aflag, newflag);
    }
}

int cursor_pos(struct keeploc *locmem, int val, int from_top)
{
    if (val > last_line)
    {
        val = DEFINE(DEF_CIRCLE) ? 1 : last_line;
    }
    if (val <= 0)
    {
        val = DEFINE(DEF_CIRCLE) ? last_line : 1;
    }

    if (val >= locmem->top_line && val < locmem->top_line + screen_len)
    {
        RMVCURS;
        locmem->crs_line = val;
        PUTCURS;
        return 0;
    }
    locmem->top_line = val - from_top;
    if (locmem->top_line <= 0)
        locmem->top_line = 1;
    locmem->crs_line = val;
    return 1;
}

int r_searchall(void)
{
    char id[20], patten[30], buf[5];
    char ans[5];
    int dt;
    int all;

    all = NA;
    modify_user_mode(QUERY);
    clear();
    usercomplete("请输入您想查询的作者帐号: ", id);
    if (id[0] == 0)
    {
        getdata(0, 30, "查询所有的作者吗?[Y/N]: ", ans, 7, DOECHO, YEA);
        if ((*ans != 'Y') && (*ans != 'y'))
        {
            return 0;
        }
        else
            all = YEA;
    }
    else if (!getuser(id))
    {
        prints("不正确的使用者代号\n");
        pressreturn();
        return 0;
    }
    getdata(1, 0, "请输入文章标题关键字: ", patten, 29, DOECHO, YEA);
    getdata(2, 0, "查询距今多少天以内的文章?: ", buf, 4, DOECHO, YEA);
    dt = atoi(buf);
    if (dt == 0)
        return 0;
    searchallboard(id, patten, dt, all, 0);
    report("网友大作查询");
    return 0;
}

int r_searchalldel(void)
{
    char id[20], patten[30], buf[5];
    char ans[5];
    int dt;
    int all;

    all = NA;
    modify_user_mode(QUERY);
    clear();
    usercomplete("请输入您想查询的作者帐号: ", id);
    if (id[0] == 0)
    {
        getdata(0, 30, "查询所有的作者吗?[Y/N]: ", ans, 7, DOECHO, YEA);
        if ((*ans != 'Y') && (*ans != 'y'))
        {
            return 0;
        }
        else
            all = YEA;
    }
    else if (!getuser(id))
    {
        prints("不正确的使用者代号\n");
        pressreturn();
        return 0;
    }
    getdata(1, 0, "请输入文章标题关键字: ", patten, 29, DOECHO, YEA);
    getdata(2, 0, "查询距今多少天以内的文章?: ", buf, 4, DOECHO, YEA);
    dt = atoi(buf);
    if (dt == 0)
        return 0;
    searchallboard(id, patten, dt, all, 1);
    report("网友大作查询");
    return 0;
}

int searchallboard(char *id, char *patten, int dt, int all, int del)
{
    FILE *fp, *fp2, *fp3;
    char f[100], buf2[150];
    char fname[STRLEN];
    char INDEX[20];
    struct fileheader xx;
    struct boardheader xx2;
    int counts = 0, n2 = 0, n3, now;
    long t0;

    if (del)
        strcpy(INDEX, ".RETURNDIR");
    else
        strcpy(INDEX, ".DIR");
    now = time(0);
    fp2 = fopen(".BOARDS", "r");
    sprintf(fname, "tmp/searchall.%s.%05d", currentuser.userid, uinfo.pid);
    fp3 = fopen(fname, "w");
    fprintf(fp3, "在所有板查询%s网友%d天以内的大作, 关键字'%s'.\n\n", id,
            dt, patten);
    dt = dt * 86400;
    while (!feof(fp2))
    {
        fread(&xx2, sizeof(xx2), 1, fp2);
        if (xx2.level & PERM_POSTMASK || HAS_PERM(xx2.level)
            || (xx2.level & PERM_NOZAP))
        {
            int n = 0;

            sprintf(f, "boards/%s/%s", xx2.filename, INDEX);

            if ((fp = fopen(f, "r")) != NULL)
            {
                n2 = 0;
                while (fread(&xx, sizeof(xx), 1, fp) > 0)
                {
                    n++;
                    t0 = atoi(xx.filename + 2);
                    if ((all == YEA || !strcasecmp(xx.owner, id)) &&
                        strstr(xx.title, patten) && (abs(now - t0) < dt))
                    {
                        counts++;
                        n2++;
                        sprintf(buf2, " %4d  %24s  [[Post:%s,M.%d.A|%s]]\n", n,
                                ctime(&t0),xx2.filename,xx.id,xx.title);
                        for (n3 = 0; n3 < 90; n3++)
                            if (buf2[n3] == 10)
                                buf2[n3] = 32;
                        fprintf(fp3, "%s\n", buf2);
                    }
                }
                fclose(fp);
                if (n2 != 0)
                    fprintf(fp3, "Above %d is found in board %s.\n\n", n2,
                            xx2.filename);
            }
        }
        if (counts >= 1000)
            break;
    }
    fprintf(fp3, "%d matched found.\n", counts);
    fclose(fp2);
    fclose(fp3);
    mail_file(fname, currentuser.userid, "网友大作查询结果");
    unlink(fname);
    return 0;
}

#ifdef MAIL_COMBINE
int Mail_combine(int ent, struct fileheader *fileinfo, char *direct)
{
    int result = 0;
    char buf[STRLEN];

    if (!HAS_PERM(PERM_MAILINFINITY)
        && (getmailboxsize(currentuser.userid, currentuser.userlevel) * 2 <
            getmailsize(currentuser.userid)))
    {
        return DONOTHING;
    }

    saveline(t_lines - 1, 0);
    move(t_lines - 1, 0);
    if (askyn("确定要执行相同主题合集吗?", YEA, NA) == 0)
    {
        saveline(t_lines - 1, 1);
        return DONOTHING;
    }
    strcpy(keyword, fileinfo->title);

    sprintf(buf, "tmp/%s.combine", currentuser.userid);
    if (dashf(buf))
        unlink(buf);

    move(t_lines - 1, 0);
    sprintf(buf, "是否从此主题第一篇开始合集 (Y)第一篇 (N)目前这一篇");
    if (askyn(buf, YEA, NA) == YEA)
    {
        result = locate_the_post(fileinfo, keyword, 5, 0, 0);
    }
    else
    {
        memcpy(&SR_fptr, fileinfo, sizeof(SR_fptr));
    }
    if (result == -1)
    {
        saveline(t_lines - 1, 1);
        return DONOTHING;
    }

    while (1)
    {
        FILE *fp;
        char temp2[1024];
        char buf[STRLEN];

        sprintf(buf, "tmp/%s.combine", currentuser.userid);

        fp = fopen(buf, "at");

        fprintf(fp,
                "\033[1;35m☆──────────────────────────────────────☆\033[m\n");
        {
            FILE *fp1;
            char *s_ptr, *e_ptr;
            int blankline = 0;
            char filename[80];
            struct fileheader *fileinfo;

            fileinfo = &SR_fptr;
            sprintf(filename, "mail/%c/%c/%s/%s",
                    toupper(currentuser.userid[0]),
                    toupper(currentuser.userid[1]), currentuser.userid,
                    fileinfo->filename);

            fp1 = fopen(filename, "rt");
            if (fgets(temp2, 256, fp1) != NULL)
            {
                if (!strstr(temp2, "寄信人"))
                    fputs(temp2, fp);
                else
                {
                    e_ptr = strchr(temp2, '\n');
                    if (e_ptr != NULL)
                        *(e_ptr) = '\0';
                    s_ptr = &temp2[7];
                    fprintf(fp, "    \033[0;1;33m%s \033[m", s_ptr);
                    fgets(temp2, 256, fp1);
                    if (fgets(temp2, 256, fp1) != NULL)
                    {
                        e_ptr = strchr(temp2, ')');
                        if (e_ptr != NULL)
                        {
                            *(e_ptr) = '\0';
                            s_ptr = strchr(temp2, '(');
                            if (s_ptr == NULL)
                                s_ptr = temp2;
                            else
                                s_ptr++;
                            fprintf(fp, "于 \033[1;34m%s\033[m 提到：\n",
                                    s_ptr);
                        }
                    }
                }
            }
            fgets(temp2, 256, fp1);
            while (!feof(fp1))
            {
                fgets(temp2, 256, fp1);
                if ((unsigned) *temp2 < '\x1b')
                {
                    if (blankline)
                        continue;
                    else
                        blankline = 1;
                }
                else
                    blankline = 0;

                if (strncmp(temp2, "--", 2) == 0)
                    break;
                fputs(temp2, fp);
            }
            fclose(fp1);
        }

        fprintf(fp, "\n");
        fclose(fp);
        result = locate_the_post(fileinfo, keyword, 1, 0, 0);
        if (result == -1)
            break;
    }

    if (!strncmp(keyword, "Re: ", 4) || !strncmp(keyword, "RE: ", 4))
        sprintf(buf, "【合集】%s", keyword + 4);
    else
        sprintf(buf, "【合集】%s", keyword);
    strcpy(keyword, buf);
    sprintf(buf, "tmp/%s.combine", currentuser.userid);
    mail_file(buf, currentuser.userid, keyword);
    unlink(buf);

    return DIRCHANGED;
}
#endif

void clear_del(void)
{
    char buf[STRLEN];
    int num;
    int realdel = 0;

    setbfile(buf, currboard, RETURN_DIR);
    num = get_num_records(buf, sizeof(struct fileheader));
    digestmode = 9;
    if (num > 0)
        delete_range(buf, 1, num, &realdel);
    digestmode = 0;
    return;
}

int r_deleteall(void)
{
    char id[20];
    char tempbuf[128];
    int id_num = 0;
    char warning[128];

    modify_user_mode(ADMIN);
    clear();
    usercomplete("请输入接受处罚的帐号: ", id);
    if (id[0] == 0)
    {
        return 0;
    }

    else if (!(id_num = getuser(id)))
    {
        prints("不正确的使用者代号\n");
        pressreturn();
        return 0;
    }

    move(3, 0);
    sprintf(warning, "你确实要删除 \033[1;31m%s \033[m的所有文章吗?", id);
    if (askyn(warning, NA, NA) == NA)
    {
        return 0;
    }

    move(5, 0);
    if (askyn("删除以后很难恢复所有文章。你真的要删除吗?", NA, NA) == NA)
    {
        return 0;
    }

    move(7, 0);
    prints("正在删除，请等待……");
    deleteallboard(id);

    lookupuser.numposts = 0;
    substitute_record(PASSFILE, &lookupuser, sizeof(struct userec),
                      id_num);

    move(9, 0);
    prints("删除完成！");

    sprintf(tempbuf, "删除 %s 的全站文章", id);
    securityreport(tempbuf);
    pressanykey();
    return 0;
}

int deleteallboard(char *id)
{
    FILE *fpboards, *fp_tmp, *fp_return;
    int nfpdir = 0;
    char f[100];
    char f_bak[100];
    char tmpfile[100];
    char returnpath[100];
    char INDEX[20];
    char INDEX_BAK[20];
    char tmp[30];
    char RETURNBIN[30];

    struct fileheader file_head;
    struct boardheader b_head;

    int find;
    int result = 0;
    char boardpath[2 * STRLEN];

    strcpy(INDEX, ".DIR");
    strcpy(INDEX_BAK, ".DIR_BAK");
    strcpy(RETURNBIN, ".RETURNDIR");
    strcpy(tmp, "tmp");

    fpboards = fopen(".BOARDS", "r");

    while (fread(&b_head, sizeof(b_head), 1, fpboards) > 0)
    {
        if (b_head.level & PERM_POSTMASK || HAS_PERM(b_head.level)
            || (b_head.level & PERM_NOZAP))
        {

            sprintf(f, "boards/%s/%s", b_head.filename, INDEX);
            sprintf(f_bak, "boards/%s/%s", b_head.filename, INDEX_BAK);
            sprintf(tmpfile, "boards/%s/%s", b_head.filename, tmp);
            sprintf(returnpath, "boards/%s/%s", b_head.filename,
                    RETURNBIN);

            memset(boardpath, 0x00, 2 * STRLEN);
            sprintf(boardpath, "boards/%s", b_head.filename);
            find = 0;

            if ((nfpdir = open(f, O_RDWR, 0644)) != -1)
            {
                flock(nfpdir, LOCK_EX);
                fp_tmp = fopen(tmpfile, "w");
                fp_return = fopen(returnpath, "a");
                while (read(nfpdir, &file_head, sizeof(file_head)) > 0)
                {
                    result = 0;
                    if ((!strcmp(file_head.owner, id)) &&
                        (!(file_head.
                           accessed[0] & (FILE_TOP1 | FILE_MARKED |
                                          FILE_DIGEST))))
                    {
                        file_head.accessed[0] = 0;
                        file_head.accessed[0] |= FILE_DELETED;
                        fwrite(&file_head, sizeof(struct fileheader), 1,
                               fp_return);
                    }
                    else
                    {
                        fwrite(&file_head, sizeof(struct fileheader), 1,
                               fp_tmp);
                    }
                }
                flock(nfpdir, LOCK_UN);
                fclose(fp_tmp);
                fclose(fp_return);
                close(nfpdir);

                unlink(f_bak);
                rename(f, f_bak);
                rename(tmpfile, f);
            }
        }
    }

    fclose(fpboards);

    return 0;
}
