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

extern struct postheader header;

#include "bbs.h"

int G_SENDMODE = NA;

#ifdef MAIL_COMBINE
int Mail_combine(int ent, struct fileheader *fileinfo, char *direct);
#endif

extern int numofsig;
extern char quote_file[], quote_user[];
char currmaildir[STRLEN];
int inmarkedmode = NA;
#define maxrecp 300

int getbmns(char *userid)
{
    FILE *bmfp = NULL;
    char bmfilename[STRLEN], tmp[20];
    int i = 0;

    sethomefile(bmfilename, userid, ".bmfile");
    bmfp = fopen(bmfilename, "r");
    if (!bmfp)
        return 0;
    for (i = 0; i < 3; i++)
    {
        fscanf(bmfp, "%s\n", tmp);
        if (feof(bmfp))
            break;
    }

    fclose(bmfp);
    return (i + 1);
}

int chkmail(void)
{
    static long lasttime = 0;
    static int ismail = 0;
    struct fileheader fh;
    struct stat st;
    int fd, size;
    register int i, offset;
    register long numfiles;
    unsigned char ch;
    extern char currmaildir[STRLEN];

    if (!HAS_PERM(PERM_BASIC))
    {
        return 0;
    }
    size = sizeof(struct fileheader);
    offset = (int) ((char *) &(fh.accessed[0]) - (char *) &(fh));
    if ((fd = open(currmaildir, O_RDONLY)) < 0)
        return (ismail = 0);
    fstat(fd, &st);
    if (lasttime >= st.st_mtime)
    {
        close(fd);
        return ismail;
    }
    lasttime = st.st_mtime;
    numfiles = st.st_size;
    numfiles = numfiles / size;
    if (numfiles <= 0)
    {
        close(fd);
        return (ismail = 0);
    }
    lseek(fd, (off_t) (st.st_size - (size - offset)), SEEK_SET);
    for (i = 0; i < numfiles; i++)
    {
        read(fd, &ch, 1);
        if (!(ch & FILE_READ))
        {
            close(fd);
            return (ismail = 1);
        }
        lseek(fd, (off_t) (-size - 1), SEEK_CUR);
    }
    close(fd);
    return (ismail = 0);
}

int check_query_mail(char *qry_mail_dir)
{
    struct fileheader fh;
    struct stat st;
    int fd, size;
    register int offset;
    register long numfiles;
    unsigned char ch;

    offset = (int) ((char *) &(fh.accessed[0]) - (char *) &(fh));
    if ((fd = open(qry_mail_dir, O_RDONLY)) < 0)
        return 0;
    fstat(fd, &st);
    numfiles = st.st_size;
    size = sizeof(struct fileheader);
    numfiles = numfiles / size;
    if (numfiles <= 0)
    {
        close(fd);
        return 0;
    }
    lseek(fd, (off_t) (st.st_size - (size - offset)), SEEK_SET);

    read(fd, &ch, 1);
    if (!(ch & FILE_READ))
    {
        close(fd);
        return YEA;
    }
    close(fd);
    return NA;
}

int mailall(void)
{
    char ans[4], fname[STRLEN], title[STRLEN];
    char doc[5][STRLEN], buf[STRLEN];
    int i;

    strcpy(title, "Ã»Ö÷Ìâ");
    modify_user_mode(SMAIL);
    clear();
    move(0, 0);
    sprintf(fname, "tmp/mailall.%s", currentuser.userid);
    prints("ÄãÒª¼Ä¸øËùÓÐµÄ£º\n");
    prints("(0) ·ÅÆú\n");
    strcpy(doc[0], "(1) ÉÐÎ´Í¨¹ýÉí·ÝÈ·ÈÏµÄÊ¹ÓÃÕß");
    strcpy(doc[1], "(2) ËùÓÐÍ¨¹ýÉí·ÝÈ·ÈÏµÄÊ¹ÓÃÕß");
    strcpy(doc[2], "(3) ËùÓÐµÄ°åÖ÷");
    strcpy(doc[3], "(4) ±¾Õ¾ÖÇÄÒÍÅ");
    strcpy(doc[4], "(5) ËùÓÐ±¾×éÖ¯»áÔ±");
    for (i = 0; i < 5; i++)
        prints("%s\n", doc[i]);
    getdata(8, 0, "ÇëÊäÈëÄ£Ê½ (0~5)? [0]: ", ans, 2, DOECHO, YEA);
    if (ans[0] - '0' < 1 || ans[0] - '0' > 5)
    {
        return NA;
    }
    sprintf(buf, "ÊÇ·ñÈ·¶¨¼Ä¸ø%s ", doc[ans[0] - '0' - 1]);
    move(9, 0);
    if (askyn(buf, NA, NA) == NA)
        return NA;
    in_mail = YEA;
    header.reply_mode = NA;
    strcpy(header.title, "Ã»Ö÷Ìâ");
    strcpy(header.ds, doc[ans[0] - '0' - 1]);
    header.postboard = NA;
    i = post_header(&header, 0);
    if (i == -1)
        return NA;
    if (i == YEA)
        sprintf(save_title, "[Type %c ¹«¸æ] %.60s", ans[0], header.title);
    setquotefile("");
    do_quote(fname, header.include_mode);
    if (vedit(fname, YEA, YEA) == -1)
    {
        in_mail = NA;
        unlink(fname);
        clear();
        return -2;
    }
    move(t_lines - 1, 0);
    clrtoeol();
    prints
        ("[5;1;32;44mÕýÔÚ¼Ä¼þÖÐ£¬ÇëÉÔºò.....                                                        [m");
    refresh();
    mailtoall(ans[0] - '0');
    move(t_lines - 1, 0);
    clrtoeol();
    unlink(fname);
    in_mail = NA;
    return 0;
}

#ifdef INTERNET_EMAIL

void m_internet(void)
{
    char receiver[68];

    modify_user_mode(SMAIL);
    if (check_maxmail())
    {
        pressreturn();
        return;
    }

    getdata(1, 0, "ÊÕÐÅÈËE-mail£º", receiver, 65, DOECHO, YEA);
    strtolower(genbuf, receiver);
    if (strstr(genbuf, ".bbs@" BBSHOST)
        || strstr(genbuf, ".bbs@localhost"))
    {
        move(3, 0);
        prints("Õ¾ÄÚÐÅ¼þ, ÇëÓÃ (S)end Ö¸ÁîÀ´¼Ä\n");
        pressreturn();
    }
    else if (!invalidaddr(receiver))
    {
        *quote_file = '\0';
        clear();
        do_send(receiver, NULL);
    }
    else
    {
        move(3, 0);
        prints("ÊÕÐÅÈË²»ÕýÈ·, ÇëÖØÐÂÑ¡È¡Ö¸Áî\n");
        pressreturn();
    }
    clear();
    refresh();
}
#endif

void m_init(void)
{
    sprintf(currmaildir, "mail/%c/%c/%s/%s",
            toupper(currentuser.userid[0]), toupper(currentuser.userid[1]),
            currentuser.userid, DOT_DIR);
}

int do_send(char *userid, char *title)
{
    struct fileheader newmessage;
    struct override fh;
    struct stat st;
    char filepath[STRLEN], fname[STRLEN], *ip;
    char save_title2[STRLEN];
    int fp, count, result;
    int internet_mail = 0;
    char tmp_fname[STRLEN];

    if (strchr(userid, '@'))
    {
        internet_mail = YEA;
        sprintf(tmp_fname, "tmp/imail.%s.%05d", currentuser.userid,
                uinfo.pid);
        strcpy(filepath, tmp_fname);
        goto edit_mail_file;
    }

    if (!getuser(userid))
        return -1;
    if (!(lookupuser.userlevel & PERM_READMAIL))
        return -3;
    sethomefile(filepath, userid, "rejects");
    if (search_record
        (filepath, &fh, sizeof(fh), cmpfnames, currentuser.userid))
        return -5;
    if (!(lookupuser.userlevel & PERM_MAILINFINITY)
        && getmailboxsize(lookupuser.userid, lookupuser.userlevel) * 2 <
        getmailsize(lookupuser.userid))
        return -4;
    sprintf(filepath, "mail/%c/%c/%s", toupper(userid[0]),
            toupper(userid[1]), userid);
    if (stat(filepath, &st) == -1)
    {
        if (mkdir(filepath, 0755) == -1)
            return -1;
    }
    else
    {
        if (!(st.st_mode & S_IFDIR))
            return -1;
    }
    memset(&newmessage, 0, sizeof(newmessage));
    sprintf(fname, "M.%ld.A", time(NULL));
    sprintf(filepath, "mail/%c/%c/%s/%s", toupper(userid[0]),
            toupper(userid[1]), userid, fname);
    ip = strrchr(fname, 'A');
    count = 0;
    while ((fp = open(filepath, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1)
    {
        if (*ip == 'Z')
            ip++, *ip = 'A', *(ip + 1) = '\0';
        else
            (*ip)++;
        sprintf(filepath, "mail/%c/%c/%s/%s", toupper(userid[0]),
                toupper(userid[1]), userid, fname);
        if (count++ > MAX_POSTRETRY)
        {
            return -1;
        }
    }
    close(fp);
    strcpy(newmessage.filename, fname);
#if defined(MAIL_REALNAMES)

    sprintf(genbuf, "%s (%s)", currentuser.userid, currentuser.realname);
#else

    sprintf(genbuf, "%s (%s)", currentuser.userid, currentuser.username);
#endif

    strncpy(newmessage.owner, genbuf, STRLEN);
    sprintf(filepath, "mail/%c/%c/%s/%s", toupper(userid[0]),
            toupper(userid[1]), userid, fname);

  edit_mail_file:
    if (title == NULL)
    {
        header.reply_mode = NA;
        strcpy(header.title, "Ã»Ö÷Ìâ");
    }
    else
    {
        header.reply_mode = YEA;
        strcpy(header.title, title);
    }
    header.postboard = NA;
    in_mail = YEA;

    setuserfile(genbuf, "signatures");
    ansimore_partial(genbuf, NA, 0, 18);
    strcpy(header.ds, userid);
    result = post_header(&header, 0);
    if (result == -1)
    {
        clear();
        return -2;
    }
    if (result == YEA)
    {
        strcpy(newmessage.title, header.title);
        strncpy(save_title, newmessage.title, STRLEN);
        sprintf(save_title2, "{%.16s} %.60s", userid, newmessage.title);
    }
    do_quote(filepath, header.include_mode);

    if (internet_mail)
    {
#ifndef INTERNET_EMAIL
        prints("¶Ô²»Æð£¬±¾Õ¾ÔÝ²»Ìá¹© InterNet Mail ·þÎñ£¡");
        pressanykey();
#else

        int res;

        if (vedit(filepath, YEA, YEA) == -1)
        {
            unlink(filepath);
            clear();
            return -2;
        }
        clear();
        prints("ÐÅ¼þ¼´½«¼Ä¸ø %s \n", userid);
        prints("±êÌâÎª£º %s \n", header.title);
        if (askyn("È·¶¨Òª¼Ä³öÂð", YEA, NA) == NA)
        {
            prints("\nÐÅ¼þÒÑÈ¡Ïû...\n");
            res = -2;
        }
        else
        {
            int filter = YEA;

#ifdef SENDMAIL_MIME_AUTOCONVERT

            int ans;

            ans = askyn("ÒÔ MIME ¸ñÊ½ËÍÐÅ", NA, NA);
            if (askyn("ÊÇ·ñ¹ýÂËANSI¿ØÖÆ·û", YEA, NA) == NA)
                filter = NA;
            if (askyn("ÊÇ·ñ±¸·Ý¸ø×Ô¼º", NA, NA) == YEA)
                mail_file(tmp_fname, currentuser.userid, save_title2);
            prints("ÇëÉÔºò, ÐÅ¼þ´«µÝÖÐ...\n");
            refresh();
            res =
                bbs_sendmail(tmp_fname, header.title, userid, filter, ans);
#else

            if (askyn("ÊÇ·ñ¹ýÂËANSI¿ØÖÆ·û", YEA, NA) == NA)
                filter = NA;
            if (askyn("ÊÇ·ñ±¸·Ý¸ø×Ô¼º", NA, NA) == YEA)
                mail_file(tmp_fname, currentuser.userid, save_title2);
            prints("ÇëÉÔºò, ÐÅ¼þ´«µÝÖÐ...\n");
            refresh();
            res = bbs_sendmail(tmp_fname, header.title, userid, filter);
#endif

        }
        unlink(tmp_fname);
        sprintf(genbuf, "mailed %s", userid);
        report(genbuf);
        return res;
#endif

    }
    else
    {
        if (vedit(filepath, YEA, YEA) == -1)
        {
            unlink(filepath);
            clear();
            return -2;
        }
        clear();
        if (askyn("ÊÇ·ñ±¸·Ý¸ø×Ô¼º", NA, NA) == YEA)
            mail_file(filepath, currentuser.userid, save_title2);
#if 0

        sprintf(genbuf, "mail/%c/%c/%s/%s", toupper(userid[0]),
                toupper(userid[1]), userid, newmessage.filename);
        if (stat(genbuf, &st) == -1)
            file_size = 0;
        else
            file_size = st.st_blksize * st.st_blocks;

        sizeptr = (int *) (newmessage.filename + STRLEN - 5);
        *sizeptr = file_size;

#endif

        sprintf(genbuf, "mail/%c/%c/%s/%s", toupper(userid[0]),
                toupper(userid[1]), userid, DOT_DIR);
        if (append_record(genbuf, &newmessage, sizeof(newmessage)) == -1)
            return -1;
        sprintf(genbuf, "mailed %s", userid);
        report(genbuf);
        autoreplymail_file(userid, newmessage.title);
        return 0;
    }
}

int autoreplymail_file(char *userid, char *title)
{
    struct fileheader newmessage;
    struct stat st;
    char fname[STRLEN], filepath[STRLEN], *ip;
    char tmpfile[STRLEN];
    int fp, count;

    memset(&newmessage, 0, sizeof(newmessage));
    getuser(userid);
    sprintf(tmpfile, "home/%c/%c/%s", toupper(lookupuser.userid[0]),
            toupper(lookupuser.userid[1]), lookupuser.userid);
    if (stat(tmpfile, &st) == -1)
    {
        return -1;
    }
    sprintf(filepath, "%s/%s", tmpfile, "autoreply");
    if ((fp = open(filepath, O_RDONLY)) == -1)
    {
        return -1;
    }
    close(fp);
    strcpy(tmpfile, filepath);
#if defined(MAIL_REALNAMES)

    sprintf(genbuf, "%s (%s)", lookupuser.userid, lookupuser.realname);
#else

    sprintf(genbuf, "%s (%s)", lookupuser.userid, lookupuser.username);
#endif

    strcpy(fname, title);
    strcpy(title, "{Auto-Reply}");
    strncpy(title + 12, fname, STRLEN - 12);
    strncpy(newmessage.owner, genbuf, STRLEN);
    strncpy(newmessage.title, title, STRLEN);
    strncpy(save_title, newmessage.title, STRLEN);
    sprintf(filepath, "mail/%c/%c/%s", toupper(currentuser.userid[0]),
            toupper(currentuser.userid[1]), currentuser.userid);
    if (stat(filepath, &st) == -1)
    {
        if (mkdir(filepath, 0755) == -1)
            return -1;
    }
    else
    {
        if (!(st.st_mode & S_IFDIR))
            return -1;
    }
    sprintf(fname, "M.%ld.A", time(NULL));
    sprintf(filepath, "mail/%c/%c/%s/%s", toupper(currentuser.userid[0]),
            toupper(currentuser.userid[1]), currentuser.userid, fname);
    ip = strrchr(fname, 'A');
    count = 0;
    while ((fp = open(filepath, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1)
    {
        if (*ip == 'Z')
            ip++, *ip = 'A', *(ip + 1) = '\0';
        else
            (*ip)++;
        sprintf(filepath, "mail/%c/%c/%s/%s",
                toupper(currentuser.userid[0]),
                toupper(currentuser.userid[1]), currentuser.userid, fname);
        if (count++ > MAX_POSTRETRY)
        {
            return -1;
        }
    }
    close(fp);
    strcpy(newmessage.filename, fname);
    sprintf(filepath, "mail/%c/%c/%s/%s", toupper(currentuser.userid[0]),
            toupper(currentuser.userid[1]), currentuser.userid, fname);
    f_cp(tmpfile, filepath, O_CREAT);
    sprintf(genbuf, "mail/%c/%c/%s/%s", toupper(currentuser.userid[0]),
            toupper(currentuser.userid[1]), currentuser.userid, DOT_DIR);
    if (append_record(genbuf, &newmessage, sizeof(newmessage)) == -1)
        return -1;
    sprintf(genbuf, "%s Auto-Reply mailed to %s ", userid,
            currentuser.userid);

    report(genbuf);
    return 0;
}

int m_send(char *userid)
{
    char uident[STRLEN];

    if (!HAS_PERM(PERM_POST))
        return 0;

    if (check_maxmail())
    {
        pressreturn();
        return 0;
    }

    if (uinfo.mode != LUSERS && uinfo.mode != LAUSERS
        && uinfo.mode != WATCHINBOARD && uinfo.mode != FRIEND
        && uinfo.mode != GMENU && uinfo.mode != QAUTHOR
        && uinfo.mode != QUERY && uinfo.mode != QAUTHOR)
    {
        move(1, 0);
        clrtoeol();
        modify_user_mode(SMAIL);
        usercomplete("ÊÕÐÅÈË£º ", uident);
        if (uident[0] == '\0')
        {
            return FULLUPDATE;
        }
    }
    else
        strcpy(uident, userid);
    modify_user_mode(SMAIL);
    clear();
    *quote_file = '\0';
    switch (do_send(uident, NULL))
    {
    case -1:
        prints("ÊÕÐÅÕß²»ÕýÈ·\n");
        break;
    case -2:
        prints("È¡Ïû\n");
        break;
    case -3:
        prints("[%s] ÎÞ·¨ÊÕÐÅ\n", uident);
        break;
    case -4:
        prints("[%s] ÐÅÏäÒÑÂú£¬ÎÞ·¨ÊÕÐÅ\n", uident);
        break;
    case -5:
        prints("[%s] ²»ÏëÊÕµ½ÄúµÄÐÅ¼þ\n", uident);
        break;
    default:
        prints("ÐÅ¼þÒÑ¼Ä³ö\n");
    }
    pressreturn();
    return FULLUPDATE;
}

int read_mail(struct fileheader *fptr)
{
    sprintf(genbuf, "mail/%c/%c/%s/%s", toupper(currentuser.userid[0]),
            toupper(currentuser.userid[1]), currentuser.userid,
            fptr->filename);
    int ch = ansimore(genbuf, NA);

    fptr->accessed[0] |= FILE_READ;
    return ch;
}

int mrd;

int delmsgs[1024];
int delcnt;

int read_new_mail(void *vfptr)
{
    struct fileheader *fptr = (struct fileheader *) vfptr;
    static int idc;
    char done = NA, delete_it;
    char fname[256];
    int ch;

    if (fptr == NULL)
    {
        delcnt = 0;
        idc = 0;
        return 0;
    }
    idc++;
    if (fptr->accessed[0])
        return 0;
    mrd = 1;
    prints("¶ÁÈ¡ %s ¼ÄÀ´µÄ '%s' ?\n", fptr->owner, fptr->title);

    getdata(1, 0, "(Y)¶ÁÈ¡ (N)²»¶Á (Q)Àë¿ª [Y]: ", genbuf, 3, DOECHO, YEA);
    if (genbuf[0] == 'q' || genbuf[0] == 'Q')
    {
        clear();
        return QUIT;
    }
    if (genbuf[0] != 'y' && genbuf[0] != 'Y' && genbuf[0] != '\0')
    {
        clear();
        return 0;
    }
    ch = read_mail(fptr);
    strncpy(fname, genbuf, sizeof(fname));

    if (substitute_record(currmaildir, fptr, sizeof(*fptr), idc))
        return -1;
    delete_it = NA;
    while (!done)
    {
        move(t_lines - 1, 0);
        prints("(R)»ØÐÅ, (D)É¾³ý, (G)¼ÌÐø ? [G]: ");
        clrtoeol();

        if (ch != 'R' && ch != KEY_LEFT)
            ch = egetch();

        switch (egetch())
        {
        case 'R':
        case 'r':
            mail_reply(idc, fptr, currmaildir);
            break;
        case 'D':
        case 'd':
            delete_it = YEA;
        default:
            done = YEA;
        }
        if (!done)
            ansimore(fname, NA);
    }
    if (delete_it)
    {
        clear();
        sprintf(genbuf, "É¾³ýÐÅ¼þ [%-.55s]", fptr->title);
        if (askyn(genbuf, NA, NA) == YEA)
        {
            sprintf(genbuf, "mail/%c/%c/%s/%s",
                    toupper(currentuser.userid[0]),
                    toupper(currentuser.userid[1]), currentuser.userid,
                    fptr->filename);
            unlink(genbuf);
            delmsgs[delcnt++] = idc;
        }
    }
    clear();
    return 0;
}

int m_new(void)
{
    if (!strcmp(currentuser.userid, "guest"))
        return 0;
    clear();
    mrd = 0;
    modify_user_mode(RMAIL);
    read_new_mail(NULL);
    apply_record(currmaildir, read_new_mail, sizeof(struct fileheader));
    while (delcnt--)
        delete_record(currmaildir, sizeof(struct fileheader),
                      delmsgs[delcnt]);
    if (!mrd)
    {
        clear();
        move(10, 30);
        prints("ÄúÏÖÔÚÃ»ÓÐÐÂÐÅ¼þ!");
        pressanykey();
    }
    return -1;
}

extern char BoardName[];

void mailtitle(void)
{
    int total = 0, used = 0;

    if (!HAS_PERM(PERM_MAILINFINITY))
    {
        total = getmailboxsize(currentuser.userid, currentuser.userlevel);
        used = getmailsize(currentuser.userid);
    }
    showtitle("ÓÊ¼þÑ¡µ¥    ", BoardName);
    prints
        ("Àë¿ª[[1;32m¡û[m,[1;32me[m] Ñ¡Ôñ[[1;32m¡ü[m, [1;32m¡ý[m] ÔÄ¶ÁÐÅ¼þ[[1;32m¡ú[m,[1;32mRtn[m] »Ø ÐÅ[[1;32mR[m] ¿³ÐÅ£¯Çå³ý¾ÉÐÅ[[1;32md[m,[1;32mD[m] ÇóÖú[[1;32mh[m][m\n");
    if (HAS_PERM(PERM_MAILINFINITY))
        prints
            ("[1;44m±àºÅ   ·¢ÐÅÕß       ÈÕ ÆÚ      ±êÌâ                        ([32mÐÅÏäÈÝÁ¿[ÎÞÉÏÏÞ][37m)   [m\n");
    else
        prints
            ("[1;44m±àºÅ   ·¢ÐÅÕß       ÈÕ ÆÚ      ±êÌâ  ([33mÄúµÄÐÅÏäÈÝÁ¿Îª[%4dK]£¬µ±Ç°ÒÑÓÃ[%4dK][37m)   [m\n",
             total, used);
    clrtobot();
}

int getmailboxsize(char *userid, unsigned int userlevel)
{
    int i = 0;

    if (userlevel & (PERM_CHATCLOAK))
        return 3000;
    if (userlevel & (PERM_BOARDS))
    {
        i = getbmns(userid);
        return (2000 + 600 * (i - 1));
    }

    if (userlevel & (PERM_BASIC))
        return 1000;
    return 5;
}

int getmailsize(char *userid)
{
    struct fileheader fcache;
    struct stat DIRst, SIZEst, st;
    char sizefile[50], dirfile[256], mailfile[256];
    FILE *fp;
    int mailsize = 9999, fd, ssize = sizeof(struct fileheader);

    setmdir(dirfile, userid);
    sprintf(sizefile, "tmp/%s.mailsize", userid);
    if (stat(dirfile, &DIRst) == -1 || DIRst.st_size == 0)
        mailsize = 0;
    else if (stat(sizefile, &SIZEst) != -1 && SIZEst.st_size != 0
             && SIZEst.st_ctime >= DIRst.st_ctime)
    {
        fp = fopen(sizefile, "r");
        if (fp)
        {
            fscanf(fp, "%d", &mailsize);
            fclose(fp);
        }
    }
    if (mailsize != 9999)
        return mailsize;

    mailsize = 0;
    fd = open(dirfile, O_RDONLY);
    if (fd != -1)
    {
        while (read(fd, &fcache, ssize) == ssize)
        {
            sprintf(mailfile, "mail/%c/%c/%s/%s",
                    toupper(userid[0]), toupper(userid[1]), userid,
                    fcache.filename);
            if (stat(mailfile, &st) != -1)
            {
                mailsize += (st.st_size / 1024 + 1);
            }
        }
        close(fd);
    }
    fp = fopen(sizefile, "w+");
    if (fp)
    {
        fprintf(fp, "%d", mailsize);
        fclose(fp);
    }
    return mailsize;
}

char *maildoent(int num, void *vent)
{
    static char buf[512];
    struct fileheader *ent = (struct fileheader *) vent;
    char b2[512];
    time_t filetime;
    char status;
    char color[10];
    char *date, *t;
    extern char ReadPost[];
    extern char ReplyPost[];
    char c1[8];
    char c2[8];
    int same = NA;

#ifdef COLOR_POST_DATE

    struct tm *mytm;
#endif

    filetime = atoi(ent->filename + 2);
    if (filetime > 740000000)
    {
        date = ctime(&filetime) + 4;
    }
    else
    {
        date = "";
    }

#ifdef COLOR_POST_DATE
    mytm = localtime(&filetime);
    strftime(buf, 5, "%w", mytm);
    sprintf(color, "[1;%dm", 30 + atoi(buf) + 1);
#else

    strcpy(color, "");
#endif

    strcpy(c1, "[1;33m");
    strcpy(c2, "[1;36m");
    if (!strcmp(ReadPost, ent->title) || !strcmp(ReplyPost, ent->title))
        same = YEA;
    strncpy(b2, ent->owner, STRLEN);
    if ((b2[strlen(b2) - 1] == '>') && strchr(b2, '<'))
    {
        t = strtok(b2, "<>");
        if (invalidaddr(t))
            t = strtok(NULL, "<>");
        if (t != NULL)
            strcpy(b2, t);
    }
    if ((t = strchr(b2, ' ')) != NULL)
        *t = '\0';
    if (ent->accessed[0] & FILE_READ)
    {
        if ((ent->accessed[0] & FILE_MARKED)
            && (ent->accessed[0] & MAIL_REPLY))
            status = 'b';
        else if (ent->accessed[0] & FILE_MARKED)
            status = 'm';
        else if (ent->accessed[0] & MAIL_REPLY)
            status = 'r';
        else
            status = ' ';
    }
    else
    {
        if (ent->accessed[0] & FILE_MARKED)
            status = 'M';
        else
            status = 'N';
    }
    if (!strncmp("Re:", ent->title, 3))
    {
        sprintf(buf, " %s%3d[m %c %-12.12s %s%6.6s[m  %s%.50s[m",
                same ? c1 : "", num, status, b2, color, date,
                same ? c1 : "", ent->title);
    }
    else
    {
        sprintf(buf, " %s%3d[m %c %-12.12s %s%6.6s[m  ¡ï %s%.47s[m",
                same ? c2 : "", num, status, b2, color, date,
                same ? c2 : "", ent->title);
    }
    return buf;
}

int mail_read(int ent, struct fileheader *fileinfo, char *direct)
{
    char buf[512], notgenbuf[128];
    char *t;
    int readnext, readprev;
    char done = NA, delete_it, replied;
    int ch;

    if (strlen(fileinfo->filename) <= 1)
        return DONOTHING;

    clear();
    readnext = NA;
    readprev = NA;
    setqtitle(fileinfo->title);
    strcpy(buf, direct);
    if ((t = strrchr(buf, '/')) != NULL)
        *t = '\0';
    sprintf(notgenbuf, "%s/%s", buf, fileinfo->filename);
    delete_it = replied = NA;
    while (!done)
    {
        ch = ansimore_withmode(notgenbuf, NA, MORE_MODE_MAIL);
        move(t_lines - 1, 0);
        prints("(R)»ØÐÅ, (D)É¾³ý, (G)¼ÌÐø? [G]: ");
        clrtoeol();

        switch (ch)
        {
        case 'R':
        case 'r':
            replied = YEA;
            mail_reply(ent, fileinfo, direct);
            break;
        case ' ':
        case 'j':
        case KEY_RIGHT:
        case KEY_DOWN:
        case KEY_PGDN:
        case ']':
            done = YEA;
            readnext = YEA;
            break;
        case KEY_UP:
        case '[':
            done = YEA;
            readprev = YEA;
            break;
        case 'D':
        case 'd':
            delete_it = YEA;
        default:
            done = YEA;
        }
    }
    clear();
    mailtitle();
    if (delete_it)
        return mail_del(ent, fileinfo, direct);
    else
    {
        fileinfo->accessed[0] |= FILE_READ;
        substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
    }
    if (readnext == YEA)
        return READ_NEXT;
    if (readprev == YEA)
        return READ_PREV;
    return FULLUPDATE;
}
int mail_reply(int ent, struct fileheader *fileinfo, char *direct)
{
    char uid[STRLEN];
    char title[STRLEN];
    char *t;

    modify_user_mode(SMAIL);
    sprintf(genbuf, "MAILER-DAEMON@%s", BBSHOST);
    if (strstr(fileinfo->owner, genbuf))
    {
        ansimore("help/mailerror-explain", YEA);
        return FULLUPDATE;
    }
    if (check_maxmail())
    {
        pressreturn();
        return 0;
    }
    clear();
    strncpy(uid, fileinfo->owner, STRLEN);
    if ((uid[strlen(uid) - 1] == '>') && strchr(uid, '<'))
    {
        t = strtok(uid, "<>");
        if (invalidaddr(t))
            t = strtok(NULL, "<>");
        if (t != NULL)
            strcpy(uid, t);
        else
        {
            prints("ÎÞ·¨Í¶µÝ\n");
            pressreturn();
            return FULLUPDATE;
        }
    }
    if ((t = strchr(uid, ' ')) != NULL)
        *t = '\0';
    if (toupper(fileinfo->title[0]) != 'R'
        || toupper(fileinfo->title[1]) != 'E' || fileinfo->title[2] != ':')
        strcpy(title, "Re: ");
    else
        title[0] = '\0';
    strncat(title, fileinfo->title, STRLEN - 5);

    sprintf(quote_file, "mail/%c/%c/%s/%s", toupper(currentuser.userid[0]),
            toupper(currentuser.userid[1]), currentuser.userid,
            fileinfo->filename);
    strcpy(quote_user, fileinfo->owner);
    switch (do_send(uid, title))
    {
    case -1:
        prints("ÎÞ·¨Í¶µÝ\n");
        break;
    case -2:
        prints("È¡Ïû»ØÐÅ\n");
        break;
    case -3:
        prints("[%s] ÎÞ·¨ÊÕÐÅ\n", uid);
        break;
    case -4:
        prints("[%s] ÐÅÏäÒÑÂú£¬ÎÞ·¨ÊÕÐÅ\n", uid);
        break;
    case -5:
        prints("[%s] ²»ÏëÊÕµ½ÄúµÄÐÅ¼þ\n", uid);
        break;
    default:
        fileinfo->accessed[0] |= MAIL_REPLY;
        substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
        prints("ÐÅ¼þÒÑ¼Ä³ö\n");
    }
    pressreturn();
    return FULLUPDATE;
}
int marked_mode(void)
{
    extern char currdirect[STRLEN];
	if(inmarkedmode == NA){
     sprintf(currdirect, "mail/%c/%c/%s/%s", toupper(currentuser.userid[0]),
            toupper(currentuser.userid[1]), currentuser.userid,MARKED_DIR);
     inmarkedmode = YEA;
     marked_mail();
        if (!dashf(currdirect))
        {
            return PARTUPDATE;
        }
    return NEWDIRECT;
    }
}


int mail_del(int ent, struct fileheader *fileinfo, char *direct)
{
    char buf[512];
    char *t;

    extern char currfile[];

    if (fileinfo->accessed[0] & FILE_MARKED)
        return DONOTHING;

    sprintf(genbuf, "É¾³ýÐÅ¼þ [%-.55s]", fileinfo->title);
    if (askyn(genbuf, NA, YEA) == NA)
    {
        move(t_lines - 1, 0);
        prints("·ÅÆúÉ¾³ýÐÅ¼þ...");
        clrtoeol();
        egetch();
        return PARTUPDATE;
    }
    strcpy(buf, direct);
    if ((t = strrchr(buf, '/')) != NULL)
        *t = '\0';
    strncpy(currfile, fileinfo->filename, STRLEN);
    if (!delete_file(direct, sizeof(*fileinfo), ent, cmpfilename))
    {
        sprintf(genbuf, "%s/%s", buf, fileinfo->filename);
        unlink(genbuf);
        check_maxmail();
        return DIRCHANGED;
    }
    move(t_lines - 1, 0);
    prints("É¾³ýÊ§°Ü...");
    clrtoeol();
    egetch();
    return PARTUPDATE;
}

#ifdef INTERNET_EMAIL

int mail_forward(int ent, struct fileheader *fileinfo, char *direct)
{
    char buf[STRLEN];
    char *p;
    struct shortfile sf;

    if (!HAS_PERM(PERM_FORWARD))
    {
        return DONOTHING;
    }
    strncpy(buf, direct, STRLEN);
    buf[STRLEN - 1] = '\0';
    if ((p = strrchr(buf, '/')) != NULL)
        *p = '\0';
    strncpy(sf.filename, fileinfo->filename, STRLEN - 4);
    sf.filename[STRLEN - 5] = '\0';
    strncpy(sf.title, fileinfo->title, STRLEN);
    sf.title[STRLEN - 1] = '\0';
    strncpy(sf.owner, fileinfo->owner, STRLEN - BM_LEN);
    sf.owner[STRLEN - BM_LEN - 1] = '\0';
    switch (doforward(buf, &sf, 0))
    {
    case 0:
        prints("ÎÄÕÂ×ª¼ÄÍê³É!\n");
        break;
    case -1:
        prints("×ª¼ÄÊ§°Ü: ÏµÍ³·¢Éú´íÎó.\n");
        break;
    case -2:
        prints("×ª¼ÄÊ§°Ü: ²»ÕýÈ·µÄÊÕÐÅµØÖ·.\n");
        break;
    case -3:
        prints("ÄúµÄÐÅÏä³¬ÏÞ£¬ÔÝÊ±ÎÞ·¨Ê¹ÓÃÓÊ¼þ·þÎñ.\n");
        break;
    default:
        prints("È¡Ïû×ª¼Ä...\n");
    }
    pressreturn();
    clear();
    return FULLUPDATE;
}

int mail_u_forward(int ent, struct fileheader *fileinfo, char *direct)
{
    char buf[STRLEN];
    char *p;
    struct shortfile sf;

    if (!HAS_PERM(PERM_FORWARD))
    {
        return DONOTHING;
    }
    strncpy(buf, direct, STRLEN);
    buf[STRLEN - 1] = '\0';
    if ((p = strrchr(buf, '/')) != NULL)
        *p = '\0';
    strncpy(sf.filename, fileinfo->filename, STRLEN - 4);
    sf.filename[STRLEN - 5] = '\0';
    strncpy(sf.title, fileinfo->title, STRLEN);
    sf.title[STRLEN - 1] = '\0';
    strncpy(sf.owner, fileinfo->owner, STRLEN - BM_LEN);
    sf.owner[STRLEN - BM_LEN - 1] = '\0';

    switch (doforward(buf, &sf, 1))
    {
    case 0:
        prints("ÎÄÕÂ×ª¼ÄÍê³É!\n");
        break;
    case -1:
        prints("×ª¼ÄÊ§°Ü: ÏµÍ³·¢Éú´íÎó.\n");
        break;
    case -2:
        prints("×ª¼ÄÊ§°Ü: ²»ÕýÈ·µÄÊÕÐÅµØÖ·.\n");
        break;
    case -3:
        prints("ÄúµÄÐÅÏä³¬ÏÞ£¬ÔÝÊ±ÎÞ·¨Ê¹ÓÃÓÊ¼þ·þÎñ.\n");
        break;
    default:
        prints("È¡Ïû×ª¼Ä...\n");
    }
    pressreturn();
    clear();
    return FULLUPDATE;
}
#endif

int mail_del_range(int ent, struct fileheader *fileinfo, char *direct)
{
    return (del_range(ent, fileinfo, direct));
}

int mail_mark(int ent, struct fileheader *fileinfo, char *direct)
{
    if (fileinfo->accessed[0] & FILE_MARKED)
        fileinfo->accessed[0] &= ~FILE_MARKED;
    else
        fileinfo->accessed[0] |= FILE_MARKED;
    substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
    return (PARTUPDATE);
}

extern int mailreadhelp(void);

struct one_key mail_comms[] = {
    {'d', mail_del,},
    {'D', mail_del_range,},
    {Ctrl('P'), m_send,},
    {'E', edit_post,},
    {'r', mail_read,},
    {'R', mail_reply,},
    {'m', mail_mark,},

    {'T', edit_title,},

    {KEY_TAB, show_user_notes,},
#ifdef INTERNET_EMAIL
    {'F', mail_forward,},
    {'U', mail_u_forward,},
#endif
    {'a', auth_search_down,},
    {'A', auth_search_up,},
    {'/', t_search_down,},
    {'?', t_search_up,},
    {'\'', post_search_down,},
    {'\"', post_search_up,},
    {']', thread_down,},
    {'[', thread_up,},
    {Ctrl('A'), show_author,},
	{Ctrl('G'), marked_mode,},
	{Ctrl('T'), marked_mode,},
	{'`', marked_mode,},
    {'\\', SR_last,},
    {'=', SR_first,},
    {'l', show_allmsgs,},
    {Ctrl('C'), do_cross,},

    {'h', mailreadhelp,},

    {'!', Q_Goodbye,},
    {'S', s_msg,},
    {'f', t_friends,},
#ifdef MAIL_COMBINE
    {'b', Mail_combine,},
#endif
    {'\0', NULL},
};

int m_read(void)
{
    if (!strcmp(currentuser.userid, "guest"))
        return 0;
    in_mail = YEA;
    i_read(RMAIL, currmaildir, mailtitle, maildoent, &mail_comms[0],
           sizeof(struct fileheader));
    in_mail = NA;
    return 0;
}

int invalidaddr(char *addr)
{
    if (*addr == '\0' || !strchr(addr, '@'))
        return 1;
    while (*addr)
    {
        if (!isalnum(*addr) && !strchr(".!@:-_", *addr))
            return 1;
        addr++;
    }
    return 0;
}

#ifdef INTERNET_EMAIL
int bbs_sendmail(char *fname, char *title, char *receiver, int filter,
                 int mime)
{
    FILE *fin, *fout;

    sprintf(genbuf, "%s -f %s.bbs@%s %s", SENDMAIL,
            currentuser.userid, BBSHOST, receiver);
    fout = popen(genbuf, "w");
    fin = fopen(fname, "r");
    if (fin == NULL || fout == NULL)
        return -1;

    fprintf(fout, "Return-Path: %s.bbs@%s\n", currentuser.userid, BBSHOST);
    fprintf(fout, "Reply-To: %s.bbs@%s\n", currentuser.userid, BBSHOST);
    fprintf(fout, "From: %s.bbs@%s\n", currentuser.userid, BBSHOST);
    fprintf(fout, "To: %s\n", receiver);
    fprintf(fout, "Subject: %s\n", title);
    fprintf(fout, "X-Forwarded-By: %s (%s)\n", currentuser.userid,
#ifdef MAIL_REALNAMES
            currentuser.realname);
#else
            currentuser.username);
#endif

    fprintf(fout, "X-Disclaimer: %s ¶Ô±¾ÐÅÄÚÈÝË¡²»¸ºÔð¡£\n", BoardName);
#ifdef SENDMAIL_MIME_AUTOCONVERT
    if (mime)
    {
        fprintf(fout, "MIME-Version: 1.0\n");
        fprintf(fout, "Content-Type: text/plain; charset=US-ASCII\n");
        fprintf(fout, "Content-Transfer-Encoding: 8bit\n");
    }
#endif
    fprintf(fout, "Precedence: junk\n\n");

    while (fgets(genbuf, 255, fin) != NULL)
    {
        if (filter)
            my_ansi_filter(genbuf);
        if (genbuf[0] == '.' && genbuf[1] == '\n')
            fputs(". \n", fout);
        else
            fputs(genbuf, fout);
    }

    fprintf(fout, ".\n");

    fclose(fin);
    pclose(fout);
    return 0;
}
#endif
int g_send(void)
{
    char maillists[STRLEN];
    int cnt;

    modify_user_mode(SMAIL);
    if (check_maxmail())
    {
        pressreturn();
        return 0;
    }
    *quote_file = '\0';
    sethomefile(maillists, currentuser.userid, "maillist");
    cnt = ListFileEdit(maillists, "±à¼­¡ºÈº·¢ÐÅ¼þ¡»Ãûµ¥", 0);
    if (cnt == 0)
    {
        prints("\nÈº·¢ÐÅ¼þÃûµ¥Îª¿Õ\n");
    }
    else
    {
        if (askyn("ÄúÈ·¶¨Òª·¢ÈºÌåÐÅ¼þÂð", NA, NA) == NA)
            return 0;
        G_SENDMODE = 2;
        switch (do_gsend(NULL, NULL, cnt))
        {
        case -1:
            prints("ÐÅ¼þÄ¿Â¼´íÎó\n");
            break;
        case -2:
            prints("È¡Ïû\n");
            break;
        default:
            prints("ÐÅ¼þÒÑ¼Ä³ö\n");
        }
        G_SENDMODE = 0;
    }
    pressreturn();
    return 0;
}

#if 0
int g_send()
{
    char uident[13], tmp[3];
    int cnt, i, n, fmode = NA;
    char maillists[STRLEN];

    modify_user_mode(SMAIL);
    if (check_maxmail())
    {
        pressreturn();
        return 0;
    }
    *quote_file = '\0';
    clear();
    sethomefile(maillists, currentuser.userid, "maillist");
    cnt = listfilecontent(maillists, 3);
    while (1)
    {
        if (cnt > maxrecp - 10)
        {
            move(2, 0);
            prints("Ä¿Ç°ÏÞÖÆ¼ÄÐÅ¸ø [1m%d[m ÈË", maxrecp);
        }
        getdata(0, 0,
                "(A)Ôö¼Ó (D)É¾³ý (I)ÒýÈëºÃÓÑ (C)Çå³ýÄ¿Ç°Ãûµ¥ (E)·ÅÆú (S)¼Ä³ö? [S]£º ",
                tmp, 2, DOECHO, YEA);
        if (tmp[0] == '\n' || tmp[0] == '\0' || tmp[0] == 's'
            || tmp[0] == 'S')
        {
            break;
        }
        if (tmp[0] == 'a' || tmp[0] == 'd' || tmp[0] == 'A'
            || tmp[0] == 'D')
        {
            move(1, 0);
            if (tmp[0] == 'a' || tmp[0] == 'A')
                usercomplete("ÇëÒÀ´ÎÊäÈëÊ¹ÓÃÕß´úºÅ(Ö»°´ ENTER ½áÊøÊäÈë): ",
                             uident);
            else
                namecomplete("ÇëÒÀ´ÎÊäÈëÊ¹ÓÃÕß´úºÅ(Ö»°´ ENTER ½áÊøÊäÈë): ",
                             uident);
            move(1, 0);
            clrtoeol();
            if (uident[0] == '\0')
                continue;
            if (!getuser(uident))
            {
                move(2, 0);
                prints("Õâ¸öÊ¹ÓÃÕß´úºÅÊÇ´íÎóµÄ.\n");
            }
        }
        switch (tmp[0])
        {
        case 'A':
        case 'a':
            if (!(lookupuser.userlevel & PERM_READMAIL))
            {
                move(2, 0);
                prints("ÎÞ·¨ËÍÐÅ¸ø: [1m%s[m\n", lookupuser.userid);
                break;
            }
            else if (seek_in_file(maillists, uident))
            {
                move(2, 0);
                prints("ÒÑ¾­ÁÐÎªÊÕ¼þÈËÖ®Ò» \n");
                break;
            }
            add_to_file(maillists, uident);
            cnt++;
            break;
        case 'E':
        case 'e':
        case 'Q':
        case 'q':
            cnt = 0;
            break;
        case 'D':
        case 'd':
            {
                if (seek_in_file(maillists, uident))
                {
                    del_from_file(maillists, uident);
                    cnt--;
                }
                break;
            }
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
                        prints("ÎÞ·¨ËÍÐÅ¸ø: [1m%s[m\n",
                               lookupuser.userid);
                        i--;
                        continue;
                    }
                    else if (seek_in_file(maillists, uident))
                    {
                        i--;
                        continue;
                    }
                    add_to_file(maillists, uident);
                    cnt++;
                }
            }
            fmode = NA;
            clear();
            break;
        case 'C':
        case 'c':
            unlink(maillists);
            cnt = 0;
            break;
        }
        if (strchr("EeQq", tmp[0]))
            break;
        move(5, 0);
        clrtobot();
        if (cnt > maxrecp)
            cnt = maxrecp;
        move(3, 0);
        clrtobot();
        listfilecontent(maillists, 3);
    }
    if (cnt > 0)
    {
        G_SENDMODE = 2;
        switch (do_gsend(NULL, NULL, cnt))
        {
        case -1:
            prints("ÐÅ¼þÄ¿Â¼´íÎó\n");
            break;
        case -2:
            prints("È¡Ïû\n");
            break;
        default:
            prints("ÐÅ¼þÒÑ¼Ä³ö\n");
        }
        G_SENDMODE = 0;
        pressreturn();
    }
    return 0;
}
#endif

int AllowGroupMail(char *userid, char *myid)
{
    struct override fh;
    char homefilepath[STRLEN * 2];

    if (HAS_PERM
        (PERM_SYSOP | PERM_BOARDS | PERM_SPECIAL7 | PERM_SPECIAL8 |
         PERM_SPECIAL6 | PERM_ACCOUNTS))
        return YEA;

    sethomefile(homefilepath, userid, "friends");

    if (search_record(homefilepath, &fh, sizeof(fh), cmpfnames, myid))
        return YEA;

    sethomefile(homefilepath, userid, "ALLOW_GMAIL");
    if (dashf(homefilepath))
        return YEA;
    else
        return NA;
}

int do_gsend(char **userid, char *title, int num)
{
    struct stat st;
    char filepath[STRLEN], tmpfile[STRLEN];
    int cnt, result;
    FILE *mp = NULL;

    in_mail = YEA;
#if defined(MAIL_REALNAMES)

    sprintf(genbuf, "%s (%s)", currentuser.userid, currentuser.realname);
#else

    sprintf(genbuf, "%s (%s)", currentuser.userid, currentuser.username);
#endif

    header.reply_mode = NA;
    strcpy(header.title, "Ã»Ö÷Ìâ");
    strcpy(header.ds, "¼ÄÐÅ¸øÒ»ÈºÈË");
    header.postboard = NA;
    sprintf(tmpfile, "tmp/gsend.%s.%05d", currentuser.userid, uinfo.pid);
    result = post_header(&header, 0);
    if (result == -1)
    {
        clear();
        return -2;
    }
    if (result == YEA)
    {
        sprintf(save_title, "[ÈºÌåÐÅ¼þ] %.60s", header.title);
    }
    do_quote(tmpfile, header.include_mode);
    if (vedit(tmpfile, YEA, YEA) == -1)
    {
        unlink(tmpfile);
        clear();
        return -2;
    }
    clear();
	if (askyn("ÊÇ·ñ±¸·Ý¸ø×Ô¼º", NA, NA) == YEA)
		mail_file(tmpfile, currentuser.userid, save_title);
    prints("[5;1;32mÕýÔÚ¼Ä¼þÖÐ£¬ÇëÉÔºò...[m");
    if (G_SENDMODE == 2)
    {
        char maillists[STRLEN];

        setuserfile(maillists, "maillist");
        if ((mp = fopen(maillists, "r")) == NULL)
        {
            return -3;
        }
    }
    for (cnt = 0; cnt < num; cnt++)
    {
        char uid[13];
        char buf[STRLEN];

        if (G_SENDMODE == 1)
            getuserid(uid, uinfo.friends[cnt]);
        else if (G_SENDMODE == 2)
        {
            if (fgets(buf, STRLEN, mp) != NULL)
            {
                if (strtok(buf, " \n\r\t") != NULL)
                    strcpy(uid, buf);
                else
                    continue;
            }
            else
            {
                cnt = num;
                continue;
            }
        }
        else
            strcpy(uid, userid[cnt]);

        sprintf(filepath, "mail/%c/%c/%s", toupper(uid[0]),
                toupper(uid[1]), uid);
        if (stat(filepath, &st) == -1)
        {
            if (mkdir(filepath, 0755) == -1)
            {
                if (G_SENDMODE == 2)
                    fclose(mp);
                return -1;
            }
        }
        else
        {
            if (!(st.st_mode & S_IFDIR))
            {
                if (G_SENDMODE == 2)
                    fclose(mp);
                return -1;
            }
        }
        mail_file(tmpfile, uid, save_title);
    }
    unlink(tmpfile);
    clear();
    if (G_SENDMODE == 2)
        fclose(mp);
    return 0;
}

int mail_file(char *tmpfile, char *userid, char *title)
{
    struct fileheader newmessage;
    struct stat st;
    char fname[STRLEN], filepath[STRLEN], *ip;
    int fp, count;

    memset(&newmessage, 0, sizeof(newmessage));
#if defined(MAIL_REALNAMES)

    sprintf(genbuf, "%s (%s)", currentuser.userid, currentuser.realname);
#else

    sprintf(genbuf, "%s (%s)", currentuser.userid, currentuser.username);
#endif

    strncpy(newmessage.owner, genbuf, STRLEN);
    strncpy(newmessage.title, title, STRLEN);
    strncpy(save_title, newmessage.title, STRLEN);

    sprintf(filepath, "mail/%c/%c/%s", toupper(userid[0]),
            toupper(userid[1]), userid);
    if (stat(filepath, &st) == -1)
    {
        if (mkdir(filepath, 0755) == -1)
            return -1;
    }
    else
    {
        if (!(st.st_mode & S_IFDIR))
            return -1;
    }
    sprintf(fname, "M.%ld.A", time(NULL));
    sprintf(filepath, "mail/%c/%c/%s/%s", toupper(userid[0]),
            toupper(userid[1]), userid, fname);
    ip = strrchr(fname, 'A');
    count = 0;
    while ((fp = open(filepath, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1)
    {
        if (*ip == 'Z')
            ip++, *ip = 'A', *(ip + 1) = '\0';
        else
            (*ip)++;
        sprintf(filepath, "mail/%c/%c/%s/%s", toupper(userid[0]),
                toupper(userid[1]), userid, fname);
        if (count++ > MAX_POSTRETRY)
        {
            return -1;
        }
    }
    close(fp);
    strcpy(newmessage.filename, fname);
    sprintf(filepath, "mail/%c/%c/%s/%s", toupper(userid[0]),
            toupper(userid[1]), userid, fname);

    f_cp(tmpfile, filepath, O_CREAT);
#if 0

    sprintf(genbuf, "mail/%c/%c/%s/%s", toupper(userid[0]),
            toupper(userid[1]), userid, fname);
    if (stat(genbuf, &st) == -1)
        file_size = 0;
    else
        file_size = st.st_blksize * st.st_blocks;

    sizeptr = (int *) (newmessage.filename + STRLEN - 5);
    *sizeptr = file_size;

#endif

    sprintf(genbuf, "mail/%c/%c/%s/%s", toupper(userid[0]),
            toupper(userid[1]), userid, DOT_DIR);
    if (append_record(genbuf, &newmessage, sizeof(newmessage)) == -1)
        return -1;
    sprintf(genbuf, "mailed %s ", userid);
    report(genbuf);
    return 0;
}

int ov_send(void)
{
    int all, i;

    modify_user_mode(SMAIL);
    if (check_maxmail())
    {
        pressreturn();
        return 0;
    }
    move(1, 0);
    clrtobot();
    move(2, 0);
    prints("¼ÄÐÅ¸øºÃÓÑÃûµ¥ÖÐµÄÈË£¬Ä¿Ç°±¾Õ¾ÏÞÖÆ½ö¿ÉÒÔ¼Ä¸ø [1m%d[m Î»¡£\n",
           maxrecp);
    if (uinfo.fnum <= 0)
    {
        prints("Äã²¢Ã»ÓÐÉè¶¨ºÃÓÑ¡£\n");
        pressanykey();
        clear();
        return 0;
    }
    else
    {
        prints("Ãûµ¥ÈçÏÂ£º\n");
    }
    G_SENDMODE = 1;
    all = (uinfo.fnum >= maxrecp) ? maxrecp : uinfo.fnum;
    for (i = 0; i < all; i++)
    {
        char uid[IDLEN + 2];

        getuserid(uid, uinfo.friends[i]);
        prints("%-12s ", uid);
        if ((i + 1) % 6 == 0)
            outc('\n');
    }
    pressanykey();
    switch (do_gsend(NULL, NULL, all))
    {
    case -1:
        prints("ÐÅ¼þÄ¿Â¼´íÎó\n");
        break;
    case -2:
        prints("ÐÅ¼þÈ¡Ïû\n");
        break;
    default:
        prints("ÐÅ¼þÒÑ¼Ä³ö\n");
    }
    pressreturn();
    G_SENDMODE = 0;
    return 0;
}

int in_group(char (*uident)[80], int cnt)
{
    int i;

    for (i = 0; i < cnt; i++)
        if (!strcmp(uident[i], uident[cnt]))
        {
            return i + 1;
        }
    return 0;
}

#ifdef INTERNET_EMAIL

int doforward(char *direct, struct shortfile *fh, int mode)
{
    static char address[STRLEN];
    char fname[STRLEN], tmpfname[STRLEN];
    char receiver[STRLEN];
    char title[STRLEN];
    int return_no, internet_mail = 0;
    int filter = YEA;
    char filepath[260] = { 0 };
    struct override baditem;

    clear();
    if (check_maxmail())
        return -3;
    if (address[0] == '\0')
    {
        strncpy(address, currentuser.email, STRLEN);
    }
    if (HAS_PERM(PERM_SETADDR))
    {
        prints("ÇëÖ±½Ó°´ Enter ½ÓÊÜÀ¨ºÅÄÚÌáÊ¾µÄµØÖ·, »òÕßÊäÈëÆäËûµØÖ·\n");
        prints("°ÑÐÅ¼þ×ª¼Ä¸ø [%s]\n", address);
        getdata(2, 0, "==> ", receiver, 70, DOECHO, YEA);
    }
    else
        strcpy(receiver, currentuser.userid);
    if (receiver[0] != '\0')
    {
        strncpy(address, receiver, STRLEN);
    }
    else
        strncpy(receiver, address, STRLEN);
    sprintf(genbuf, ".bbs@%s", BBSHOST);
    if (strstr(receiver, genbuf) || strstr(receiver, ".bbs@localhost"))
    {
        char *pos;

        pos = strchr(address, '.');
        *pos = '\0';
    }
    if (strpbrk(address, "@."))
        internet_mail = YEA;
    if (!internet_mail)
    {
        if (!getuser(address))
            return -1;
        if (!(lookupuser.userlevel & PERM_MAILINFINITY)
            && getmailboxsize(lookupuser.userid,
                              lookupuser.userlevel) * 2 <
            getmailsize(lookupuser.userid))
        {
            prints("[%s] ÐÅÏäÒÑÂú£¬ÎÞ·¨ÊÕÐÅ¡£\n", address);
            return -4;
        }

        sethomefile(filepath, lookupuser.userid, "rejects");
        if (search_record
            (filepath, &baditem, sizeof(baditem), cmpfnames,
             currentuser.userid))
        {
            prints("[%s] ²»ÏëÊÕµ½ÄúµÄÐÅ¼þ¡£\n", lookupuser.userid);
            return -4;
        }

    }
    sprintf(genbuf, "È·¶¨½«ÎÄÕÂ¼Ä¸ø %s Âð", address);
    if (askyn(genbuf, YEA, NA) == 0)
        return 1;
    if (invalidaddr(address))
        if (!getuser(address))
            return -2;
    sprintf(tmpfname, "tmp/forward.%s.%05d", currentuser.userid,
            uinfo.pid);

    if (in_mail || (strstr(direct, "0Announce") != NULL))
        sprintf(genbuf, "%s/%s", direct, fh->filename);
    else
        sprintf(genbuf, "%s/%s/%s", direct, getfolderstr(fh->filename),
                fh->filename);
    f_cp(genbuf, tmpfname, O_CREAT);

    if (askyn("ÊÇ·ñÐÞ¸ÄÎÄÕÂÄÚÈÝ", NA, NA) == 1)
    {
        if (vedit(tmpfname, NA, NA) == -1)
        {
            if (askyn("ÊÇ·ñ¼Ä³öÎ´ÐÞ¸ÄµÄÎÄÕÂ", YEA, NA) == 0)
            {
                unlink(tmpfname);
                clear();
                return 1;
            }
        }
        clear();
    }
    if (internet_mail)
        if (askyn("ÊÇ·ñ¹ýÂËANSI¿ØÖÆ·û", YEA, NA) == NA)
            filter = NA;
    add_crossinfo(tmpfname, 2);
    prints("×ª¼ÄÐÅ¼þ¸ø %s, ÇëÉÔºò....\n", address);
    refresh();

    if (mode == 0)
        strcpy(fname, tmpfname);
    else if (mode == 1)
    {
        sprintf(fname, "tmp/file.uu%05d", uinfo.pid);
        sprintf(genbuf, "uuencode %s fb-bbs.%05d > %s",
                tmpfname, uinfo.pid, fname);
        system(genbuf);
    }
    if (!strstr(fh->title, "(×ª¼Ä)"))
        sprintf(title, "%.70s(×ª¼Ä)", fh->title);
    else
        strcpy(title, fh->title);
    if (!internet_mail)
        return_no = mail_file(fname, lookupuser.userid, title);
    else
    {
#ifdef SENDMAIL_MIME_AUTOCONVERT
        if (askyn("ÒÔ MIME ¸ñÊ½ËÍÐÅ", NA, NA) == YEA)
            return_no = bbs_sendmail(fname, title, address, filter, YEA);
        else
            return_no = bbs_sendmail(fname, title, address, filter, NA);
#else

        return_no = bbs_sendmail(fname, title, address, filter);
#endif

    }
    if (mode == 1)
    {
        unlink(fname);
    }
    unlink(tmpfname);
    return (return_no);
}
#endif
