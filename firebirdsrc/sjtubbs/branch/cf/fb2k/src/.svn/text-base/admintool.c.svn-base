#ifndef WITHOUT_ADMIN_TOOLS
#include <stdio.h>
#include "bbs.h"

extern int cmpbnames ();
extern int numboards;
extern int cleanmail ();
extern char *chgrp ();
extern int dowall ();
extern int cmpuids ();
extern int t_cmpuids ();

extern int noreply;

int showperminfo (int, int);

#ifndef NEW_CREATE_BRD
char cexplain[STRLEN];
#endif
char buf2[STRLEN];
char lookgrp[30];
char bnames[3][20];

//this file, change all + 8 to + 13 by alt@


struct GroupInfo
{
    char name[SectNum][16];
    char items[SectNum][32];
    char chs[SectNum][8];
    time_t update;
    int num;
}
GroupsInfo;

FILE *cleanlog;

int
getbnames (userid, bname, find)
char *userid, *bname;
int *find;
{
    int oldbm = 0;
    FILE *bmfp;
    char bmfilename[STRLEN], tmp[20];

    *find = 0;
    sethomefile (bmfilename, userid, ".bmfile");
    bmfp = fopen (bmfilename, "r");
    if (!bmfp)
        return 0;
    for (oldbm = 0; oldbm < 3;)
    {
        fscanf (bmfp, "%s\n", tmp);
        if (!strcmp (bname, tmp))
            *find = oldbm + 1;
        strcpy (bnames[oldbm++], tmp);
        if (feof (bmfp))
            break;
    }
    fclose (bmfp);
    return oldbm;
}


int
m_info ()
{
    struct userec uinfo;
    int id;

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
    {
        return;
    }
    clear ();
    stand_title ("ĞŞ¸ÄÊ¹ÓÃÕß×ÊÁÏ");
    if (!gettheuserid (1, "ÇëÊäÈëÊ¹ÓÃÕß´úºÅ: ", &id))
        return -1;
    memcpy (&uinfo, &lookupuser, sizeof (uinfo));

    move (1, 0);
    clrtobot ();
    disply_userinfo (&uinfo);
    uinfo_query (&uinfo, 1, id);
    return 0;
}

int
m_ordainBM ()
{
    int id, pos, oldbm = 0, i, find, bm = 1;
    struct boardheader fh;
    FILE *bmfp;
    char bmfilename[STRLEN], bname[STRLEN];
    char buf[5][STRLEN];

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
        return;

    clear ();
    stand_title ("ÈÎÃü°åÖ÷\n");
    clrtoeol ();
    if (!gettheuserid (2, "ÊäÈëÓûÈÎÃüµÄÊ¹ÓÃÕßÕÊºÅ: ", &id))
        return 0;
    if (!strcmp (lookupuser.userid, "guest"))
    {
        move (5, 0);
        prints ("Äã²»ÄÜÈÎÃü guest µ±°åÖ÷");
        pressanykey ();
        clear ();
        return -1;
    }
    if (!gettheboardname
            (3, "ÊäÈë¸ÃÊ¹ÓÃÕß½«¹ÜÀíµÄÌÖÂÛÇøÃû³Æ: ", &pos, &fh, bname))
        return -1;
    if (fh.BM[0] != '\0')
    {
        if (!strncmp (fh.BM, "SYSOP", 5))
        {
            move (4, 0);
            if (askyn ("¸ÃÌÖÂÛÇø°åÖ÷ÊÇ SYSOP, ÄãÈ·¶¨¸Ã°åĞèÒª°åÖ÷Âğ", NA, NA)
                    == NA)
            {
                clear ();
                return -1;
            }
            fh.BM[0] = '\0';
        }
        else
        {
            for (i = 0, oldbm = 1; fh.BM[i] != '\0'; i++)
                if (fh.BM[i] == ' ')
                    oldbm++;
            if (oldbm == 3)
            {
                move (5, 0);
                prints ("%s ÌÖÂÛÇøÒÑÓĞÈıÃû°åÖ÷", bname);
                pressreturn ();
                clear ();
                return -1;
            }
            bm = 0;
        }
    }
    oldbm = getbnames (lookupuser.userid, bname, &find);
    if (find || oldbm == 3)
    {
        move (5, 0);
        prints (" %s ÒÑ¾­ÊÇ%s°åµÄ°åÖ÷ÁË", lookupuser.userid,
                find ? "¸Ã" : "Èı¸ö");
        pressanykey ();
        clear ();
        return -1;
    }
    prints ("\nÄã½«ÈÎÃü %s Îª %s °å°å%s.\n", lookupuser.userid, bname,
            bm ? "Ö÷" : "¸±");
    if (askyn ("ÄãÈ·¶¨ÒªÈÎÃüÂğ?", NA, NA) == NA)
    {
        prints ("È¡ÏûÈÎÃü°åÖ÷");
        pressanykey ();
        clear ();
        return -1;
    }
    for (i = 0; i < 5; i++)
        buf[i][0] = '\0';
    move (8, 0);
    prints ("ÇëÊäÈëÈÎÃü¸½ÑÔ(×î¶àÎåĞĞ£¬°´ Enter ½áÊø)");
    for (i = 0; i < 5; i++)
    {
        getdata (i + 9, 0, ": ", buf[i], STRLEN - 5, DOECHO, YEA);
        if (buf[i][0] == '\0')
            break;
    }
    strcpy (bnames[oldbm], bname);
    if (!oldbm)
    {
        char secu[STRLEN];

        lookupuser.userlevel |= PERM_BOARDS;
        substitute_record (PASSFILE, &lookupuser, sizeof (struct userec), id);
        sprintf (secu, "°åÖ÷ÈÎÃü, ¸øÓè %s µÄ°åÖ÷È¨ÏŞ", lookupuser.userid);
        securityreport (secu);
        move (15, 0);
        prints (secu);
    }
    if (fh.BM[0] == '\0')
        strcpy (genbuf, lookupuser.userid);
    else
        sprintf (genbuf, "%s %s", fh.BM, lookupuser.userid);
    strncpy (fh.BM, genbuf, sizeof (fh.BM));
    sprintf (genbuf, "%-38.38s(BM: %s)", fh.title + 11, fh.BM);
    get_grp (fh.filename);
    edit_grp (fh.filename, lookgrp, fh.title + 11, genbuf);
    substitute_record (BOARDS, &fh, sizeof (fh), pos);
    sethomefile (bmfilename, lookupuser.userid, ".bmfile");
    bmfp = fopen (bmfilename, "w+");
    if (bmfp)
    {
        for (i = 0; i < oldbm + 1; i++)
            fprintf (bmfp, "%s\n", bnames[i]);
        fclose (bmfp);
    }
    sprintf (bmfilename, "ÈÎÃü %s Îª %s ÌÖÂÛÇø%s",
             lookupuser.userid, fh.filename, bm ? "°åÖ÷" : "°å¸±");
    securityreport (bmfilename);
    move (16, 0);
    prints (bmfilename);
	if(strncmp(bname, "BMTraining", 10))	//not a bmt board
	{
		sprintf (genbuf, "\n\t\t\t¡¾ Í¨¸æ ¡¿\n\n"
             "\tÈÎÃü %s Îª %s °å%s£¡\n"
             "\tÇë %s ÔÚÈıÌìÄÚÔÚ BMhome °å±¨µ½¡£\n"
             "\tÓâÆÚÎ´ÄÜ±¨µ½Õß£¬½«È¡Ïû°åÖ÷È¨ÏŞ¡£\n",
             lookupuser.userid, bname, bm ? "°åÖ÷" : "°å¸±", lookupuser.userid);
	}
	else	//bmt board
	{
		sprintf (genbuf, "\n\t\t\t¡¾ Í¨¸æ ¡¿\n\n"
             "\tÈÎÃü %s Îª %s °å%s£¡\n"
             "\tÇë %s ÔÚÈıÌìÄÚÔÚ District%c °å±¨µ½¡£\n"
             "\tÓâÆÚÎ´ÄÜ±¨µ½Õß£¬½«È¡Ïû°åÖ÷È¨ÏŞ¡£\n",
             lookupuser.userid, bname, bm ? "°åÖ÷" : "°å¸±", lookupuser.userid, bname[10]);
	}
    for (i = 0; i < 5; i++)
    {
        if (buf[i][0] == '\0')
            break;
        if (i == 0)
            strcat (genbuf, "\n\nÕ¾³¤ÈÎÃü¸½ÑÔ£º\n");
        strcat (genbuf, buf[i]);
        strcat (genbuf, "\n");
    }
    strcpy (currboard, bname);
#ifdef ORDAINBM_POST_BOARDNAME
	if(strncmp(bname, "BMTraining", 10) == 0)
	{
		autoreport (bmfilename, genbuf, lookupuser.userid, TRAIN_ANNOUNCE_BOARDNAME);
	}
	else
	{
		autoreport (bmfilename, genbuf, lookupuser.userid, ORDAINBM_POST_BOARDNAME);
	}

#else

    autoreport (bmfilename, genbuf, lookupuser.userid, NULL);
#endif

    pressanykey ();
    return 0;
}

int
m_retireBM ()
{
    int id, pos, right = 0, oldbm = 0, i, j, bmnum;
    int find, bm = 1;
    struct boardheader fh;
    FILE *bmfp;
    char bmfilename[STRLEN], buf[5][STRLEN];
    char bname[STRLEN], usernames[3][STRLEN];

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
        return;

    clear ();
    stand_title ("°åÖ÷ÀëÖ°\n");
    clrtoeol ();
    if (!gettheuserid (2, "ÊäÈëÓûÀëÖ°µÄ°åÖ÷ÕÊºÅ: ", &id))
        return -1;
    if (!gettheboardname (3, "ÇëÊäÈë¸Ã°åÖ÷Òª´ÇÈ¥µÄ°åÃû: ", &pos, &fh, bname))
        return -1;
    oldbm = getbnames (lookupuser.userid, bname, &find);
    if (!oldbm || !find)
    {
        move (5, 0);
        prints (" %s %s°å°åÖ÷£¬ÈçÓĞ´íÎó£¬ÇëÍ¨Öª³ÌĞòÕ¾³¤¡£",
                lookupuser.userid, (oldbm) ? "²»ÊÇ¸Ã" : "Ã»ÓĞµ£ÈÎÈÎºÎ");
        pressanykey ();
        clear ();
        return -1;
    }
    for (i = find - 1; i < oldbm; i++)
    {
        if (i != oldbm - 1)
            strcpy (bnames[i], bnames[i + 1]);
    }
    bmnum = 0;
    for (i = 0, j = 0; fh.BM[i] != '\0'; i++)
    {
        if (fh.BM[i] == ' ')
        {
            usernames[bmnum][j] = '\0';
            bmnum++;
            j = 0;
        }
        else
        {
            usernames[bmnum][j++] = fh.BM[i];
        }
    }
    usernames[bmnum++][j] = '\0';
    for (i = 0, right = 0; i < bmnum; i++)
    {
        if (!strcmp (usernames[i], lookupuser.userid))
        {
            right = 1;
            if (i)
                bm = 0;
        }
        if (right && i != bmnum - 1)
            strcpy (usernames[i], usernames[i + 1]);
    }
    if (!right)
    {
        move (5, 0);
        prints
        ("¶Ô²»Æğ£¬ %s °å°åÖ÷Ãûµ¥ÖĞÃ»ÓĞ %s £¬ÈçÓĞ´íÎó£¬ÇëÍ¨Öª³ÌĞòÕ¾³¤¡£",
         bname, lookupuser.userid);
        pressanykey ();
        clear ();
        return -1;
    }
    prints ("\nÄã½«È¡Ïû %s µÄ %s °å°å%sÖ°Îñ.\n",
            lookupuser.userid, bname, bm ? "Ö÷" : "¸±");
    if (askyn ("ÄãÈ·¶¨ÒªÈ¡ÏûËûµÄ¸Ã°å°åÖ÷Ö°ÎñÂğ?", NA, NA) == NA)
    {
        prints ("\nºÇºÇ£¬Äã¸Ä±äĞÄÒâÁË£¿ %s ¼ÌĞøÁôÈÎ %s °å°åÖ÷Ö°Îñ£¡",
                lookupuser.userid, bname);
        pressanykey ();
        clear ();
        return -1;
    }
    if (bmnum - 1)
    {
        sprintf (genbuf, "%s", usernames[0]);
        for (i = 1; i < bmnum - 1; i++)
            sprintf (genbuf, "%s %s", genbuf, usernames[i]);
    }
    else
        genbuf[0] = '\0';
    strncpy (fh.BM, genbuf, sizeof (fh.BM));
    if (fh.BM[0] != '\0')
        //next 5 line, change 8 to 13 by alt@
        sprintf (genbuf, "%-38.38s(BM: %s)", fh.title + 11, fh.BM);
    else
        sprintf (genbuf, "%-38.38s", fh.title + 11);
    get_grp (fh.filename);
    edit_grp (fh.filename, lookgrp, fh.title + 11, genbuf);
    substitute_record (BOARDS, &fh, sizeof (fh), pos);
    sprintf (genbuf, "È¡Ïû %s µÄ %s ÌÖÂÛÇø°åÖ÷Ö°Îñ", lookupuser.userid,
             fh.filename);
    securityreport (genbuf);
    move (8, 0);
    prints (genbuf);
    sethomefile (bmfilename, lookupuser.userid, ".bmfile");
    if (oldbm - 1)
    {
        bmfp = fopen (bmfilename, "w+");
        if (bmfp)
        {
            for (i = 0; i < oldbm - 1; i++)
                fprintf (bmfp, "%s\n", bnames[i]);
            fclose (bmfp);
        }
    }
    else
    {
        char secu[STRLEN];

        unlink (bmfilename);
        if (!(lookupuser.userlevel & PERM_OBOARDS)
                && !(lookupuser.userlevel & PERM_SYSOP))
        {
            lookupuser.userlevel &= ~PERM_BOARDS;
            substitute_record (PASSFILE, &lookupuser, sizeof (struct userec),
                               id);
            sprintf (secu, "°åÖ÷Ğ¶Ö°, È¡Ïû %s µÄ°åÖ÷È¨ÏŞ", lookupuser.userid);
            securityreport (secu);
            move (9, 0);
            prints (secu);
        }
    }
    prints ("\n\n");
    if (askyn ("ĞèÒªÔÚÏà¹Ø°åÃæ·¢ËÍÍ¨¸æÂğ?", YEA, NA) == NA)
    {
        pressanykey ();
        return 0;
    }
    prints ("\n");
    if (askyn ("Õı³£ÀëÈÎÇë°´ Enter ¼üÈ·ÈÏ£¬³·Ö°³Í·£°´ N ¼ü", YEA, NA) == YEA)
        right = 1;
    else
        right = 0;
    if (right)
        sprintf (bmfilename, "%s °å%s %s ÀëÈÎÍ¨¸æ", bname,
                 bm ? "°åÖ÷" : "°å¸±", lookupuser.userid);
    else
        sprintf (bmfilename, "[Í¨¸æ]³·³ı %s °å%s %s ", bname,
                 bm ? "°åÖ÷" : "°å¸±", lookupuser.userid);
    strcpy (currboard, bname);
    if (right)
    {
        sprintf (genbuf, "\n\t\t\t¡¾ Í¨¸æ ¡¿\n\n"
                 "\t¾­Õ¾Îñ×éÌÖÂÛ£º\n"
                 "\tÍ¬Òâ %s ´ÇÈ¥ %s °åµÄ%sÖ°Îñ¡£\n"
                 "\tÔÚ´Ë£¬¶ÔËûÔø¾­ÔÚ %s °åµÄĞÁ¿àÀÍ×÷±íÊ¾¸ĞĞ»¡£\n\n"
                 "\tÏ£Íû½ñºóÒ²ÄÜÖ§³Ö±¾°åµÄ¹¤×÷.",
                 lookupuser.userid, bname, bm ? "°åÖ÷" : "°å¸±", bname);
    }
    else
    {
        sprintf (genbuf, "\n\t\t\t¡¾³·Ö°Í¨¸æ¡¿\n\n"
                 "\t¾­Õ¾Îñ×éÌÖÂÛ¾ö¶¨£º\n"
                 "\t³·³ı %s °å%s %s µÄ%sÖ°Îñ¡£\n",
                 bname, bm ? "°åÖ÷" : "°å¸±", lookupuser.userid,
                 bm ? "°åÖ÷" : "°å¸±");
    }
    for (i = 0; i < 5; i++)
        buf[i][0] = '\0';
    move (14, 0);
    prints ("ÇëÊäÈë%s¸½ÑÔ(×î¶àÎåĞĞ£¬°´ Enter ½áÊø)",
            right ? "°åÖ÷ÀëÈÎ" : "°åÖ÷³·Ö°");
    for (i = 0; i < 5; i++)
    {
        getdata (i + 15, 0, ": ", buf[i], STRLEN - 5, DOECHO, YEA);
        if (buf[i][0] == '\0')
            break;
        if (i == 0)
            strcat (genbuf, right ? "\n\nÀëÈÎ¸½ÑÔ£º\n" : "\n\n³·Ö°ËµÃ÷£º\n");
        strcat (genbuf, buf[i]);
        strcat (genbuf, "\n");
    }
#ifdef ORDAINBM_POST_BOARDNAME
	if(strncmp(bname, "BMTraining", 10) == 0)
	{
		autoreport (bmfilename, genbuf, NULL, TRAIN_ANNOUNCE_BOARDNAME);
	}
	else
	{
		autoreport (bmfilename, genbuf, NULL, ORDAINBM_POST_BOARDNAME);
	}
#else

    autoreport (bmfilename, genbuf, NULL, NULL);
#endif

    prints ("\nÖ´ĞĞÍê±Ï£¡");
    pressanykey ();
    return 0;
}

#ifdef NEW_CREATE_BRD
int
getGroupSet (void)
{
    FILE *mfp;
    char buf[256];
    char *ptr;
    int i = 0, groupNum, j = 0, k = 0;
    struct stat st;

    if (stat ("etc/menu.ini", &st) == -1 || st.st_size == 0)
    {
        GroupsInfo.update = 0;
        return 0;
    }
    if (GroupsInfo.update >= st.st_mtime)
    {
        return 1;
    }
    GroupsInfo.update = st.st_mtime;
    mfp = fopen ("etc/menu.ini", "r");
    if (mfp == NULL)
    {
        GroupsInfo.update = 0;
        return 0;
    }
    for (i = 0; i < SectNum; i++)
    {
        for (j = 0; j < 16; j++)
            GroupsInfo.name[i][j] = '\0';
        for (j = 0; j < 32; j++)
            GroupsInfo.items[i][j] = '\0';
        for (j = 0; j < 8; j++)
            GroupsInfo.chs[i][j] = '\0';
    }
    j = 0;
    while (fgets (buf, 256, mfp) != NULL)
    {
        if (j >= SectNum && k >= SectNum)
            break;
        my_ansi_filter (buf);
        if (buf[0] == '#')
            continue;
        if (strstr (buf, "EGroups"))
        {
            if (buf[0] != '@')
                continue;
            ptr = strchr (buf, '\"');
            if (ptr == NULL)
                continue;
            while (ptr++)
            {
                if (*ptr == '\"')
                    break;
                //add by alt 4 ×ÖÄ¸·ÖÇø
                if (*ptr >= 'a' && *ptr <= 'z')
                    *ptr -= 32;	//Ğ¡Ğ´Í³Ò»µ½´óĞ´
                if (*ptr >= 'A' && *ptr <= 'Z')
                    *ptr -= 7;	//×ÖÄ¸Í³Ò»µ½Êı×Ö
                groupNum = *ptr - '0';
                if (groupNum < SectNum && groupNum >= 0)
                    break;
            }
            if (*ptr == '\"' || *ptr == '\0')
                continue;
            strcpy (buf, ptr);
            ptr = strchr (buf, ')');
            if (ptr == NULL)
                continue;
            for (i = 1; *(ptr + i) == ' ' && *(ptr + i) != '\0'; i++)
                ;
            ptr += i;
            for (i = 0;
                    *(ptr + i) != ' ' && *(ptr + i) != '\0'
                    && *(ptr + i) != '\"' && i < 16; i++)
            {
                GroupsInfo.name[groupNum][i] = *(ptr + i);
            }
            GroupsInfo.name[groupNum][i] = '\0';
            ptr += i;
            if (*ptr != '\0' && *ptr != '\n')
            {
                for (i = 1; *(ptr + i) == ' ' && *(ptr + i) != '\0'; i++)
                    ;
                ptr += i;
                if (*ptr != '\0' && *ptr != '\n')
                {
                    for (i = 0;
                            *(ptr + i) != '\"' && *(ptr + i) != ' '
                            && *(ptr + i) != '\0' && i < 32; i++)
                    {
                        GroupsInfo.items[groupNum][i] = *(ptr + i);
                    }
                    GroupsInfo.items[groupNum][i] = '\0';
                }
                else
                    GroupsInfo.items[groupNum][0] = '\0';
            }
            else
                GroupsInfo.items[groupNum][0] = '\0';
            j++;
            continue;
        }
        if (strstr (buf, "EGROUP"))
        {
            if (buf[0] != 'E')
                continue;
            if (buf[6] >= 'a' && buf[6] <= 'z')
                buf[6] -= 32;	//Ğ¡Ğ´Í³Ò»µ½´óĞ´
            if (buf[6] >= 'A' && buf[6] <= 'Z')
                buf[6] -= 7;	//×ÖÄ¸Í³Ò»µ½Êı×Ö
            groupNum = buf[6] - '0';
            if (groupNum >= SectNum || groupNum < 0)
                continue;
            ptr = strchr (buf, '\"');
            if (ptr == NULL)
                continue;
            ptr++;
            for (i = 0;
                    *(ptr + i) != '\"' && *(ptr + i) != ' '
                    && *(ptr + i) != '\0' && i < 8; i++)
            {
                GroupsInfo.chs[groupNum][i] = *(ptr + i);
            }
            GroupsInfo.chs[groupNum][i] = '\0';
            k++;
        }
    }
    fclose (mfp);
    if (j != k)
    {
        GroupsInfo.update = 0;
        return 0;
    }

    GroupsInfo.num = j;
    return 1;
}

int
chkGroupsInfo ()
{
    int haveError = NA, i;

    clear ();
    if (getGroupSet () == 0)
        haveError = YEA;
    else
    {
        for (i = 0; i < SectNum; i++)
            if ((GroupsInfo.name[i][0] == '\0') ^ (GroupsInfo.chs[i][0] == '\0'))
                haveError = YEA;
    }
    if (haveError != YEA)
        return 1;
    prints ("\n¡¾\033[0;1;4;33m×¢Òâ\033[m¡¿ÏµÍ³·¢ÏÖ menu.ini µÄÅäÖÃ"
            "¿ÉÄÜ´æÔÚÎÊÌâ£¬½¨Òé¼ì²éºóÔÙ¼ÌĞø½øĞĞ£¡\n        ");
    prints ("Èç¹û²»Ã÷°×¸ÃÌáÊ¾µÄº¬Òå£¬Çë¿½±´±¾ÆÁĞÅÏ¢µ½¹àË®Õ¾±¨¸æÎÊÌâ£¡\n\n");
    prints ("±¾Õ¾Ä¿Ç°µÄÌÖÂÛÇø·ÖÀàÇé¿öÈçÏÂ£º\n\n");
    prints (" ·ÖÇø   ·ÖÇøÃû³Æ     ·ÖÇøÀà±ğÃèÊö             Àà±ğ´úÂë\n");
    prints ("-------------------------------------------------------\n");
    for (i = 0; i < SectNum; i++)
    {
        if (GroupsInfo.name[i][0] == '\0' && GroupsInfo.chs[i][0] == '\0')
            continue;
        prints ("GROUP_%d %-12s %-24s \"%s\"\n", i,
                GroupsInfo.name[i][0] ==
                '\0' ? "######" : GroupsInfo.name[i],
                GroupsInfo.items[i][0] ==
                '\0' ? "######" : GroupsInfo.items[i],
                GroupsInfo.chs[i][0] == '\0' ? "######" : GroupsInfo.chs[i]);
    }
    prints ("\n\n");
    if (askyn
            ("ÉÏÃæµÄĞÅÏ¢±íÃ÷ menu.ini ¿ÉÄÜÓĞÎó£¬ÄúÈÔÈ»Ïë¼ÌĞøÖ´ĞĞÂğ", NA, NA) == YEA)
        return 1;
    return 0;
}

void ShowBrdSet (struct boardheader fh);

int
ChangeTheBoard (struct boardheader *oldfh, int pos)
{
    struct boardheader newfh;
    char buf[STRLEN], title[STRLEN], vbuf[100], ch;
    char num[2], chs[2], items[9], nntp[3], group[16];
    int i, j, a_mv;

    if (oldfh)
        memcpy (&newfh, oldfh, sizeof (newfh));
    else
        memset (&newfh, 0, sizeof (newfh));
    move (2, 0);
    if (oldfh)
    {
        prints
        ("Äú½«ÖØĞÂÉè¶¨ [\033[32m%s\033[m] µÄ°åÃæÊôĞÔ [Enter-->Éè¶¨²»±ä]",
         oldfh->filename);
    }
    else
        prints ("Äú½«¿ªÊ¼´´½¨Ò»¸ö\033[32mĞÂ\033[mµÄÌÖÂÛÇø [ENTER-->È¡Ïû´´½¨]");
    while (1)
    {
        struct boardheader dh;

        getdata (3, 0, "ÌÖÂÛÇøÃû³Æ(Ó¢ÎÄÃû): ", buf, BDNAME_LEN, DOECHO, YEA);
        if (buf[0] == '\0')
        {
            if (oldfh)
                strcpy (newfh.filename, oldfh->filename);
            else
                return -1;
            break;
        }
        if (killwordsp (buf) == 0)
            continue;
        if (!valid_brdname (buf))
            continue;
        if (search_record (BOARDS, &dh, sizeof (dh), cmpbnames, buf))
        {
            prints ("\n´íÎó! ´ËÌÖÂÛÇøÒÑ¾­´æÔÚ!!");
            if (oldfh)
                continue;
            pressanykey ();
            return -1;
        }
        strcpy (newfh.filename, buf);
        break;
    }
    prints ("\n[ÌÖÂÛÇøËµÃ÷]·ÖÇø´úÂë + ·ÖÇøÃèÊö + ×ªĞÅ±êÖ¾ + ÌÖÂÛÇø±êÌâ\n");
    if (oldfh)
    {
        prints ("³õÊ¼Éè¶¨£º%11s\033[32m%s\033[m\n", oldfh->title,
                oldfh->title + 11);
    }
    else
        prints ("¸ñÊ½·¶Àı£ºa[±¾Õ¾] ¡ğ \033[32mÕâÊÇÒ»¸öÕ¾ÄÚÌÖÂÛÇø\033[m\n");
    while (1)
    {
        if (oldfh)
            strcpy (buf, oldfh->title + 11);
        else
            buf[0] = '\0';
        getdata (8, 0, "ÌÖÂÛÇø±êÌâ(ÖĞÎÄÃû): ", buf, 41, DOECHO, NA);
        if (buf[0] == '\0')
        {
            if (oldfh == NULL)
                return -1;
            continue;
        }
        if (killwordsp (buf) == 0)
            continue;
        strcpy (title, buf);
        break;
    }
    if (askyn ("¸ÃÌÖÂÛÇøĞèÒª²ÎÓë×ªĞÅÂğ", NA, NA) == YEA)
    {
        strcpy (nntp, "¡ñ");
        newfh.flag |= OUT_FLAG;
    }
    else
    {
        strcpy (nntp, "¡ğ");
        newfh.flag &= ~OUT_FLAG;
    }
    if (oldfh)
    {
        ch = oldfh->title[0];
        for (i = 0; i < SectNum; i++)
        {
            if (GroupsInfo.chs[i][0] != '\0' && strchr (GroupsInfo.chs[i], ch))
                break;
        }
        if (i >= SectNum)
            ch = '0';
        else
            ch = '0' + i;
    }
    else
        ch = '0';
    for (i = 0, j = 3; i < SectNum; i++)
    {
        if (GroupsInfo.name[i][0] == '\0')
            continue;
        move (j++, 60);
        prints ("©§%s(%d)%-12s\033[m©§",
                (i + '0' == ch) ? "\033[32m" : "", i, GroupsInfo.name[i]);
    }
    while (1)
    {
        if (ch > '0' + 9)
            ch += 7;		//Êı×Ö±ä×ÖÄ¸
        num[0] = ch;
        num[1] = '\0';
        getdata (11, 0, "¸ÃÌÖÂÛÇø·ÅÖÃÔÚÄÄÒ»·ÖÇø(²Î¿¼ÓÒ±ßÌáÊ¾±àºÅ)£¿: ",
                 num, 2, DOECHO, NA);
        if (num[0] >= 'a' && num[0] <= 'z')
            num[0] -= 32;		//Ğ¡Ğ´Í³Ò»µ½´óĞ´
        if (num[0] >= 'A' && num[0] <= 'Z')
            num[0] -= 7;		//×ÖÄ¸Í³Ò»µ½Êı×Ö
        if (num[0] == '\0' || num[0] < '0' || num[0] >= ('0' + SectNum))
            continue;
        if (GroupsInfo.name[num[0] - '0'][0] == '\0')
            continue;
        break;
    }
    prints ("\nµÚ %d ÇøµÄ·ÖÀà·ûºÅ²Î¿¼£º[\033[32m%s\033[m] ·ÖÀàÃèÊö²Î¿¼£º%s",
            num[0] - '0', GroupsInfo.chs[num[0] - '0'],
            GroupsInfo.items[num[0] - '0']);
    while (1)
    {
        if (oldfh == NULL
                || !strchr (GroupsInfo.chs[num[0] - '0'], oldfh->title[0]))
            chs[0] = GroupsInfo.chs[num[0] - '0'][0];
        else
            chs[0] = oldfh->title[0];
        chs[1] = '\0';
        getdata (14, 0, "ÇëÊäÈë¸ÃÌÖÂÛÇøµÄ·ÖÀà·ûºÅ: ", chs, 2, DOECHO, NA);
        if (chs[0] == '\0')
            continue;
        if (strchr (GroupsInfo.chs[num[0] - '0'], chs[0]))
            break;
    }
    move (12, 0);
    prints ("\nµÚ %d ÇøµÄ·ÖÀà·ûºÅ²Î¿¼£º[%s] ·ÖÀàÃèÊö²Î¿¼£º\033[32m%s\033[m",
            num[0] - '0', GroupsInfo.chs[num[0] - '0'],
            GroupsInfo.items[num[0] - '0']);
    while (1)
    {
        if (oldfh)
        {
            strncpy (buf, oldfh->title + 1, 7);
            buf[6] = '\0';
        }
        else
            buf[0] = '\0';
        getdata (15, 0, "ÇëÊäÈë¸ÃÌÖÂÛÇøµÄ·ÖÀàÃèÊö: ", buf, 7, DOECHO, NA);
        if (buf[0] == '\0')
            continue;
        if (killwordsp (buf) == 0)
            continue;
        strcpy (items, buf);
        break;
    }
    sprintf (newfh.title, "%c%-6s %s %s", chs[0], items, nntp, title);
    if (oldfh == NULL || oldfh->BM[0] == '\0' || oldfh->BM[0] == ' '
            || !strncmp (newfh.BM, "SYSOP", 5))
    {
        prints
        ("\n°åÖ÷ÉèÖÃ  [ Èç¹û²»ĞèÒª°åÖ÷£¬ÔòÏµÍ³×Ô¶¯ÉèÖÃÎªÓÉ SYSOP ¹ÜÀí]\n");
        if (askyn ("±¾°åĞèÒª°åÖ÷Âğ", YEA, NA) == NA)
            strcpy (newfh.BM, "SYSOP");
        else
            newfh.BM[0] = '\0';
    }
    else
    {
        prints ("\n°åÖ÷ÉèÖÃ  [ Ä¿Ç°°åÃæµÄ°åÖ÷ÊÇ: %s ]\n", oldfh->BM);
        if (askyn
                ("µ±³öÏÖ°åÖ÷Ãûµ¥´íÎóµÄÊ±ºò£¬²ÅĞèÒªĞŞ¸Ä´Ë´¦¡£ĞèÒªĞŞ¸Ä°åÖ÷Ãûµ¥Âğ",
                 NA, NA) == YEA)
        {
            prints ("¡¾\033[31m¾¯¸æ\033[m¡¿ĞŞ¸Ä°åÖ÷½ö¹©³ö´íĞŞÕıÊ¹ÓÃ£¬"
                    "°åÖ÷ÈÎÃâÇëÎğ¸Ä¶¯´Ë´¦£¡\n");
            strcpy (newfh.BM, oldfh->BM);
            getdata (20, 0, "°åÖ÷Ãûµ¥: ", newfh.BM, sizeof (newfh.BM),
                     DOECHO, NA);
            if (newfh.BM[0] == ' ')
                newfh.BM[0] = '\0';
        }
        else
            strcpy (newfh.BM, oldfh->BM);
    }
    clear ();
    if (askyn ("±¾ÌÖÂÛÇøĞèÒªÉèÖÃ READ »ò POST ÏŞÖÆÂğ", NA, NA) == YEA)
    {
        char ans[4];

        sprintf (buf, "ÏŞÖÆ (R)ÔÄ¶Á »ò (P)ÕÅÌù ÎÄÕÂ [%c]: ",
                 oldfh ? (newfh.level & PERM_POSTMASK ? 'P' : 'R') : 'P');
        getdata (3, 0, buf, ans, 2, DOECHO, YEA);
        if (ans[0] == '\0')
        {
            ans[0] = oldfh ? (newfh.level & PERM_POSTMASK ? 'P' : 'R') : 'P';
            ans[1] = '\0';
        }
        if ((newfh.level & PERM_POSTMASK) && (*ans == 'R' || *ans == 'r'))
            newfh.level &= ~PERM_POSTMASK;
        else if (!(newfh.level & PERM_POSTMASK) && (*ans == 'P' || *ans == 'p'))
            newfh.level |= PERM_POSTMASK;
        clear ();
        move (2, 0);
        prints ("Éè¶¨ %s '%s' ÌÖÂÛÇøµÄÈ¨ÏŞ\n",
                newfh.level & PERM_POSTMASK ? "ÕÅÌù" : "ÔÄ¶Á", newfh.filename);
        newfh.level = setperms (newfh.level, "È¨ÏŞ", NUMPERMS, showperminfo);
        clear ();
    }
    move (1, 0);
	//modified by littlesword@SJTUbbs,positive_anony
    if (askyn
            ("ÉèÖÃ±¾°åÎªÄäÃû°åÃæÂğ", newfh.flag & ANONY_FLAG ? YEA : NA, NA) == YEA)
		if (askyn ("ÉèÖÃ±¾°åÄäÃû·¢ÎÄÎªÄ¬ÈÏÂğ", newfh.flag & ANONY_FLAG ? YEA : NA , NA) == YEA)
			newfh.flag |= ANONY_FLAG;
		else
			newfh.flag |= POSITIVE_ANONY_FLAG;
    else
	{
        newfh.flag &= ~ANONY_FLAG;
		newfh.flag &= ~POSITIVE_ANONY_FLAG;
	}
    move (2, 0);
    if (askyn ("ÉèÖÃ±¾°åÎª²»¿É Re Ä£Ê½°åÃæÂğ",
               (newfh.flag & NOREPLY_FLAG) ? YEA : NA, NA) == YEA)
        newfh.flag |= NOREPLY_FLAG;
    else
        newfh.flag &= ~NOREPLY_FLAG;
    move (3, 0);
    if (oldfh != NULL)
    {
        //      if(askyn("ÊÇ·ñÒÆ¶¯¾«»ªÇøµÄÎ»ÖÃ",NA,NA)==YEA) a_mv = 2;
        //      else a_mv = 0;
        a_mv = 2;
    }
    else
    {
        if (askyn ("±¾°åĞèÒªÉèÁ¢¾«»ªÇøÂğ", YEA, NA) == YEA)
            a_mv = 1;
        else
            a_mv = 0;
    }
    if (a_mv != 0)
    {
        ch = num[0];
        for (i = 0, j = 1; i < SectNum; i++)
        {
            if (GroupsInfo.name[i][0] == '\0')
                continue;
            move (j++, 50);
            prints ("©§%s(%d)%s GROUP_%d\033[m",
                    (i + '0' == ch) ? "\033[32m" : "", i,
                    GroupsInfo.name[i], i);
        }
        while (1)
        {
            strcpy (chs, num);
            if (*chs > '0' + 9)
                *chs += 7;		//Êı×Ö·Öµ½×ÖÄ¸
            getdata (j + 1, 48, "¾«»ªÇø·Ö²¼ÈçÉÏ£¬ÇëÑ¡Ôñ: ", chs, 2, DOECHO, NA);
            if (chs[0] >= 'a' && chs[0] <= 'z')
                chs[0] -= 32;	//Ğ¡Ğ´Í³Ò»µ½´óĞ´
            if (chs[0] >= 'A' && chs[0] <= 'Z')
                chs[0] -= 7;	//×ÖÄ¸Í³Ò»µ½Êı×Ö
            if (chs[0] == '\0' || chs[0] < '0' || chs[0] >= '0' + SectNum)
                continue;
            if (GroupsInfo.name[chs[0] - '0'][0] == '\0')
                continue;
            break;
        }
    }
    ShowBrdSet (newfh);
    if (oldfh)
    {
        if (askyn ("ÄúÈ·ÈÏÒª¸ü¸ÄÂğ", NA, NA) == NA)
            return -1;
        i = pos;
    }
    else
        i = getbnum ("");
    if (chs[0] > '0' + 9)
        chs[0] += 7;		//Êı×Ö·ÖÀëµ½×ÖÄ¸
    sprintf (group, "GROUP_%c", chs[0]);
    if (oldfh == NULL)
    {
        strcpy (vbuf, "vote/");
        strcat (vbuf, newfh.filename);
        setbpath (genbuf, newfh.filename);
        if ((!dashd (genbuf) && mkdir (genbuf, 0755) == -1)
                || (!dashd (vbuf) && mkdir (vbuf, 0755) == -1))
        {
            prints ("\n³õÊ¼»¯ÌÖÂÛÇøÄ¿Â¼Ê±³ö´í!\n");
            pressreturn ();
            clear ();
            return -1;
        }
        if (a_mv != 0)
        {
            if (newfh.BM[0] != '\0')
                sprintf (vbuf, "%-38.38s(BM: %s)", newfh.title + 11, newfh.BM);
            else
                sprintf (vbuf, "%-38.38s", newfh.title + 11);
            if (chs[0] >= 'A' && chs[0] <= 'Z')
                chs[0] -= 7;	//×ÖÄ¸Í³Ò»µ½Êı×Ö
            if (add_grp
                    (group, GroupsInfo.name[chs[0] - '0'], newfh.filename,
                     vbuf) == -1)
                prints ("\n³ÉÁ¢¾«»ªÇøÊ§°Ü....\n");
            else
                prints ("ÒÑ¾­ÖÃÈë¾«»ªÇø...\n");
        }
    }
    else
    {				// ĞŞ¸ÄÌÖÂÛÇø
        char tmp_grp[30];

        if (strcmp (oldfh->filename, newfh.filename))
        {
            char old[256], tar[256];

            a_mv = 1;
            setbpath (old, oldfh->filename);
            setbpath (tar, newfh.filename);
            rename (old, tar);
            sprintf (old, "vote/%s", oldfh->filename);
            sprintf (tar, "vote/%s", newfh.filename);
            rename (old, tar);
        }
        if (newfh.BM[0] != '\0')
            sprintf (vbuf, "%-38.38s(BM: %s)", newfh.title + 11, newfh.BM);
        else
            sprintf (vbuf, "%-38.38s", newfh.title + 11);
        get_grp (oldfh->filename);
        edit_grp (oldfh->filename, lookgrp, oldfh->title + 11, vbuf);
        if (a_mv)
        {
            get_grp (oldfh->filename);
            strcpy (tmp_grp, lookgrp);
            if (strcmp (tmp_grp, group) || a_mv == 1)
            {
                char tmpbuf[160], oldpath[STRLEN], newpath[STRLEN];

                sprintf (tmpbuf, "%s:", oldfh->filename);
                del_from_file ("0Announce/.Search", tmpbuf);
                if (newfh.BM[0] != '\0')
                    sprintf (vbuf, "%-38.38s(BM: %s)", newfh.title + 11,
                             newfh.BM);
                else
                    sprintf (vbuf, "%-38.38s", newfh.title + 11);
                if (chs[0] >= 'A' && chs[0] <= 'Z')
                    chs[0] -= 7;	//×ÖÄ¸Í³Ò»µ½Êı×Ö
                if (add_grp (group, GroupsInfo.name[chs[0] - '0'],
                             newfh.filename, vbuf) == -1)
                    prints ("\n³ÉÁ¢¾«»ªÇøÊ§°Ü....\n");
                else
                    prints ("ÒÑ¾­ÖÃÈë¾«»ªÇø...\n");
                sprintf (newpath, "0Announce/groups/%s/%s", group,
                         newfh.filename);
                sprintf (oldpath, "0Announce/groups/%s/%s", tmp_grp,
                         oldfh->filename);
                if (dashd (oldpath))
                {
                    deltree (newpath);
                }
                rename (oldpath, newpath);
                del_grp (tmp_grp, oldfh->filename, oldfh->title + 11);
            }
        }
    }
    if (i > 0)
    {
        substitute_record (BOARDS, &newfh, sizeof (newfh), i);
        if (oldfh)
        {
            sprintf (genbuf, "¸ü¸ÄÌÖÂÛÇø %s µÄ×ÊÁÏ --> %s",
                     oldfh->filename, newfh.filename);
            report (genbuf);
        }
    }
    else if (append_record (BOARDS, &newfh, sizeof (newfh)) == -1)
    {
        prints ("\n³ÉÁ¢ĞÂ°åÊ§°Ü£¡\n");
        pressanykey ();
        clear ();
        return -1;
    }
    numboards = -1;
    {
        char secu[STRLEN];

        if (oldfh)
            sprintf (secu, "ĞŞ¸ÄÌÖÂÛÇø£º%s(%s)", oldfh->filename, newfh.filename);
        else
            sprintf (secu, "³ÉÁ¢ĞÂ°å£º%s", newfh.filename);
        securityreport (secu);
    }
    pressanykey ();
    clear ();
}

int
m_newbrd ()
{
    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
        return -1;
    if (chkGroupsInfo () == 0)
        return -1;
    clear ();
    stand_title ("¿ªÆôĞÂÌÖÂÛÇø");
    ChangeTheBoard (NULL, 0);
    return 0;
}

void
ShowBrdSet (struct boardheader fh)
{
    int i = 4;

    move (i, 0);
    prints ("ÌÖÂÛÇø [%s] »ù±¾Çé¿ö£º", fh.filename);
    i += 2;
    move (i++, 0);
    prints ("ÏîÄ¿Ãû³Æ           ÏîÄ¿ÊôĞÔ");
    move (i++, 0);
    prints ("------------------------------");
    move (i++, 0);
    prints ("ÌÖÂÛÇøÃû³Æ       : %s", fh.filename);
    move (i++, 0);
    prints ("ÌÖÂÛÇø¹ÜÀíÔ±     : %s", fh.BM[0] == '\0' ? "ÉĞÎŞ°åÖ÷" : fh.BM);
    move (i++, 0);
    prints ("ÊÇ·ñÎª¿ÉÄäÃûÌÖÂÛÇø : %s",
            ((fh.flag & ANONY_FLAG) || (fh.flag & POSITIVE_ANONY_FLAG)) ? "ÄäÃû" : "·ÇÄäÃû");
    move (i++, 0);
    prints ("ÎÄÕÂÊÇ·ñ¿ÉÒÔ»Ø¸´ : %s",
            (fh.flag & NOREPLY_FLAG) ? "²»¿É Re" : "¿ÉÒÔ");
    move (i++, 0);
    prints ("READ/POST ÏŞÖÆ   : ");
    if (fh.level & ~PERM_POSTMASK)
    {
        prints ("%s", (fh.level & PERM_POSTMASK) ? "POST" :
                (fh.level & PERM_NOZAP) ? "ZAP" : "READ");
    }
    else
        prints ("ÎŞÏŞÖÆ");
    move (i++, 0);
    prints ("±¾ÌÖÂÛÇøËµÃ÷     : %s\n\n", fh.title);
}

int
m_editbrd ()
{
    int pos;
    struct boardheader fh;
    char bname[STRLEN];

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
        return -1;
    if (chkGroupsInfo () == 0)
        return -1;
    clear ();
    stand_title ("ĞŞ¸ÄÌÖÂÛÇø»ù±¾ÊôĞÔ");
    if (!gettheboardname (2, "ÊäÈëÌÖÂÛÇøÃû³Æ: ", &pos, &fh, bname))
        return -1;
    ShowBrdSet (fh);
    move (15, 0);
    if (askyn ("ÊÇ·ñĞèÒª¸ü¸ÄÒÔÉÏÊôĞÔ", NA, NA) == YEA)
    {
        clear ();
        stand_title ("ĞŞ¸ÄÌÖÂÛÇø»ù±¾ÊôĞÔ");
        ChangeTheBoard (&fh, pos);
    }
    return 0;
}
#else
int
m_newbrd ()
{
    struct boardheader newboard;
    extern int numboards;
    char ans[4];
    char vbuf[100];
    char *group;
    int bid;

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
        return;
    clear ();
    stand_title ("¿ªÆôĞÂÌÖÂÛÇø");
    memset (&newboard, 0, sizeof (newboard));
    move (2, 0);
    ansimore2 ("etc/boardref", NA, 3, 7);
    while (1)
    {
        getdata (10, 0, "ÌÖÂÛÇøÃû³Æ:   ", newboard.filename, BDNAME_LEN,
                 DOECHO, YEA);
        if (newboard.filename[0] != 0)
        {
            struct boardheader dh;

            if (search_record
                    (BOARDS, &dh, sizeof (dh), cmpbnames, newboard.filename))
            {
                prints ("\n´íÎó! ´ËÌÖÂÛÇøÒÑ¾­´æÔÚ!!");
                pressanykey ();
                return -1;
            }
        }
        else
            return -1;
        if (valid_brdname (newboard.filename))
            break;
        prints ("\n²»ºÏ·¨Ãû³Æ!!");
    }
    newboard.flag = 0;
    while (1)
    {
        getdata (11, 0, "ÌÖÂÛÇøËµÃ÷:   ", newboard.title, 60, DOECHO, YEA);
        if (newboard.title[0] == '\0')
            return -1;
        if (strstr (newboard.title, "¡ñ") || strstr (newboard.title, "¡Ñ"))
        {
            newboard.flag |= OUT_FLAG;
            break;
        }
        else if (strstr (newboard.title, "¡ğ"))
        {
            newboard.flag &= ~OUT_FLAG;
            break;
        }
        else
            prints ("´íÎóµÄ¸ñÊ½, ÎŞ·¨ÅĞ¶ÏÊÇ·ñ×ªĞÅ!!");
    }
    strcpy (vbuf, "vote/");
    strcat (vbuf, newboard.filename);
    setbpath (genbuf, newboard.filename);
    if (getbnum (newboard.filename) > 0 || mkdir (genbuf, 0755) == -1
            || mkdir (vbuf, 0755) == -1)
    {
        prints ("\n´íÎóµÄÌÖÂÛÇøÃû³Æ!!\n");
        pressreturn ();
        clear ();
        return -1;
    }
    move (12, 0);
    if (askyn ("±¾°å³ÏÕ÷°åÖ÷Âğ(·ñÔòÓÉ SYSOP ¹ÜÀí)?", YEA, NA) == NA)
        strcpy (newboard.BM, "SYSOP");
    else
        newboard.BM[0] = '\0';
    move (13, 0);
    if (askyn ("ÊÇ·ñÏŞÖÆ´æÈ¡È¨Á¦", NA, NA) == YEA)
    {
        getdata (14, 0, "ÏŞÖÆ Read/Post? [R]: ", ans, 2, DOECHO, YEA);
        if (*ans == 'P' || *ans == 'p')
            newboard.level = PERM_POSTMASK;
        else
            newboard.level = 0;
        move (1, 0);
        clrtobot ();
        move (2, 0);
        prints ("Éè¶¨ %s È¨Á¦. ÌÖÂÛÇø: '%s'\n",
                (newboard.level & PERM_POSTMASK ? "POST" : "READ"),
                newboard.filename);
        newboard.level =
            setperms (newboard.level, "È¨ÏŞ", NUMPERMS, showperminfo);
        clear ();
    }
    else
        newboard.level = 0;
    move (14, 0);
    if (askyn ("ÊÇ·ñ¼ÓÈëÄäÃû°å", NA, NA) == YEA)
		if (askyn ("ÊÇ·ñÉèÖÃÄäÃûÎªÄ¬ÈÏ", NA, NA) == YEA)
			newboard.flag |= ANONY_FLAG;
		else
			newboard.flag |= POSITIVE_ANONY_FLAG;
    else
	{
        newboard.flag &= ~ANONY_FLAG;
		newboard.flag &= ~POSITIVE_ANONY_FLAG;
	}
    if (askyn ("¸Ã°åµÄÈ«²¿ÎÄÕÂ¾ù²»¿ÉÒÔ»Ø¸´", NA, NA) == YEA)
        newboard.flag |= NOREPLY_FLAG;
    else
        newboard.flag &= ~NOREPLY_FLAG;
    if ((bid = getbnum ("")) > 0)
    {
        substitute_record (BOARDS, &newboard, sizeof (newboard), bid);
    }
    else if (append_record (BOARDS, &newboard, sizeof (newboard)) == -1)
    {
        pressreturn ();
        clear ();
        return -1;
    }
    group = chgrp ();
    if (group != NULL)
    {
        if (newboard.BM[0] != '\0')
            sprintf (vbuf, "%-38.38s(BM: %s)", newboard.title + 11, newboard.BM);
        else
            sprintf (vbuf, "%-38.38s", newboard.title + 11);
        if (add_grp (group, cexplain, newboard.filename, vbuf) == -1)
            prints ("\n³ÉÁ¢¾«»ªÇøÊ§°Ü....\n");
        else
            prints ("ÒÑ¾­ÖÃÈë¾«»ªÇø...\n");
    }
    numboards = -1;
    prints ("\nĞÂÌÖÂÛÇø³ÉÁ¢\n");
    {
        char secu[STRLEN];

        sprintf (secu, "³ÉÁ¢ĞÂ°å£º%s", newboard.filename);
        securityreport (secu);
    }
    pressreturn ();
    clear ();
    return 0;
}

int
m_editbrd ()
{
    char bname[STRLEN], buf[STRLEN], oldtitle[STRLEN], vbuf[256], *group;
    char oldpath[STRLEN], newpath[STRLEN], tmp_grp[30];
    int pos, noidboard, a_mv;
    struct boardheader fh, newfh;

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
        return;
    clear ();
    stand_title ("ĞŞ¸ÄÌÖÂÛÇø×ÊÑ¶");
    if (!gettheboardname (2, "ÊäÈëÌÖÂÛÇøÃû³Æ: ", &pos, &fh, bname))
        return -1;
    noidboard = (fh.flag & ANONY_FLAG) || (fh.flag & POSITIVE_ANONY_FLAG);
    noreply = fh.flag & NOREPLY_FLAG;
    move (3, 0);
    memcpy (&newfh, &fh, sizeof (newfh));
    prints ("ÌÖÂÛÇøÃû³Æ:     %s\n", fh.filename);
    prints ("ÌÖÂÛÇøËµÃ÷:     %s\n", fh.title);
    prints ("ÌÖÂÛÇø¹ÜÀíÔ±:   %s\n", fh.BM);
    prints ("¿ÉÄäÃûÌÖÂÛÇø:   %s\n", (noidboard) ? "Yes" : "No");
    prints ("ÎÄÕÂ²»¿ÉÒÔ»Ø¸´: %s\n", (noreply) ? "Yes" : "No");
    strcpy (oldtitle, fh.title);
    prints ("ÏŞÖÆ %s È¨Á¦: %s", (fh.level & PERM_POSTMASK) ? "POST" :
            (fh.level & PERM_NOZAP) ? "ZAP" : "READ",
            (fh.level & ~PERM_POSTMASK) == 0 ? "²»ÉèÏŞ" : "ÓĞÉèÏŞ");
    move (8, 0);
    if (askyn ("ÊÇ·ñ¸ü¸ÄÒÔÉÏ×ÊÑ¶", NA, NA) == NA)
    {
        clear ();
        return 0;
    }
    move (9, 0);
    prints ("Ö±½Ó°´ <Return> ²»ĞŞ¸Ä´ËÀ¸×ÊÑ¶...");
    while (1)
    {
        getdata (10, 0, "ĞÂÌÖÂÛÇøÃû³Æ: ", genbuf, BDNAME_LEN, DOECHO, YEA);
        if (*genbuf != 0)
        {
            struct boardheader dh;

            if (search_record (BOARDS, &dh, sizeof (dh), cmpbnames, genbuf))
            {
                move (2, 0);
                prints ("´íÎó! ´ËÌÖÂÛÇøÒÑ¾­´æÔÚ!!");
                move (10, 0);
                clrtoeol ();
                continue;
            }
            if (valid_brdname (genbuf))
            {
                strncpy (newfh.filename, genbuf, sizeof (newfh.filename));
                strcpy (bname, genbuf);
                break;
            }
            else
            {
                move (2, 0);
                prints ("²»ºÏ·¨µÄÌÖÂÛÇøÃû³Æ!");
                move (10, 0);
                clrtoeol ();
                continue;
            }
        }
        else
            break;
    }
    ansimore2 ("etc/boardref", NA, 11, 7);
    while (1)
    {
        getdata (18, 0, "ĞÂÌÖÂÛÇøËµÃ÷: ", genbuf, 60, DOECHO, YEA);
        if (*genbuf != 0)
            strncpy (newfh.title, genbuf, sizeof (newfh.title));
        else
            break;
        if (strstr (newfh.title, "¡ñ") || strstr (newfh.title, "¡Ñ"))
        {
            newfh.flag |= OUT_FLAG;
            break;
        }
        else if (strstr (newfh.title, "¡ğ"))
        {
            newfh.flag &= ~OUT_FLAG;
            break;
        }
        else
            prints ("\n´íÎóµÄ¸ñÊ½, ÎŞ·¨ÅĞ¶ÏÊÇ·ñ×ªĞÅ!!");
    }
    move (20, 0);
    clrtoeol ();
    move (19, 0);
    if (fh.BM[0] != '\0' && strcmp (fh.BM, "SYSOP"))
    {
        if (askyn ("ĞŞ¸ÄÌÖÂÛÇø¹ÜÀíÔ±¡£×¢Òâ£ºÈ·ÓĞ´íÎó²Å¿ÉĞŞ¸Ä¡£²»ĞŞ¸Ä°´»Ø³µ",
                   NA, NA) == YEA)
        {
            getdata (19, 0,
                     "ÌÖÂÛÇø¹ÜÀíÔ±(¡¾×¢Òâ¡¿°åÖ÷ÈÎÃâÇëÎğ¸Ä¶¯´Ë´¦£¡): ",
                     newfh.BM, sizeof (newfh.BM), DOECHO, YEA);
            if (newfh.BM[0] == '\0')
                strcpy (newfh.BM, fh.BM);
            else if (newfh.BM[0] == ' ')
                newfh.BM[0] = '\0';
            else
                ;
        }
    }
    else
    {
        if (askyn ("±¾°å³ÏÕ÷°åÖ÷Âğ(·ñ£¬ÔòÓÉ SYSOP ¹ÜÀí)?", YEA, NA) == NA)
            strncpy (newfh.BM, "SYSOP", sizeof (newfh.BM));
        else
            strncpy (newfh.BM, "\0", sizeof (newfh.BM));
    }
    sprintf (buf, "ÄäÃû°å£¨Ä¬ÈÏÄäÃû£© (Y/N)? [%c]: ", (noidboard) ? 'Y' : 'N');
    getdata (20, 0, buf, genbuf, 4, DOECHO, YEA);
    if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
    {
        if (*genbuf == 'y' || *genbuf == 'Y')
            newfh.flag |= ANONY_FLAG;
        else
            newfh.flag &= ~ANONY_FLAG;
    }
    sprintf (buf, "ÄäÃû°å£¨ÄäÃû²»Ä¬ÈÏ£© (Y/N)? [%c]: ", (noidboard) ? 'Y' : 'N');
    getdata (20, 0, buf, genbuf, 4, DOECHO, YEA);
    if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
    {
        if (*genbuf == 'y' || *genbuf == 'Y')
            newfh.flag |= POSITIVE_ANONY_FLAG;
        else
            newfh.flag &= ~POSITIVE_ANONY_FLAG;
    }
    sprintf (buf, "ÎÄÕÂÊÇ·ñ ²»¿ÉÒÔ »Ø¸´ (Y/N)? [%c]: ", (noreply) ? 'Y' : 'N');
    getdata (20, 30, buf, genbuf, 4, DOECHO, YEA);
    if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
    {
        if (*genbuf == 'y' || *genbuf == 'Y')
            newfh.flag |= NOREPLY_FLAG;
        else
            newfh.flag &= ~NOREPLY_FLAG;
    }
    move (21, 0);
    if (askyn ("ÊÇ·ñÒÆ¶¯¾«»ªÇøµÄÎ»ÖÃ", NA, NA) == YEA)
        a_mv = 2;
    else
        a_mv = 0;
    move (22, 0);
    if (askyn ("ÊÇ·ñ¸ü¸Ä´æÈ¡È¨ÏŞ", NA, NA) == YEA)
    {
        char ans[4];

        sprintf (genbuf, "ÏŞÖÆ (R)ÔÄ¶Á »ò (P)ÕÅÌù ÎÄÕÂ [%c]: ",
                 (newfh.level & PERM_POSTMASK ? 'P' : 'R'));
        getdata (23, 0, genbuf, ans, 2, DOECHO, YEA);
        if ((newfh.level & PERM_POSTMASK) && (*ans == 'R' || *ans == 'r'))
            newfh.level &= ~PERM_POSTMASK;
        else if (!(newfh.level & PERM_POSTMASK) && (*ans == 'P' || *ans == 'p'))
            newfh.level |= PERM_POSTMASK;
        clear ();
        move (2, 0);
        prints ("Éè¶¨ %s '%s' ÌÖÂÛÇøµÄÈ¨ÏŞ\n",
                newfh.level & PERM_POSTMASK ? "ÕÅÌù" : "ÔÄ¶Á", newfh.filename);
        newfh.level = setperms (newfh.level, "È¨ÏŞ", NUMPERMS, showperminfo);
        clear ();
        getdata (0, 0, "È·¶¨Òª¸ü¸ÄÂğ? (Y/N) [N]: ", genbuf, 4, DOECHO, YEA);
    }
    else
    {
        getdata (23, 0, "È·¶¨Òª¸ü¸ÄÂğ? (Y/N) [N]: ", genbuf, 4, DOECHO, YEA);
    }
    if (*genbuf != 'Y' && *genbuf != 'y')
    {
        clear ();
        return 0;
    }
    {
        char secu[STRLEN];

        sprintf (secu, "ĞŞ¸ÄÌÖÂÛÇø£º%s(%s)", fh.filename, newfh.filename);
        securityreport (secu);
    }
    if (strcmp (fh.filename, newfh.filename))
    {
        char old[256], tar[256];

        a_mv = 1;
        setbpath (old, fh.filename);
        setbpath (tar, newfh.filename);
        rename (old, tar);
        sprintf (old, "vote/%s", fh.filename);
        sprintf (tar, "vote/%s", newfh.filename);
        rename (old, tar);
    }
    if (newfh.BM[0] != '\0')
        sprintf (vbuf, "%-38.38s(BM: %s)", newfh.title + 11, newfh.BM);
    else
        sprintf (vbuf, "%-38.38s", newfh.title + 11);
    get_grp (fh.filename);
    edit_grp (fh.filename, lookgrp, oldtitle + 11, vbuf);
    if (a_mv >= 1)
    {
        group = chgrp ();
        get_grp (fh.filename);
        strcpy (tmp_grp, lookgrp);
        if (strcmp (tmp_grp, group) || a_mv != 2)
        {
            char tmpbuf[160];

            sprintf (tmpbuf, "%s:", fh.filename);
            del_from_file ("0Announce/.Search", tmpbuf);
            if (group != NULL)
            {
                if (newfh.BM[0] != '\0')
                    sprintf (vbuf, "%-38.38s(BM: %s)", newfh.title + 11,
                             newfh.BM);
                else
                    sprintf (vbuf, "%-38.38s", newfh.title + 11);
                if (add_grp (group, cexplain, newfh.filename, vbuf) == -1)
                    prints ("\n³ÉÁ¢¾«»ªÇøÊ§°Ü....\n");
                else
                    prints ("ÒÑ¾­ÖÃÈë¾«»ªÇø...\n");
                sprintf (newpath, "0Announce/groups/%s/%s", group,
                         newfh.filename);
                sprintf (oldpath, "0Announce/groups/%s/%s", tmp_grp,
                         fh.filename);
                if (dashd (oldpath))
                {
                    deltree (newpath);
                }
                rename (oldpath, newpath);
                del_grp (tmp_grp, fh.filename, fh.title + 11);
            }
        }
    }
    substitute_record (BOARDS, &newfh, sizeof (newfh), pos);
    sprintf (genbuf, "¸ü¸ÄÌÖÂÛÇø %s µÄ×ÊÁÏ --> %s", fh.filename,
             newfh.filename);
    report (genbuf);
    numboards = -1;		/* force re-caching */
    clear ();
    return 0;
}
#endif

int
m_mclean ()
{
    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
    {
        return;
    }
    clear ();
    stand_title ("Çå³ıË½ÈËĞÅ¼ş");
    move (1, 0);
    prints ("Çå³ıËùÓĞÒÑ¶ÁÇÒÎ´ mark µÄĞÅ¼ş\n");
    if (askyn ("È·¶¨Âğ", NA, NA) == NA)
    {
        clear ();
        return 0;
    }
    {
        char secu[STRLEN];

        sprintf (secu, "Çå³ıËùÓĞÊ¹ÓÃÕßÒÑ¶ÁĞÅ¼ş¡£");
        securityreport (secu);
    }

    cleanlog = fopen ("mailclean.log", "w");
    move (3, 0);
    prints ("ÇëÄÍĞÄµÈºò.\n");
    refresh ();
    if (apply_record (PASSFILE, cleanmail, sizeof (struct userec)) == -1)
    {
        move (4, 0);
        prints ("apply PASSFILE err...\n");
        pressreturn ();
        clear ();
        return -1;
    }
    move (4, 0);
    fclose (cleanlog);
    prints ("Çå³ıÍê³É! ¼ÇÂ¼µµ mailclean.log.\n");
    pressreturn ();
    clear ();
    return 0;
}

/*
int
m_trace()
{
	struct stat ostatb, cstatb;
	int     otflag, ctflag, done = 0;
	char    ans[3];
	char   *msg;
	modify_user_mode(ADMIN);
	if (!check_systempasswd()) {
		return;
	}
	clear();
	stand_title("Set Trace Options");
	while (!done) {
		move(2, 0);
		otflag = stat("trace", &ostatb);
		ctflag = stat("trace.chatd", &cstatb);
		prints("Ä¿Ç°Éè¶¨:\n");
		trace_state(otflag, "Ò»°ã", ostatb.st_size);
		trace_state(ctflag, "ÁÄÌì", cstatb.st_size);
		move(9, 0);
		prints("<1> ÇĞ»»Ò»°ã¼ÇÂ¼\n");
		prints("<2> ÇĞ»»ÁÄÌì¼ÇÂ¼\n");
		getdata(12, 0, "ÇëÑ¡Ôñ (1/2/Exit) [E]: ", ans, 2, DOECHO, YEA);
 
		switch (ans[0]) {
		case '1':
			if (otflag) {
				touchfile("trace");
				msg = "Ò»°ã¼ÇÂ¼ ON";
			} else {
				rename("trace", "trace.old");
				msg = "Ò»°ã¼ÇÂ¼ OFF";
			}
			break;
		case '2':
			if (ctflag) {
				touchfile("trace.chatd");
				msg = "ÁÄÌì¼ÇÂ¼ ON";
			} else {
				rename("trace.chatd", "trace.chatd.old");
				msg = "ÁÄÌì¼ÇÂ¼ OFF";
			}
			break;
		default:
			msg = NULL;
			done = 1;
		}
		move(t_lines - 2, 0);
		if (msg) {
			prints("%s\n", msg);
			report(msg);
		}
	}
	clear();
	return 0;
}
*/

int
m_register ()
{
    FILE *fn;
    char ans[3], /*fname[20], */fname_new[30];
    int x, y, wid, len;
    char uident[STRLEN];

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
    {
        return;
    }
    clear ();

    stand_title ("Éè¶¨Ê¹ÓÃÕß×¢²á×ÊÁÏ");
    for (;;)
    {
        getdata (1, 0,
                 "(0)Àë¿ª  (1)Éó²é×¢²á×ÊÁÏ  (2)²éÑ¯Ê¹ÓÃÕß×¢²á×ÊÁÏ: ",
                 ans, 2, DOECHO, YEA);
        if (ans[0] == '1' || ans[0] == '2'/* || ans[0] == '3'*/)
            break;
        else
            return 0;
    }
//    sprintf(fname, "new_register");
	sprintf(fname_new, "register_sjtubbs");
//    if (ans[0] == '1')
//    {
//        if ((fn = fopen (fname, "r")) == NULL)
//        {
//            prints ("\n\nÄ¿Ç°²¢ÎŞĞÂ×¢²á×ÊÁÏ.");
//            pressreturn ();
//        }
//        else
//        {
//            y = 3, x = wid = 0;
//            while (fgets (genbuf, STRLEN, fn) != NULL && x < 65)
//            {
//                if (strncmp (genbuf, "userid: ", 8) == 0)
//                {
//                    move (y++, x);
//                    prints ("%s", genbuf + 8);
//                    len = strlen (genbuf + 8);
//                    if (len > wid)
//                        wid = len;
//                    if (y >= t_lines - 2)
//                    {
//                        y = 3;
//                        x += wid + 2;
//                    }
//                }
//            }
//            fclose (fn);
//            if (askyn ("Éè¶¨×ÊÁÏÂğ", NA, YEA) == YEA)
//            {
//                securityreport ("Éè¶¨Ê¹ÓÃÕß×¢²á×ÊÁÏ");
//                scan_register_form (fname, NULL);
//            }
//        }
//    }
    
	//else //ĞÂ°æ±¾µÄ×¢²áµ¥£¬by rwzmm @ sjtubbs
	if(ans[0] == '1')
	{
		struct stat st;
		int record_ns = 0;
		struct regform record = {0};
		if ((fn = fopen (fname_new, "r")) == NULL)
        {
            prints ("\n\nÄ¿Ç°²¢ÎŞĞÂ×¢²á×ÊÁÏ.");
            pressreturn ();
        }

		else
		{
			stat(fname_new, &st);
			record_ns = st.st_size / sizeof(struct regform);
			if(record_ns == 0)
			{
				prints ("\n\nÄ¿Ç°²¢ÎŞĞÂ×¢²á×ÊÁÏ.");
				pressreturn ();
			}
			else
			{
				move(3,0);
				prints("Ä¿Ç°¹²ÓĞĞÂ×¢²á×ÊÁÏ %d ·İ¡£", record_ns);

				y = 5, x = wid = 0;
				while ((fread (&record, sizeof(struct regform), 1, fn) >0) && (x < 65))
				{
					move (y++, x);
					prints ("%s", record.userid);
					len = IDLEN;
					if (len > wid)
						wid = len;
					if (y >= t_lines - 2)
					{
						y = 5;
						x += wid + 2;
					}
				}
				fclose (fn);
				if (askyn ("Éè¶¨×ÊÁÏÂğ", NA, YEA) == YEA)
				{
					securityreport ("Éè¶¨Ê¹ÓÃÕß×¢²á×ÊÁÏ");
					scan_new_register_form (fname_new, NULL);
				}
			}
		}
	}
    else
    {
        move (1, 0);
        usercomplete ("ÇëÊäÈëÒª²éÑ¯µÄ´úºÅ: ", uident);
        if (uident[0] != '\0')
            if (!getuser (uident))
            {
                move (2, 0);
                prints ("´íÎóµÄÊ¹ÓÃÕß´úºÅ...");
            }
            else
            {
                sprintf (genbuf, "home/%c/%s/register",
                         toupper (lookupuser.userid[0]), lookupuser.userid);
                if ((fn = fopen (genbuf, "r")) != NULL)
                {
                    prints ("\n×¢²á×ÊÁÏÈçÏÂ:\n\n");
                    for (x = 1; x <= 15; x++)
                    {
                        if (fgets (genbuf, STRLEN, fn))
                            prints ("%s", genbuf);
                        else
                            break;
                    }
                }
                else
                {
                    prints ("\n\nÕÒ²»µ½Ëû/ËıµÄ×¢²á×ÊÁÏ!!\n");
//                    scan_register_form ();
					scan_new_register_form(fname_new, lookupuser.userid);
                }
            }
        pressanykey ();
    }
    clear ();
    return 0;
}

int
d_board ()
{
    struct boardheader binfo;
    int bid, ans;
    char bname[STRLEN];
    extern char lookgrp[];
    extern int numboards;

    if (!HAS_PERM (PERM_BLEVELS))
    {
        return 0;
    }
    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
    {
        return;
    }
    clear ();
    stand_title ("É¾³ıÌÖÂÛÇø");
    make_blist ();
    move (1, 0);
    namecomplete ("ÇëÊäÈëÌÖÂÛÇø: ", bname);
    if (bname[0] == '\0')
        return 0;
    bid = getbnum (bname);
    if (get_record (BOARDS, &binfo, sizeof (binfo), bid) == -1)
    {
        move (2, 0);
        prints ("²»ÕıÈ·µÄÌÖÂÛÇø\n");
        pressreturn ();
        clear ();
        return 0;
    }
    if (binfo.BM[0] != '\0' && strcmp (binfo.BM, "SYSOP"))
    {
        move (5, 0);
        prints ("¸Ã°å»¹ÓĞ°åÖ÷£¬ÔÚÉ¾³ı±¾°åÇ°£¬ÇëÏÈÈ¡Ïû°åÖ÷µÄÈÎÃü¡£\n");
        pressanykey ();
        clear ();
        return 0;
    }
    ans = askyn ("ÄãÈ·¶¨ÒªÉ¾³ıÕâ¸öÌÖÂÛÇø", NA, NA);
    if (ans != 1)
    {
        move (2, 0);
        prints ("È¡ÏûÉ¾³ıĞĞ¶¯\n");
        pressreturn ();
        clear ();
        return 0;
    }
    {
        char secu[STRLEN];

        sprintf (secu, "É¾³ıÌÖÂÛÇø£º%s", binfo.filename);
        securityreport (secu);
    }
    if (seek_in_file ("0Announce/.Search", bname))
    {
        move (4, 0);
        if (askyn ("ÒÆ³ı¾«»ªÇø", NA, NA) == YEA)
        {
            get_grp (binfo.filename);
            del_grp (lookgrp, binfo.filename, binfo.title + 11);
        }
    }
    if (seek_in_file ("etc/junkboards", bname))
        del_from_file ("etc/junkboards", bname);
    if (seek_in_file ("0Announce/.Search", bname))
    {
        char tmpbuf[160];

        sprintf (tmpbuf, "%s:", bname);
        del_from_file ("0Announce/.Search", tmpbuf);
    }
    if (binfo.filename[0] == '\0')
        return -1;			/* rrr - precaution */
    sprintf (genbuf, "boards/%s", binfo.filename);
    f_rm (genbuf);
    sprintf (genbuf, "vote/%s", binfo.filename);
    f_rm (genbuf);
    sprintf (genbuf, " << '%s' ±» %s É¾³ı >>",
             binfo.filename, currentuser.userid);
    memset (&binfo, 0, sizeof (binfo));
    strncpy (binfo.title, genbuf, STRLEN);
    binfo.level = PERM_SYSOP;
    substitute_record (BOARDS, &binfo, sizeof (binfo), bid);

    move (4, 0);
    prints ("\n±¾ÌÖÂÛÇøÒÑ¾­É¾³ı...\n");
    pressreturn ();
    numboards = -1;
    clear ();
    return 0;
}

int
d_user (cid)
char *cid;
{
    int id, num, i;
    char secu[STRLEN];

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
    {
        return;
    }
    clear ();
    stand_title ("É¾³ıÊ¹ÓÃÕßÕÊºÅ");
    move (2, 0);
    if (!gettheuserid (1, "ÇëÊäÈëÓûÉ¾³ıµÄÊ¹ÓÃÕß´úºÅ: ", &id))
        return 0;
    if (!strcmp (lookupuser.userid, "SYSOP"))
    {
        prints ("\n¶Ô²»Æğ£¬Äã²»¿ÉÒÔÉ¾³ı SYSOP ÕÊºÅ!!\n");
        pressreturn ();
        clear ();
        return 0;
    }
    if (!strcmp (lookupuser.userid, currentuser.userid))
    {
        prints ("\n¶Ô²»Æğ£¬Äã²»¿ÉÒÔÉ¾³ı×Ô¼ºµÄÕâ¸öÕÊºÅ!!\n");
        pressreturn ();
        clear ();
        return 0;
    }
    prints ("\n\nÒÔÏÂÊÇ [%s] µÄ²¿·Ö×ÊÁÏ:\n", lookupuser.userid);
    prints ("    User ID:  [%s]\n", lookupuser.userid);
    prints ("    êÇ   ³Æ:  [%s]\n", lookupuser.username);
    prints ("    ĞÕ   Ãû:  [%s]\n", lookupuser.realname);
    strcpy (secu, "bTCPRD#@XWBA#VS-DOM-F0s23456789t");
    for (num = 0; num < strlen (secu); num++)
        if (!(lookupuser.userlevel & (1 << num)))
            secu[num] = '-';
    prints ("    È¨   ÏŞ: %s\n\n", secu);

    num = getbnames (lookupuser.userid, secu, &num);
    if (num)
    {
        prints ("[%s] Ä¿Ç°ÉĞµ£ÈÎÁË %d ¸ö°åµÄ°åÖ÷: ", lookupuser.userid, num);
        for (i = 0; i < num; i++)
            prints ("%s ", bnames[i]);
        prints ("\nÇëÏÈÊ¹ÓÃ°åÖ÷Ğ¶Ö°¹¦ÄÜÈ¡ÏûÆä°åÖ÷Ö°ÎñÔÙ×ö¸Ã²Ù×÷.");
        pressanykey ();
        clear ();
        return 0;
    }

    sprintf (genbuf, "ÄãÈ·ÈÏÒªÉ¾³ı [%s] Õâ¸ö ID Âğ", lookupuser.userid);
    if (askyn (genbuf, NA, NA) == NA)
    {
        prints ("\nÈ¡ÏûÉ¾³ıÊ¹ÓÃÕß...\n");
        pressreturn ();
        clear ();
        return 0;
    }
    sprintf (secu, "É¾³ıÊ¹ÓÃÕß£º%s", lookupuser.userid);
    securityreport (secu);
    sprintf (genbuf, "mail/%c/%s", toupper (lookupuser.userid[0]),
             lookupuser.userid);
    f_rm (genbuf);
    sprintf (genbuf, "home/%c/%s", toupper (lookupuser.userid[0]),
             lookupuser.userid);
    f_rm (genbuf);
    lookupuser.userlevel = 0;
    strcpy (lookupuser.address, "");
    strcpy (lookupuser.username, "");
    strcpy (lookupuser.realname, "");
    strcpy (lookupuser.termtype, "");
    prints ("\n%s ÒÑ¾­±»Ãğ¾øÁË...\n", lookupuser.userid);
    lookupuser.userid[0] = '\0';
    substitute_record (PASSFILE, &lookupuser, sizeof (lookupuser), id);
    setuserid (id, lookupuser.userid);
    pressreturn ();
    clear ();
    return 1;
}

int
x_level ()
{
    int id;
    unsigned int newlevel;

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
    {
        return;
    }
    clear ();
    move (0, 0);
    prints ("¸ü¸ÄÊ¹ÓÃÕßÈ¨ÏŞ\n");
    clrtoeol ();
    move (1, 0);
    usercomplete ("ÊäÈëÓû¸ü¸ÄµÄÊ¹ÓÃÕßÕÊºÅ: ", genbuf);
    if (genbuf[0] == '\0')
    {
        clear ();
        return 0;
    }
    if (!(id = getuser (genbuf)))
    {
        move (3, 0);
        prints ("Invalid User Id");
        clrtoeol ();
        pressreturn ();
        clear ();
        return 0;
    }
    move (1, 0);
    clrtobot ();
    move (2, 0);
    prints ("Éè¶¨Ê¹ÓÃÕß '%s' µÄÈ¨ÏŞ \n", genbuf);
    newlevel = setperms (lookupuser.userlevel, "È¨ÏŞ", NUMPERMS, showperminfo);
    move (2, 0);
    if (newlevel == lookupuser.userlevel)
        prints ("Ê¹ÓÃÕß '%s' È¨ÏŞÃ»ÓĞ±ä¸ü\n", lookupuser.userid);
    else
    {
        lookupuser.userlevel = newlevel;
        {
            char secu[STRLEN];

            sprintf (secu, "ĞŞ¸Ä %s µÄÈ¨ÏŞ", lookupuser.userid);
            securityreport (secu);
        }

        substitute_record (PASSFILE, &lookupuser, sizeof (struct userec), id);
        if (!(lookupuser.userlevel & PERM_LOGINOK))
        {
            char src[STRLEN], dst[STRLEN];

            sethomefile (dst, lookupuser.userid, "register.old");
            if (dashf (dst))
                unlink (dst);
            sethomefile (src, lookupuser.userid, "register");
            if (dashf (src))
                rename (src, dst);
        }
        prints ("Ê¹ÓÃÕß '%s' È¨ÏŞÒÑ¾­¸ü¸ÄÍê±Ï.\n", lookupuser.userid);
    }
    pressreturn ();
    clear ();
    return 0;
}


int
x_denylevel ()
{
    int id;
    char ans[7];

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
    {
        return;
    }
    clear ();
    move (0, 0);
    prints ("¸ü¸ÄÊ¹ÓÃÕß»ù±¾È¨ÏŞ»ò×ÊÁÏ\n");
    clrtoeol ();
    move (1, 0);
    usercomplete ("ÊäÈëÓû¸ü¸ÄµÄÊ¹ÓÃÕßÕÊºÅ: ", genbuf);
    if (genbuf[0] == '\0')
    {
        clear ();
        return 0;
    }
    if (!(id = getuser (genbuf)))
    {
        move (3, 0);
        prints ("Invalid User Id");
        clrtoeol ();
        pressreturn ();
        clear ();
        return 0;
    }
    move (1, 0);
    clrtobot ();
    move (2, 0);
    prints ("Éè¶¨Ê¹ÓÃÕß '%s' µÄ»ù±¾È¨ÏŞ»ò×ÊÁÏ \n\n", genbuf);
    prints ("(1) ·â½û·¢±íÎÄÕÂÈ¨Á¦       (A) »Ö¸´·¢±íÎÄÕÂÈ¨Á¦\n");
    prints ("(2) È¡Ïû»ù±¾ÉÏÕ¾È¨Á¦       (B) »Ö¸´»ù±¾ÉÏÕ¾È¨Á¦\n");
    prints ("(3) ½ûÖ¹½øÈëÁÄÌìÊÒ         (C) »Ö¸´½øÈëÁÄÌìÊÒÈ¨Á¦\n");
    prints ("(4) ½ûÖ¹ºô½ĞËûÈËÁÄÌì       (D) »Ö¸´ºô½ĞËûÈËÁÄÌìÈ¨Á¦\n");
    prints ("(5) ½ûÖ¹¸øËûÈË·¢ÏûÏ¢       (E) »Ö¸´¸øËûÈË·¢ÏûÏ¢È¨Á¦\n");
    prints ("(6) ĞŞ¸Ä %s »ù±¾×ÊÁÏ\n", genbuf);
    getdata (12, 0, "ÇëÊäÈëÄãµÄ´¦Àí: ", ans, 3, DOECHO, YEA);
    switch (ans[0])
    {
    case '1':
        lookupuser.userlevel &= ~PERM_POST;
        break;
    case 'a':
    case 'A':
        lookupuser.userlevel |= PERM_POST;
        break;
    case '2':
        lookupuser.userlevel &= ~PERM_BASIC;
        break;
    case 'b':
    case 'B':
        lookupuser.userlevel |= PERM_BASIC;
        break;
    case '3':
        lookupuser.userlevel &= ~PERM_CHAT;
        break;
    case 'c':
    case 'C':
        lookupuser.userlevel |= PERM_CHAT;
        break;
    case '4':
        lookupuser.userlevel &= ~PERM_PAGE;
        break;
    case 'd':
    case 'D':
        lookupuser.userlevel |= PERM_PAGE;
        break;
    case '5':
        lookupuser.userlevel &= ~PERM_MESSAGE;
        break;
    case 'e':
    case 'E':
        lookupuser.userlevel |= PERM_MESSAGE;
        break;
    case '6':
        uinfo_change1 (12, &lookupuser, &lookupuser);
        break;
    default:
        prints ("\n Ê¹ÓÃÕß '%s' »ù±¾È¨Á¦ºÍ×ÊÁÏÃ»ÓĞ±ä¸ü\n", lookupuser.userid);
        pressreturn ();
        clear ();
        return 0;
    }
    {
        char secu[STRLEN];

        sprintf (secu, "ĞŞ¸Ä %s µÄ»ù±¾È¨ÏŞ»ò×ÊÁÏ", lookupuser.userid);
        securityreport (secu);
    }
    substitute_record (PASSFILE, &lookupuser, sizeof (struct userec), id);
    prints ("\n Ê¹ÓÃÕß '%s' »ù±¾È¨ÏŞ»ò×ÊÁÏÒÑ¾­¸ü¸ÄÍê±Ï.\n", lookupuser.userid);
    pressreturn ();
    clear ();
    return 0;
}

void
a_edits ()
{
    int aborted;
    char ans[7], buf[STRLEN], buf2[STRLEN];
    int ch, num, confirm;
    extern struct BCACHE *brdshm;
    static char *e_file[] =
        { "../Welcome", "../Welcome2", "issue", "logout", "../vote/notes",
          "menu.ini", "../.badname", "../.bad_email", "../.bad_host",
          "autopost", "junkboards", "sysops", "expire.ctl", "whatdate",
          "../NOLOGIN", "../NOREGISTER", "../.blockmail", "s_fill",
          "f_fill", "register", "firstlogin", "chatstation",
          "bbsnet.ini", "bbsnet2.ini", "notbackupboards",
          "DISTmaster.ini", "macroboards",  "goodbm", NULL
        };
    static char *explain_file[] =
        { "ÌØÊâ½øÕ¾¹«²¼À¸", "½øÕ¾»­Ãæ", "½øÕ¾»¶Ó­µµ", "ÀëÕ¾»­Ãæ",
          "¹«ÓÃ±¸ÍüÂ¼", "menu.ini", "²»¿É×¢²áµÄ ID", "²»¿ÉÈ·ÈÏÖ®E-Mail",
          "²»¿ÉÉÏÕ¾Ö®Î»Ö·",
          "Ã¿ÈÕ×Ô¶¯ËÍĞÅµµ", "²»ËãPOSTÊıµÄ°å", "¹ÜÀíÕßÃûµ¥", "¶¨Ê±¿³ĞÅÉè¶¨µµ",
          "¼ÍÄîÈÕÇåµ¥", "ÔİÍ£µÇÂ½(NOLOGIN)", "ÔİÍ£×¢²á(NOREGISTER)",
          "×ªĞÅºÚÃûµ¥", "×¢²á³É¹¦ĞÅ¼ş", "×¢²áÊ§°ÜĞÅ¼ş", "ĞÂÓÃ»§×¢²á·¶Àı",
          "ÓÃ»§µÚÒ»´ÎµÇÂ½¹«¸æ", "¹ú¼Ê»áÒéÌüÇåµ¥", "BBSNET ×ªÕ¾Çåµ¥",
          "BBSNET2 ×ªÕ¾Çåµ¥", "Çø¶ÎÉ¾³ı²»Ğè±¸·İÖ®Çåµ¥", "Çø³¤Ãûµ¥",
          "ÔÊĞíÊ¹ÓÃºê¶¨Òå°å","ÓÅĞã°åÖ÷Ãûµ¥", NULL
        };

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
    {
        return;
    }
    clear ();
    move (1, 0);
    prints ("±àĞŞÏµÍ³µµ°¸\n\n");
    for (num = 0;
            HAS_PERM (PERM_SYSOP) ? e_file[num] != NULL
            && explain_file[num] != NULL : explain_file[num] != "menu.ini"; num++)
    {
        prints ("[[1;32m%2d[m] %s", num + 1, explain_file[num]);
        if (num < 17)
            move (4 + num, 0);
        else
            move (num - 14, 50);
    }
    prints ("[[1;32m%2d[m] ¶¼²»Ïë¸Ä\n", num + 1);

    getdata (23, 0, "ÄãÒª±àĞŞÄÄÒ»ÏîÏµÍ³µµ°¸: ", ans, 3, DOECHO, YEA);
    ch = atoi (ans);
    if (!isdigit (ans[0]) || ch <= 0 || ch > num || ans[0] == '\n'
            || ans[0] == '\0')
        return;
    ch -= 1;
    sprintf (buf2, "etc/%s", e_file[ch]);
    move (3, 0);
    clrtobot ();
    sprintf (buf, "(E)±à¼­ (D)É¾³ı %s? [E]: ", explain_file[ch]);
    getdata (3, 0, buf, ans, 2, DOECHO, YEA);
    if (ans[0] == 'D' || ans[0] == 'd')
    {
        sprintf (buf, "ÄãÈ·¶¨ÒªÉ¾³ı %s Õâ¸öÏµÍ³µµ", explain_file[ch]);
        confirm = askyn (buf, NA, NA);
        if (confirm != 1)
        {
            move (5, 0);
            prints ("È¡ÏûÉ¾³ıĞĞ¶¯\n");
            pressreturn ();
            clear ();
            return;
        }
        {
            char secu[STRLEN];

            sprintf (secu, "É¾³ıÏµÍ³µµ°¸£º%s", explain_file[ch]);
            securityreport (secu);
        }
        unlink (buf2);
        move (5, 0);
        prints ("%s ÒÑÉ¾³ı\n", explain_file[ch]);
        pressreturn ();
        clear ();
        return;
    }
    modify_user_mode (EDITSFILE);
    aborted = vedit (buf2, NA, YEA);	/* ²»Ìí¼ÓÎÄ¼şÍ·, ÔÊĞíĞŞ¸ÄÍ·²¿ĞÅÏ¢ */
    clear ();
    if (aborted != -1)
    {
        prints ("%s ¸üĞÂ¹ı", explain_file[ch]);
        {
            char secu[STRLEN];

            sprintf (secu, "ĞŞ¸ÄÏµÍ³µµ°¸£º%s", explain_file[ch]);
            securityreport (secu);
        }

        if (!strcmp (e_file[ch], "../Welcome"))
        {
            unlink ("Welcome.rec");
            prints ("\nWelcome ¼ÇÂ¼µµ¸üĞÂ");
        }
        else if (!strcmp (e_file[ch], "whatdate"))
        {
            brdshm->fresh_date = time (0);
            prints ("\n¼ÍÄîÈÕÇåµ¥ ¸üĞÂ");
        }
    }
    pressreturn ();
}

int
wall ()
{
    if (!HAS_PERM (PERM_SYSOP))
        return 0;
    modify_user_mode (MSG);
    move (2, 0);
    clrtobot ();
    if (!get_msg ("ËùÓĞÊ¹ÓÃÕß", buf2, 1))
    {
        return 0;
    }
    if (apply_ulist (dowall) == -1)
    {
        move (2, 0);
        prints ("ÏßÉÏ¿ÕÎŞÒ»ÈË\n");
        pressanykey ();
    }
    prints ("\nÒÑ¾­¹ã²¥Íê±Ï...\n");
    pressanykey ();
    return 1;
}

int
setsystempasswd ()
{
    FILE *pass;
    char passbuf[20], prepass[20];

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
        return;
    if (strcmp (currentuser.userid, "SYSOP"))
    {
        clear ();
        move (10, 20);
        prints ("¶Ô²»Æğ£¬ÏµÍ³ÃÜÂëÖ»ÄÜÓÉ SYSOP ĞŞ¸Ä£¡");
        pressanykey ();
        return;
    }
    getdata (2, 0, "ÇëÊäÈëĞÂµÄÏµÍ³ÃÜÂë(Ö±½Ó»Ø³µÔòÈ¡ÏûÏµÍ³ÃÜÂë): ",
             passbuf, 19, NOECHO, YEA);
    if (passbuf[0] == '\0')
    {
        if (askyn ("ÄãÈ·¶¨ÒªÈ¡ÏûÏµÍ³ÃÜÂëÂğ?", NA, NA) == YEA)
        {
            unlink ("etc/.syspasswd");
            securityreport ("[32mÈ¡ÏûÏµÍ³ÃÜÂë[37m");
        }
        return;
    }
    getdata (3, 0, "È·ÈÏĞÂµÄÏµÍ³ÃÜÂë: ", prepass, 19, NOECHO, YEA);
    if (strcmp (passbuf, prepass))
    {
        move (4, 0);
        prints ("Á½´ÎÃÜÂë²»ÏàÍ¬, È¡Ïû´Ë´ÎÉè¶¨.");
        pressanykey ();
        return;
    }
    if ((pass = fopen ("etc/.syspasswd", "w")) == NULL)
    {
        move (4, 0);
        prints ("ÏµÍ³ÃÜÂëÎŞ·¨Éè¶¨....");
        pressanykey ();
        return;
    }
    fprintf (pass, "%s\n", genpasswd (passbuf));
    fclose (pass);
    move (4, 0);
    prints ("ÏµÍ³ÃÜÂëÉè¶¨Íê³É....");
    pressanykey ();
    return;
}

int
x_csh ()
{
    char buf[PASSLEN];
    int save_pager;
    int magic;

    if (!HAS_PERM (PERM_SYSOP))
    {
        return 0;
    }
    if (!check_systempasswd ())
    {
        return;
    }
    modify_user_mode (SYSINFO);
    clear ();
    getdata (1, 0, "ÇëÊäÈëÍ¨ĞĞ°µºÅ: ", buf, PASSLEN, NOECHO, YEA);
    if (*buf == '\0' || !checkpasswd (currentuser.passwd, buf))
    {
        prints ("\n\n°µºÅ²»ÕıÈ·, ²»ÄÜÖ´ĞĞ¡£\n");
        pressreturn ();
        clear ();
        return;
    }
    randomize ();
    magic = rand () % 1000;
    prints ("\nMagic Key: %d", magic * 5 - 2);
    getdata (4, 0, "Your Key : ", buf, PASSLEN, NOECHO, YEA);
    if (*buf == '\0' || !(atoi (buf) == magic))
    {
        securityreport ("Fail to shell out");
        prints ("\n\nKey ²»ÕıÈ·, ²»ÄÜÖ´ĞĞ¡£\n");
        pressreturn ();
        clear ();
        return;
    }
    securityreport ("Shell out");
    modify_user_mode (SYSINFO);
    clear ();
    refresh ();
    reset_tty ();
    save_pager = uinfo.pager;
    uinfo.pager = 0;
    update_utmp ();
    do_exec ("csh", NULL);
    restore_tty ();
    uinfo.pager = save_pager;
    update_utmp ();
    clear ();
    return 0;
}
#endif

//add 2002.5.26 by mudboy
//mudboy add this function
int
add_to_QZ (char *userid, int num)
{
    FILE *fp, *nfp;
    char t[256], tempbuf[256], fnnew[80], buf[1024];
    char *t1, *t2, *t3;
    char datafile[80] = "etc/DISTmaster.ini";
    int counts;

    if ((fp = fopen (datafile, "r")) == NULL)
        return -1;
    sprintf (fnnew, "%s.%d", datafile, getuid ());
    if ((nfp = fopen (fnnew, "w")) == NULL)
    {
        fclose (fp);
        return -1;
    }
    counts = 0;
    while (fgets (t, 255, fp) != NULL)
    {
        strcpy (tempbuf, t);
        t1 = strtok (t, " \t\r\n");
        if (t1 == NULL || t1[0] == '#')
        {
            fprintf (nfp, "%s", tempbuf);
            continue;
        }
        counts++;
        if (counts != num)
        {
            fprintf (nfp, "%s", tempbuf);
            continue;
        }
        if (!isalpha (t1[0]))
        {
            fprintf (nfp, "%s\n", userid);
            continue;
        }
        t2 = strtok (NULL, " \t\r\n");
        if (t2 == NULL)
        {
            sprintf (buf, "%s\t%s", t1, userid);
            fprintf (nfp, "%s\n", buf);
            continue;
        }
        t3 = strtok (NULL, " \t\r\n");
        if (t3 == NULL)
        {
            sprintf (buf, "%s\t%s\t%s", t1, t2, userid);
            fprintf (nfp, "%s\n", buf);
            continue;
        }
        else
            return -1;
    }
    fclose (fp);
    fclose (nfp);
    return (rename (fnnew, datafile) + 1);
}

//mudboy add this function
int
m_ordainQZ ()
{
    int id, i, j, counts;
    char num[2];
    char buf[5][STRLEN];
    char secu[STRLEN];
    char title[STRLEN], msg[1024];

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
        return;

    clear ();
    stand_title ("ÈÎÃüÇø³¤\n");
    clrtoeol ();
    if (!gettheuserid (2, "ÊäÈëÓûÈÎÃüµÄÊ¹ÓÃÕßÕÊºÅ: ", &id))
        return 0;
    if (!strcmp (lookupuser.userid, "guest"))
    {
        move (5, 0);
        prints ("Äã²»ÄÜÈÎÃü guest µ±Çø³¤");
        pressanykey ();
        clear ();
        return -1;
    }
    if (chkGroupsInfo () == 0)
        return -1;
    for (i = 1, j = 1; i < SectNum; i++)
    {
        if (GroupsInfo.name[i][0] == '\0')
            continue;
        move (j++, 60);
        prints ("©§(%d)%-12s\033[m©§", i, GroupsInfo.name[i]);
    }
    num[0] = 0;
    getdata (j, 0, "ÈÎÃüÄÄÒ»·ÖÇø(²Î¿¼ÓÒ±ßÌáÊ¾±àºÅ)£¿: ", num, 2, DOECHO, NA);

    if (num[0] >= 'a' && num[0] <= 'z')
        num[0] -= 32;		//Ğ¡Ğ´Í³Ò»µ½´óĞ´
    if (num[0] >= 'A' && num[0] <= 'Z')
        num[0] -= 7;		//×ÖÄ¸Í³Ò»µ½Êı×Ö
    if (num[0] == '\0' || num[0] < '1' || num[0] >= '0' + SectNum)
        return -1;
    if (GroupsInfo.name[num[0] - '0'][0] == '\0')
        return -1;
    counts = WHICHDIST (lookupuser.userid);
    if (counts < 12)
    {
        if(counts == 10)
            prints ("%s ÒÑ¾­ÊÇ A ÇøµÄÇø³¤ÁË.", lookupuser.userid);
        else if(counts == 11)
            prints ("%s ÒÑ¾­ÊÇ B ÇøµÄÇø³¤ÁË.", lookupuser.userid);
        else
            prints ("%s ÒÑ¾­ÊÇ %d ÇøµÄÇø³¤ÁË.", lookupuser.userid, counts);

        pressanykey ();
        clear ();
        return -1;
    }
    if(num[0] > '9')
    {
        num[0] += 7;
        prints ("\nÄã½«ÈÎÃü %s Îª %s ÇøµÄÇø³¤Âğ?\n", lookupuser.userid, num);
        num[0] -= 7;
    }
    else
        prints ("\nÄã½«ÈÎÃü %s Îª %s ÇøµÄÇø³¤Âğ?\n", lookupuser.userid, num);
    if (askyn ("ÄãÈ·¶¨ÒªÈÎÃüÂğ?", NA, NA) == NA)
    {
        prints ("È¡ÏûÈÎÃüÇø³¤");
        pressanykey ();
        clear ();
        return -1;
    }
    if (add_to_QZ (lookupuser.userid, num[0] - '0') == -1)
    {
        if(num[0] > '9')
        {
            num[0] += 7;
            prints ("%s Çø¼º¾­ÓĞÈı¸öÇø³¤ÁË.", num);
            num[0] -= 7;
        }
        else
            prints ("%s Çø¼º¾­ÓĞÈı¸öÇø³¤ÁË.", num);
        pressanykey ();
        clear ();
        return -1;
    }
    for (i = 0; i < 5; i++)
        buf[i][0] = '\0';

    clear ();
    move (8, 0);
    prints ("ÇëÊäÈëÈÎÃü¸½ÑÔ(×î¶àÎåĞĞ£¬°´ Enter ½áÊø)");
    for (i = 0; i < 5; i++)
    {
        getdata (i + 9, 0, ": ", buf[i], STRLEN - 5, DOECHO, YEA);
        if (buf[i][0] == '\0')
            break;
    }
    lookupuser.userlevel |= PERM_SPECIAL7;
    substitute_record (PASSFILE, &lookupuser, sizeof (struct userec), id);
    sprintf (secu, "Çø³¤ÈÎÃü, ¸øÓè %s Çø³¤È¨ÏŞ", lookupuser.userid);
    securityreport (secu);
    move (15, 0);
    prints (secu);

    if(num[0] > '9')
    {
        num[0] += 7;
        sprintf (title, "ÈÎÃü %s Îª %s ÇøÇø³¤", lookupuser.userid, num);
        sprintf (msg, "\n\t\t\t¡¾ Í¨¸æ ¡¿\n\n\tÈÎÃü %s Îª %s ÇøÇø³¤£¡\n", lookupuser.userid, num);
        num[0] -= 7;
    }
    else
    {
        sprintf (title, "ÈÎÃü %s Îª %s ÇøÇø³¤", lookupuser.userid, num);
        sprintf (msg, "\n\t\t\t¡¾ Í¨¸æ ¡¿\n\n\tÈÎÃü %s Îª %s ÇøÇø³¤£¡\n", lookupuser.userid, num);

    }
    for (i = 0; i < 5; i++)
    {
        if (buf[i][0] == '\0')
            break;
        if (i == 0)
            strcat (msg, "\n\nÕ¾³¤ÈÎÃü¸½ÑÔ£º\n");
        strcat (msg, buf[i]);
        strcat (msg, "\n");
    }
    strcpy (currboard, "announce");
    autoreport (title, msg, NULL, NULL);
    pressanykey ();
}

//mudboy add this function
int
del_from_QZ (char *userid, int num)
{
    FILE *fp, *nfp;
    char t[256], tempbuf[256], fnnew[STRLEN];
    char *t1, *t2, *t3;
    char datafile[80] = BBSHOME "/etc/DISTmaster.ini";
    int counts;

    if ((fp = fopen (datafile, "r")) == NULL)
        return -1;
    sprintf (fnnew, "%s.%d", datafile, getuid ());
    if ((nfp = fopen (fnnew, "w")) == NULL)
    {
        fclose (fp);
        return -1;
    }
    counts = 0;
    while (fgets (t, 255, fp) != NULL)
    {
        strcpy (tempbuf, t);
        t1 = strtok (t, " \t\r\n");
        if (t1 == NULL || t1[0] == '#')
        {
            fprintf (nfp, "%s", tempbuf);
            continue;
        }
        counts++;
        if (counts != num)
        {
            fprintf (nfp, "%s", tempbuf);
            continue;
        }
        if (!isalpha (t1[0]))
        {
            return -1;
        }
        if (strcasecmp (t1, userid) != 0)
            fprintf (nfp, "%s\t", t1);
        t2 = strtok (NULL, " \t\r\n");
        if (t2 == NULL)
        {
            if (strcasecmp (t1, userid) != 0)
                return -1;
            fprintf (nfp, "/%d\n", counts);
            continue;
        }
        if (strcasecmp (t2, userid) != 0)
            fprintf (nfp, "%s\t", t2);
        t3 = strtok (NULL, " \t\r\n");
        if (t3 == NULL)
        {
            if (strcasecmp (t1, userid) != 0 && strcasecmp (t2, userid) != 0)
                return -1;
            fprintf (nfp, "\n");
            continue;
        }
        if (strcasecmp (t3, userid) != 0)
            fprintf (nfp, "%s\n", t3);
        if (strcasecmp (t1, userid) != 0 && strcasecmp (t2, userid) != 0
                && strcasecmp (t2, userid) != 0)
            return -1;
    }
    fclose (fp);
    fclose (nfp);
    return (rename (fnnew, datafile) + 1);
}

//mudboy add this function
int
m_retireQZ ()
{
    int id, i, counts;
    char buf[5][STRLEN];
    char secu[STRLEN];
    char title[STRLEN], msg[1024];

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
        return;

    clear ();
    stand_title ("Çø³¤ÀëÈÎ\n");
    clrtoeol ();
    if (!gettheuserid (2, "ÊäÈëÓûÀëÈÎµÄÊ¹ÓÃÕßÕÊºÅ: ", &id))
        return 0;
    counts = WHICHDIST (lookupuser.userid);
    if (counts == 12)
    {
        prints ("%s ²»ÊÇÈÎºÎÒ»¸öÇøµÄÇø³¤ÁË.", lookupuser.userid);
        pressanykey ();
        clear ();
        return -1;
    }
    prints ("\nÄã½«È¡Ïû %s µÄ Çø³¤Ö°Îñ\n", lookupuser.userid);
    if (askyn ("ÄãÈ·¶¨ÒªÈ¡ÏûÂğ?", NA, NA) == NA)
    {
        prints ("È¡ÏûÇø³¤ÀëÈÎ");
        pressanykey ();
        clear ();
        return -1;
    }
    if (del_from_QZ (lookupuser.userid, counts) != 1)
    {
        prints ("ÀëÈÎ²Ù×÷·¢Éú´íÎó,Çë¼ì²éetc/DISTmaster.ini");
        pressanykey ();
        clear ();
        return -1;
    }
    for (i = 0; i < 5; i++)
        buf[i][0] = '\0';
    clear ();
    move (8, 0);
    prints ("ÇëÊäÈëÀëÈÎ¸½ÑÔ(×î¶àÎåĞĞ£¬°´ Enter ½áÊø)");
    for (i = 0; i < 5; i++)
    {
        getdata (i + 9, 0, ": ", buf[i], STRLEN - 5, DOECHO, YEA);
        if (buf[i][0] == '\0')
            break;
    }
    lookupuser.userlevel &= ~(PERM_SPECIAL7);
    substitute_record (PASSFILE, &lookupuser, sizeof (struct userec), id);
    sprintf (secu, "Çø³¤ÀëÈÎ, È¡Ïû %s Çø³¤È¨ÏŞ", lookupuser.userid);
    securityreport (secu);
    move (15, 0);
    prints (secu);

    if(counts == 10)
    {
        sprintf (title, "A ÇøÇø³¤ %s ÀëÈÎÍ¨¸æ", lookupuser.userid);
        sprintf (msg,
                 "\n\t\t\t¡¾ Í¨¸æ ¡¿\n\n\t¾­Õ¾Îñ×éÌÖÂÛ£º\n\tÍ¬Òâ %s ´ÇÈ¥ A ÇøÇø³¤µÄÖ°Îñ¡£\n\tÔÚ´Ë£¬¶ÔËûÔø¾­ÔÚ A ÇøµÄĞÁ¿àÀÍ×÷±íÊ¾¸ĞĞ»¡£\n\n",
                 lookupuser.userid);
    }

    else if(counts == 11)
    {
        sprintf (title, "B ÇøÇø³¤ %s ÀëÈÎÍ¨¸æ", lookupuser.userid);
        sprintf (msg,
                 "\n\t\t\t¡¾ Í¨¸æ ¡¿\n\n\t¾­Õ¾Îñ×éÌÖÂÛ£º\n\tÍ¬Òâ %s ´ÇÈ¥ B ÇøÇø³¤µÄÖ°Îñ¡£\n\tÔÚ´Ë£¬¶ÔËûÔø¾­ÔÚ B ÇøµÄĞÁ¿àÀÍ×÷±íÊ¾¸ĞĞ»¡£\n\n",
                 lookupuser.userid);
    }


    else
    {

        sprintf (title, "%d ÇøÇø³¤ %s ÀëÈÎÍ¨¸æ", counts, lookupuser.userid);
        sprintf (msg,
                 "\n\t\t\t¡¾ Í¨¸æ ¡¿\n\n\t¾­Õ¾Îñ×éÌÖÂÛ£º\n\tÍ¬Òâ %s ´ÇÈ¥ %d ÇøÇø³¤µÄÖ°Îñ¡£\n\tÔÚ´Ë£¬¶ÔËûÔø¾­ÔÚ %d ÇøµÄĞÁ¿àÀÍ×÷±íÊ¾¸ĞĞ»¡£\n\n",
                 lookupuser.userid, counts, counts);
    }


    for (i = 0; i < 5; i++)
    {
        if (buf[i][0] == '\0')
            break;
        if (i == 0)
            strcat (msg, "\n\nÕ¾³¤¸½ÑÔ:\n");
        strcat (msg, buf[i]);
        strcat (msg, "\n");
    }
    strcpy (currboard, "announce");
    autoreport (title, msg, NULL, NULL);
    pressanykey ();
}

//add end 2002.5.26 by mudboy

