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

#define FLAG_XUHUI				1
#define FLAG_MINHANG			2
#define FLAG_UNUSE_CAMPUS		3
#define FLAG_OUT_CAMPUS		0
#define FLAG_UNKOWN			10

static IPTABLES currentIptable = { {
                                    {0xCA788000, 0xFFFFF000, {0, 0}, 'U',
                                     NA},
                                    {0xD3505000, 0xFFFFF000, {0, 0}, 'M',
                                     NA},
                                    {0x3B4E0000, 0xFFFFC000, {0, 0}, 'M',
                                     NA},
                                    {0xDBE46000, 0xFFFFE000, {0, 0}, 'M',
                                     NA},
                                    {0xDAC1B000, 0xFFFFF000, {0, 0}, 'M',
                                     NA},
                                    {0xCA701A00, 0xFFFFFF00, {0, 0}, 'X',
                                     NA},
                                    {0xCA79B000, 0xFFFFF800, {0, 0}, 'U',
                                     NA},
                                    {0xCA780000, 0xFFFF0000,
                                     {0xCA780000, 0xCA7823FF}, 'X', YEA},
                                    {0xCA780000, 0xFFFF0000,
                                     {0xCA782400, 0xCA783FFF}, 'M', YEA},
                                    {0xD3500000, 0xFFFF0000,
                                     {0xD3502000, 0xD35035FF}, 'X', YEA},
                                    {0xD3503000, 0xFFFFF000,
                                     {0xD3503600, 0xD3503FFF}, 'M', YEA},
                                    {0x3ac48000, 0xFFFFC000, {0, 0}, 'M',
                                     NA}
                                    }
};

unsigned long ipintonum(char *ip)
{
    union ip_str
    {
        unsigned char ip_s[4];
        unsigned long ip_num;
    } ip_u;
    char buf[128];
    char *p;
    int i;

    sprintf(buf, "%s", ip);
    if (!strchr(buf, '.'))
        return 0;
    p = strtok(buf, ".");
    for (i = 3; i >= 0; i--)
    {
        ip_u.ip_s[i] = atoi(p);
        if ((p = strtok(NULL, ".")) == NULL)
            break;
    }
    return ip_u.ip_num;
}

int checkip(char *ipaddr)
{
    int index;
    unsigned long ipnum;
    char iptemp[16];
    IPTABLES *iptabp;
    IPRULE *iprule;

    iptabp = &currentIptable;
    strncpy(iptemp, ipaddr, sizeof(iptemp));
    iptemp[sizeof(iptemp) - 1] = '\0';
    if ((ipnum = ipintonum(iptemp)) == 0)
        return FLAG_UNKOWN;

#ifdef DEBUG_STAT
    prints("ip is %s and ip3 is %d .\n", ipaddr, ipnum);
#endif

    for (index = 0; index < RULES_MAX; index++)
    {
        iprule = &iptabp->iprules[index];

        if ((ipnum & iprule->mask) == iprule->match)
        {

#ifdef DEBUG_STAT
            prints("It is matched. range is %d/%d . \n",
                   iprule->range[INDEX_MIN], iprule->range[INDEX_MAX]);
#endif

            if ((iprule->isRanged == NA)
                || (ipnum >= iprule->range[INDEX_MIN]
                    && ipnum <= iprule->range[INDEX_MAX]))
            {

#ifdef DEBUG_STAT
                prints("It is in ranged. the area is %c \n", iprule->area);
#endif

                if (iprule->area == 'X')
                    return FLAG_XUHUI;
                else if (iprule->area == 'M')
                    return FLAG_MINHANG;
                else if (iprule->area == 'U')
                    return FLAG_UNUSE_CAMPUS;
                else
                    return FLAG_UNKOWN;
            }

        }
    }
    return FLAG_OUT_CAMPUS;
}

static int insjtunetwork_v6(char *ip)
{
    /* currently,  2001:da8:8000::/48 is used in sjtu */
    return strncmp(ip, "2001:da8:8000:", strlen("2001:da8:8000:"))==0;
}

int insjtunetwork(char *ip)
{
    if (strchr(ip,':')!=NULL) /* if this is an ipv6 address */
    {
        return insjtunetwork_v6(ip);
    }

    return checkip(ip) != FLAG_OUT_CAMPUS;
}

void cleanstats(struct userstat *userstatsp)
{
    userstatsp->malesum = 0;
    userstatsp->femalesum = 0;
    userstatsp->incamsum = 0;
    userstatsp->outcamsum = 0;
    userstatsp->xuhuisum = 0;
    userstatsp->minhangsum = 0;
}

struct userstat userstatist(void)
{
    struct userstat currentstats, *statsp;
    struct user_info *uentp;
    extern struct UTMPFILE *utmpshm;
    extern struct userec lookupuser;
    int i, max;

    statsp = &currentstats;
    cleanstats(statsp);
    resolve_utmp();
    max = USHM_SIZE - 1;
    while (max > 0 && utmpshm->uinfo[max].active == 0)
        max--;
    for (i = 0; i <= max; i++)
    {
        uentp = &(utmpshm->uinfo[i]);
        if (uentp->uid && uentp->pid && uentp->active)
        {
            getuser(uentp->userid);
            if (lookupuser.gender == 'M')
                statsp->malesum++;
            if (lookupuser.gender == 'F')
                statsp->femalesum++;

            if (lookupuser.lasthost[0] != '\0')
                switch (checkip(lookupuser.lasthost))
                {
                case FLAG_OUT_CAMPUS:
                    statsp->outcamsum++;
                    break;

                case FLAG_XUHUI:
                    statsp->xuhuisum++;
                    statsp->incamsum++;
                    break;

                case FLAG_MINHANG:
                    statsp->minhangsum++;
                    statsp->incamsum++;
                    break;

                case FLAG_UNUSE_CAMPUS:
                    statsp->incamsum++;
                    break;

                case FLAG_UNKOWN:
                default:
                    break;
                }
        }
    }
    return currentstats;
}

int showStatist(void)
{
    struct userstat currentStat, *statsp;
    int ratioGG, ratioInCam, ratioMh;
    int sum;

    modify_user_mode(LUSERS);
    clear();
    prints("[1;32mÍ³¼ÆÖÐ£¬ÇëµÈ´ý¡£¡£¡£ \n\n");

    currentStat = userstatist();
    statsp = &currentStat;

    sum = statsp->femalesum + statsp->malesum;
    ratioGG = (int) (statsp->malesum * 100 / sum);
    ratioInCam = (int) (statsp->incamsum * 100 / sum);
    ratioMh = (int) (statsp->minhangsum * 100 / statsp->incamsum);

    clear();
    prints("[1;32mµ±Ç°ÔÚÏßÓÃ»§ÖÐ: \n\n");
    if (HAS_PERM(PERM_SYSOP))
    {
        prints
            ("[1;32mÐ£ÄÚ/Ð£Íâ: [[1;36m%d[1;32m/[1;36m%d[1;32m], ±ÈÀýÎª[[1;36m%d%%[1;32m/[1;36m%d%%[1;32m]\n\n",
             statsp->incamsum, statsp->outcamsum,
             ratioInCam, 100 - ratioInCam);
    }
    prints
        (" GG / MM : [[1;36m%d[1;32m/[1;36m%d[1;32m], ±ÈÀýÎª[[1;36m%d%%[1;32m/[1;36m%d%%[1;32m]\n\n",
         statsp->malesum, statsp->femalesum, ratioGG, 100 - ratioGG);
    prints
        ("[1;32mÐì»ã/ãÉÐÐ: [[1;36m%d[1;32m/[1;36m%d[1;32m], ±ÈÀýÎª[[1;36m%d%%[1;32m/[1;36m%d%%[1;32m]\n",
         statsp->xuhuisum, statsp->minhangsum, 100 - ratioMh, ratioMh);

    pressreturn();
    return 0;

}
