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

#ifdef WITHOUT_ADMIN_TOOLS
#define kick_user
#endif

void mail_info(char *lastword);

void offline(void)
{
    int i;
    char buf[STRLEN], lastword[640];

    modify_user_mode(OFFLINE);
    clear();
    if (HAS_PERM(PERM_SYSOP) || HAS_PERM(PERM_BOARDS)
        || HAS_PERM(PERM_BLEVELS)
        || HAS_PERM(PERM_ADMINMENU) || HAS_PERM(PERM_SEEULEVELS)
        || HAS_PERM(PERM_XEMPT) || HAS_PERM(PERM_SPECIAL6)
        || HAS_PERM(PERM_SPECIAL7) || HAS_PERM(PERM_SPECIAL8))
    {
        move(1, 0);
        prints("\n\nƒ˙”–÷ÿ»Œ‘⁄…Ì, ≤ªƒ‹ÀÊ±„◊‘…±¿≤!!\n");
        pressreturn();
        clear();
        return;
    }
    if (currentuser.stay < 86400)
    {
        move(1, 0);
        prints("\n\n∂‘≤ª∆, ƒ˙ªπŒ¥πª◊ ∏Ò÷¥––¥À√¸¡Ó!!\n");
        prints("«Î mail ∏¯ SYSOP Àµ√˜◊‘…±‘≠“Ú, –ª–ª°£\n");
        pressreturn();
        clear();
        return;
    }
    getdata(1, 0, "«Î ‰»Îƒ„µƒ√‹¬Î: ", buf, PASSLEN, NOECHO, YEA);
    if (*buf == '\0' || !checkpasswd(currentuser.passwd, buf))
    {
        prints("\n\n∫‹±ß«∏, ƒ˙ ‰»Îµƒ√‹¬Î≤ª’˝»∑°£\n");
        pressreturn();
        clear();
        return;
    }
    getdata(3, 0, "«ÎŒ ƒ„Ω– ≤˜·√˚◊÷? ", buf, NAMELEN, DOECHO, YEA);
    if (*buf == '\0' || strcmp(buf, currentuser.realname))
    {
        prints("\n\n∫‹±ß«∏, Œ“≤¢≤ª»œ ∂ƒ„°£\n");
        pressreturn();
        clear();
        return;
    }
    clear();
    move(1, 0);
    prints
        ("[1;5;31mæØ∏Ê[0;1;31m£∫ ◊‘…±··, ƒ˙Ω´Œﬁ∑®‘Ÿ”√¥À’ ∫≈Ω¯»Î±æ’æ£°£°");
    move(3, 0);
    prints("[1;32mµ´’ ∫≈“™‘⁄ 30 ÃÏ··≤≈ª·…æ≥˝°£∫√ƒ—π˝‡∏ :( .....[m");
    move(5, 0);
    i = 0;
    if (askyn("’Ê «…·≤ªµ√ƒ„£¨ƒ„◊ﬂ÷Æ«∞”– ≤√¥ª∞œÎÀµ√¥", YEA, NA) == YEA)
    {
        strcpy(lastword, ">\n>");
        buf[0] = '\0';
        for (i = 0; i < 8; i++)
        {
            getdata(i + 6, 0, ": ", buf, 77, DOECHO, YEA);
            if (buf[0] == '\0')
                break;
            strcat(lastword, buf);
            strcat(lastword, "\n>");
        }
        if (i == 0)
            lastword[0] = '\0';
        else
            strcat(lastword, "\n\n");
        move(i + 8, 0);
        if (i == 0)
            prints("∞•£¨ƒ„ªπ « ≤√¥∂º≤ª‘∏“‚Àµ£¨ «≤ª «ªπ”––ƒÀºŒ¥¡À£ø");
        else if (i <= 4)
            prints("ø¥◊≈ƒ„„æ„≤µƒ¡≥£¨Œ“–ƒ∂ºÀÈ¡À ... ");
        else
            prints
                ("Œ“ª·º«µ√ƒ„µƒ£¨≈Û”—£¨Œ““≤÷™µ¿ƒ„µƒ¿Îø™“≤ «√ª”–∞Ï∑®µƒ ¬£¨∫√◊ﬂ¡À");
    }
    move(i + 10, 0);
    if (askyn("ƒ„»∑∂®“™¿Îø™’‚∏ˆ¥Ûº“Õ•", NA, NA) == 1)
    {
        clear();
        currentuser.userlevel = 0;
        substitute_record(PASSFILE, &currentuser, sizeof(struct userec),
                          usernum);
        mail_info(lastword);
        modify_user_mode(OFFLINE);
        kick_user(&uinfo);
        exit(0);
    }
}

void getuinfo(FILE * fn)
{
    int num;
    char buf[40];

    fprintf(fn, "\n\nÀ˚µƒ¥˙∫≈     : %s\n", currentuser.userid);
    fprintf(fn, "À˚µƒÍ«≥∆     : %s\n", currentuser.username);
    fprintf(fn, "’Ê µ–’√˚     : %s\n", currentuser.realname);
    fprintf(fn, "æ”◊°◊°÷∑     : %s\n", currentuser.address);
    fprintf(fn, "µÁ◊”” º˛–≈œ‰ : %s\n", currentuser.email);
    fprintf(fn, "’Ê µ E-mail  : %s\n", currentuser.reginfo);
    fprintf(fn, "Ident ◊ ¡œ   : %s\n", currentuser.ident);
    getdatestring(currentuser.firstlogin, NA);
    fprintf(fn, "’ ∫≈Ω®¡¢»’∆⁄ : %s\n", datestring);
    getdatestring(currentuser.lastlogin, NA);
    fprintf(fn, "◊ÓΩ¸π‚¡Ÿ»’∆⁄ : %s\n", datestring);
    fprintf(fn, "◊ÓΩ¸π‚¡Ÿª˙∆˜ : %s\n", currentuser.lasthost);
    fprintf(fn, "…œ’æ¥Œ ˝     : %d ¥Œ\n", currentuser.numlogins);
    fprintf(fn, "Œƒ’¬ ˝ƒø     : %d\n", currentuser.numposts);
    fprintf(fn, "…œ’æ◊‹ ± ˝   : %ld –° ± %ld ∑÷÷”\n",
            currentuser.stay / 3600, (currentuser.stay / 60) % 60);
    strcpy(buf, "bTCPRp#@XWBA#VS-DOM-F0123456789t");
    for (num = 0; num < 30; num++)
        if (!(currentuser.userlevel & (1 << num)))
            buf[num] = '-';
    buf[num] = '\0';
    fprintf(fn, " π”√’ﬂ»®œﬁ   : %s\n\n", buf);
}

void mail_info(char *lastword)
{
    FILE *fn;
    time_t now;
    char filename[STRLEN];

    now = time(0);
    getdatestring(now, NA);
    sprintf(filename, "tmp/suicide.%s", currentuser.userid);
    if ((fn = fopen(filename, "w")) != NULL)
    {
        fprintf(fn,
                "[1m%s[m “—æ≠‘⁄ [1m%s[m µ«º«◊‘…±¡À£¨“‘œ¬ «À˚µƒ◊ ¡œ£¨«Î±£¡Ù...",
                currentuser.userid, datestring);
        getuinfo(fn);
        fclose(fn);
        Postfile(filename, "syssecurity", "µ«º«◊‘…±Õ®÷™(30ÃÏ··…˙–ß)...",
                 2);
        unlink(filename);
    }
    if ((fn = fopen(filename, "w")) != NULL)
    {
        fprintf(fn, "¥Ûº“∫√,\n\n");
        fprintf(fn, "Œ“ « %s (%s)°£Œ“º∫æ≠æˆ∂®‘⁄ 30 ÃÏ∫Û¿Îø™’‚¿Ô¡À°£\n\n",
                currentuser.userid, currentuser.username);
        getdatestring(currentuser.firstlogin, NA);
        fprintf(fn,
                "◊‘ %14.14s ÷¡ΩÒ£¨Œ““—æ≠¿¥¥À %d ¥Œ¡À£¨‘⁄’‚◊‹º∆ %ld ∑÷÷”µƒÕ¯¬Á…˙√¸÷–£¨\n",
                datestring, currentuser.numlogins, currentuser.stay / 60);
        fprintf(fn,
                "Œ“”÷»Á∫Œª·«·“◊…·∆˙ƒÿ£øµ´ «Œ“µ√◊ﬂ¡À...  µ„µ„µŒµŒ£≠£≠æ°‘⁄Œ“–ƒ÷–£°\n\n");
        fprintf(fn, "%s", lastword);
        fprintf(fn,
                "≈Û”—√«£¨«Î∞— %s ¥”ƒ„√«µƒ∫√”—√˚µ•÷–ƒ√µÙ∞…°£“ÚŒ™Œ“º∫æ≠æˆ∂®¿Îø™’‚¿Ô¡À!\n\n",
                currentuser.userid);
        fprintf(fn, "ªÚ–Ì”–≥Ø“ª»’Œ“ª·ªÿ¿¥µƒ°£ ’‰÷ÿ!! ‘Ÿº˚!!\n\n\n");
        getdatestring(now, NA);
        fprintf(fn, "%s ”⁄ %s ¡Ù.\n\n", currentuser.userid, datestring);
        fclose(fn);
        Postfile(filename, "message", "«·«·µƒŒ“◊ﬂ¡À...", 2);
        unlink(filename);
    }
}
