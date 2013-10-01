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
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/
/*
$Id: maintain.c,v 1.3 2004/07/17 10:28:30 jawahh Exp $
*/

#include "bbs.h"


#define	READ_MAX_REG_ID_ONCE	30	//√ø¥Œ»°30∏ˆidΩ¯ƒ⁄¥Ê°£
//#define	READ_MAX_REG_ID_ONCE	5		//≤‚ ‘µƒ ±∫Ú»°5∏ˆ°£


#ifndef NEW_CREATE_BRD
char cexplain[STRLEN];
#endif
char lookgrp[30];
FILE *cleanlog;
extern struct UTMPFILE *utmpshm;

int
check_systempasswd ()
{
    FILE *pass;
    char passbuf[20], prepass[STRLEN];

    clear ();
    if ((pass = fopen ("etc/.syspasswd", "r")) != NULL)
    {
        fgets (prepass, STRLEN, pass);
        fclose (pass);
        prepass[strlen (prepass) - 1] = '\0';
        getdata (1, 0, "«Î ‰»ÎœµÕ≥√‹¬Î: ", passbuf, 19, NOECHO, YEA);
        if (passbuf[0] == '\0' || passbuf[0] == '\n')
            return NA;
        if (!checkpasswd (prepass, passbuf))
        {
            move (2, 0);
            prints ("¥ÌŒÛµƒœµÕ≥√‹¬Î...");
            securityreport ("œµÕ≥√‹¬Î ‰»Î¥ÌŒÛ...");
            pressanykey ();
            return NA;
        }
    }
    return YEA;
}


autoreport (title, str, userid, sysboard)
char *title;
char *str;
char *userid;
char *sysboard;
{
    FILE *se;
    char fname[STRLEN];
    int savemode;

    savemode = uinfo.mode;
    report (title);
    sprintf (fname, "tmp/%s.%s.%05d", BBSID, currentuser.userid, uinfo.pid);
    if ((se = fopen (fname, "w")) != NULL)
    {
        fprintf (se, "%s", str);
        fclose (se);
        if (userid != NULL)
            mail_file (fname, userid, title);
        if (sysboard != NULL)
            Postfile (fname, sysboard, title, 1);
        if (currboard != NULL)
			Postfile (fname, currboard, title, 1);
        unlink (fname);
        modify_user_mode (savemode);
    }
}

int
securityreport (str)
char *str;
{
    FILE *se;
    char fname[STRLEN];
    int savemode;

    savemode = uinfo.mode;
    report (str);
    sprintf (fname, "tmp/security.%s.%05d", currentuser.userid, uinfo.pid);
    if ((se = fopen (fname, "w")) != NULL)
    {
        fprintf (se, "œµÕ≥∞≤»´º«¬º\n[1m‘≠“Ú£∫%s[m\n", str);
        fprintf (se, "“‘œ¬ «∏ˆ»À◊ ¡œ");
        getuinfo (se);
        fclose (se);
        Postfile (fname, "syssecurity", str, 2);
        unlink (fname);
        modify_user_mode (savemode);
    }
}

int
get_grp (seekstr)
char seekstr[STRLEN];
{
    FILE *fp;
    char buf[STRLEN];
    char *namep;

    if ((fp = fopen ("0Announce/.Search", "r")) == NULL)
        return 0;
    while (fgets (buf, STRLEN, fp) != NULL)
    {
        namep = strtok (buf, ": \n\r\t");
        if (namep != NULL && ci_strcmp (namep, seekstr) == 0)
        {
            fclose (fp);
            strtok (NULL, "/");
            namep = strtok (NULL, "/");
            if (strlen (namep) < 30)
            {
                strcpy (lookgrp, namep);
                return 1;
            }
            else
                return 0;
        }
    }
    fclose (fp);
    return 0;
}

void
stand_title (title)
char *title;
{
    clear ();
    standout ();
    prints ("%s", title);
    standend ();
}

int
valid_brdname (brd)
char *brd;
{
    char ch;

    ch = *brd++;
    if (!isalnum (ch) && ch != '_')
        return 0;
    while ((ch = *brd++) != '\0')
    {
        if (!isalnum (ch) && ch != '_')
            return 0;
    }
    return 1;
}

#ifndef NEW_CREATE_BRD
char *
chgrp ()
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

    clear ();
    move (2, 0);
    prints ("—°‘Òæ´ª™«¯µƒƒø¬º\n\n");
    for (i = 0;; i++)
    {
        if (explain[i] == NULL || groups[i] == NULL)
            break;
        prints ("[1;32m%2d[m. %-20s%-20s\n", i, explain[i], groups[i]);
    }
    sprintf (buf, "«Î ‰»Îƒ„µƒ—°‘Ò(0~%d): ", --i);
    while (1)
    {
        getdata (i + 6, 0, buf, ans, 4, DOECHO, YEA);
        if (!isdigit (ans[0]))
            continue;
        ch = atoi (ans);
        if (ch < 0 || ch > i || ans[0] == '\r' || ans[0] == '\0')
            continue;
        else
            break;
    }
    sprintf (cexplain, "%s", explain[ch]);

    return groups[ch];
}
#endif

char curruser[IDLEN + 2];
extern int delmsgs[];
extern int delcnt;

void
domailclean (fhdrp)
struct fileheader *fhdrp;
{
    static int newcnt, savecnt, deleted, idc;
    char buf[STRLEN];

    if (fhdrp == NULL)
    {
        fprintf (cleanlog, "new = %d, saved = %d, deleted = %d\n",
                 newcnt, savecnt, deleted);
        newcnt = savecnt = deleted = idc = 0;
        if (delcnt)
        {
            sprintf (buf, "mail/%c/%s/%s", toupper (curruser[0]), curruser,
                     DOT_DIR);
            while (delcnt--)
                delete_record (buf, sizeof (struct fileheader), delmsgs[delcnt]);
        }
        delcnt = 0;
        return;
    }
    idc++;
    if (!(fhdrp->accessed[0] & FILE_READ))
        newcnt++;
    else if (fhdrp->accessed[0] & FILE_MARKED)
        savecnt++;
    else
    {
        deleted++;
        sprintf (buf, "mail/%c/%s/%s", toupper (curruser[0]), curruser,
                 fhdrp->filename);
        unlink (buf);
        delmsgs[delcnt++] = idc;
    }
}

int
cleanmail (urec)
struct userec *urec;
{
    struct stat statb;

    if (urec->userid[0] == '\0' || !strcmp (urec->userid, "new"))
        return 0;
    sprintf (genbuf, "mail/%c/%s/%s", toupper (urec->userid[0]),
             urec->userid, DOT_DIR);
    fprintf (cleanlog, "%s: ", urec->userid);
    if (stat (genbuf, &statb) == -1)
        fprintf (cleanlog, "no mail\n");
    else if (statb.st_size == 0)
        fprintf (cleanlog, "no mail\n");
    else
    {
        strcpy (curruser, urec->userid);
        delcnt = 0;
        apply_record (genbuf, domailclean, sizeof (struct fileheader));
        domailclean (NULL);
    }
    return 0;
}


void
trace_state (flag, name, size)
int flag, size;
char *name;
{
    char buf[STRLEN];

    if (flag != -1)
    {
        sprintf (buf, "ON (size = %d)", size);
    }
    else
    {
        strcpy (buf, "OFF");
    }
    prints ("%sº«¬º %s\n", name, buf);
}

/*

int
scan_register_form (regfile, searchid)
char *regfile;
char *searchid;
{
    static char *field[] = { "usernum", "userid", "realname", "dept",
                             "addr", "phone", "assoc", NULL
                           };
    static char *finfo[] = { "’ ∫≈Œª÷√", "…Í«Î’ ∫≈", "’Ê µ–’√˚", "—ß–£œµº∂",
                             "ƒø«∞◊°÷∑", "¡¨¬ÁµÁª∞", "–£ ”— ª·", NULL
                           };
    static char *reason[] = { "«Î»∑ µÃÓ–¥’Ê µ–’√˚.",
							  "«ÎŒ π”√≤ª—≈Í«≥∆.",
		                      "«ÎœÍÃÓ–¥—ß–£‘∫œµƒÍº∂£¨‘⁄÷∞»À ø«ÎÃÓ–¥π§◊˜µ•Œª∫Õ≤ø√≈.",
                              "«ÎÃÓ–¥ÕÍ’˚µƒæ”◊°µÿ÷∑ªÚµ•Œªµÿ÷∑£®∑«µÁ◊”” º˛–≈œ‰£©.", 
							  "«ÎœÍÃÓ¡¨¬ÁµÁª∞.", 
							  "ƒ˙µƒ◊ ¡œ”–∂‡¥¶≤ª∫œ“™«Û,«Î»œ’ÊÃÓ–¥√øœÓ◊ ¡œ.",
                              "«Î”√÷–ŒƒÃÓ–¥…Í«Îµ•.", 
							  "∂‘≤ª∆£¨∏√IDŒ™±æ’æ≤ªΩ” ‹µƒID", 
							  NULL
                            };
    struct userec uinfo;
    FILE *fn, *fout, *freg;
    char fdata[7][STRLEN];
    char fname[STRLEN], buf[STRLEN];
    char ans[5], *ptr, *uid;
    char userid[IDLEN];
    int n, unum;

    if (searchid)
        strcpy (userid, searchid);
    uid = currentuser.userid;
    stand_title ("“¿–Ú…Ë∂®À˘”––¬◊¢≤·◊ ¡œ");
    sprintf (fname, "%s.tmp", regfile);
    move (2, 0);
    if (dashf (fname))
    {
        move (1, 0);
        prints ("∆‰À˚ SYSOP ’˝‘⁄≤Èø¥◊¢≤·…Í«Îµ•, «ÎºÏ≤È π”√’ﬂ◊¥Ã¨.\n");
        getdata (2, 0, "ƒ„»∑∂®√ª”–∆‰À˚ SYSOP ‘⁄…Û∫À◊¢≤·µ•¬ £ø [y/N]: ",
                 ans, 2, DOECHO, YEA);
        if (ans[0] == 'Y' || ans[0] == 'y')
            f_cp (fname, regfile, O_APPEND);
        else
        {
            pressreturn ();
            return -1;
        }
    }
    rename (regfile, fname);
    if ((fn = fopen (fname, "r")) == NULL)
    {
        move (2, 0);
        prints ("œµÕ≥¥ÌŒÛ, Œﬁ∑®∂¡»°◊¢≤·◊ ¡œµµ: %s\n", fname);
        pressreturn ();
        return -1;
    }
    memset (fdata, 0, sizeof (fdata));
    while (fgets (genbuf, STRLEN, fn) != NULL)
    {
        if ((ptr = (char *) strstr (genbuf, ": ")) != NULL)
        {
            *ptr = '\0';
            for (n = 0; field[n] != NULL; n++)
            {
                if (strcmp (genbuf, field[n]) == 0)
                {
                    strcpy (fdata[n], ptr + 2);
                    if ((ptr = (char *) strchr (fdata[n], '\n')) != NULL)
                        *ptr = '\0';
                }
            }
        }
        else if ((unum = getuser (fdata[1])) == 0)
        {
            move (2, 0);
            clrtobot ();
            prints ("œµÕ≥¥ÌŒÛ, ≤ÈŒﬁ¥À’ ∫≈.\n\n");
            for (n = 0; field[n] != NULL; n++)
                prints ("%s     : %s\n", finfo[n], fdata[n]);
            pressreturn ();
            memset (fdata, 0, sizeof (fdata));
        }
        else
        {
            if (searchid && strcmp (userid, fdata[1]))
                ans[0] = 'S';
            else
            {
                memcpy (&uinfo, &lookupuser, sizeof (uinfo));
                sethomefile (buf, uinfo.userid, "register");
                if (dashf (buf))
                    ans[0] = 'd';
                else
                {
                    move (1, 0);
                    prints ("’ ∫≈Œª÷√     : %d\n", unum);
                    disply_userinfo (&uinfo);
                    move (15, 0);
                    printdash (NULL);
                    for (n = 0; field[n] != NULL; n++)
                        prints ("%s     : %s\n", finfo[n], fdata[n]);
                    if (uinfo.userlevel & PERM_LOGINOK)
                    {
                        move (t_lines - 1, 0);
                        prints ("¥À’ ∫≈≤ª–Ë‘ŸÃÓ–¥◊¢≤·µ•.\n");
                        igetkey ();
                        ans[0] = 'D';
                    }
                    else
                    {
                        getdata (t_lines - 1, 0,
                                 " «∑ÒΩ” ‹¥À◊ ¡œ (Y/N/Q/Del/Skip)? [S]: ",
                                 ans, 3, DOECHO, YEA);
                    }
                    move (1, 0);
                    clrtobot ();
                }
            }
            switch (ans[0])
            {
            case 'D':
            case 'd':
                break;
            case 'Y':
            case 'y':
                prints ("“‘œ¬ π”√’ﬂ◊ ¡œ“—æ≠∏¸–¬:\n");
                n = strlen (fdata[5]);
                if (n + strlen (fdata[3]) > 60)
                {
                    if (n > 40)
                        fdata[5][n = 40] = '\0';
                    fdata[3][60 - n] = '\0';
                }
                strncpy (uinfo.realname, fdata[2], NAMELEN);
                strncpy (uinfo.address, fdata[4], NAMELEN);
                sprintf (genbuf, "%s$%s@%s", fdata[3], fdata[5], uid);
                genbuf[STRLEN - 16] = '\0';
                strncpy (uinfo.reginfo, genbuf, STRLEN - 17);
                uinfo.lastjustify = time (0);
                substitute_record (PASSFILE, &uinfo, sizeof (uinfo), unum);
                sethomefile (buf, uinfo.userid, "register");
                if (dashf (buf))
                {
                    sethomefile (genbuf, uinfo.userid, "register.old");
                    rename (buf, genbuf);
                }
                if ((fout = fopen (buf, "w")) != NULL)
                {
                    for (n = 0; field[n] != NULL; n++)
                        fprintf (fout, "%s: %s\n", field[n], fdata[n]);
                    n = time (NULL);
                    getdatestring (n, NA);
                    fprintf (fout, "Date: %s\n", datestring);
                    fprintf (fout, "Approved: %s\n", uid);
                    fclose (fout);
                }
                mail_file ("etc/s_fill", uinfo.userid,
                           "πßÏ˚ƒ„£¨ƒ„“—æ≠ÕÍ≥…◊¢≤·°£");
                sethomefile (buf, uinfo.userid, "mailcheck");
                unlink (buf);

                //                              remove by alt@sjtu
                //                              sprintf(genbuf, "»√ %s Õ®π˝…Ì∑÷»∑»œ.", uinfo.userid);
                //                              securityreport(genbuf);
                break;
            case 'Q':
            case 'q':
                if ((freg = fopen (regfile, "a")) != NULL)
                {
                    for (n = 0; field[n] != NULL; n++)
                        fprintf (freg, "%s: %s\n", field[n], fdata[n]);
                    fprintf (freg, "----\n");
                    while (fgets (genbuf, STRLEN, fn) != NULL)
                        fputs (genbuf, freg);
                    fclose (freg);
                }
                break;
            case 'N':
            case 'n':
                for (n = 0; field[n] != NULL; n++)
                    prints ("%s: %s\n", finfo[n], fdata[n]);
                printdash (NULL);
                move (9, 0);
                prints ("«Î—°‘Ò/ ‰»ÎÕÀªÿ…Í«Î±Ì‘≠“Ú, ∞¥ <enter> »°œ˚.\n\n");
                for (n = 0; reason[n] != NULL; n++)
                    prints ("%d) %s\n", n, reason[n]);
                getdata (12 + n, 0, "ÕÀªÿ‘≠“Ú: ", buf, 60, DOECHO, YEA);
                if (buf[0] != '\0')
                {
                    if (buf[0] >= '0' && buf[0] < '0' + n)
                    {
                        strcpy (buf, reason[buf[0] - '0']);
                    }
                    sprintf (genbuf, "<◊¢≤· ß∞‹> - %s", buf);
                    strncpy (uinfo.address, genbuf, NAMELEN);
                    substitute_record (PASSFILE, &uinfo, sizeof (uinfo), unum);
                    mail_file ("etc/f_fill", uinfo.userid, uinfo.address);
                    / * user_display( &uinfo, 1 ); * /
                    / * pressreturn(); * /
                    break;
                }
                move (10, 0);
                clrtobot ();
                prints ("»°œ˚ÕÀªÿ¥À◊¢≤·…Í«Î±Ì.\n");
                / * run default -- put back to regfile * /
            default:
                if ((freg = fopen (regfile, "a")) != NULL)
                {
                    for (n = 0; field[n] != NULL; n++)
                        fprintf (freg, "%s: %s\n", field[n], fdata[n]);
                    fprintf (freg, "----\n");
                    fclose (freg);
                }
            }
            memset (fdata, 0, sizeof (fdata));
        }
    }
    fclose (fn);
    unlink (fname);
    return (0);
}
*/

// º«¬º’ π‹Õ≥º∆ ˝æ›µƒ–°∫Ø ˝

void id_record(char* userid, int TYPE)
//type: 0: accept, 1: reject, 2: delete
{
	char buf[STRLEN];
	char garbage[2] = {"xy"};//“ÚŒ™ΩˆΩˆ“™Õ≥º∆∏ˆ ˝£¨À˘“‘’‚∏ˆ◊÷∑˚¥Æ≤¢√ª”– µº “‚“Â
	FILE* fp = NULL;
	char mode[3][10] = 
	{
		"accept",
		"reject",
		"del"
	};
	
	if((TYPE <0) ||(TYPE>2))
		return;
	
	sprintf(buf, "etc/idadmin.%s.%s", userid, mode[TYPE]);
	fp = fopen(buf, "a");
	if(fp==NULL)
		return;
	fwrite(garbage, 2, 1, fp);
	fclose(fp);
	return;
}


// –¬µƒ≈˙◊¢≤·µ•µƒ∑Ω∑®, by rwzmm @ sjtubbs tech group

int
scan_new_register_form (char* regfile, char* searchid)
{
    static char *reason[] = 
	{ 
		"«Î»∑ µÃÓ–¥’Ê µ–’√˚.",
		"«ÎœÍÃÓ–¥—ß–£‘∫œµƒÍº∂£¨‘⁄÷∞»À ø«ÎÃÓ–¥π§◊˜µ•Œª∫Õ≤ø√≈.",
		"«ÎÃÓ–¥ÕÍ’˚µƒæ”◊°µÿ÷∑ªÚµ•Œªµÿ÷∑£®∑«µÁ◊”” º˛–≈œ‰£©.", 
		"∂‘≤ª∆£¨∏√IDŒ™±æ’æ≤ªΩ” ‹µƒID.", 
		"«ÎŒ π”√≤ª—≈Í«≥∆.",
		"ƒ˙µƒ◊ ¡œ”–∂‡¥¶≤ª∫œ“™«Û,«Î»œ’ÊÃÓ–¥√øœÓ◊ ¡œ.", 
		"«Î”√÷–ŒƒÃÓ–¥…Í«Îµ•.",
		"«ÎœÍÃÓ¡¨¬ÁµÁª∞", 
		NULL
	};

	struct userec uinfo;
    FILE *fout, *freg;
    char fname[STRLEN], buf[STRLEN];
    char ans[5], *uid;
    char userid[IDLEN];
    int n, unum;
	struct stat st = {0};
	int record_ns = 0;
	int temp_read_ns = 0;
	int k = 0;
	int x = 0;
	
    if (searchid)
	{
		strcpy (userid, searchid);
	}
    uid = currentuser.userid;
	stand_title ("“¿¥Œ…Ë∂® π”√’ﬂ◊¢≤·◊ ¡œ (new method by rwzmm, ^_^)");
    sprintf (fname, "%s.tmp", regfile/*, currentuser.userid*/);
    move (2, 0);

	if(dashf(fname))
	{
		move (1, 0);
        prints ("∆‰À˚’ ∫≈π‹¿Ì‘±’˝‘⁄≤Èø¥◊¢≤·…Í«Îµ•, «ÎºÏ≤È π”√’ﬂ◊¥Ã¨.\n");
		prints ("ƒø«∞±æ’æ’ ∫≈π‹¿Ì‘±£∫\033[1;36msupermaltose darkfighter starboy \033[1;35myymomo \033[1;36myisuoyinyu\033[m\n\n");
		prints ("warning: \033[1;31m»Áπ˚∂‡»ÀÕ¨ ±≈˙◊¢≤·µ•£¨ª·‘Ï≥…—œ÷ÿ∫Ûπ˚£°\033[m\n");
        getdata (5, 0, "ƒ„»∑∂®√ª”–∆‰À˚’ ∫≈π‹¿Ì‘±‘⁄…Û∫À◊¢≤·µ•¬ £ø [y/N]: ",
			ans, 2, DOECHO, YEA);
        if (ans[0] == 'Y' || ans[0] == 'y')
		{
			FILE* reg;
			FILE* tmp;
			struct regform record = {0};
			tmp = fopen(fname, "r");
			reg = fopen(regfile, "a");
			while(fread(&record, sizeof(struct regform), 1, tmp) > 0)
			{
				if(record.status == 0)
					fwrite(&record, sizeof(struct regform), 1, reg);
			}
			fclose(reg);
			fclose(tmp);
		}
        else
        {
            pressreturn ();
            return -1;
        }
	}

	if(!dashf(regfile))
		return -1;
	stat(regfile, &st);
	record_ns = st.st_size / sizeof(struct regform);
	temp_read_ns = record_ns;
//	while(1)
	for(k=0; k< (record_ns + READ_MAX_REG_ID_ONCE - 1)/READ_MAX_REG_ID_ONCE; k++)
	{
		FILE* fp_reg; 
		FILE* fp_tmp;
		FILE* fp_bak;
		int i = 0;
		int j = 0;
		char tempbuf[STRLEN*2];
		//int x = 0;
		struct regform record[READ_MAX_REG_ID_ONCE];
		struct regform rec_tmp= {0};
		int read_ns = 0;
		char backup[20];

		sprintf(backup, "register_backup");

//read date from regfile to tmp
		if(!dashf(regfile))
			break;

		sprintf(backup, "register_backup");
		memset(record, 0x00, sizeof(struct regform)*READ_MAX_REG_ID_ONCE);
		fp_reg = fopen(regfile, "r");
		fp_bak = fopen(backup, "w");
		fp_tmp = fopen(fname, "w");
		if(temp_read_ns > READ_MAX_REG_ID_ONCE)
		{
			temp_read_ns -= READ_MAX_REG_ID_ONCE;
			fread(record, sizeof(struct regform), READ_MAX_REG_ID_ONCE, fp_reg);
			fwrite(record, sizeof(struct regform), READ_MAX_REG_ID_ONCE, fp_tmp);
			while(fread(&rec_tmp, sizeof(struct regform), 1, fp_reg) > 0)
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
			while(fread(&rec_tmp, sizeof(struct regform), 1, fp_reg) > 0)
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
//deal with tmp
		
		for(i=0; i<read_ns; i++)
		{
//			if(record[i].userid == NULL)
//				break;

//œ÷‘⁄record¡–±Ì¿Ô√Ê”–read_nsÃı◊¢≤·–≈œ¢–Ë“™¥¶¿Ì
			if ((unum = getuser (record[i].userid)) == 0)
			{
				move (2, 0);
				clrtobot ();
				prints ("œµÕ≥¥ÌŒÛ, ≤ÈŒﬁ¥À’ ∫≈.\n\n");
				prints ("’ ∫≈Œª÷√     : %d\n", record[i].usernum);
				prints ("…Í«Î’ ∫≈     : %s\n", record[i].userid);
				prints ("’Ê µ–’√˚     : %s\n", record[i].realname);
				prints ("—ß–£œµº∂     : %s\n", record[i].dept);
				prints ("ƒø«∞◊°÷∑     : %s\n", record[i].addr);
				prints ("¡™¬ÁµÁª∞     : %s\n", record[i].phone);
				record[i].status |= REG_DELETE;
				fp_tmp = fopen(fname, "w");
				fwrite(record, sizeof(struct regform), read_ns, fp_tmp);
				fclose(fp_tmp);
				pressreturn ();
			}

			else
			{
				if (searchid && strncmp (userid, record[i].userid, IDLEN))
					ans[0] = 'S';
				else
				{
					memcpy (&uinfo, &lookupuser, sizeof (uinfo));
					sethomefile (buf, uinfo.userid, "register");
					if (dashf (buf))
						ans[0] = 'd';
					else
					{
						move (1, 0);
						prints ("’ ∫≈Œª÷√     : %d\n", unum);
						disply_userinfo (&uinfo);
						move (15, 0);
						printdash (NULL);
						prints ("’ ∫≈Œª÷√     : %d, %s\n", /*unum*/record[i].usernum, record[i].date);
						prints ("…Í«Î’ ∫≈     : %s\n", record[i].userid);
						prints ("’Ê µ–’√˚     : %s\n", record[i].realname);
						prints ("—ß–£œµº∂     : %s\n", record[i].dept);
						prints ("ƒø«∞◊°÷∑     : %s\n", record[i].addr);
						prints ("¡™¬ÁµÁª∞     : %s\n", record[i].phone);
						
						if (uinfo.userlevel & PERM_LOGINOK)
						{
							move (t_lines - 1, 0);
							prints ("¥À’ ∫≈≤ª–Ë‘ŸÃÓ–¥◊¢≤·µ•.\n");
							igetkey ();
							ans[0] = 'D';
						}
						else
						{
							getdata (t_lines - 1, 0,
								" «∑ÒΩ” ‹¥À◊ ¡œ (Y/N/Q/Del/Skip)? [S]: ",
								ans, 3, DOECHO, YEA);
						}
						move (1, 0);
						clrtobot ();
					}
				}
				switch(ans[0]) 
				{
				case 'D':
				case 'd':
					record[i].status |= REG_DELETE;
					fp_tmp = fopen(fname, "w");
					fwrite(record, sizeof(struct regform), read_ns, fp_tmp);
					fclose(fp_tmp);
					id_record(currentuser.userid, 2);	//delete
					break;
				case 'Y':
				case 'y':
					prints ("“‘œ¬ π”√’ﬂ◊ ¡œ“—æ≠∏¸–¬:\n\n");
					strncpy (uinfo.realname, record[i].realname, NAMELEN);
					strncpy (uinfo.address, record[i].addr, NAMELEN);
					sprintf (genbuf, "%s$%s@%s", record[i].dept, record[i].phone, uid);
					genbuf[STRLEN - 16] = '\0';
					strncpy (uinfo.reginfo, genbuf, STRLEN - 17);
					uinfo.lastjustify = time (0);
					substitute_record (PASSFILE, &uinfo, sizeof (uinfo), unum);
					sethomefile (buf, uinfo.userid, "register");
					if (dashf (buf))
					{
						sethomefile (genbuf, uinfo.userid, "register.old");
						rename (buf, genbuf);
					}
					if ((fout = fopen (buf, "w")) != NULL)
					{
						fprintf (fout, "usernum: %d, %s\n", /*unum*/record[i].usernum, record[i].date);
						fprintf (fout, "userid: %s\n", record[i].userid);
						fprintf (fout, "realname: %s\n", record[i].realname);
						fprintf (fout, "dept: %s\n", record[i].dept);
						fprintf (fout, "addr: %s\n", record[i].addr);
						fprintf (fout, "phone: %s\n\n", record[i].phone);
					
						n = time (NULL);
						getdatestring (n, NA);
						fprintf (fout, "Date: %s\n", datestring);
						fprintf (fout, "Approved: %s\n", uid);
						fclose (fout);
					}
//modified by rwzmm @ sjtubbs, show user info after fresh register record
					sprintf (tempbuf, "home/%c/%s/register",
						toupper (userid[0]), userid);
					if ((fout = fopen (tempbuf, "r")) != NULL)
					{
						for (x = 1; x <= 15; x++)
						{
							if (fgets (tempbuf, STRLEN, fout))
								prints ("%s", tempbuf);
							else
								break;
						}
						fclose(fout);
					}
					
					mail_file ("etc/s_fill", uinfo.userid,
						"πßÏ˚ƒ„£¨ƒ„“—æ≠ÕÍ≥…◊¢≤·°£");
					sethomefile (buf, uinfo.userid, "mailcheck");
					unlink (buf);
					record[i].status |= REG_ACCEPT;
//					sprintf(genbuf, "»√ %s Õ®π˝…Ì∑÷»∑»œ.", uinfo.userid);
//					securityreport(genbuf);
					fp_tmp = fopen(fname, "w");
					fwrite(record, sizeof(struct regform), read_ns, fp_tmp);
					fclose(fp_tmp);
					id_record(currentuser.userid, 0);//accept
					break;
				case 'Q':
				case 'q':
					if ((freg = fopen (regfile, "a")) != NULL)
					{
						for(j=0; j<read_ns; j++)
						{
							if(record[j].status == 0)
							{
								fwrite(&record[j], sizeof(struct regform), 1, freg);
							}
						}
						fclose (freg);
					}
					goto end;
					break;	
				case 'N':
				case 'n':
					prints ("’ ∫≈Œª÷√: %d, %s\n", /*unum*/record[i].usernum, record[i].date);
					prints ("…Í«Î’ ∫≈: %s\n", record[i].userid);
					prints ("’Ê µ–’√˚: %s\n", record[i].realname);
					prints ("—ß–£œµº∂: %s\n", record[i].dept);
					prints ("ƒø«∞◊°÷∑: %s\n", record[i].addr);
					prints ("¡™¬ÁµÁª∞: %s\n", record[i].phone);

					printdash (NULL);
					move (9, 0);
					prints ("«Î—°‘Ò/ ‰»ÎÕÀªÿ…Í«Î±Ì‘≠“Ú, ∞¥ <enter> »°œ˚.\n\n");
					for (n = 0; reason[n] != NULL; n++)
						prints ("%d) %s\n", n, reason[n]);
					getdata (12 + n, 0, "ÕÀªÿ‘≠“Ú: ", buf, 60, DOECHO, YEA);
					if (buf[0] != '\0')
					{
						if (buf[0] >= '0' && buf[0] < '0' + n)
						{
							strcpy (buf, reason[buf[0] - '0']);
						}
						sprintf (genbuf, "<◊¢≤· ß∞‹> - %s", buf);
						strncpy (uinfo.address, genbuf, NAMELEN);
						substitute_record (PASSFILE, &uinfo, sizeof (uinfo), unum);
						mail_file ("etc/f_fill", uinfo.userid, uinfo.address);
						/* user_display( &uinfo, 1 ); */
						/* pressreturn(); */
						record[i].status |= REG_REJECT;
						fp_tmp = fopen(fname, "w");
						fwrite(record, sizeof(struct regform), read_ns, fp_tmp);
						fclose(fp_tmp);
						id_record(currentuser.userid, 1);//reject
						break;
					}
					move (10, 0);
					clrtobot ();
					prints ("»°œ˚ÕÀªÿ¥À◊¢≤·…Í«Î±Ì.\n");
					/* run default -- put back to regfile */					
				default:
					if ((freg = fopen (regfile, "a")) != NULL)
					{
						if(record[i].status ==0)
							fwrite(&record[i], sizeof(struct regform), 1, freg);
						fclose (freg);
					}
					record[i].status |= REG_REJECT;
					fp_tmp = fopen(fname, "w");
					fwrite(record, sizeof(struct regform), read_ns, fp_tmp);
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
extern int cmpuids ();
extern int t_cmpuids ();
int
kick_user (struct user_info *userinfo)
{
    int id, ind,i;
    struct user_info uin;
    struct userec kuinfo;
    char kickuser[40], buffer[40];

    if (uinfo.mode != LUSERS && uinfo.mode != OFFLINE && uinfo.mode != FRIEND)
    {
        modify_user_mode (ADMIN);
        stand_title ("Ãﬂ π”√’ﬂœ¬’æ");
        move (1, 0);
        usercomplete (" ‰»Î π”√’ﬂ’ ∫≈: ", kickuser);
        if (*kickuser == '\0')
        {
            clear ();
            return 0;
        }
        if (!(id = getuser (kickuser)))
        {
            move (3, 0);
            prints ("Œﬁ–ßµƒ”√ªß ID£°");
            clrtoeol ();
            pressreturn ();
            clear ();
            return 0;
        }
        move (1, 0);
        clrtoeol ();
        sprintf (genbuf, "ÃﬂµÙ π”√’ﬂ : [%s].", kickuser);
        move (2, 0);
        if (askyn (genbuf, NA, NA) == NA)
        {
            move (2, 0);
            prints ("»°œ˚Ãﬂ π”√’ﬂ..\n");
            pressreturn ();
            clear ();
            return 0;
        }
        search_record (PASSFILE, &kuinfo, sizeof (kuinfo), cmpuids, kickuser);
        ind = search_ulist (&uin, t_cmpuids, id);
    }
    else
    {
        uin = *userinfo;
        strcpy (kickuser, uin.userid);
        ind = YEA;
    }
    if (!ind || !uin.active || (uin.pid && kill (uin.pid, 0) == -1))
    {
        if (uinfo.mode != LUSERS && uinfo.mode != OFFLINE
                && uinfo.mode != FRIEND)
        {
            move (3, 0);
            prints ("”√ªß [%s] ≤ª‘⁄œﬂ…œ", kickuser);
            clrtoeol ();
            pressreturn ();
            clear ();
        }
        return 0;
    }
	if(is_web_user(&uin))
	{
		for(i=0;i<MAXACTIVE;i++)
			if(!strcasecmp(uin.userid,utmpshm->uinfo[i].userid))
				break;
		if(i!=MAXACTIVE)
			kick_web_user(&(utmpshm->uinfo[i]));
	}
	else
	    kill (uin.pid, SIGHUP);
    sprintf (buffer, "kicked %s", kickuser);
    report (buffer);
    sprintf (genbuf, "%s (%s)", kuinfo.userid, kuinfo.username);
    log_usies ("KICK ", genbuf);
    move (2, 0);
    if (uinfo.mode != LUSERS && uinfo.mode != OFFLINE && uinfo.mode != FRIEND)
    {
        prints ("”√ªß [%s] “—æ≠±ªÃﬂœ¬’æ.\n", kickuser);
        pressreturn ();
        clear ();
    }
    return 1;
}
#endif
