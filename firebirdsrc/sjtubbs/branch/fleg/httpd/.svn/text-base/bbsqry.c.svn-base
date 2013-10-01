#include "www.h"

int IsGoodBm(char* id)
{
    return file_has_word("etc/goodbm", id);
}

int bbsqry_main()
{
    FILE *fp;
    char userid[14], filename[80], buf[512];
    struct userec *x;
    struct user_info *u;
    int i, tmp1, tmp2, num, clr;

    init_all();
    strsncpy(userid, getparm("userid"), 13);
    printf("<center>");
    printf("<hr color=green>\n");
    if (userid[0] == 0)
    {
        printf("<form action=bbsqry>\n");
        printf
        ("ÇëÊäÈëÒª²éÑ¯µÄÓÃ»§Ãû: <input name=userid maxlength=12 size=12> ");
        printf("<input type=submit value=È·¶¨²éÑ¯>\n");
        printf("</form><hr>\n");
        http_quit();
    }
    x = getuser(userid);
    if (x == 0)
    {
        printf("ÓÃ»§ [%s] ²»´æÔÚ.", userid);
        http_quit();
    }
    printf
    ("<table width=610 style='BORDER: 2px solid; BORDER-COLOR: D0F0C0;'>\n");
    printf("<tr><td><pre>\n");
    sprintf(buf,
            "%s ([33m%s[37m) ¹²ÉÏÕ¾ [32m%d[m ´Î£¬·¢±íÎÄÕÂ [32m%d[m Æª",
            x->userid, x->username, x->numlogins, x->numposts);
    hprintf("%s", buf);
    show_special(x->userid);
    printf("\n");
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
    hprintf("ÉÏ´ÎÔÚ [[32m%s[37m] ´Ó [[32m%s[37m] µ½±¾Õ¾Ò»ÓÎ¡£\n",
            Ctime(x->lastlogin), x->lasthost);
    count_mails(x->userid, &tmp1, &tmp2);
    hprintf("ĞÅÏä£º[[32m%s[37m]  ", tmp2 ? "¡Ñ" : "  ");
    /*
       if(x->user_exp_type!=3)
       hprintf("¾­ÑéÖµ£º[[32m%d[37m]([33m%s[37m) ", countexp(x), cexp(countexp(x), x->user_exp_type));
       else
       hprintf("¾­ÑéÖµ£º([33m²»¸æËßÄã[37m) ");
       hprintf("±íÏÖÖµ£º[[32m%d[37m]([33m%s[37m) ", countperf(x), cperf(countperf(x)));
     */
    hprintf("ÉúÃüÁ¦£º[[32m%d[37m]¡£\n", count_life_value(x));
    num = 0;
    show_duty(x);
    for (i = 0; i < MAXACTIVE; i++)
    {
        u = &(shm_utmp->uinfo[i]);
        if (!strcmp(u->userid, x->userid))
        {
            if (u->active == 0 || u->pid == 0 || u->invisible
                    && !HAS_PERM(PERM_SEECLOAK))
                continue;
            num++;
            if (num == 1)
                hprintf("Ä¿Ç°ÔÚÕ¾ÉÏ, ×´Ì¬ÈçÏÂ:\n");
            if (u->invisible)
                hprintf("[36mC[37m");
            hprintf("[32m%s[m ", ModeType(u->mode));
            if (num % 5 == 0)
                printf("\n");
            if (!strcmp(currentuser.userid, "SYSOP"))
            {
                //int bnum=u->board;
                //      if(bnum) printf("(%s)", shm_bcache->bcache[bnum-1].filename);
            }
        }
    }
    if (num == 0)
    {
        sprintf(filename, "home/%c/%s", toupper(x->userid[0]), x->userid);
        hprintf("Ä¿Ç°²»ÔÚÕ¾ÉÏ, ÉÏ´ÎÀëÕ¾Ê±¼ä [[1;32m%s[m]\n\n",
                (x->lastlogout >=
                 x->lastlogin) ? Ctime(x->lastlogout) : Ctime(((time(0) - x->lastlogin) / 120) % 47 + 1 + x->lastlogin));
    }
    printf("\n");
    sprintf(filename, "home/%c/%s/plans", toupper(x->userid[0]),
            x->userid);
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
        hprintf("[36mÃ»ÓĞ¸öÈËËµÃ÷µµ[37m\n");
    }
    printf("</pre>");
    printf("</table>\n");
    printf("<br><a href=bbspstmail?userid=%s&title=Ã»Ö÷Ìâ>[Ğ´ĞÅÎÊºò]</a> ",
           x->userid);
    printf("<a href=bbssendmsg?destid=%s>[·¢ËÍÑ¶Ï¢]</a> ", x->userid);
    printf("<a href=bbsfadd?userid=%s>[¼ÓÈëºÃÓÑ]</a> ", x->userid);
    printf("<a href=bbsfdel?userid=%s>[É¾³ıºÃÓÑ]</a>", x->userid);
    printf("</center>\n");
    http_quit();
}

int show_special(char *id2)
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
        if (fscanf(fp, "%s %s", id1, name) <= 0)
            break;
        if (!strcmp(id1, id2))
            hprintf(" [1;31m¡ï[0;36m%s[1;31m¡ï[m", name);
    }
    fclose(fp);
}

//added by littlesword@SJTUbbs
//show_duty copy from fb2k

void show_duty(struct userec* lookupuser)
{
	    /************ÒÔÏÂÊÇÏÔÊ¾Ö°ÎñµÄ´úÂë add by jieer 2001.2.15  ******/
    if ((lookupuser->userlevel & PERM_SYSOP))
        printf ("Ö°Îñ:[<font class=c32>±¾Õ¾Õ¾³¤<font class=c37>]\n");
    /*	else if((lookupuser->userlevel & PERM_SPECIAL8))
    		printf("ÏÖÒÑ¹éÒşÌïÔ°");
    		*/
    //////////////////////////////////////////////////////////////////////////

    else if (lookupuser->userlevel & PERM_SPECIAL8)
    {
        printf("Ö°Îñ:[<font class=c32>Õ¾ÎñÖúÀí<font class=c37>]");
        if(lookupuser->userlevel & PERM_ACCOUNTS)
            printf (" [<font class=c32>ÕËºÅ¹ÜÀíÔ±<font class=c37>]");
        if ((lookupuser->userlevel & PERM_ANNOUNCE) ||
                (lookupuser->userlevel & PERM_OBOARDS) ||
                (lookupuser->userlevel & PERM_OVOTE))
            printf (" [<font class=c32>±¾Õ¾×Ü¹Ü<font class=c37>]");
        if (lookupuser->userlevel & PERM_ACHATROOM)
            printf (" [<font class=c32>ÄÏÑó×Ü¹Ü<font class=c37>]");
        if (lookupuser->userlevel & PERM_ACBOARD)
            printf (" [<font class=c32>ÃÀ¹¤×Ü¹Ü<font class=c37>]");
        if (lookupuser->userlevel & PERM_CHATCLOAK)
            printf (" [<font class=c32>±¾Õ¾ÖÇÄÒ<font class=c37>]");
		if (lookupuser->userlevel & PERM_SPECIAL6)
			printf (" [<font class=c32>¼¼ÊõÖÇÄÒ<font class=c37>]");
		if (lookupuser->userlevel & PERM_XEMPT)
		{
			if(IsGoodBm(lookupuser->userid))
				printf (" [<font class=c32>ÓÅĞã°åÖ÷<font class=c37>]");
		}
        if (lookupuser->userlevel & PERM_BOARDS)
            show_bm(toupper(lookupuser->userid[0]),lookupuser->userid);
        printf ("\n");
    }

    //////////////////////////////////////////////////////////////////////////

    else if ((lookupuser->userlevel & PERM_ACCOUNTS))
    {
        printf ("Ö°Îñ:[<font class=c32>ÕËºÅ¹ÜÀíÔ±<font class=c37>]");
        if ((lookupuser->userlevel & PERM_ANNOUNCE) ||
                (lookupuser->userlevel & PERM_OBOARDS) ||
                (lookupuser->userlevel & PERM_OVOTE))
            printf (" [<font class=c32>±¾Õ¾×Ü¹Ü<font class=c37>]");
        if (lookupuser->userlevel & PERM_ACHATROOM)
            printf (" [<font class=c32>ÄÏÑó×Ü¹Ü<font class=c37>]");
        if (lookupuser->userlevel & PERM_ACBOARD)
            printf (" [<font class=c32>ÃÀ¹¤×Ü¹Ü<font class=c37>]");
        if (lookupuser->userlevel & PERM_CHATCLOAK)
            printf (" [<font class=c32>±¾Õ¾ÖÇÄÒ<font class=c37>]");
		if (lookupuser->userlevel & PERM_SPECIAL6)
			printf (" [<font class=c32>¼¼ÊõÖÇÄÒ<font class=c37>]");
        if ((lookupuser->userlevel & PERM_SPECIAL7) && !(lookupuser->userlevel & PERM_OBOARDS))
			//show_dm(toupper(lookupuser->userid[0]),lookupuser->userid);
			show_dm( lookupuser->userid );

		if (lookupuser->userlevel & PERM_XEMPT)
		{
			if(IsGoodBm(lookupuser->userid))
				printf (" [<font class=c32>ÓÅĞã°åÖ÷<font class=c37>]");
		}
        if (lookupuser->userlevel & PERM_BOARDS)
            show_bm(toupper(lookupuser->userid[0]),lookupuser->userid);
        printf ("\n");
    }
    else if ((lookupuser->userlevel & PERM_ANNOUNCE) ||
             (lookupuser->userlevel & PERM_OBOARDS) ||
             (lookupuser->userlevel & PERM_OVOTE))
    {
        printf ("Ö°Îñ:[<font class=c32>±¾Õ¾×Ü¹Ü<font class=c37>]");
        if (lookupuser->userlevel & PERM_CHATCLOAK)
            printf (" [<font class=c32>±¾Õ¾ÖÇÄÒ<font class=c37>]");
		if (lookupuser->userlevel & PERM_SPECIAL6)
			printf (" [<font class=c32>¼¼ÊõÖÇÄÒ<font class=c37>]");
        if ((lookupuser->userlevel & PERM_SPECIAL7) && !(lookupuser->userlevel & PERM_OBOARDS))
			show_dm( lookupuser->userid  );
			//show_dm(toupper(lookupuser->userid[0]),lookupuser->userid);
            /*printf (" [\033[1;32m±¾Õ¾Çø³¤]");*/
		if (lookupuser->userlevel & PERM_XEMPT)
		{
			if(IsGoodBm(lookupuser->userid))
				printf (" [<font class=c32>ÓÅĞã°åÖ÷<font class=c37>]");
		}
        if (lookupuser->userlevel & PERM_BOARDS)
            show_bm(toupper(lookupuser->userid[0]),lookupuser->userid);
        printf ("\n");
    }
    else if (lookupuser->userlevel & PERM_ACHATROOM)
    {
        printf ("Ö°Îñ:[<font class=c32>ÄÏÑó×Ü¹Ü<font class=c37>]");
		if (lookupuser->userlevel & PERM_SPECIAL6)
			printf (" [<font class=c32>¼¼ÊõÖÇÄÒ<font class=c37>]");
        if ((lookupuser->userlevel & PERM_SPECIAL7) && !(lookupuser->userlevel & PERM_OBOARDS))
			show_dm( lookupuser->userid  );
			//show_dm(toupper(lookupuser->userid[0]),lookupuser->userid);
            /*printf (" [\033[1;32m±¾Õ¾Çø³¤\033[0;1m]");*/
		if (lookupuser->userlevel & PERM_XEMPT)
		{
			if(IsGoodBm(lookupuser->userid))
				printf (" [<font class=c32>ÓÅĞã°åÖ÷<font class=c37>]");
		}
        if (lookupuser->userlevel & PERM_BOARDS)
            show_bm(toupper(lookupuser->userid[0]),lookupuser->userid);
        printf ("\n");
    }
    else if (lookupuser->userlevel & PERM_ACBOARD)
    {
        printf ("Ö°Îñ:[<font class=c32>ÃÀ¹¤×Ü¹Ü<font class=c37>]");
		if (lookupuser->userlevel & PERM_SPECIAL6)
			printf (" [<font class=c32>¼¼ÊõÖÇÄÒ<font class=c37>]");
        if ((lookupuser->userlevel & PERM_SPECIAL7) && !(lookupuser->userlevel & PERM_OBOARDS))
			show_dm( lookupuser->userid  );
			//show_dm(toupper(lookupuser->userid[0]),lookupuser->userid);
            /*printf (" [\033[1;32m±¾Õ¾Çø³¤\033[0;1m]");*/
		if (lookupuser->userlevel & PERM_XEMPT)
		{
			if(IsGoodBm(lookupuser->userid))
				printf (" [<font class=c32>ÓÅĞã°åÖ÷<font class=c37>]");
		}
        if (lookupuser->userlevel & PERM_BOARDS)
            show_bm(toupper(lookupuser->userid[0]),lookupuser->userid);
        printf ("\n");
    }
    else if (lookupuser->userlevel & PERM_CHATCLOAK)
    {
        printf ("Ö°Îñ:[<font class=c32>±¾Õ¾ÖÇÄÒ<font class=c37>]");
        if ((lookupuser->userlevel & PERM_SPECIAL7) && !(lookupuser->userlevel & PERM_OBOARDS))
			show_dm( lookupuser->userid  );
			//show_dm(toupper(lookupuser->userid[0]),lookupuser->userid);
            /*printf (" [\033[1;32m±¾Õ¾Çø³¤\033[0;1m]");*/

		if (lookupuser->userlevel & PERM_XEMPT)
		{
			if(IsGoodBm(lookupuser->userid))
				printf (" [<font class=c32>ÓÅĞã°åÖ÷<font class=c37>]");
		}
        if (lookupuser->userlevel & PERM_BOARDS)
            show_bm(toupper(lookupuser->userid[0]),lookupuser->userid);
        printf ("\n");
    }
    else if (lookupuser->userlevel & PERM_SPECIAL6)
    {
        printf ("Ö°Îñ:[<font class=c32>¼¼ÊõÖÇÄÒ<font class=c37>]");
		if (lookupuser->userlevel & PERM_XEMPT)
		{
			if(IsGoodBm(lookupuser->userid))
				printf (" [<font class=c32>ÓÅĞã°åÖ÷<font class=c37>]");
		}
        if (lookupuser->userlevel & PERM_BOARDS)
            show_bm(toupper(lookupuser->userid[0]),lookupuser->userid);
        printf ("\n");
    }
    //modify by mudboy 2002.6.16 display QZ when query
    else if ((lookupuser->userlevel & PERM_SPECIAL7) && !(lookupuser->userlevel & PERM_OBOARDS))
    {
		printf ("Ö°Îñ:");
		show_dm( lookupuser->userid );
        //show_dm(toupper(lookupuser->userid[0]),lookupuser->userid);
		/*printf ("Ö°Îñ:\033[0;1m[\033[1;32m±¾Õ¾Çø³¤\033[0;1m]");*/
		if (lookupuser->userlevel & PERM_XEMPT)
		{
			if(IsGoodBm(lookupuser->userid))
				printf (" [<font class=c32>ÓÅĞã°åÖ÷<font class=c37>]");
		}
        if (lookupuser->userlevel & PERM_BOARDS)
            show_bm(toupper(lookupuser->userid[0]),lookupuser->userid);
        printf ("\n");
    }
    else if(lookupuser->userlevel & PERM_XEMPT)
    {
		int	tmp = 0;
		if(IsGoodBm(lookupuser->userid))
		{
			printf ("Ö°Îñ:[<font class=c32>ÓÅĞã°åÖ÷<font class=c37>]");
			tmp = 1;
		}
		
        if (lookupuser->userlevel & PERM_BOARDS)
		{
			if(tmp == 0)
			{
				printf ("Ö°Îñ:");
			}
            show_bm(toupper(lookupuser->userid[0]),lookupuser->userid);
			tmp = 1;
		}
        if(tmp)
		{
			printf ("\n");
		}
    }

    else if (lookupuser->userlevel & PERM_BOARDS)
    {
        printf ("Ö°Îñ:");
        show_bm(toupper(lookupuser->userid[0]),lookupuser->userid);
        printf("\n");
    }
	return;
}
//added end

/*
int show_sysop(char *id)
{
    if (user_perm(id, PERM_SYSOP))
        printf("<font class=c31>¡ï±¾Õ¾Õ¾³¤¡ï <font class=c37>");
}

int show_bm(char *id)
{
    /*    int i, num = 0;
        char buf[256];
     
        for (i = 0; i < MAXBOARD; i++) {
            char *ptr, *brd;
     
            strsncpy(buf, shm_bcache->bcache[i].BM, 200);
            brd = shm_bcache->bcache[i].filename;
            ptr = strtok(buf, " ");
            while (ptr) {
                if (!strcasecmp(id, ptr)) {
                    if (has_read_perm(&currentuser, brd)) {
                        if (num == 0)
                            printf("¡ïÏÖÈÎ");
                        else
                            printf(",");
                        if (num % 5 == 4)
                            printf("\n");
                        num++;
                        printf("[<a href=bbsdoc?board=%s>%s</a>]", brd, brd);
                    }
                }
                ptr = strtok(0, " ");
            }
        }
        if (num > 0)
            printf("°å°åÖ÷(°å¸±)¡ï\n\n");
    */
/*
    FILE *stream;
    int i=0;
    char bm_file_path[28],buffer[51],*ch;
    sprintf(bm_file_path,"home/%c/%s/.bmfile",toupper(id[0]),id);

    if((stream=fopen(bm_file_path,"r"))==NULL)
        printf ("");
    else
    {
        printf("ÏÖÈÎ");
        while(fgets(buffer,32,stream)&& i<4)
        {
            if(ch = strchr(buffer,10))
                *ch='\0';
            printf ("[<a href=bbsdoc?board=%s>%s</a>]",buffer,buffer);
            i++;
        }
        fclose(stream);
        printf("°å°åÖ÷(°å¸±)\n");
    }
}
*/