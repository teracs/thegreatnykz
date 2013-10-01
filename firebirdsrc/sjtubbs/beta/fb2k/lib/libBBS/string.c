/*
 * string.c			-- there's some useful function about string
 *	
 * of SEEDNetBBS generation 1 (libtool implement)
 *
 * Copyright (c) 1998, 1999, Edward Ping-Da Chuang <edwardc@edwardc.dhs.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * CVS: $Id: string.c,v 1.2 2004/06/29 14:59:21 rwzmm Exp $
 */

#ifdef BBS
#include "bbs.h"
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>               /* for time_t prototype */
#endif


char *substr(char *string, int from, int to)
{
    char *result;
    int i, j;

    result = (char *) malloc(strlen(string) + 1);

    j = 0;
    for (i = from; i < to + 1; i++)
    {
        if (string[i] == '\0' || i >= (int)strlen(string))
            break;
        result[j] = string[i];
        j++;
    }

    return ((char *) result);

}

char *stringtoken(char *string, char tag, int *log)
{
    int i, j;
    char *result;

    result = (char *) malloc(strlen(string) + 1);

    j = 0;
    for (i = *log;; i++)
    {
        if (i == (int)strlen(string) || i >= (int)strlen(string))
            break;
        if (string[i] == 0)
            break;
        if (string[i] == tag)
            break;
        result[j] = string[i];
        j++;
    }

    *log = i + 1;
    result[j] = '\0';
    return ((char *) result);
}




void strtolower(char *dst, char *src)
{
    for (; *src; src++)
        *dst++ = tolower(*src);
    *dst = '\0';
}

void strtoupper(char *dst, char *src)
{
    for (; *src; src++)
        *dst++ = toupper(*src);
    *dst = '\0';
}

int killwordsp(char *str)
{
    char *ptr;

    ptr = str;
    while (*ptr == ' ')
        ptr++;
    if (*ptr != '\0')
    {
        int i = 0;

        while (1)
        {
            str[i] = *ptr;
            if (*ptr == '\0')
                break;
            ptr++;
            i++;
        }
        ptr = str + i - 1;
        while (*ptr == ' ')
            ptr--;
        *(ptr + 1) = '\0';
        return 1;
    }
    else
        str[0] = '\0';
    return 0;
}

int is_alpha(int ch)
{
    return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'));
}

int my_ansi_filter(char *source)
{
    char *pw=source,*pr=source;
    int flag = 0;

    for (; *pr!='\0'; ++pr)
    {
        if (*pr == '')
        {
            flag = 1;
            continue;
        }
        else if (flag == 1 && isalpha(*pr))
        {
            flag = 0;
            continue;
        }
        else if (flag == 1)
        {
            continue;
        }
        else
        {
            *pw = *pr;
			++pw;
        }
    }
    *pw = '\0';
	return (pw-source);
}

char *ansi_filter(char *source)
{
    char *result, ch[3];
    int i, flag = 0, slen = strlen(source);

    result = (char *) malloc((slen + 10) * sizeof(char));

    for (i = 0; i < slen; i++)
    {
        if (source[i] == '')
        {
            flag = 1;
            continue;
        }
        else if (flag == 1 && is_alpha(source[i]))
        {
            flag = 0;
            continue;
        }
        else if (flag == 1)
        {
            continue;
        }
        else
        {
            sprintf(ch, "%c", source[i]);
            strcat(result, ch);
        }
    }

    return (char *) result;
}

char *Cdate(time_t *clock)
{
    //      char *foo;
    static char foo[22];
    struct tm *mytm = localtime(clock);

    //      foo = (char *)malloc(22);
    strftime(foo, 22, "%D %T %a", mytm);
    return (foo);
}
