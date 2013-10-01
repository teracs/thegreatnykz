#include "www.h"

static void show_honor(struct userec *lookupuser)
{
    FILE *fp;
    char line[STRLEN], *honortype, *userid, *honor;

    fp = fopen("etc/honors", "r");
    if (fp == NULL)
        return;
    while (fgets(line, STRLEN, fp))
    {
        honortype = strtok(line, " \t\r\n");
        userid = strtok(NULL, " \t\r\n");
        honor = strtok(NULL, " \t\r\n");
        if (honortype == NULL || userid == NULL || honor == NULL || honortype[0] == '#')
            continue;

        if (!strcmp(userid, lookupuser->userid))
        {
            switch (honortype[0])
            {
            case 'H':
                printf("»Ÿ”˛:");
                break;
            case 'T':
                printf("÷∞ŒÒ:");
                break;
            }
            
            printf(" [<font class=c32>%s</font><font class=c37>]\n", honor);
            fclose(fp);
            return;
        }
    }
    fclose(fp);
}

static void show_user_title(struct userec *lookupuser)
{
    if (lookupuser->userlevel & PERM_SYSOP)
    {
        if (lookupuser->userlevel & PERM_HONOR)
            show_honor(lookupuser);
        else
            printf("÷∞ŒÒ:[<a href=bbsdoc?board=sysop><font class=c32>±æ’æ’æ≥§</a><font class=c37>]\n");
    }
    else if (lookupuser->userlevel & PERM_HASTITLE || lookupuser->userlevel & PERM_HONOR)
    {
        if (lookupuser->userlevel & PERM_HASTITLE)
        {
            printf("÷∞ŒÒ:");
            
            if (lookupuser->userlevel & (PERM_SPECIAL8 | PERM_OBOARDS))
                printf(" [<a href=bbsdoc?board=sysop><font class=c32>’æŒÒ÷˙¿Ì</a><font class=c37>]");
            if (lookupuser->userlevel & PERM_ACCOUNTS)
                printf(" [<a href=bbsdoc?board=message><font class=c32>’À∫≈π‹¿Ì‘±</a><font class=c37>]");
            if (lookupuser->userlevel & PERM_OVOTE)
                printf(" [<a href=bbsdoc?board=vote><font class=c32>Õ∂∆±π‹¿Ì‘±</a><font class=c37>]");
            if (lookupuser->userlevel & PERM_ACHATROOM)
                printf(" [<font class=c32>ƒœ—Û◊‹π‹<font class=c37>]");
            if (lookupuser->userlevel & PERM_ACBOARD)
                printf(" [<a href=bbsdoc?board=Bulletin><font class=c32>√¿π§◊‹π‹</a><font class=c37>]");
            if (lookupuser->userlevel & PERM_CHATCLOAK)
                printf(" [<font class=c32>±æ’æ÷«ƒ“<font class=c37>]");
            else if (lookupuser->userlevel & PERM_SPECIAL6)
                printf(" [<a href=bbsdoc?board=BBSdev><font class=c32>ºº ı÷«ƒ“</a><font class=c37>]");
            if (lookupuser->userlevel & PERM_ARBITRATOR)
                printf(" [<font class=c32>±æ’æ÷Ÿ≤√<font class=c37>]");
            if ((lookupuser->userlevel & PERM_SPECIAL7)
                    && !(lookupuser->userlevel & PERM_OBOARDS))
                show_dm(lookupuser->userid);
            if (lookupuser->userlevel & PERM_BOARDS)
                show_bm(toupper(lookupuser->userid[0]), lookupuser->userid);

            printf(" ");
        }

        if (lookupuser->userlevel & PERM_HONOR)
            show_honor(lookupuser);
        else
            printf("\n");
    }
}

static void show_special(char *id2)
{
    FILE *fp;
    char id1[80], name[80];

    fp = fopen("etc/sysops", "r");
    if (fp == 0)
        return;
    while (1)
    {
        id1[0] = 0;
        name[0] = 0;
        if (fscanf(FCGI_ToFILE(fp), "%s %s", id1, name) <= 0)
            break;
        if (!strcmp(id1, id2))
            hprintf(" [1;31m°Ô[0;36m%s[1;31m°Ô[m", name);
    }
    fclose(fp);
}

int bbsqry_main()
{
    FILE *fp;
    char userid[14], filename[80], buf[512];
    struct userec *x;
    struct user_info *u;
    int i, tmp1, tmp2, num, clr;

    cache_age = 30 * 60;
    init_all();
    strsncpy(userid, getparm("userid"), 13);
    printf("<center>");
    printf("<hr>\n");
    if (userid[0] == 0)
    {
        printf("<form action=bbsqry>\n");
        printf
            ("«Î ‰»Î“™≤È—Øµƒ”√ªß√˚: <input name=userid maxlength=12 size=12> ");
        printf("<input type=submit value=»∑∂®≤È—Ø>\n");
        printf("</form><hr>\n");
        http_quit();
    }
    x = getuser(userid);
    if (x == 0)
    {
        printf("”√ªß [%s] ≤ª¥Ê‘⁄.", nohtml4(userid));
        http_quit();
    }
    printf("<table class=show_border width=610>\n");
    printf("<tr><td><pre class=tight>\n");
    sprintf(buf,
            "%s ([33m%s[37m) π≤…œ’æ [32m%d[m ¥Œ  Õ¯¡‰[[32m%ld[m]ÃÏ  ",
            x->userid, x->username, x->numlogins,
            (time(0) - x->firstlogin) / 86400);
    hprintf("%s", buf);
    show_special(x->userid);
    if (x->userdefine & DEF_COLOREDSEX)
    {
        clr = (x->gender == 'F') ? 35 : 36;
    }
    else
    {
        clr = 32;
    }
    if (x->userdefine & DEF_S_HOROSCOPE)
        hprintf("[[1;%dm%s[m]", clr,
                horoscope(x->birthmonth, x->birthday));
    printf("\n");
    getdatestring(x->lastlogin, NA);
    hprintf("…œ ¥Œ ‘⁄: [[1;32m%s[m] ¥” [[1;32m%s[m] µΩ±æ’æ“ª”Œ°£\n",
            datestring, x->lasthost);
    count_mails(x->userid, &tmp1, &tmp2);

    num = 0;
    for (i = 0; i < MAXACTIVE; i++)
    {
        u = &(shm_utmp->uinfo[i]);
        if (!strcmp(u->userid, x->userid))
        {
            if (u->active == 0 || u->pid == 0 || (u->invisible
                                                  &&
                                                  !HAS_PERM
                                                  (PERM_SEECLOAK)))
                continue;
            num++;
            if (num == 1)
            {
                loadfriend(u->userid);
                hprintf
                    ("ƒø«∞‘⁄œﬂ£∫[[1;32m—∂œ¢∆˜£∫([36m%s[32m) ∫ÙΩ–∆˜£∫([36m%s[32m)[m] ",
                     canmsg(u) ? "¥Úø™" : "πÿ±’",
                     canpage(isfriend(x->userid),
                             u->pager) ? "¥Úø™" : "πÿ±’");
                hprintf("…˙√¸¡¶:[[32m%d[37m] ", count_life_value(x));
                hprintf("Œƒ’¬:[[32m%d[37m] ", x->numposts);
                hprintf("–≈œ‰:[[32m%s[37m]\n", tmp2 ? "°—" : "  ");
                show_user_title(x);
                hprintf("ƒø«∞ %s ◊¥Ã¨»Áœ¬£∫\n", x->userid);
            }
            if (u->invisible)
                hprintf("[36mC[37m");
            hprintf("[32m%s[m ", ModeType(u->mode));
            if (num % 5 == 0)
                printf("\n");
        }
    }
    if (num == 0)
    {
        sprintf(filename, "home/%c/%c/%s", toupper(x->userid[0]),
                toupper(x->userid[1]), x->userid);
        getdatestring(x->lastlogout >=
                      x->lastlogin ? x->
                      lastlogout : ((time(0) - x->lastlogin) / 120) % 47 +
                      1 + x->lastlogin, NA);
        hprintf("¿Î’æ ±º‰: [[1;32m%s[m] ", datestring);
        hprintf("…˙√¸¡¶:[[32m%d[37m] ", count_life_value(x));
        hprintf("Œƒ’¬:[[32m%d[37m] ", x->numposts);
        hprintf("–≈œ‰:[[32m%s[37m]\n", tmp2 ? "°—" : "  ");
        show_user_title(x);
    }

    printf("\n\n");

    sprintf(filename, "home/%c/%c/%s/plans", toupper(x->userid[0]),
            toupper(x->userid[1]), x->userid);
    fp = fopen(filename, "r");
    if (fp)
    {
        while (1)
        {
            if (fgets(buf, 256, fp) == 0)
                break;
            hhprintf("%s", buf);
        }
        fclose(fp);
    }
    else
    {
        hprintf("[36m√ª”–∏ˆ»ÀÀµ√˜µµ[37m\n");
    }
    printf("</pre>");
    printf("</table>\n");
    printf("<hr><a href=bbspstmail?userid=%s&title=√ª÷˜Ã‚>[–¥–≈Œ ∫Ú]</a> ",
           x->userid);
    printf("<a href=bbssendmsg?destid=%s>[∑¢ÀÕ—∂œ¢]</a> ", x->userid);
    printf("<a href=bbsfadd?userid=%s>[º”»Î∫√”—]</a> ", x->userid);
    printf("<a href=bbsfdel?userid=%s>[…æ≥˝∫√”—]</a> ", x->userid);
    if (loginok)
        printf("<a href=\"/wiki/%s\">[ÀÆ‘¥∞Ÿø∆“≥√Ê]</a>", x->userid);

    printf("</center>\n");
    printf("</html>");
    return 0;
}
