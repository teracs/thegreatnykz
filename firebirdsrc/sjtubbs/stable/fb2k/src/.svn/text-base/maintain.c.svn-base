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

#define	READ_MAX_REG_ID_ONCE	30

#ifndef NEW_CREATE_BRD
char cexplain[STRLEN];
#endif
char lookgrp[30];
FILE *cleanlog;
extern struct UTMPFILE *utmpshm;

int check_systempasswd(void)
{
    FILE *pass;
    char passbuf[20], prepass[STRLEN];

    clear();
    if ((pass = fopen("etc/.syspasswd", "r")) != NULL)
    {
        fgets(prepass, STRLEN, pass);
        fclose(pass);
        prepass[strlen(prepass) - 1] = '\0';
        getdata(1, 0, "«Î ‰»ÎœµÕ≥√‹¬Î: ", passbuf, 19, NOECHO, YEA);
        if (passbuf[0] == '\0' || passbuf[0] == '\n')
            return NA;
        if (!checkpasswd(prepass, passbuf))
        {
            move(2, 0);
            prints("¥ÌŒÛµƒœµÕ≥√‹¬Î...");
            securityreport("œµÕ≥√‹¬Î ‰»Î¥ÌŒÛ...");
            pressanykey();
            return NA;
        }
    }
    return YEA;
}

void autoreport(char *title, char *str, char *userid, char *sysboard)
{
    FILE *se;
    char fname[STRLEN];
    int savemode;

    savemode = uinfo.mode;
    report(title);
    sprintf(fname, "tmp/%s.%s.%05d", BBSID, currentuser.userid, uinfo.pid);
    if ((se = fopen(fname, "w")) != NULL)
    {
        fprintf(se, "%s", str);
        fclose(se);
        if (userid != NULL)
            mail_file(fname, userid, title);
        if (sysboard != NULL)
            Postfile(fname, sysboard, title, 1);
        if (currboard != NULL)
            Postfile(fname, currboard, title, 1);
        unlink(fname);
        modify_user_mode(savemode);
    }
}

int securityreport(char *str)
{
    FILE *se;
    char fname[STRLEN];
    int savemode;

    savemode = uinfo.mode;
    report(str);
    sprintf(fname, "tmp/security.%s.%05d", currentuser.userid, uinfo.pid);
    if ((se = fopen(fname, "w")) != NULL)
    {
        fprintf(se, "œµÕ≥∞≤»´º«¬º\n[1m‘≠“Ú£∫%s[m\n", str);
        fprintf(se, "“‘œ¬ «∏ˆ»À◊ ¡œ");
        getuinfo(se);
        fclose(se);
        Postfile(fname, "syssecurity", str, 2);
        unlink(fname);
        modify_user_mode(savemode);
    }
    return 0;
}

int get_grp(char *seekstr)
{
    FILE *fp;
    char buf[STRLEN];
    char *namep;

    if ((fp = fopen("0Announce/.Search", "r")) == NULL)
        return 0;
    while (fgets(buf, STRLEN, fp) != NULL)
    {
        namep = strtok(buf, ": \n\r\t");
        if (namep != NULL && strcasecmp(namep, seekstr) == 0)
        {
            fclose(fp);
            strtok(NULL, "/");
            namep = strtok(NULL, "/");
            if (strlen(namep) < 30)
            {
                strcpy(lookgrp, namep);
                return 1;
            }
            else
                return 0;
        }
    }
    fclose(fp);
    return 0;
}

void stand_title(char *title)
{
    clear();
    standout();
    prints("%s", title);
    standend();
}

int valid_brdname(char *brd)
{
    char ch;

    ch = *brd++;
    if (!isalnum(ch) && ch != '_')
        return 0;
    while ((ch = *brd++) != '\0')
    {
        if (!isalnum(ch) && ch != '_')
            return 0;
    }
    return 1;
}

#ifndef NEW_CREATE_BRD
char *chgrp(void)
{
    int i, ch;
    char buf[STRLEN], ans[6];

    static char *explain[] = {
        "±æ’æœµÕ≥",
        "Õ≈ÃÂΩª¡˜",
        "BBS œµÕ≥",
        "µÁƒ‘ºº ı",
        "—ß ıø∆—ß",
        "–›œ–”È¿÷",
        "÷™–‘∏––‘",
        "ÃÂ”˝–¬Œ≈",
        NULL
    };

    static char *groups[] = {
        "GROUP_0",
        "GROUP_1",
        "GROUP_2",
        "GROUP_3",
        "GROUP_4",
        "GROUP_5",
        "GROUP_6",
        "GROUP_7",
        NULL
    };

    clear();
    move(2, 0);
    prints("—°‘Òæ´ª™«¯µƒƒø¬º\n\n");
    for (i = 0;; i++)
    {
        if (explain[i] == NULL || groups[i] == NULL)
            break;
        prints("[1;32m%2d[m. %-20s%-20s\n", i, explain[i], groups[i]);
    }
    sprintf(buf, "«Î ‰»Îƒ„µƒ—°‘Ò(0~%d): ", --i);
    while (1)
    {
        getdata(i + 6, 0, buf, ans, 4, DOECHO, YEA);
        if (!isdigit(ans[0]))
            continue;
        ch = atoi(ans);
        if (ch < 0 || ch > i || ans[0] == '\r' || ans[0] == '\0')
            continue;
        else
            break;
    }
    sprintf(cexplain, "%s", explain[ch]);

    return groups[ch];
}
#endif

char curruser[IDLEN + 2];
extern int delmsgs[];
extern int delcnt;

int domailclean(void *vfhdrp)
{
    struct fileheader *fhdrp = (struct fileheader *) vfhdrp;
    static int newcnt, savecnt, deleted, idc;
    char buf[STRLEN];

    if (fhdrp == NULL)
    {
        fprintf(cleanlog, "new = %d, saved = %d, deleted = %d\n",
                newcnt, savecnt, deleted);
        newcnt = savecnt = deleted = idc = 0;
        if (delcnt)
        {
            sprintf(buf, "mail/%c/%c/%s/%s", toupper(curruser[0]),
                    toupper(curruser[1]), curruser, DOT_DIR);
            while (delcnt--)
                delete_record(buf, sizeof(struct fileheader),
                              delmsgs[delcnt]);
        }
        delcnt = 0;
        return 0;
    }
    idc++;
    if (!(fhdrp->accessed[0] & FILE_READ))
        newcnt++;
    else if (fhdrp->accessed[0] & FILE_MARKED)
        savecnt++;
    else
    {
        deleted++;
        sprintf(buf, "mail/%c/%c/%s/%s", toupper(curruser[0]),
                toupper(curruser[1]), curruser, fhdrp->filename);
        unlink(buf);
        delmsgs[delcnt++] = idc;
    }
    return 0;
}

int cleanmail(void *vurec)
{
    struct userec *urec = (struct userec *) vurec;
    struct stat statb;

    if (urec->userid[0] == '\0' || !strcmp(urec->userid, "new"))
        return 0;
    sprintf(genbuf, "mail/%c/%c/%s/%s", toupper(urec->userid[0]),
            toupper(urec->userid[1]), urec->userid, DOT_DIR);
    fprintf(cleanlog, "%s: ", urec->userid);
    if (stat(genbuf, &statb) == -1)
        fprintf(cleanlog, "no mail\n");
    else if (statb.st_size == 0)
        fprintf(cleanlog, "no mail\n");
    else
    {
        strcpy(curruser, urec->userid);
        delcnt = 0;
        apply_record(genbuf, domailclean, sizeof(struct fileheader));
        domailclean(NULL);
    }
    return 0;
}

void trace_state(int flag, char *name, int size)
{
    char buf[STRLEN];

    if (flag != -1)
    {
        sprintf(buf, "ON (size = %d)", size);
    }
    else
    {
        strcpy(buf, "OFF");
    }
    prints("%sº«¬º %s\n", name, buf);
}

void id_record(char *userid, int TYPE)
{
    char buf[STRLEN];
    char garbage[2] = { "xy" };
    FILE *fp = NULL;
    char mode[3][10] = {
        "accept",
        "reject",
        "del"
    };

    if ((TYPE < 0) || (TYPE > 2))
        return;

    sprintf(buf, "etc/idadmin.%s.%s", userid, mode[TYPE]);
    fp = fopen(buf, "a");
    if (fp == NULL)
        return;
    fwrite(garbage, 2, 1, fp);
    fclose(fp);
    return;
}

int scan_new_register_form(char *regfile, char *searchid)
{
    static char *reason[] = {
        "«Î»∑ µÃÓ–¥’Ê µ–’√˚.",
        "«ÎœÍœ∏ÃÓ–¥µ±«∞ªÚ±œ“µ—ß–£‘∫œµƒÍº∂.",
	"«ÎœÍœ∏ÃÓ–¥π§◊˜µ•Œª∫Õ≤ø√≈.",
        "«ÎÃÓ–¥ÕÍ’˚µÿ÷∑µΩ¬•∂∞∫≈.",
        "ƒ˙µƒ◊ ¡œ”–∂‡¥¶≤ª∫œ“™«Û,«Î»œ’ÊÃÓ–¥√øœÓ◊ ¡œ.",
        "«Î”√÷–ŒƒÃÓ–¥…Í«Îµ•.",
        NULL
    };

    struct userec urec;
    FILE *fout, *freg;
    char fname[STRLEN], buf[STRLEN];
    char infname[STRLEN];
    char ans[5], *uid;
    char userid[IDLEN];
    int n, unum;
    struct stat st;
    int record_ns = 0;
    int temp_read_ns = 0;
    int k = 0;
    int x = 0;

    if (searchid)
    {
        strcpy(userid, searchid);
    }
    uid = currentuser.userid;
    stand_title("“¿¥Œ…Ë∂® π”√’ﬂ◊¢≤·◊ ¡œ (new method by rwzmm, ^_^)");
    sprintf(fname, "%s.tmp", regfile);
    sprintf(infname, "tmp/reginf.%s.%05d", currentuser.userid, uinfo.pid);
    move(2, 0);

    if (dashf(fname))
    {
        move(1, 0);
        prints("∆‰À˚’ ∫≈π‹¿Ì‘±’˝‘⁄≤Èø¥◊¢≤·…Í«Îµ•, «ÎºÏ≤È π”√’ﬂ◊¥Ã¨.\n");
        prints
            ("ƒø«∞±æ’æ’ ∫≈π‹¿Ì‘±£∫\033[1;36msupermaltose darkfighter starboy \033[1;35myymomo \033[1;36myisuoyinyu\033[m\n\n");
        prints
            ("warning: \033[1;31m»Áπ˚∂‡»ÀÕ¨ ±≈˙◊¢≤·µ•£¨ª·‘Ï≥…—œ÷ÿ∫Ûπ˚£°\033[m\n");
        getdata(5, 0, "ƒ„»∑∂®√ª”–∆‰À˚’ ∫≈π‹¿Ì‘±‘⁄…Û∫À◊¢≤·µ•¬ £ø [y/N]: ",
                ans, 2, DOECHO, YEA);
        if (ans[0] == 'Y' || ans[0] == 'y')
        {
            FILE *reg;
            FILE *tmp;
            struct regform record;

            tmp = fopen(fname, "r");
            reg = fopen(regfile, "a");
            while (fread(&record, sizeof(struct regform), 1, tmp) > 0)
            {
                if (record.status == 0)
                    fwrite(&record, sizeof(struct regform), 1, reg);
            }
            fclose(reg);
            fclose(tmp);
        }
        else
        {
            pressreturn();
            return -1;
        }
    }

    if (!dashf(regfile))
        return -1;
    stat(regfile, &st);
    record_ns = st.st_size / sizeof(struct regform);
    temp_read_ns = record_ns;

    for (k = 0;
         k < (record_ns + READ_MAX_REG_ID_ONCE - 1) / READ_MAX_REG_ID_ONCE;
         k++)
    {
        FILE *fp_reg;
        FILE *fp_tmp;
        FILE *fp_bak;
        int i = 0;
        int j = 0;
        char tempbuf[STRLEN * 2];

        struct regform record[READ_MAX_REG_ID_ONCE];
        struct regform rec_tmp;
        int read_ns = 0;
        char backup[20];

        sprintf(backup, "register_backup");

        if (!dashf(regfile))
            break;

        sprintf(backup, "register_backup");
        memset(record, 0x00,
               sizeof(struct regform) * READ_MAX_REG_ID_ONCE);
        fp_reg = fopen(regfile, "r");
        fp_bak = fopen(backup, "w");
        fp_tmp = fopen(fname, "w");
        if (temp_read_ns > READ_MAX_REG_ID_ONCE)
        {
            temp_read_ns -= READ_MAX_REG_ID_ONCE;
            fread(record, sizeof(struct regform), READ_MAX_REG_ID_ONCE,
                  fp_reg);
            fwrite(record, sizeof(struct regform), READ_MAX_REG_ID_ONCE,
                   fp_tmp);
            while (fread(&rec_tmp, sizeof(struct regform), 1, fp_reg) > 0)
            {
                fwrite(&rec_tmp, sizeof(struct regform), 1, fp_bak);
            }
            fclose(fp_bak);
            fclose(fp_tmp);
            fclose(fp_reg);
            unlink(regfile);
            rename(backup, regfile);
            read_ns = READ_MAX_REG_ID_ONCE;
        }
        else
        {
            fread(record, sizeof(struct regform), temp_read_ns, fp_reg);
            fwrite(record, sizeof(struct regform), temp_read_ns, fp_tmp);
            while (fread(&rec_tmp, sizeof(struct regform), 1, fp_reg) > 0)
            {
                fwrite(&rec_tmp, sizeof(struct regform), 1, fp_bak);
            }
            fclose(fp_bak);
            fclose(fp_tmp);
            fclose(fp_reg);
            unlink(regfile);
            rename(backup, regfile);
            read_ns = temp_read_ns;
        }

        for (i = 0; i < read_ns; i++)
        {
            FILE *fp_inf;

            if ((unum = getuser(record[i].userid)) == 0)
            {
                move(2, 0);
                clrtobot();
                prints("œµÕ≥¥ÌŒÛ, ≤ÈŒﬁ¥À’ ∫≈.\n\n");
                prints("’ ∫≈Œª÷√     : %d\n", record[i].usernum);
                prints("…Í«Î’ ∫≈     : %s\n", record[i].userid);
                prints("’Ê µ–’√˚     : %s\n", record[i].realname);
                prints("—ß–£œµº∂     : %s\n", record[i].dept);
                prints("ƒø«∞◊°÷∑     : %s\n", record[i].addr);
                prints("¡™¬ÁµÁª∞     : %s\n", record[i].phone);
                record[i].status |= REG_DELETE;
                fp_tmp = fopen(fname, "w");
                fwrite(record, sizeof(struct regform), read_ns, fp_tmp);
                fclose(fp_tmp);
                pressreturn();
            }

            else
            {
                if (searchid && strncmp(userid, record[i].userid, IDLEN))
                    ans[0] = 'S';
                else
                {
                    memcpy(&urec, &lookupuser, sizeof(urec));
                    sethomefile(buf, urec.userid, "register");
                    if (dashf(buf))
                        ans[0] = 'd';
                    else
                    {
                        move(1, 0);
                        prints("’ ∫≈Œª÷√     : %d\n", unum);
                        disply_userinfo(&urec);
                        move(15, 0);
                        printdash(NULL);
                        prints("’ ∫≈Œª÷√     : %d, %s\n",
                               record[i].usernum, record[i].date);
                        prints("…Í«Î’ ∫≈     : %s\n", record[i].userid);
                        prints("’Ê µ–’√˚     : %s\n", record[i].realname);
                        prints("—ß–£œµº∂     : %s\n", record[i].dept);
                        prints("ƒø«∞◊°÷∑     : %s\n", record[i].addr);
                        prints("¡™¬ÁµÁª∞     : %s\n", record[i].phone);

                        if (urec.userlevel & PERM_LOGINOK)
                        {
                            move(t_lines - 1, 0);
                            prints("¥À’ ∫≈≤ª–Ë‘ŸÃÓ–¥◊¢≤·µ•.\n");
                            igetkey();
                            ans[0] = 'D';
                        }
                        else
                        {
                            getdata(t_lines - 1, 0,
                                    " «∑ÒΩ” ‹¥À◊ ¡œ (Y/N/Q/Del/Skip)? [S]: ",
                                    ans, 3, DOECHO, YEA);
                        }
                        move(1, 0);
                        clrtobot();
                    }
                }
                switch (ans[0])
                {
                case 'D':
                case 'd':
                    record[i].status |= REG_DELETE;
                    fp_tmp = fopen(fname, "w");
                    fwrite(record, sizeof(struct regform), read_ns,
                           fp_tmp);
                    fclose(fp_tmp);
                    id_record(currentuser.userid, 2);
                    break;
                case 'Y':
                case 'y':
                    prints("“‘œ¬ π”√’ﬂ◊ ¡œ“—æ≠∏¸–¬:\n\n");
                    strncpy(urec.realname, record[i].realname, NAMELEN);
                    strncpy(urec.address, record[i].addr, NAMELEN);
                    sprintf(genbuf, "%s$%s@%s", record[i].dept,
                            record[i].phone, uid);
                    genbuf[STRLEN - 16] = '\0';
                    strncpy(urec.reginfo, genbuf, STRLEN - 17);
                    urec.lastjustify = time(0);
                    substitute_record(PASSFILE, &urec, sizeof(urec),
                                      unum);
                    sethomefile(buf, urec.userid, "register");
                    if (dashf(buf))
                    {
                        sethomefile(genbuf, urec.userid, "register.old");
                        rename(buf, genbuf);
                    }
                    if ((fout = fopen(buf, "w")) != NULL)
                    {
                        fprintf(fout, "usernum: %d, %s\n",
                                record[i].usernum, record[i].date);
                        fprintf(fout, "userid: %s\n", record[i].userid);
                        fprintf(fout, "realname: %s\n",
                                record[i].realname);
                        fprintf(fout, "dept: %s\n", record[i].dept);
                        fprintf(fout, "addr: %s\n", record[i].addr);
                        fprintf(fout, "phone: %s\n\n", record[i].phone);

                        n = time(NULL);
                        getdatestring(n, NA);
                        fprintf(fout, "Date: %s\n", datestring);
                        fprintf(fout, "Approved: %s\n", uid);
                        fclose(fout);
                    }

                    sprintf(tempbuf, "home/%c/%c/%s/register",
                            toupper(userid[0]), toupper(userid[1]),
                            userid);
                    if ((fout = fopen(tempbuf, "r")) != NULL)
                    {
                        for (x = 1; x <= 15; x++)
                        {
                            if (fgets(tempbuf, STRLEN, fout))
                                prints("%s", tempbuf);
                            else
                                break;
                        }
                        fclose(fout);
                    }

                    in_mail = YEA;
                    strncpy(save_title, "πßÏ˚ƒ„£¨ƒ„“—æ≠ÕÍ≥…◊¢≤·°£", STRLEN);
                    fp_inf = fopen(infname, "w");
                    write_header(fp_inf, 0, 0);
                    fclose(fp_inf);
                    f_cp("etc/s_fill", infname, O_APPEND);
                    mail_file(infname, urec.userid,
                              "πßÏ˚ƒ„£¨ƒ„“—æ≠ÕÍ≥…◊¢≤·°£");
                    unlink(infname);
                    in_mail = NA;
                    sethomefile(buf, urec.userid, "mailcheck");
                    unlink(buf);
                    record[i].status |= REG_ACCEPT;

                    fp_tmp = fopen(fname, "w");
                    fwrite(record, sizeof(struct regform), read_ns,
                           fp_tmp);
                    fclose(fp_tmp);
                    id_record(currentuser.userid, 0);
                    break;
                case 'Q':
                case 'q':
                    if ((freg = fopen(regfile, "a")) != NULL)
                    {
                        for (j = 0; j < read_ns; j++)
                        {
                            if (record[j].status == 0)
                            {
                                fwrite(&record[j], sizeof(struct regform),
                                       1, freg);
                            }
                        }
                        fclose(freg);
                    }
                    goto end;
                    break;
                case 'N':
                case 'n':
                    prints("’ ∫≈Œª÷√: %d, %s\n",
                           record[i].usernum, record[i].date);
                    prints("…Í«Î’ ∫≈: %s\n", record[i].userid);
                    prints("’Ê µ–’√˚: %s\n", record[i].realname);
                    prints("—ß–£œµº∂: %s\n", record[i].dept);
                    prints("ƒø«∞◊°÷∑: %s\n", record[i].addr);
                    prints("¡™¬ÁµÁª∞: %s\n", record[i].phone);

                    printdash(NULL);
                    move(9, 0);
                    prints
                        ("«Î—°‘Ò/ ‰»ÎÕÀªÿ…Í«Î±Ì‘≠“Ú, ∞¥ <enter> »°œ˚.\n\n");
                    for (n = 0; reason[n] != NULL; n++)
                        prints("%d) %s\n", n, reason[n]);
                    getdata(12 + n, 0, "ÕÀªÿ‘≠“Ú: ", buf, 60, DOECHO, YEA);
                    if (buf[0] != '\0')
                    {
                        if (buf[0] >= '0' && buf[0] < '0' + n)
                        {
                            strcpy(buf, reason[buf[0] - '0']);
                        }
                        sprintf(genbuf, "<◊¢≤· ß∞‹> - %s", buf);
                        strncpy(urec.address, genbuf, NAMELEN);
                        substitute_record(PASSFILE, &urec, sizeof(urec),
                                          unum);
                        in_mail = YEA;
                        strncpy(save_title, urec.address, STRLEN);
                        fp_inf = fopen(infname, "w");
                        write_header(fp_inf, 0, 0);
                        fclose(fp_inf);
                        f_cp("etc/f_fill", infname, O_APPEND);
                        mail_file(infname, urec.userid,
                                  urec.address);
                        unlink(infname);
                        in_mail = NA;

                        record[i].status |= REG_REJECT;
                        fp_tmp = fopen(fname, "w");
                        fwrite(record, sizeof(struct regform), read_ns,
                               fp_tmp);
                        fclose(fp_tmp);
                        id_record(currentuser.userid, 1);
                        break;
                    }
                    move(10, 0);
                    clrtobot();
                    prints("»°œ˚ÕÀªÿ¥À◊¢≤·…Í«Î±Ì.\n");

                default:
                    if ((freg = fopen(regfile, "a")) != NULL)
                    {
                        if (record[i].status == 0)
                            fwrite(&record[i], sizeof(struct regform), 1,
                                   freg);
                        fclose(freg);
                    }
                    record[i].status |= REG_REJECT;
                    fp_tmp = fopen(fname, "w");
                    fwrite(record, sizeof(struct regform), read_ns,
                           fp_tmp);
                    fclose(fp_tmp);
                }

            }

        }
    }
  end:
    unlink(fname);
    return 0;
}

#ifndef WITHOUT_ADMIN_TOOLS

int kick_user(struct user_info *userinfo)
{
    int id, ind, i;
    struct user_info uin;
    struct userec kuinfo;
    char kickuser[40], buffer[40];

    if (uinfo.mode != LUSERS && uinfo.mode != OFFLINE
        && uinfo.mode != FRIEND && uinfo.mode != WATCHINBOARD)
    {
        modify_user_mode(ADMIN);
        stand_title("Ãﬂ π”√’ﬂœ¬’æ");
        move(1, 0);
        usercomplete(" ‰»Î π”√’ﬂ’ ∫≈: ", kickuser);
        if (*kickuser == '\0')
        {
            clear();
            return 0;
        }
        if (!(id = getuser(kickuser)))
        {
            move(3, 0);
            prints("Œﬁ–ßµƒ”√ªß ID£°");
            clrtoeol();
            pressreturn();
            clear();
            return 0;
        }
        move(1, 0);
        clrtoeol();
        sprintf(genbuf, "ÃﬂµÙ π”√’ﬂ : [%s].", kickuser);
        move(2, 0);
        if (askyn(genbuf, NA, NA) == NA)
        {
            move(2, 0);
            prints("»°œ˚Ãﬂ π”√’ﬂ..\n");
            pressreturn();
            clear();
            return 0;
        }
        search_record(PASSFILE, &kuinfo, sizeof(kuinfo), cmpuids,
                      kickuser);
        ind = search_ulist(&uin, t_cmpuids, id);
    }
    else
    {
        uin = *userinfo;
        strcpy(kickuser, uin.userid);
        ind = YEA;
    }
    if (!ind || !uin.active || (uin.pid && kill(uin.pid, 0) == -1))
    {
        if (uinfo.mode != LUSERS && uinfo.mode != OFFLINE
            && uinfo.mode != FRIEND)
        {
            move(3, 0);
            prints("”√ªß [%s] ≤ª‘⁄œﬂ…œ", kickuser);
            clrtoeol();
            pressreturn();
            clear();
        }
        return 0;
    }
    if (is_web_user(&uin))
    {
        for (i = 0; i < MAXACTIVE; i++)
            if (!strcasecmp(uin.userid, utmpshm->uinfo[i].userid))
                break;
        if (i != MAXACTIVE)
            kick_web_user(&(utmpshm->uinfo[i]));
    }
    else
        kill(uin.pid, SIGHUP);
    sprintf(buffer, "kicked %s", kickuser);
    report(buffer);
    sprintf(genbuf, "%s (%s)", kuinfo.userid, kuinfo.username);
    log_usies("KICK ", genbuf);
    move(2, 0);
    if (uinfo.mode != LUSERS && uinfo.mode != OFFLINE
        && uinfo.mode != FRIEND && uinfo.mode != WATCHINBOARD)
    {
        prints("”√ªß [%s] “—æ≠±ªÃﬂœ¬’æ.\n", kickuser);
        pressreturn();
        clear();
    }
    return 1;
}
#endif
