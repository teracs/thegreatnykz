#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <setjmp.h>
#include <glib.h>


#define FWEB_STDIO_DEFINES
#include "fwebio.h"
#undef FWEB_STDIO_DEFINES

#include "bbs.h"
#include "shm.h"
#include "kernel.h"
#include "const.h"
#include "cgic.h"
#include "mempool.h"
#include "fweb.h"
#include "module.h"



char seccode[SECNUM][5] = {
	"a", "bc", "d", "efg", "hij", "kl", "o", "p", "qrs", "tu", "vw", "x"
};

//add by rwzmm@sjtubbs

#define MAX_MSG_SIZE 1024



char secname[SECNUM][2][20] = {
	{"BBS ÏµÍ³", "[±¾Õ¾][Õ¾ÄÚ][BBS]"},
	{"ÉÏº£½»´ó", "[±¾Ð£]"},
	{"Ñ§×ÓÔºÐ£", "[¸ßÐ£][µØÇø]"},
	{"µçÄÔ¼¼Êõ", "[µçÄÔ]"},
	{"Ñ§Êõ¿ÆÑ§", "[¿ÆÑ§]"},
	{"ÒÕÊõÎÄ»¯", "[ÎÄÒÕ][ÓïÑÔ]"},
	{"ÌåÓýÔË¶¯", "[ÌåÓý][ÔË¶¯]"},
	{"ÐÝÏÐÓéÀÖ", "[ÐÝÏÐ][ÒôÀÖ][ÓéÀÖ]"},
	{"ÖªÐÔ¸ÐÐÔ", "[ÖªÐÔ] [¸ÐÐÔ]"},
	{"Éç»áÐÅÏ¢", "[Éç»á][ÐÅÏ¢][¿¼ÊÔ]"},
	{"ÉçÍÅÈºÌå", "[ÉçÍÅ][ÈºÌå]"},
	{"ÓÎÏ·×¨Çø", "[ÓÎÏ·]"}
};

int loginok = 0;
int pic_mod;

struct userec currentuser;
struct user_info *u_info;

char fromhost[256];
char datestring[30];

int my_t_lines = 20;
int my_link_mode = 0;
int my_def_mode = 0;
int my_font_mode = 0;
int my_fontn_mode = 1;
int my_ansisize_mode = 1;
int my_ansifont_mode = 0;
int my_theme_mode = 1;
int silence = 0;


int
file_has_word(char *file, char *word)
{
	FILE *fp;
	char buf[256], buf2[256];

	fp = fopen(file, "r");
	if (fp == 0)
		return 0;
	while (1)
	{
		bzero(buf, 256);
		if (fgets(buf, 255, fp) == 0)
			break;
		sscanf(buf, "%s", buf2);
		if (!strcasecmp(buf2, word))
		{
			fclose(fp);
			return 1;
		}
	}
	fclose(fp);
	return 0;
}



int
f_append(char *file, char *buf)
{
	FILE *fp;

	fp = fopen(file, "a");
	if (fp == 0)
		return 1;
	fprintf(fp, "%s", buf);
	fclose(fp);
	return 0;
}

struct stat *
f_stat(char *file)
{
	static struct stat buf;

	bzero(&buf, sizeof(buf));
	if (stat(file, &buf) == -1)
		bzero(&buf, sizeof(buf));
	return &buf;
}

int
file_size(char *file)
{
	return f_stat(file)->st_size;
}

int
file_rtime(char *file)
{
	return f_stat(file)->st_atime;
}

int
file_exist(char *file)
{
	return file_time(file) != 0;
}

int
file_isdir(char *file)
{
	return (f_stat(file)->st_mode & S_IFDIR) != 0;
}

int
file_isfile(char *file)
{
	return (f_stat(file)->st_mode & S_IFREG) != 0;
}

int
file_time(char *file)
{
	return f_stat(file)->st_mtime;
}

int
get_record(void *buf, int size, int num, char *file)
{
	FILE *fp;

	if (size < 1 || size > 4096)
		return 0;
	if (num < 0 || num > 1000000)
		return 0;
	bzero(buf, size);
	fp = fopen(file, "r");
	if (fp == 0)
		return 0;
	fseek(fp, num * size, SEEK_SET);
	if (fread(buf, size, 1, fp) <= 0)
	{
		fclose(fp);
		return 0;
	}
	fclose(fp);
	return 1;
}

int
put_record(void *buf, int size, int num, char *file)
{
	FILE *fp;

	if (size < 1 || size > 4096)
		return 0;
	if (num < 0 || num > 1000000)
		return 0;
	fp = fopen(file, "r+");
	if (fp == 0)
		return 0;
	fseek(fp, num * size, SEEK_SET);
	fwrite(buf, size, 1, fp);
	fclose(fp);
	return 1;
}

int
del_record(char *file, int size, int num)
{
	FILE *fp;
	int total, i;
	char buf[4096];

	if (size < 1 || size > 4096)
		return 0;
	total = file_size(file) / size;
	if (total < 1 || total > 1000000)
		return 0;
	fp = fopen(file, "r+");
	if (fp == 0)
		return 0;
	flock(fileno(fp), LOCK_EX);
	for (i = num + 1; i <= total - 1; i++)
	{
		fseek(fp, i * size, SEEK_SET);
		if (fread(buf, size, 1, fp) <= 0)
			break;
		fseek(fp, (i - 1) * size, SEEK_SET);
		fwrite(buf, size, 1, fp);
	}
	ftruncate(fileno(fp), (total - 1) * size);
	flock(fileno(fp), LOCK_UN);
	fclose(fp);
	return 1;
}

char *
Ctime(time_t t)
{
	static char s[80];

	sprintf(s, "%24.24s", ctime(&t));
	return s;
}

char *
noansi(char *s)
{
	static char buf[1024];
	int i = 0, mode = 0;

	while (s[0] && i < 1023)
	{
		if (mode == 0)
		{
			if (s[0] == 27)
			{
				mode = 1;
			}
			else
			{
				buf[i] = s[0];
				i++;
			}
		}
		else
		{
			if (!strchr(";[0123456789", s[0]))
				mode = 0;
		}
		s++;
	}
	buf[i] = 0;
	return buf;
}

char *
nohtml(char *s)
{
	static char buf[1024];
	int i = 0, mode = 0;
	

	while (s[0] && i < 1023)
	{
		if (mode == 0)
		{
			if (s[0] == '<')
			{
				mode = 1;
			}
			else
			{
				buf[i] = s[0];
				i++;
			}
		}
		else
		{
			if (s[0] == '>')
				mode = 0;
		}
		s++;
	}
	buf[i] = 0;
	return buf;
}

char *
strright(char *s, int len)
{
	int l = strlen(s);

	if (len <= 0)
		return "";
	if (len >= l)
		return s;
	return s + (l - len);
}

char *
strcasestr(const char *s1, const char *s2)
{
	int l;

	l = strlen(s2);
	while (s1[0])
	{
		if (!strncasecmp(s1, s2, l))
			return (char *)s1;
		s1++;
	}
	return 0;
}

int
strsncpy(char *s1, char *s2, int n)
{
	int l = strlen(s2);

	if (n < 0)
		return 1;
	if (n > l + 1)
		n = l + 1;
	strncpy(s1, s2, n - 1);
	s1[n - 1] = 0;
	return 0;
}

char *
ltrim(char *s)
{
	char *s2 = s;

	if (s[0] == 0)
		return s;
	while (s2[0] && strchr(" \t\r\n", s2[0]))
		s2++;
	return s2;
}

char *
rtrim(char *s)
{
	static char t[1024], *t2;

	if (s[0] == 0)
		return s;
	strsncpy(t, s, 1024);
	t2 = t + strlen(s) - 1;
	while (strchr(" \t\r\n", t2[0]) && t2 > t)
		t2--;
	t2[1] = 0;
	return t;
}

char *
trim(char *s)
{
	return ltrim(rtrim(s));
}




char *
getsenv(char *s)
{
	char *t = getenv(s);

	if (t)
		return t;
	return "";
}



int
strnncpy(char *s, int *l, char *s2)
{
	strncpy(s + (*l), s2, strlen(s2));
	(*l) += strlen(s2);
	return 0;
}

int
hsprintf(char *s, char *fmt, ...)
{
	char buf[1024], ansibuf[80], buf2[80];
	char *tmp;
	int c, bold, m, i, l, len;
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, 1023, fmt, ap);
	va_end(ap);
	s[0] = 0;
	l = strlen(buf);
	len = 0;
	bold = 0;
	for (i = 0; i < l; i++)
	{
		c = buf[i];
		if (c == '&')
		{
			strnncpy(s, &len, "&amp;");
		}
		else if (c == '<')
		{
			strnncpy(s, &len, "&lt;");
		}
		else if (c == '>')
		{
			strnncpy(s, &len, "&gt;");
		}
		else if (c == 27)
		{
			if (buf[i + 1] != '[')
				continue;
			for (m = i + 2; m < l && m < i + 24; m++)
				if (strchr("0123456789;", buf[m]) == 0)
					break;
			strsncpy(ansibuf, &buf[i + 2], m - (i + 2) + 1);
			i = m;
			if (buf[i] != 'm')
				continue;
			if (strlen(ansibuf) == 0)
			{
				bold = 0;
				strnncpy(s, &len, "<font class=c37>");
			}
			tmp = strtok(ansibuf, ";");
			while (tmp)
			{
				c = atoi(tmp);
				tmp = strtok(0, ";");
				if (c == 0)
				{
					strnncpy(s, &len, "<font class=c37>");
					bold = 0;
				}
				if (c >= 30 && c <= 37)
				{
					if (bold == 1)
						sprintf(buf2,
							"<font class=d%d>",
							c);
					if (bold == 0)
						sprintf(buf2,
							"<font class=c%d>",
							c);
					strnncpy(s, &len, buf2);
				}
			}
		}
		else
		{
			s[len] = c;
			len++;
		}
	}
	s[len] = 0;
	return 0;
}


int
hprintf(char *fmt, ...)
{
	char buf[8096], buf2[1024];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf2, 1023, fmt, ap);
	va_end(ap);
	hsprintf(buf, "%s", buf2);
	printf("%s", buf);
	return 0;
}

//add by cf
int
hfprintf(FILE * stream, char *fmt, ...)
{
	char buf[8096], buf2[1024];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf2, 1023, fmt, ap);
	va_end(ap);
	hsprintf(buf, "%s", buf2);
	fprintf(stream, "%s", buf);
	return 0;
}

int
hhprintf(char *fmt, ...)
{
	char buf0[1024], buf[1024], *s, *getparm(char *var);
	int len = 0;
	int my_link_mode;
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, 1023, fmt, ap);
	va_end(ap);
	buf[1023] = 0;
	s = buf;
	my_link_mode = atoi(getparm("my_link_mode"));
	if (my_link_mode == 1)
		return hprintf("%s", buf);
	if (!strcasestr(s, "http://") && !strcasestr(s, "ftp://")
	    && !strcasestr(s, "mailto:"))
		return hprintf("%s", buf);
	while (s[0])
	{
		if (!strncasecmp(s, "http://", 7)
		    || !strncasecmp(s, "mailto:", 7)
		    || !strncasecmp(s, "ftp://", 6))
		{
			char *tmp, *url;

			if (len > 0)
			{
				buf0[len] = 0;
				hprintf("%s", buf0);
				len = 0;
			}
			tmp = strtok(s, "\'\" \r\t)(;\n");
			if (tmp == 0)
				break;
			url = tmp;
			if(!strncmp(UPLOAD_FILE_URL_HEADER,tmp,strlen(UPLOAD_FILE_URL_HEADER)))
			{
				url += strlen("http://"BBSHOST);
			}
			if (pic_mod==2
			    &&
			    (strcasestr(tmp, ".gif")
			      || strcasestr(tmp, ".jpg")
			      || strcasestr(tmp, ".png")
			     || strcasestr(tmp, ".jfif")))
			{
				
				printf("<IMG SRC=\"%s\" >",nohtml(url));
				tmp = strtok(0, "");
				if (tmp == 0)
					return 0;
				return hhprintf(tmp);
			}

			if (pic_mod==2 && strcasestr(tmp, ".swf"))
			{
				printf("<EMBED src=\"%s\" TYPE=\"application/x-shockwave-flash\" HEIGHT=300 width=400 AllowScriptAccess=\"never\"></EMBED>", nohtml(url));
				tmp = strtok(0, "");
				if (tmp == 0)
					return 0;
				return hhprintf("%s", tmp);
			}
			printf("<a target=_blank href='%s'>",nohtml(url));
			printf("%s</a>", nohtml(tmp));
			tmp = strtok(0, "");
			if (tmp == 0)
				return printf("\n");
			return hhprintf("%s", tmp);
		}
		else
		{
			buf0[len] = s[0];
			if (len < 1000)
				len++;
			s++;
		}
	}
	return 0;
}

int
hhfprintf(FILE * stream, char *fmt, ...)
{
	char buf0[1024], buf[1024], *s, *getparm(char *var);
	int len = 0;
	int my_link_mode;
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, 1023, fmt, ap);
	va_end(ap);
	buf[1023] = 0;
	s = buf;
	my_link_mode = atoi(getparm("my_link_mode"));
	if (my_link_mode == 1)
		return hfprintf(stream, "%s", buf);
	if (!strcasestr(s, "http://") && !strcasestr(s, "ftp://")
	    && !strcasestr(s, "mailto:"))
		return hfprintf(stream, "%s", buf);
	while (s[0])
	{
		if (!strncasecmp(s, "http://", 7)
		    || !strncasecmp(s, "mailto:", 7)
		    || !strncasecmp(s, "ftp://", 6))
		{
			char *tmp, *url;

			if (len > 0)
			{
				buf0[len] = 0;
				hfprintf(stream, "%s", buf0);
				len = 0;
			}
			tmp = strtok(s, "\'\" \r\t)(;\n");
			if (tmp == 0)
				break;
			url = tmp;
			if(!strncmp(UPLOAD_FILE_URL_HEADER,tmp,strlen(UPLOAD_FILE_URL_HEADER)))
			{
				url += strlen("http://"BBSHOST);
			}
			if (pic_mod==2
			    &&
			    (strcasestr(tmp, ".gif")
			      || strcasestr(tmp, ".jpg")
			      || strcasestr(tmp, ".png")
			     || strcasestr(tmp, ".jfif")))
			{
				fprintf(stream,
						"<IMG SRC=\"%s\" >",nohtml(url));
				tmp = strtok(0, "");
				if (tmp == 0)
					return 0;
				return hhfprintf(stream, tmp);
			}
			if (pic_mod==2 && strcasestr(tmp, ".swf"))
			{
				char *url = tmp;
				if(strncmp(UPLOAD_FILE_URL_HEADER,tmp,strlen(UPLOAD_FILE_URL_HEADER)))
				{
					url += strlen("http://"BBSHOST);
				}
				fprintf(stream,
						"<EMBED src=\"%s\" TYPE=\"application/x-shockwave-flash\" HEIGHT=300 width=400 AllowScriptAccess=\"never\"></EMBED>",
						nohtml(url));
				tmp = strtok(0, "");
				if (tmp == 0)
					return 0;
				return hhfprintf(stream, "%s", tmp);
			}
			fprintf(stream, "<a target=_blank href='%s'>",nohtml(url));
			fprintf(stream,"%s</a>",nohtml(tmp));
			tmp = strtok(0, "");
			if (tmp == 0)
				return fprintf(stream, "%s", "\n");
			return hhfprintf(stream, "%s", tmp);
		}
		else
		{
			buf0[len] = s[0];
			if (len < 1000)
				len++;
			s++;
		}
	}
	return 0;
}


void
html_head(void)
{
	printf("<html>\n");
	printf("<meta HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=gb2312\">\n");
	printf("<link rel=stylesheet type=text/css href='%s'>\n", CSS_FILE);
	printf("<script type=\"text/javascript\" src=\"/check.js\"></script>\n");
}

void
set_fromhost(void)
{
	/*
	char buf3[1024];
	char *pstr, *pcom;
	strncpy(buf3, getsenv("HTTP_X_FORWARDED_FOR"), 1024);

	if (buf3[0])
	{
		int stop = 0;
		pstr = buf3;
		while (isspace(*pstr))
			++pstr;
		while (!stop)
		{
			if (isdigit(pstr[0]) && strncmp(pstr, "127.", 4)
			    && strncmp(pstr, "192.168.", 8)
			    && strncmp(pstr, "10.", 3)
			    && strncmp(pstr, "172.1", 5)
			    && strncmp(pstr, "172.2", 5)
			    && strncmp(pstr, "172.3", 5))
			{
				stop = 1;
				pcom = strchr(pstr, ',');
				if (NULL != pcom)
					*pcom = '\0';
				strncpy(fromhost, pstr, 32);
				//setenv("HTTP_X_FORWARDED_FOR", pstr, 1);
			}
			else
			{
				pcom = strchr(pstr, ',');
				if (NULL == pcom)
					stop = 1;
				else
				{
					++pcom;
					while (isspace(*pcom))
						++pcom;
					pstr = pcom;
				}
			}
		}
	}
	if (fromhost[0] == '\0')
	*/
		strsncpy(fromhost, getsenv("REMOTE_ADDR"), 60);
}

int
bbs_http_init(void)
{
	set_fromhost();
	if (!silence)
	{
		cgi_head();
		html_head();
	}
	return 0;
}


int
__to16(char c)
{
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	if (c >= '0' && c <= '9')
		return c - '0';
	return 0;
}

int
__unhcode(char *s)
{
	int m, n;

	for (m = 0, n = 0; s[m] != 0; m++, n++)
	{
		if (s[m] == '+')
		{
			s[n] = ' ';
			continue;
		}
		if (s[m] == '%')
		{
			s[n] = __to16(s[m + 1]) * 16 + __to16(s[m + 2]);
			m += 2;
			continue;
		}
		s[n] = s[m];
	}
	s[n] = 0;
	return 0;
}
/*
static char *parms[1024];
static int parms_num = 0;
char *
getparm(char *var)
{
	int n;

	if (cgiFormStringSpaceNeeded(var, &n) != cgiFormSuccess)
		return "";
	parms[parms_num++] = g_new(char, n);
	cgiFormString(var, parms[parms_num - 1], n);
	return parms[parms_num - 1];
}
void
resetparms(void)
{
	int i;
	for (i = 0; i < parms_num; i++)
	{
		g_free(parms[i]);
	}
	parms_num = 0;
}
*/
char *
getparm(char *var)
{
	int n;
	char *ptr;

	if (cgiFormStringSpaceNeeded(var, &n) != cgiFormSuccess)
		return "";
	ptr = mp_alloc(n);
	cgiFormString(var, ptr, n);
	return ptr;
}


int
is_web_user(struct user_info *user)
{
	return (user->mode == 10001);
}

int
user_init(struct userec *x, struct user_info **y)
{
	struct userec *x2;
	char id[20];
	int i, key;
	cgiCookieString("utmpuserid", id, 20);
	cgiCookieInteger("utmpnum", &i, -1);
	cgiCookieInteger("utmpkey", &key, -1);

	if (i < 0 || i >= MAXACTIVE)
		return 0;
	(*y) = &(shm_utmp->uinfo[i]);
	if (strncmp((*y)->from, fromhost, 60))
		return 0;
	if ((*y)->utmpkey != key)
		return 0;
	if ((*y)->active == 0)
		return 0;
	if ((*y)->userid[0] == 0)
		return 0;
	if (!is_web_user(*y))
		return 0;
	if (!strcasecmp((*y)->userid, "new"))
		return 0;
	x2 = getuser((*y)->userid);
	if (x2 == 0)
		return 0;
	if (strcmp(x2->userid, id))
		return 0;
	memcpy(x, x2, sizeof(*x));
	if (!strcasecmp((*y)->userid, "guest"))
		return 0;
	return 1;
}

int
getdatestring(time_t now, int Chang)
{
	struct tm *tm;
	char weeknum[7][3] = { "Ìì", "Ò»", "¶þ", "Èý", "ËÄ", "Îå", "Áù" };

	tm = localtime(&now);
	sprintf(datestring, "%4dÄê%02dÔÂ%02dÈÕ%02d:%02d:%02d ÐÇÆÚ%2s",
		tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
		tm->tm_hour, tm->tm_min, tm->tm_sec, weeknum[tm->tm_wday]);
	return (tm->tm_sec % 10);
}

int
post_mail(char *userid, char *title, char *file, char *id,
	  char *nickname, char *ip, int sig)
{
	FILE *fp, *fp2;
	char buf3[256], dir[256];
	struct fileheader header;
	int t, i;

	if (strstr(userid, "@"))
		return post_imail(userid, title, file, id, nickname, ip, sig);
	bzero(&header, sizeof(header));
	strcpy(header.owner, id);
	for (i = 0; i < 100; i++)
	{
		t = time(0) + i;
		sprintf(buf3, "mail/%c/%c/%s/M.%d.A", toupper(userid[0]), toupper(userid[1]), userid,
			(int) (i + time(0)));
		if (!file_exist(buf3))
			break;
	}
	if (i >= 99)
		return -1;
	sprintf(header.filename, "M.%d.A", t);
	strsncpy(header.title, title, 60);
	fp = fopen(buf3, "w");
	if (fp == 0)
		return -2;
	fp2 = fopen(file, "r");
	getdatestring(time(0), NA);
	fprintf(fp, "¼ÄÐÅÈË: %s (%s)\n", id, nickname);
	fprintf(fp, "±ê  Ìâ: %s\n", title);
	fprintf(fp, "·¢ÐÅÕ¾: %s (%s)\n", BBSNAME, datestring);
	fprintf(fp, "À´  Ô´: %s\n\n", ip);
	if (fp2)
	{
		while (1)
		{
			if (fgets(buf3, 256, fp2) == NULL)
				break;
			fprintf2(fp, buf3);
		}
		fclose(fp2);
	}
	fprintf(fp, "\n--\n");
	sig_append(fp, id, sig);
	fprintf(fp, "\n\n[1;%dm¡ù À´Ô´:¡¤%s http://%s¡¤[FROM: %.46s][m\n",
		31 + rand() % 7, BBSNAME, BBSHOST, ip);
	fclose(fp);
	sprintf(dir, "mail/%c/%c/%s/.DIR", toupper(userid[0]), toupper(userid[1]), userid);
	fp = fopen(dir, "a");
	if (fp == 0)
		return -1;
	fwrite(&header, sizeof(header), 1, fp);
	fclose(fp);
	return 0;
}

int
post_imail(char *userid, char *title, char *file, char *id,
	   char *nickname, char *ip, int sig)
{
	//All the variables have no use, so I deleted them.  --hongliang
	//Nov. 20th, 2003 2:25
	/*
	   FILE *fp1, *fp2;
	   char buf[256];
	   int i;
	 */

	return 0;
}

int
post_article(char *board, char *title, char *file, char *id,
	     char *nickname, char *ip, int sig, int autocr,
	     unsigned int currid, unsigned int reid)
{
	FILE *fp, *fp2;
	char buf3[1024];
	struct shortfile *brd;
	struct fileheader header;
	int t = 0, count=0,filesd;

	brd = getbcache(board);
	if (brd == 0)
		return -1;
	board = brd->filename;
	bzero(&header, sizeof(header));
	strcpy(header.owner, id);

/*	header.id = currid;
	header.reid = reid;*/
	//add by heack 2005-06-13
	t = time(0);
	char fulltitle[20];
	sprintf(fulltitle, "M.%d.A", t);
	sprintf(buf3, "boards/%s/%s/%s", board, getfolderstr(fulltitle), fulltitle);

	while ((filesd = open(buf3, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1)
	{
    	t++;
	sprintf(fulltitle, "M.%d.A", t);
    	sprintf(buf3, "boards/%s/%s/%s", board, getfolderstr(fulltitle), fulltitle);
    	if (count++ > MAX_POSTRETRY)
    	{
        		return -1;
    	}
	}
	fchmod(filesd, 0644);
	close(filesd);

	if(0 == currid)//modified by chaser 2006.05.25
	{
		header.id=t;//reply
		header.reid=reid;
	}
	else
	{
		header.id=t;//new article
		header.reid=t;
	}//modify end
	
	/*for (i = 0; i < 100; i++)
	{
		t = time(0) + i;
		sprintf(buf3, "boards/%s/M.%d.A", board, t);
		if (!file_exist(buf3))
			break;
	}
	if (i >= 99)
		return -1;*/
	sprintf(header.filename, "M.%d.A", t);
	strsncpy(header.title, title, 60);
	fp = fopen(buf3, "w");
	fp2 = fopen(file, "r");
	getdatestring(time(0), NA);
	fprintf(fp,
		"·¢ÐÅÈË: %s (%s), ÐÅÇø: %s\n±ê  Ìâ: %s\n·¢ÐÅÕ¾: %s (%s)\n\n",
		id, nickname, board, title, BBSNAME, datestring);
	if (fp2 != 0)
	{
		while (1)
		{
			if (fgets(buf3, 1000, fp2) == NULL)
				break;
			if (autocr)
			{
				fprintf2(fp, buf3);
			}
			else
			{
				fprintf(fp, "%s", buf3);
			}
		}
		fclose(fp2);
	}
	fprintf(fp, "\n--\n");
	sig_append(fp, id, sig);
	fprintf(fp, "\n[1;%dm¡ù À´Ô´:¡¤%s %s¡¤[FROM: %.46s][m\n",
		31 + rand() % 7, BBSNAME, BBSHOST, ip);
	fclose(fp);
	sprintf(buf3, "boards/%s/.DIR", board);
	fp = fopen(buf3, "a");
	fwrite(&header, sizeof(header), 1, fp);
	fclose(fp);
	return t;
}

int
sig_append(FILE * fp, char *id, int sig)
{
	FILE *fp2;
	char path[256];
	char buf[100][256];
	int i, total;
	struct userec *x;

	if (sig < 0 || sig > 10)
		return 0;
	x = getuser(id);
	if (x == 0)
		return 0;
	sprintf(path, "home/%c/%c/%s/signatures", toupper(id[0]), toupper(id[1]), id);
	fp2 = fopen(path, "r");
	if (fp2 == 0)
		return 0;
	for (total = 0; total < 100; total++)
		if (fgets(buf[total], 255, fp2) == 0)
			break;
	fclose(fp2);
	for (i = sig * 6; i < sig * 6 + 6; i++)
	{
		if (i >= total)
			break;
		fprintf(fp, "%s", buf[i]);
	}
	return 0;
}

char *
anno_path_of(char *board)
{
	FILE *fp;
	static char buf[256], buf1[80], buf2[80];

	fp = fopen("0Announce/.Search", "r");
	if (fp == 0)
		return "";
	snprintf(buf,255,"%s%c",board,':');
	while (1)
	{
		if (fscanf(FCGI_ToFILE(fp), "%s %s", buf1, buf2) <= 0)
			break;
		if (!strcasecmp(buf1, buf))
		{
			sprintf(buf, "/%s", buf2);
			fclose(fp);
			return url_quote(buf);
		}
	}
	fclose(fp);
	return "";
}

//The following code is add by hongliang on Nov. 23th, 2003 07:25
//For District Master
//start-hongliang
/* Added by old@sjtu.edu to access master of district 2001.2.1 */
/*added by aegis@010921 to modify chk_currDM()*/
//mudboy modify to WHICHDIST(char *userid) from WHICHDIST()
int
WHICHDIST(char *userid)
{
	char datafile[80] = BBSHOME "/etc/DISTmaster.ini";
	FILE *fp;
	int counts = 1;
	char t[256], *t1, *t2, *t3;
	fp = fopen(datafile, "r");
	if (fp == NULL)
		return 12;
	while (fgets(t, 255, fp) && counts <= 11)
	{
		t1 = strtok(t, " \t\n");
		if (t1 == NULL || t1[0] == '#')
			continue;
		if (!isalpha(t1[0]))
		{
			counts++;
			continue;
		}
		t2 = strtok(NULL, " \t\n");
		if (t2 != NULL)
			t3 = strtok(NULL, " \t\n");
		else
			t3 = NULL;
		if ((t1 != NULL && !strcasecmp(t1, userid))
		    || (t2 != NULL && !strcasecmp(t2, userid)) || (t3 != NULL
								   &&
								   !strcasecmp
								   (t3,
								    userid)))
		{
			fclose(fp);
			return (counts);
		}
		counts++;
	}
	fclose(fp);
	return (counts);
}

int
has_DM_perm(struct userec *user, char *board)
{
	char BoardsFile[256], category;
	int fd, total, index, Dindex, DMLevel;
	struct boardheader *buffer;
	struct stat st;

	char CateInSect[SectNum][5] =
		{ "a", "bc", "d", "efg", "hij", "kl", "o", "p", "qrs", "tu",
		"vw", "x"
	};
	/* {"a", "bcd", "efg", "hij", "kl", "mn", "op", "qr","s"}; */
    /****************************************************************************/
	/*  ²ÉÓÃÀàËÆbbs2wwwµÄ°ì·¨£¬Èç¹ûÐÞ¸ÄÁËÌÖÂÛÇøµÄ·ÖÀà£¬ÇëÊÓÇé¿öÐÞ¸ÄÒÔÏÂ¶¨Òå     */
	/*                                                                          */
	/*  ·ÖÀàÂëºÍ·ÖÀà²Ëµ¥ÔÚmenu.iniÖÐÉè¶¨,Firebird BBS 2.52ÖÐÈ±Ê¡µÄ·ÖÀàÓÐ:       */
	/*  0¡¾Õ¾ÄÚ¡¿¡¢C¡¾±¾Ð£¡¿¡¢A¡¾Çó¾È¡¿¡¢E¡¾×ÊÑ¶¡¿¡¢I¡¾Ð£Ô°¡¿¡¢N¡¾µçÄÔ¡¿¡¢R¡¾   */
	/*  ÏµÍ³¡¿¡¢a¡¾ÐÝÏÐ¡¿¡¢e¡¾ÎÄÒÕ¡¿¡¢i¡¾Ñ§¿Æ¡¿¡¢m¡¾ÒôÀÖ¡¿¡¢n¡¾Éç½»¡¿¡¢r¡¾ÔË¶¯  */
	/*  ¡¿¡¢s¡¾Ö°°ô¡¿¡¢x¡¾Ì¸Ìì¡¿¡¢f¡¾ÐÂÎÅ¡¿µÈÀà                                 */
    /****************************************************************************/

	if (!user_perm(user, PERM_SPECIAL7))
		return 0;

	sprintf(BoardsFile, "%s/.BOARDS", BBSHOME);
	fd = open(BoardsFile, O_RDONLY);
	if (fd == -1)
		return 0;
	(void) fstat(fd, &st);
	total = st.st_size / sizeof(struct boardheader);
	buffer = (struct boardheader *)  malloc(st.st_size);
	if (buffer == NULL)
	{
		close(fd);
		return 0;
	}
	if (read(fd, buffer, (size_t) st.st_size) < (ssize_t) st.st_size)
	{
		close(fd);
		free(buffer);
		return 0;
	}
	close(fd);

	for (index = 0; index < total; index++)
	{
		if (!strcmp(buffer[index].filename, board))
			break;
	}
	if (index == total)
		free(buffer);
		return 0;

	category = buffer[index].title[0];

	for (Dindex = 0; Dindex < SectNum; Dindex++)
	{
		if (strchr(CateInSect[Dindex], category) != NULL)
			break;
	}

	if (Dindex == SectNum)
		free(buffer);
		return 0;

	DMLevel = WHICHDIST(currentuser.userid);	//mudboy modify
	free(buffer);
	if (Dindex == DMLevel)
		return 1;
	else
		return 0;
}

//add by zsong, let directory bm has bm permission of all his sub-boards
int has_parent_perm(struct userec *user, char *board)
{
	struct shortfile *x;
	char buf[256], *bm;	
	int parent = 0;
	x = getbcache(board);	
	parent = x->group-1;
	if(parent <0)
		return 0;
	strncpy(buf, shm_bcache->bcache[parent].BM, BM_LEN-4);
	bm = strtok(buf, ",: ;&()\n");
	while (bm)
	{
		if (!strcasecmp(bm, user->userid))
			return 1;
		bm = strtok(0, ",: ;&()\n");
	}
	return 0;
}
//add end

int
has_BM_perm(struct userec *user, char *board)
{
	struct shortfile *x;
	char buf[256], *bm;

	x = getbcache(board);
	if (x == 0)
		return 0;
	if (user_perm(user, PERM_BLEVELS))
		return 1;
	//added by hongliang on Nov. 23th, 2003 for District Master
	if (user_perm(user, PERM_SPECIAL7) && has_DM_perm(user, board))
		return 1;

	if (!user_perm(user, PERM_BOARDS))
		return 0;
	if(has_parent_perm(user, board))
		return 1;
	strcpy(buf, x->BM);
	bm = strtok(buf, ",: ;&()\n");
	while (bm)
	{
		if (!strcasecmp(bm, user->userid))
			return 1;
		bm = strtok(0, ",: ;&()\n");
	}
	if (has_badmin_perm(user, x))
		return 1;
	return 0;
}



int
has_badmin_perm(struct userec *user, struct shortfile *x)
{
	FILE *fp;
	char buf[256], code[80], userid[80];
	int ret = 0;

	fp = fopen("etc/admins", "r");
	if (!fp)
		return 0;
	while (1)
	{
		if (fgets(buf, 255, fp) == 0)
			break;
		if (buf[0] == '#')
			continue;
		userid[0] = 0;
		sscanf(buf, "%s %s", code, userid);
		if (!strcmp(userid, currentuser.userid))
		{
			if (strchr(code, x->title[0]))
				ret = 1;
			break;
		}
	}
	fclose(fp);
	return ret;
}

/**
 * ugly code :(
 * if this board need any special user perm to read, 
 * when call has_read_perm(), is_normal_board will be set to 0
 * otherwise, set to 1
 * default is 0
 * by jawahh
 **/
int is_normal_board = 0;
int
has_read_perm(struct userec *user, char *board)
{
	struct shortfile *x;	/* °åÃæ²»´æÔÚ·µ»Ø0, pºÍz°åÃæ·µ»Ø1, ÓÐÈ¨ÏÞ°åÃæ·µ»Ø1. */
	char pathbuf[80];
	FILE *fp;

	fp = NULL;

	if (board[0] <= 32)
		return 0;
	x = getbcache(board);
	if (x == 0)
		return 0;
	
	// add by zsong, sub-board, ignore all dirs
//	if (x->flag2[0] & BOARD_DIR_FLAG)
//		return 0;
	//add end

	if (x->level == 0)
	{
		sprintf(pathbuf, "boards/%s/board.allow", x->filename);
		fp = fopen(pathbuf, "r");
		if (fp == NULL)
		{
			is_normal_board = 1;
			return 1;
		}
		else
		{
			fclose(fp);
			if (file_has_word(pathbuf, user->userid))
				return 1;
			else
				return 0;
		}
	}



	if (x->level & (PERM_POSTMASK | PERM_NOZAP))
	{
		sprintf(pathbuf, "boards/%s/board.allow", x->filename);
		fp = fopen(pathbuf, "r");
		if (fp == NULL)
			return 1;
		else
		{
			fclose(fp);
			if (file_has_word(pathbuf, user->userid))
				return 1;
			else
				return 0;
		}
	}

	if (!user_perm(user, PERM_BASIC))
		return 0;

	if (user_perm(user, x->level))
	{
		sprintf(pathbuf, "boards/%s/board.allow", x->filename);
		fp = fopen(pathbuf, "r");
		if (fp == NULL)
			return 1;
		else
		{
			fclose(fp);
			if (file_has_word(pathbuf, user->userid))
				return 1;
			else
				return 0;
		}
	}

	return 0;
}

int
ci_strcmp(register char *s1, register char *s2)
{
	char c1, c2;

	while (1)
	{
		c1 = *s1++;
		c2 = *s2++;
		if (c1 >= 'a' && c1 <= 'z')
			c1 += 'A' - 'a';
		if (c2 >= 'a' && c2 <= 'z')
			c2 += 'A' - 'a';
		if (c1 != c2)
			return (c1 - c2);
		if (c1 == 0)
			return 0;
	}
}

struct deny denyuser[256];
int denynum = 0;

int
loaddenyuser(char *board)
{
	FILE *fp;
	char path[80], buf[256];

	sprintf(path, "boards/%s/deny_users", board);
	fp = fopen(path, "r");
	if (fp == 0)
		return 0;
	while (denynum < 100)
	{
		if (fgets(buf, 80, fp) == 0)
			break;
		sscanf(buf, "%s %s %s %s", denyuser[denynum].id,
		       denyuser[denynum].exp, denyuser[denynum].autoundeny,
		       denyuser[denynum].free_time);
		denynum++;
	}
	fclose(fp);
	return 0;
}

int
inform(char *board, char *user)
{
	FILE *fp;
	char path[80], title[80];
	time_t now = time(0);

	sprintf(title, "[¹«¸æ] »Ö¸´%sÔÚ%s°åµÄPOSTÈ¨", user, board);
	sprintf(path, "tmp/%d.tmp", getpid());
	fp = fopen(path, "w");
	fprintf(fp, "¡¾±¾¹«¸æÓÉ×Ô¶¯·¢ÐÅÏµÍ³×Ô¶¯ÕÅÌù¡¿\n\n");
	fprintf(fp, "%s±»°åÎñÈËÔ±[%s]»Ö¸´ÁË%s°åPOSTÈ¨.\n", user,
		currentuser.userid, board);
	fclose(fp);
	post_article(board, title, path, "SJTUBBS", "×Ô¶¯·¢ÐÅÏµÍ³",
		     "×Ô¶¯·¢ÐÅÏµÍ³", -1, 0, now, now);

	//BUG
	//Bug-description:  web can not auto post a article to DENY_POST_BOARDNAME('penalty' in SJTUBBS)
	//Bug-solution:             post it :)
	//Bug-fixer:                hongliang
	//Date:                     Nov. 21th, 2003 14:14
	//start-hongliang
#ifdef DENY_POST_BOARDNAME

	if (strncmp(board, "BMTraining", 10) == 0)
	{
		post_article(TRAIN_ANNOUNCE_BOARDNAME, title, path, "SJTUBBS",
			     "×Ô¶¯·¢ÐÅÏµÍ³", "×Ô¶¯·¢ÐÅÏµÍ³", -1, 0, now, now);
	}
	else
	{
		post_article(DENY_POST_BOARDNAME, title, path, "SJTUBBS",
			     "×Ô¶¯·¢ÐÅÏµÍ³", "×Ô¶¯·¢ÐÅÏµÍ³", -1, 0, now, now);
	}
#endif
	//end-hongliang

	post_mail(user, title, path, currentuser.userid, currentuser.username,
		  fromhost, -1);
	unlink(path);
	printf("ÏµÍ³ÒÑ¾­·¢ÐÅÍ¨ÖªÁË%s.<br>\n", user);
	return 0;
}


void
inform_autoundeny(char *board, char *user)
{
	FILE *fp;
	char path[80], title[80];
	time_t now = time(0);

	sprintf(title, "[¹«¸æ] »Ö¸´%sÔÚ%s°åµÄPOSTÈ¨", user, board);
	sprintf(path, "tmp/%d.tmp", getpid());
	fp = fopen(path, "w");
	fprintf(fp, "¡¾±¾¹«¸æÓÉ×Ô¶¯·¢ÐÅÏµÍ³×Ô¶¯ÕÅÌù¡¿\n\n");
	fprintf(fp,
		"\n  %s ÍøÓÑ£º\n\n\tÒò·â½ûÊ±¼äÒÑ¹ý£¬ÏÖ»Ö¸´ÄúÔÚ [%s] °åµÄ¡º·¢±íÎÄÕÂ¡»È¨Á¦¡£\n\n"
		"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tBy SJTUBBS", user, board);
	fclose(fp);
	post_article(board, title, path, "SJTUBBS", "×Ô¶¯·¢ÐÅÏµÍ³",
		     "×Ô¶¯·¢ÐÅÏµÍ³", -1, 0, now, now);

#ifdef DENY_POST_BOARDNAME
	if (strncmp(board, "BMTraining", 10) == 0)
	{
		post_article(TRAIN_ANNOUNCE_BOARDNAME, title, path, "SJTUBBS",
			     "×Ô¶¯·¢ÐÅÏµÍ³", "×Ô¶¯·¢ÐÅÏµÍ³", -1, 0, now, now);
	}
	else
	{
		post_article(DENY_POST_BOARDNAME, title, path, "SJTUBBS",
			     "×Ô¶¯·¢ÐÅÏµÍ³", "×Ô¶¯·¢ÐÅÏµÍ³", -1, 0, now, now);
	}
#endif

	post_mail(user, title, path, currentuser.userid, currentuser.username,
		  fromhost, -1);
	unlink(path);
}


int
savedenyuser(char *board)
{
	FILE *fp;
	int i;
	char path[80], *exp;

	sprintf(path, "boards/%s/deny_users", board);
	fp = fopen(path, "w");
	if (fp == 0)
		return 0;
	for (i = 0; i < denynum; i++)
	{
		int m;

		exp = denyuser[i].exp;
		if (denyuser[i].id[0] == 0)
			continue;
		for (m = 0; exp[m]; m++)
		{
			if (exp[m] <= 32 && exp[m] > 0)
				exp[m] = '.';
		}
		//fprintf(fp, "%-12s %s %d\n", denyuser[i].id, denyuser[i].exp, denyuser[i].free_time);
		fprintf(fp, "%-12s %-40s %s %16s\n", denyuser[i].id,
			denyuser[i].exp, denyuser[i].autoundeny,
			denyuser[i].free_time);
		denynum++;
	}
	fclose(fp);
	return 0;
}

//this function is added by hongliang on Nov. 23th, 2003 13:00 for auto save deny users
void
bbsautoundeny(char board[80], char *userid)
{
	int i;

	loaddenyuser(board);
	for (i = 0; i < denynum; i++)
	{
		if (!strcasecmp(denyuser[i].id, userid))
		{
			denyuser[i].id[0] = 0;
			savedenyuser(board);
			inform_autoundeny(board, userid);
		}
	}
}

//this function is added by hongliang on Nov. 23th, 2003 for auto undeny user
int
seek_in_denyfile(char *board, char *seekstr)
{
	FILE *fp;
	char buf[STRLEN], buff[STRLEN];
	char *namep, *date;
	char flag;
	time_t nowtime;

	sprintf(buff, "boards/%s/deny_users", board);
	if ((fp = fopen(buff, "r")) == NULL)
		return 0;
	while (fgets(buf, STRLEN, fp) != NULL)
	{
		namep = (char *) strtok(buf, ": \n\r\t");
		if (namep != NULL && ci_strcmp(namep, seekstr) == 0)
		{
			fclose(fp);
			nowtime = time(0);
			getdatestring(nowtime, NA);
			flag = buf[55];
			date = (char *) (buf + 58);
			if (flag == 'a' && strncmp(datestring, date, 16) >= 0)
			{
				bbsautoundeny(board, seekstr);
				return 0;
			}
			return 1;
		}
	}
	fclose(fp);
	return 0;
}

int
has_post_perm(struct userec *user, char *board)
{
	char buf3[256];
	struct shortfile *x;

	x = getbcache(board);

	if (x == 0)
		return 0;
	strcpy(board, x->filename);
	if (!strcasecmp(board, "Forum") && !loginok)
	{
		sprintf(buf3, "boards/%s/deny_users", x->filename);
		if (file_has_word(buf3, "guest"))
			return 0;
		return 1;
	}
	if (!has_read_perm(user, board))
		return 0;
	if (x == 0)
		return 0;
	if (!loginok)
		return 0;
	sprintf(buf3, "boards/%s/deny_users", x->filename);
	if (file_has_word(buf3, user->userid))
	{
		//added by hongliang on Nov. 23th, 2003 for auto undeny
		if (seek_in_denyfile(board, user->userid))
		{
			//still denying
			return 0;
		}
		else
		{
			//auto undeny
			return 1;
		}
	}
	if (!HAS_PERM(PERM_BASIC))
		return 0;
	if (!strcasecmp(board, "sysop")	/* && user_perm(user, PERM_LOGINOK) */
		)
		return 1;
	if (!strcasecmp(board, "BBSHelp")
	    /*&& !user_perm(user, PERM_LOGINOK) */ )
		return 1;
	//next two lines deleted,bu acdsee
	//if (!strcasecmp(board, "Appeal") && user_perm(user, PERM_LOGINOK))
	//    return 1;
	if (user_perm(user, PERM_SYSOP))
		return 1;
	if (!user_perm(user, PERM_BASIC))
		return 0;
	if (!user_perm(user, PERM_POST))
		return 0;
#ifdef REAL_NAME_EMAIL
	if (!user_perm(user, PERM_REALNAME))
		return 0;
#endif
	if (!(x->level & PERM_NOZAP) && x->level
	    && !user_perm(user, x->level))
		return 0;
	return 1;
}

struct shortfile *
getbcache(char *board)
{
	int i;

	if (board[0] == 0)
		return 0;
	for (i = 0; i < MAXBOARD; i++)
		if (!strcasecmp(board, shm_bcache->bcache[i].filename))
			return &shm_bcache->bcache[i];
	return 0;
}

int
getbnum(char *board)
{
	int i;

	if (board[0] == 0)
		return 0;
	for (i = 0; i < MAXBOARD; i++)
		if (!strcasecmp(board, shm_bcache->bcache[i].filename))
			return i + 1;
	return 0;
}

int
count_on_board(char *board)
{
	int i, j, s = 0;

	i = getbnum(board);
	if (i <= 0 || i > MAXBOARD)
		return 0;
	for (j = 0; j < MAXACTIVE; j++)
	{
		if (shm_utmp->uinfo[j].userid[0] < ' '
		    || !shm_utmp->uinfo[j].active || !shm_utmp->uinfo[j].pid)
			continue;
		//        if(shm_utmp->uinfo[j].board==i) s++;
	}
	return s;
}

int
count_mails(char *id, int *total, int *unread)
{
	struct fileheader x1;
	char buf[256];
	FILE *fp;

	*total = 0;
	*unread = 0;
	if (getuser(id) == 0)
		return 0;
	sprintf(buf, "%s/mail/%c/%c/%s/.DIR", BBSHOME, toupper(id[0]), toupper(id[1]), id);
	fp = fopen(buf, "r");
	if (fp == 0)
		return 0;
	while (fread(&x1, sizeof(x1), 1, fp) > 0)
	{
		(*total)++;
		if (!(x1.accessed[0] & FILE_READ))
			(*unread)++;
	}
	fclose(fp);
	return 0;
}

int
findnextutmp(char *id, int from)
{
	int i;

	if (from < 0)
		from = 0;
	for (i = from; i < MAXACTIVE; i++)
		if (shm_utmp->uinfo[i].active)
			if (!strcasecmp(shm_utmp->uinfo[i].userid, id))
				return i;
	return -1;
}

int
sethomefile(char *buf, char *id, char *file)
{
	sprintf(buf, "home/%c/%c/%s/%s", toupper(id[0]), toupper(id[1]), id, file);
	return 0;
}

int
add_web_msg(char *id)
{
	int i;
	FILE *fp;
	char buf[1048], file[256];

	memset(buf, 0x20, 1048);
	buf[1047] = 0x00;

	sprintf(file, "home/%c/%c/%s/.msgdir", toupper(id[0]), toupper(id[1]), id);
	i = file_size(file) / sizeof(struct msgdir);
	if (get_record(buf, sizeof(struct msgdir), i - 1, file) <= 0)
		return 0;
	for (i = 0; i < 1048; i++)
	{
		if (buf[i] == 0x00)
			buf[i] = 0x20;
	}
	sprintf(file, "home/%c/%c/%s/wwwmsg", toupper(id[0]), toupper(id[1]), id);
	fp = fopen(file, "a");
	fwrite(buf, sizeof(struct msgdir), 1, fp);
	fclose(fp);
	return 0;

}

int
safewrite(int fd, char *buf, int size)
{
	int cc, sz = size, origsz = size;
	char *bp = buf;

	do
	{
		cc = write(fd, bp, sz);
		if ((cc < 0) && (errno != EINTR))
		{
			//                        report("safewrite err!");
			return -1;
		}
		if (cc > 0)
		{
			bp += cc;
			sz -= cc;
		}
	}
	while (sz > 0);
	return origsz;
}

		  //add by rwzmm@sjtubbs
	  //write msg to targetuser's .msgdir
int
f_append_msg(char *path, char *uid, char *msg)
{
	struct msgdir m;
	int fd;
	// tracerecord("begin append!\n");
	if ((fd = open(path, O_WRONLY | O_CREAT, 0644)) == -1)
	{
		//            report("open file error in append_msg()");
		// tracerecord("open file error in append_msg()\n");
		return -1;
	}
	memset(&m, 0, sizeof(m));
	strncpy(m.userid, uid, IDLEN + 2);
	m.pid = getpid();	//here may cause a bug. worned by rwzmm@sjtubbs
	m.tt = time(NULL);
	strncpy(m.msg, msg, MAX_MSG_SIZE);
	flock(fd, LOCK_EX);
	lseek(fd, 0, SEEK_END);
	if (safewrite(fd, (char *) &m, sizeof(m)) == -1)
		// tracerecord("apprec write err!\n");
		//            report("apprec write err!");
		flock(fd, LOCK_UN);
	close(fd);

	// tracerecord("end append!\n");
	return 0;

}


int
send_msg(char *myuserid, int mypid, char *touserid, int topid,
	 char msg[256], int isWeb)
{
	char msgbuf[256], msgbuf2[256], buf3[256];
	unsigned int i;

	for (i = 0; i < strlen(msg); i++)
		if ((0 < msg[i] && msg[i] <= 27) || msg[i] == -1)
			msg[i] = 32;
	if (mypid <= 0)
		return -1;
	sprintf(msgbuf,
		"[0;1;44;36m%-12.12s[33m([36m%-5.5s[33m):[37m%-54.54s[31m(^Z»Ø)[m[%05dm\n",
		myuserid, Ctime(time(0)) + 11, msg, mypid);
	sprintf(msgbuf2, "[1;32;40m·¢ËÍ£º[1;33;40m%s[m (%5.5s):%s\n",
		touserid, Ctime(time(0)) + 11, msg);

	sethomefile(buf3, touserid, "msgfile");
	f_append(buf3, msgbuf);

	sethomefile(buf3, touserid, "msgfile.me");
	f_append(buf3, msgbuf);

	//add by rwzmm@sjtubbs
	//write msg to targetuser's .msgdir
	sethomefile(buf3, touserid, ".msgdir");
	//  tracerecord("msgdir is open!\n");
	f_append_msg(buf3, myuserid, msgbuf);	//rwzmm add this function
	// tracerecord("msgdir is colsed!\n");

	sethomefile(buf3, myuserid, "msgfile.me");
	f_append(buf3, msgbuf2);
	if (isWeb)		/* ·¢ËÍ¸øwebÓÃ»§ */
	{
		add_web_msg(touserid);
		return 0;
	}
	/*·¢ËÍ¸øtelnet */
	if (topid <= 0)
		return -1;
//    kill(topid, SIGTTOU); //this signal isn't used, why here?
	kill(topid, SIGUSR2);
	return 0;
}

char *
horoscope(int month, int day)
{
	int date = month * 100 + day;

	if (month < 1 || month > 12 || day < 1 || day > 31)
		return "²»Ïê";
	if (date < 121 || date >= 1222)
		return "Ä¦ôÉ×ù";
	if (date < 219)
		return "Ë®Æ¿×ù";
	if (date < 321)
		return "Ë«Óã×ù";
	if (date < 421)
		return "ÄµÑò×ù";
	if (date < 521)
		return "½ðÅ£×ù";
	if (date < 622)
		return "Ë«×Ó×ù";
	if (date < 723)
		return "¾ÞÐ·×ù";
	if (date < 823)
		return "Ê¨×Ó×ù";
	if (date < 923)
		return "´¦Å®×ù";
	if (date < 1024)
		return "Ìì³Ó×ù";
	if (date < 1123)
		return "ÌìÐ«×ù";
	if (date < 1222)
		return "ÉäÊÖ×ù";
	return "";
}

char *
ModeType(int mode)
{
	switch (mode)
	{
	case IDLE:
		return "";
	case NEW:
		return "ÐÂÕ¾ÓÑ×¢²á";
	case LOGIN:
		return "½øÈë±¾Õ¾";
	case DIGEST:
		return "ä¯ÀÀ¾«»ªÇø";
	case MMENU:
		return "Ö÷Ñ¡µ¥";
	case ADMIN:
		return "¹ÜÀíÕßÑ¡µ¥";
	case SELECT:
		return "Ñ¡ÔñÌÖÂÛÇø";
	case READBRD:
		return "ÀÀ±éÌìÏÂ";
	case READNEW:
		return "ÀÀÐÂÎÄÕÂ";
	case READING:
		return "Æ·Î¶ÎÄÕÂ";
	case POSTING:
		return "·¢±íÎÄÕÂ";
	case MAIL:
		return "´¦ÀíÐÅ¼ã";
	case SMAIL:
		return "¼ÄÓïÐÅ¸ë";
	case RMAIL:
		return "ÔÄÀÀÐÅ¼ã";
	case TMENU:
		return "ÁÄÌìÑ¡µ¥";
	case LUSERS:
		return "»·¹ËËÄ·½";
	case FRIEND:
		return "Ñ°ÕÒºÃÓÑ";
	case MONITOR:
		return "Ì½ÊÓÃñÇé";
	case QUERY:
		return "²éÑ¯ÍøÓÑ";
	case TALK:
		return "ÁÄÌì";
	case PAGE:
		return "ºô½Ð";
	case CHAT1:
		return "Chat1";
	case CHAT2:
		return "Chat2";
	case CHAT3:
		return "Chat3";
	case CHAT4:
		return "Chat4";
	case LAUSERS:
		return "Ì½ÊÓÍøÓÑ";
	case XMENU:
		return "ÏµÍ³×ÊÑ¶";
	case VOTING:
		return "Í¶Æ±";
	case EDITWELC:
		return "±à¼­Welc";
	case EDITUFILE:
		return "±à¼­¸öÈËµµ";
	case EDITSFILE:
		return "±àÐÞÏµÍ³µµ";
	case ZAP:
		return "¶©ÔÄÌÖÂÛÇø";
	case LOCKSCREEN:
		return "ÆÁÄ»Ëø¶¨";
	case NOTEPAD:
		return "ÁôÑÔ°å";
	case GMENU:
		return "¹¤¾ßÏä";
	case MSG:
		return "ËÍÑ¶Ï¢";
	case USERDEF:
		return "×Ô¶©²ÎÊý";
	case EDIT:
		return "ÐÞ¸ÄÎÄÕÂ";
	case OFFLINE:
		return "×ÔÉ±ÖÐ..";
	case EDITANN:
		return "±àÐÞ¾«»ª";
	case LOOKMSGS:
		return "²ì¿´Ñ¶Ï¢";
		//case WFRIEND:   return "Ñ°ÈËÃû²á";
	case WNOTEPAD:
		return "Óû×ß»¹Áô";
	case BBSNET:
		return "BBSNET..";
	case 10001:
		return "Webä¯ÀÀ";
	default:
		return "È¥ÁËÄÄ¶ù??";
	}
}

/*
char   *cexp(int exp) {
        if(exp==-9999) 	return "Ã»µÈ¼¶";
        if(exp<=100)  	return "ÐÂÊÖÉÏÂ·";
        if(exp<=450)	return "Ò»°ãÕ¾ÓÑ";
        if(exp<=850)	return "ÖÐ¼¶Õ¾ÓÑ";
        if(exp<=1500)	return "¸ß¼¶Õ¾ÓÑ";
        if(exp<=2500)	return "ÀÏÕ¾ÓÑ";
        if(exp<=3000)	return "³¤ÀÏ¼¶";
        if(exp<=5000)	return "±¾Õ¾ÔªÀÏ";
        if(exp<=10000)	return "¿ª¹ú´óÀÏ";
        if(exp<=50000)  return "³¬¼¶´óÀÏ";
	return "·µÆÓ¹éÕæ";
}
*/
char *
cexp0(int exp, int type)
{
	char *cexp2(), *cexp3();

	if (type == 1)
		return cexp2(exp);
	if (type == 2)
		return cexp3(exp);
	if (type == 3)
		return "";
	if (exp == -9999)
		return "Ã»µÈ¼¶";
	if (exp <= 100)
		return "ÐÂÊÖÉÏÂ·";
	if (exp <= 450)
		return "Ò»°ãÕ¾ÓÑ";
	if (exp <= 850)
		return "ÖÐ¼¶Õ¾ÓÑ";
	if (exp <= 1500)
		return "¸ß¼¶Õ¾ÓÑ";
	if (exp <= 2500)
		return "ÀÏÕ¾ÓÑ";
	if (exp <= 3000)
		return "³¤ÀÏ¼¶";
	if (exp <= 5000)
		return "±¾Õ¾ÔªÀÏ";
	if (exp <= 10000)
		return "¿ª¹ú´óÀÏ";
	if (exp <= 50000)
		return "³¬¼¶´óÀÏ";
	return "·µè±¹éÕæ";
}

char *
cexp2(int exp)
{
	if (exp <= 100)
		return "ÁÐ±ø";
	if (exp <= 300)
		return "ÉÏµÈ±ø";
	if (exp <= 500)
		return "ÏÂÊ¿";
	if (exp <= 700)
		return "ÖÐÊ¿";
	if (exp <= 1000)
		return "ÉÏÊ¿";
	if (exp <= 1500)
		return "¾üÊ¿³¤";
	if (exp <= 2000)
		return "ÉÙÎ¾";
	if (exp <= 3000)
		return "ÖÐÎ¾";
	if (exp <= 4000)
		return "ÉÏÎ¾";
	if (exp <= 5000)
		return "ÉÙÐ£";
	if (exp <= 6000)
		return "ÖÐÐ£";
	if (exp <= 7000)
		return "ÉÏÐ£";
	if (exp <= 8000)
		return "×¼½«";
	if (exp <= 9000)
		return "ÉÙ½«";
	if (exp <= 10000)
		return "ÖÐ½«";
	if (exp <= 12000)
		return "ÉÏ½«";
	return "ÔªË§";
}

char *
cexp3(int exp)
{
	if (exp <= 150)
		return "»¨×Ñ";
	if (exp <= 500)
		return "ÎÞÃûÐ¡»¨";
	if (exp <= 2000)
		return "¿Õ¹ÈÓÄÀ¼";
	if (exp <= 3000)
		return "ÓÀºãÃµ¹å";
	if (exp <= 10000)
		return "°ÙºÏÌìÏÉ";
	return "»¨Éñ";
}

char *
cperf(int perf)
{
	if (perf == -9999)
		return "Ã»µÈ¼¶";
	if (perf <= 5)
		return "¸Ï¿ì¼ÓÓÍ";
	if (perf <= 12)
		return "Å¬Á¦ÖÐ";
	if (perf <= 35)
		return "»¹²»´í";
	if (perf <= 50)
		return "ºÜºÃ";
	if (perf <= 90)
		return "ÓÅµÈÉú";
	if (perf <= 140)
		return "Ì«ÓÅÐãÁË";
	if (perf <= 200)
		return "±¾Õ¾Ö§Öù";
	if (perf <= 500)
		return "Éñ¡«¡«";
	if (perf <= 2500)
		return "»úÆ÷ÈË!";
	return "³¬¼¶»úÆ÷ÈË!";
}

int
count_life_value(struct userec *urec)
{
	int value;

	if ((urec->userlevel & PERM_XEMPT)
	    || strcmp(urec->userid, "SYSOP") == 0
	    || strcmp(urec->userid, "guest") == 0)
		return 999;
/*
	if(urec->userlevel & PERM_666LIFE)
		return 666;*/

	value = (time(0) - urec->lastlogin) / 60;
	/* new user should register in 30 mins */
	if (strcmp(urec->userid, "new") == 0)
	{
		return (30 - value) * 60;
	}
	if (urec->userlevel & PERM_666LIFE)
	{
		return (666 * 1440 - value) / 1440;
	}
	if (urec->numlogins <= 3)
		return (15 * 1440 - value) / 1440;
	if (!(urec->userlevel & PERM_LOGINOK))
		return (30 * 1440 - value) / 1440;
	if (urec->stay > 1000000)
		return (365 * 1440 - value) / 1440;
	return (120 * 1440 - value) / 1440;
}

int
countexp(struct userec *x)
{
	int tmp;

	if (!strcasecmp(x->userid, "guest"))
		return -9999;
	tmp = x->numposts + x->numlogins / 3 + (time(0) -
						x->firstlogin) / 86400 +
		x->stay / 3600;
	if (tmp < 0)
		tmp = 0;
	return tmp;
}

int
countperf(struct userec *x)
{
	int day, logins, posts;

	if (!strcasecmp(x->userid, "guest"))
		return -9999;
	day = (time(0) - x->firstlogin) / 86400 + 1;
	logins = x->numlogins;
	posts = x->numposts;
	if (day <= 0 || logins <= 0 || posts < 0)
		return 0;
	return (10 * posts / logins + 10 * logins / day);
}

int
modify_mode(struct user_info *x, int newmode)
{
	if (x == 0)
		return -1;
	x->mode = newmode;
	return 0;
}

int
save_user_data(struct userec *x)
{
	FILE *fp;
	int n;

	n = getusernum(x->userid);
	if (n < 0 || n > 1000000)
		return 0;
	fp = fopen(".PASSWDS", "r+");
	if (fp == 0)
		return 0;
	fseek(fp, n * sizeof(struct userec), SEEK_SET);
	fwrite(x, sizeof(struct userec), 1, fp);
	fclose(fp);
	return 1;
}

int
is_bansite(char *ip)
{
	FILE *fp;
	char buf3[256];

	fp = fopen(".bansite", "r");
	if (fp == 0)
		return 0;
	while (fscanf(FCGI_ToFILE(fp), "%s", buf3) > 0)
		if (!strcasecmp(buf3, ip))
			return 1;
	fclose(fp);
	return 0;
}

int
user_perm(struct userec *x, int level)
{
	return (x->userlevel & level);
}

int
getusernum(char *id)
{
	int i;

	if (id[0] == 0)
		return -1;
	for (i = 0; i < MAXUSERS; i++)
	{
		if (!strcasecmp(shm_ucache->userid[i], id))
			return i;
	}
	return -1;
}

int
getuser_like(char *userid)
{
	int i;

	for (i = 0; i < MAXUSERS; i++)
	{
		if (str_is_like(userid, shm_ucache->userid[i]))
			return i + 1;
	}
	return 0;
}


int
str_is_like(char *userid, char *userid2)
{
	int i;

	if (strlen(userid) != strlen(userid2))
		return 0;
	for (i = 0; userid[i] && i < 14; i++)
	{
		if (toupper(userid[i]) == toupper(userid2[i]))
			continue;
		if (strchr("0Oo", userid[i]) && strchr("0Oo", userid2[i]))
			continue;
		if (strchr("Il1", userid[i]) && strchr("Il1", userid2[i]))
			continue;
		return 0;
	}
	return 1;
}

struct userec *
getuser(char *id)
{
	static struct userec userec1;
	int uid;
	FILE *fp;

	if(id==NULL || id[0]==0)
		return 0;
	uid = getusernum(id);
	if (uid < 0)
		return 0;
	fp = fopen(".PASSWDS", "r");
	fseek(fp, uid * sizeof(userec1), SEEK_SET);
	bzero(&userec1, sizeof(userec1));
	fread(&userec1, sizeof(userec1), 1, fp);
	fclose(fp);
	return &userec1;
}

int
checkpasswd(char *pw_crypted, char *pw_try)
{
	return !strcmp(crypt1(pw_try, pw_crypted), pw_crypted);
}

char *
crypt_passwd(char *pw)
{
	char salt[3];

	sprintf(salt, "%c%c", 65 + rand() % 26, 65 + rand() % 26);
	return crypt1(pw, salt);
}

int
checkuser(char *id, char *pw)
{
	struct userec *x;

	x = getuser(id);
	if (x == 0)
		return 0;
	return checkpasswd(x->passwd, pw);
}

int
count_id_num(char *id)
{
	int i, total = 0;

	for (i = 0; i < MAXACTIVE; i++)
		if (shm_utmp->uinfo[i].active
		    && !strcasecmp(shm_utmp->uinfo[i].userid, id))
			total++;
	return total;
}

int
count_online()
{
	int i, total = 0;

	for (i = 0; i < MAXACTIVE; i++)
		if (shm_utmp->uinfo[i].active)
			total++;
	return total;
}

int
count_online2()
{
	int i, total = 0;

	for (i = 0; i < MAXACTIVE; i++)
		if (shm_utmp->uinfo[i].active
		    && shm_utmp->uinfo[i].invisible == 0)
			total++;
	return total;
}

struct override fff[200];
int friendnum = 0;
int friend_inited = 0;

int
loadfriend(char *id)
{
	FILE *fp;
	char file[256];

	friend_inited = 1;
	if (!loginok)
		return -1;
	sprintf(file, "home/%c/%c/%s/friends", toupper(id[0]), toupper(id[1]), id);
	fp = fopen(file, "r");
	if (fp)
	{
		friendnum = fread(fff, sizeof(fff[0]), 200, fp);
		fclose(fp);
	}
	return 0;
}

int
isfriend(char *id)
{
	int n;

	if (!friend_inited)
		loadfriend(currentuser.userid);
	for (n = 0; n < friendnum; n++)
		if (!strcasecmp(id, fff[n].id))
			return 1;
	return 0;
}

struct override bbb[32];
int badnum = 0;

int
loadbad(char *id)
{
	FILE *fp;
	char file[256];

	if (!loginok)
		return -1;
	sprintf(file, "home/%c/%c/%s/rejects", toupper(id[0]), toupper(id[1]), id);
	fp = fopen(file, "r");
	if (fp)
	{
		badnum = fread(bbb, sizeof(bbb[0]), 30, fp);
		fclose(fp);
	}
	return 0;
}

int
isbad(char *id1, char *id2)
{
	int n;

	loadbad(id1);
	for (n = 0; n < badnum; n++)
		if (!strcasecmp(id2, bbb[n].id))
			return 1;
	return 0;
}

int
init_all()
{
	bbs_http_init();


	loginok = user_init(&currentuser, &u_info);
	g_message("%s %s %s?%s", cgiRemoteAddr, currentuser.userid,
		  cgiScriptName, cgiQueryString);
	pic_mod = 2;
	if (loginok || !strcasecmp("guest", currentuser.userid))
		u_info->idle_time = time(0);
	load_my_www();
	if (!silence && my_font_mode)
	{
		printf("<style type='text/css'>\n");
		printf("TABLE,TH,TD,P,INPUT,BODY,SELECT {FONT-SIZE: 14px;}");
		printf("</style>\n");
	}
	if (!silence && !my_fontn_mode)
	{
		printf("<style type='text/css'>\n");
		printf("TABLE,TH,TD,P,INPUT,BODY,SELECT {FONT-FAMILY: \"ËÎÌå\";}</style>\n");
		printf("</style>\n");
	}
	if (!silence && !my_ansisize_mode)
	{
		printf("<style type='text/css'>\n");
		printf("PRE, TEXTAREA {FONT-SIZE: 9pt; line-height: 9pt}</style>\n");
		printf("</style>\n");
	}
	if (!silence && my_ansifont_mode)
	{
		printf("<style type='text/css'>\n");
		printf("PRE, TEXTAREA {FONT-FAMILY: \"Verdana\", Tahoma, sans-serif, ËÎÌå;}</style>\n");
		printf("</style>\n");
	}
	if (!silence)
	{
		if (my_theme_mode == 1)
			printf("<link rel=stylesheet type=text/css href='"
			       UPLOAD_SERVER_HEADER "/blue.css'>\n");
		else if (my_theme_mode == 2)
			printf("<link rel=stylesheet type=text/css href='"
			       UPLOAD_SERVER_HEADER "/red.css'>\n");
	}
	return 0;
}

char *
void1(char *s)
{
	int i;
	int flag = 0;

	for (i = 0; s[i]; i++)
	{
		if (flag == 0)
		{
			if (s[i] > '\x80' && s[i] <'\xFF' )
				flag = 1;
		}
		else
		{
			flag = 0;
			if (s[i] < '\x20' && s[i]>'\0')
				s[i - 1] = '\x20';
		}
	}
	if (flag)
		s[i - 1] = '\0';
	return s;
}

char *
sec(char c)
{
	int i;

	for (i = 0; i < SECNUM; i++)
	{
		if (strchr(seccode[i], c))
			return secname[i][0];
	}
	return "(unknown.)";
}

char *
flag_str(int access)
{
	static char buf[80];
	char *flag1 = "", *flag2 = "";

	strcpy(buf, "  ");
	if (access & FILE_DIGEST)
		flag2 = "G";
	if (access & FILE_MARKED)
		flag2 = "M";
	if ((access & FILE_MARKED) && (access & FILE_DIGEST))
		flag2 = "B";
	sprintf(buf, "%s%s", flag1, flag2);
	return buf;
}

char *
flag_str2(int access)
{
	static char buf[80];
	strcpy(buf, "   ");
	if (access & FILE_DIGEST)
		buf[0] = 'G';
	if (access & FILE_MARKED)
		buf[0] = 'M';
	if ((access & FILE_MARKED) && (access & FILE_DIGEST))
		buf[0] = 'B';
	return buf;
}

char *
userid_str(char *s)
{
	static char buf[512];
	char buf2[256], tmp[256], *ptr, *ptr2;
	int len;

	strsncpy(tmp, s, 255);
	buf[0] = 0;
	ptr = strtok(tmp, " ,();\r\n\t");
	while (ptr && strlen(buf) < 400)
	{
		if ((ptr2 = strchr(ptr, '.')))
		{
			ptr2[1] = 0;
			ptr2 = strchr(ptr, '<');
			if (ptr2)
				ptr2[0] = ' ';
			strcat(buf, ptr);
			strcat(buf, " ");
		}
		else
		{
			ptr = nohtml(ptr);
			sprintf(buf2, "<a href=bbsqry?userid=%s>%s</a> ", ptr,
				ptr);
			strcat(buf, buf2);
		}
		ptr = strtok(0, " ,();\r\n\t");
	}
	len = strlen(buf);
	if (len > 0 && buf[len - 1] == ' ')
		buf[len - 1] = 0;
	return buf;
}

int
fprintf2(FILE * fp, char *s)
{
	int i, tail = 0, sum = 0;

	if (s[0] == ':' && s[1] == ' ' && strlen(s) > 79)
	{
		sprintf(s + 76, "..\n");
		fprintf(fp, "%s", s);
		return 0;
	}
	for (i = 0; s[i]; i++)
	{
		fprintf(fp, "%c", s[i]);
		sum++;
		if (tail)
		{
			tail = 0;
		}
		else if (s[i] < 0)
		{
			tail = s[i];
		}
		if (sum >= 78 && tail == 0)
		{
			fprintf(fp, "\n");
			sum = 0;
		}
	}
	return 0;
}

struct fileheader *
get_file_ent(char *board, char *file, int isPost)
{
	FILE *fp;
	char dir[80];
	static struct fileheader x;
	int num = 0;
	if(1 == isPost)
	{
		sprintf(dir, "boards/%s/.DIR", board);
	}
	else
	{
		sprintf(dir, "mail/%c/%c/%s/.DIR", toupper (currentuser.userid[0]), toupper (currentuser.userid[1]), currentuser.userid);
	}
	fp = fopen(dir, "r");
	while (1)
	{
		if (fread(&x, sizeof(x), 1, fp) <= 0)
			break;
		if (!strcmp(x.filename, file))
		{
			fclose(fp);
			return &x;
		}
		num++;
	}
	fclose(fp);
	return 0;
}

char *
getbfroma(char *path)
{
	FILE *fp;
	static char buf1[180], buf2[180];

	if (path[0] == 0)
		return "";
	path++;
	fp = fopen("0Announce/.Search", "r");
	if (fp == 0)
		return "";
	while (1)
	{
		bzero(buf1, sizeof(buf1));
		bzero(buf2, sizeof(buf2));
		if (fscanf(FCGI_ToFILE(fp), "%s %s", buf1, buf2) <= 0)
			break;
		if (buf1[0])
			buf1[strlen(buf1) - 1] = 0;
		if (buf1[0] == '*')
			continue;
		if (!strncmp(buf2, path, strlen(buf2))
		    && !strncmp(buf2, path, strlen(path)))
		{
			fclose(fp);
			return buf1;
		}
	}
	fclose(fp);
	return "";
}

int
load_my_www()
{
	FILE *fp;
	char path[256], buf[256], buf1[256], buf2[256];

	if (!loginok)
		return -1;
	sprintf(path, "home/%c/%c/%s/.mywww", toupper(currentuser.userid[0]), toupper(currentuser.userid[1]),
		currentuser.userid);
	fp = fopen(path, "r");
	if (fp)
	{
		while (1)
		{
			if (fgets(buf, 80, fp) == 0)
				break;
			if (sscanf(buf, "%80s %80s", buf1, buf2) != 2)
				continue;
			if (!strcmp(buf1, "t_lines"))
				my_t_lines = atoi(buf2);
			if (!strcmp(buf1, "link_mode"))
				my_link_mode = atoi(buf2);
			if (!strcmp(buf1, "def_mode"))
				my_def_mode = atoi(buf2);
			if (!strcmp(buf1, "font_mode"))
				my_font_mode = atoi(buf2);
			if (!strcmp(buf1, "fontn_mode"))
				my_fontn_mode = atoi(buf2);
			if (!strcmp(buf1, "ansisize_mode"))
				my_ansisize_mode = atoi(buf2);
			if (!strcmp(buf1, "ansifont_mode"))
				my_ansifont_mode = atoi(buf2);
			if (!strcmp(buf1, "theme_mode"))
				my_theme_mode = atoi(buf2);
		}
		fclose(fp);
	}
	return 0;
}

int
has_fill_form()
{
	FILE *fp;

//modified by rwzmm @ sjtubbs, new register form!
//the following code is for old register modula 

//      fp = fopen("new_register", "r");
//    if (fp == 0)
//        return 0;
//    while (1)
//    {
//        if (fgets(buf, 100, fp) == 0)
//            break;
//        r = sscanf(buf, "%s %s", tmp, userid);
//        if (r == 2)
//        {
//            if (!strcasecmp(tmp, "userid:")
//                    && !strcasecmp(userid, currentuser.userid))
//            {
//                fclose(fp);
//                return 1;
//            }
//        }
//    }
//    fclose(fp);
//    return 0;

//end

////the following code is for new register modula

	struct regform form;
	memset(&form,0,sizeof(form));
	fp = fopen("register_sjtubbs", "r");
	if (fp == NULL)
		return 0;
	while (fread(&form, sizeof(struct regform), 1, fp) > 0)
	{
		if (strncmp(form.userid, currentuser.userid, IDLEN) == 0)
		{
			fclose(fp);
			return 1;
		}
		memset(&form, 0x0, sizeof(struct regform));
	}
	fclose(fp);
	return 0;

////end


}

char *
nohtml2(char *s)
{
	static char buf[1024];

	hsprintf(buf, "%s", s);
	return buf;
}

char *
void3(char *s)
{
	static char buf[1024];
	int i = 0, j = 0;

	for (i = 0; s[i] && j < 1000; i++)
	{
		if (s[i] == 34)
		{
			sprintf(buf + j, "&#34;");
			j += 5;
		}
		else if (s[i] == 39)
		{
			sprintf(buf + j, "&#39;");
			j += 5;
		}
		else
		{
			buf[j] = s[i];
			j++;
		}
	}
	buf[j] = 0;
	return buf;
}

float
get_load(int i)
{
	static float x[3];
	static int last_atime = 0;
	FILE *fp;

	if (i < 0 || i > 2)
		return -1;
	if (abs(time(0) - last_atime) <= 5)
		return x[i];
	last_atime = time(0);
	fp = fopen("/proc/loadavg", "r");
	if (fp == 0)
		return -1;
	fscanf(FCGI_ToFILE(fp), "%g %g %g", &x[0], &x[1], &x[2]);
	fclose(fp);
	return x[i];
}



void *
file_map(char *file, int len)
{
	FILE *fp;
	void *buf;

	fp = fopen(file, file_exist(file) ? "r+" : "w+");
	if (fp == 0)
		return 0;
	if (file_size(file) != len)
		ftruncate(fileno(fp), len);
	buf = mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(fp), 0);
	fclose(fp);
	if (buf == MAP_FAILED)
		buf = 0;
	if (buf == 0)
		return 0;
	return buf;
}

int
check_maxmail2(int *maxmail, int *size)
{
	char filename[256], cmd[256], mailpath[256], maildir[256], tmpf[256];
	FILE *tempfile, *fp;

	*size = 0;
	*maxmail = 5000;
	if (!loginok)
		return 0;
	if (HAS_PERM(PERM_BASIC))
		*maxmail = 1000;
	if (HAS_PERM(PERM_BOARDS))
		*maxmail = 2000;
	if (HAS_PERM(PERM_CHATCLOAK))
		*maxmail = 3000;
	sprintf(mailpath, "mail/%c/%c/%s/", toupper(currentuser.userid[0]), toupper(currentuser.userid[1]),
		currentuser.userid);
	sprintf(tmpf, "%s.size", mailpath);
	sprintf(maildir, "%s.DIR", mailpath);
	if (file_time(maildir) == file_time(tmpf))
	{
		fp = fopen(tmpf, "r");
		if (fp)
		{
			fscanf(FCGI_ToFILE(fp), "%d %d", maxmail, size);
			fclose(fp);
		}
		return 1;
	}
	sprintf(filename, "/tmp/size.%s.%d", currentuser.userid, u_info->pid);
	sprintf(cmd, "/usr/bin/du -bk %s > %s", mailpath, filename);
	system(cmd);
	if ((tempfile = fopen(filename, "r")) == NULL)
		return 0;
	if ((fscanf(FCGI_ToFILE(tempfile), "%d", size)) == 0)
		return 0;
	fclose(tempfile);
	unlink(filename);
	fp = fopen(tmpf, "w");
	fprintf(fp, "%d %d\n", *maxmail, *size);
	fclose(fp);
	currentuser.nummails = file_size(maildir) / sizeof(struct fileheader);
	save_user_data(&currentuser);
	return 2;
}

char *
nohtml3(char *s)
{
	static char buf[512];
	int i, j = 0;

	bzero(buf, 512);
	for (i = 0; s[i] && j < 500; i++)
	{
		if (s[i] == '\'')
		{
			strcat(buf, "%27");
			j += 3;
		}
		else if (s[i] == '\"')
		{
			strcat(buf, "%22");
			j += 3;
		}
		else
		{
			buf[j] = s[i];
			j++;
		}
	}
	return buf;
}

char *
nohtml5(char *s)
{
	//BUG
	//Bug-description:  if the subject of the topic contains '+', '/', '-', it would crash.
	//Bug-solution:             change this function like rawurlencode() in php
	//Bug-fixer:                hongliang
	//Date:                     Nov. 19th, 2003 17:15
	static unsigned char hexchars[] = "0123456789ABCDEF";	//added by hongliang
	static char buf[512];
	int i, j = 0;

	bzero(buf, 512);
	//start-hongliang
	for (i = 0, j = 0; s[i] && j < 500; ++i, ++j)
	{
		buf[j] = (unsigned char) s[i];
		if ((buf[j] < '/' && buf[i] != '-' && buf[i] != '.')
		    || (buf[i] < 'A' && buf[i] > '9') || (buf[i] > 'Z'
							  && buf[i] < 'a'
							  && buf[i] != '_')
		    || (buf[i] > 'z'))
		{
			buf[j++] = '%';
			buf[j++] = hexchars[(unsigned char) s[i] >> 4];
			buf[j] = hexchars[(unsigned char) s[i] & 15];
		}
	}
	buf[j] = '\0';

	return buf;


}

char *
nohtml4(char *s)
{
	static char buf[512];
	int i, j = 0;

	bzero(buf, 512);
	for (i = 0; s[i] && i < 500; i++)
	{
		if (s[i] == '&')
		{
			strcat(buf, "&amp;");
			j += 5;
		}
		else if (s[i] == '<')
		{
			strcat(buf, "&lt;");
			j += 4;
		}
		else if (s[i] == '>')
		{
			strcat(buf, "&gt;");
			j += 4;
		}
		else
		{
			buf[j] = s[i];
			j++;
		}
	}
	return buf;
}

static char *days[] = {
	"Sun",
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat"
};

static char *months[] = {
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};

char *
Gtime(time_t t)
{
	static char buf[64];
	struct tm *gt;
	gt = gmtime(&t);
	sprintf(buf,
		"%s, %02d-%s-%04d %02d:%02d:%02d GMT",
		days[gt->tm_wday],
		gt->tm_mday,
		months[gt->tm_mon],
		gt->tm_year + 1900, gt->tm_hour, gt->tm_min, gt->tm_sec);
	return buf;
}

struct tm *
Ltime(time_t t)
{
	return localtime(&t);
}

int
Day(time_t t)
{
	struct tm *p;

	p = Ltime(t);
	return (p->tm_year + 1900) * 10000 + (p->tm_mon + 1) * 100 +
		p->tm_mday;
}

int
today(void)
{
	return Day(time(0));
}


char *
eff_size(char *file)
{
	static char buf[256];
	int size;

	size = (file_size(file) - 200) / 2;

	if (size < 0)
		size = 0;
	if (size < 2048)
	{
		sprintf(buf,
			"(<font style='font-size:12px; color:#008080'>%d×Ö</font>)",
			size);
	}
	else
	{
		sprintf(buf,
			"(<font style='font-size:12px; color:#f00000'>%d.%dK</font>)",
			size / 1000, (size / 100) % 10);
	}
	return buf;
}


int
base64(char c)
{
	if (c >= 'A' && c <= 'Z')
		return c - 'A';
	if (c >= 'a' && c <= 'z')
		return c - 'a' + 26;
	if (c >= '0' && c <= '9')
		return c - '0' + 52;
	if (c == '+')
		return 62;
	if (c == '/')
		return 63;
	return 0;
}

char *
unbase64(char *buf)
{
	static char buf2[256];
	int len = strlen(buf), i, j = 0;

	for (i = 0; i < len; i += 4)
	{
		if (buf[i] == '=' || buf[i] < ' ')
			break;
		buf2[j] = 4 * base64(buf[i]) + base64(buf[i + 1]) / 16;
		j++;
		if (buf[i + 2] == '=' || buf[i + 2] < ' ')
			break;
		buf2[j] =
			16 * (base64(buf[i + 1]) % 16) +
			base64(buf[i + 2]) / 4;
		j++;
		if (buf[i + 3] == '=' || buf[i + 3] < ' ')
			break;
		buf2[j] = 64 * (base64(buf[i + 2]) % 4) + base64(buf[i + 3]);
		j++;
	}
	buf2[j] = 0;
	return buf2;
}

int
junkboard(char *board)
{
	if (strstr(JUNKBOARDS, board))
		return 1;
	else
		return file_has_word("etc/junkboards", board);
	// add your code here;
}

int
LoadMyBrdsFromGoodBrd(char *userid, char mybrd[GOOD_BRC_NUM][80])
{
	FILE *fp;
	char fname[STRLEN];
	int i = 0;

	sprintf(fname, "home/%c/%c/%s/.goodbrd", toupper(userid[0]), toupper(userid[1]), userid);
	if ((fp = fopen(fname, "r")))
	{
		for (i = 0; i < GOOD_BRC_NUM; i++)
		{
			if (EOF == fscanf(FCGI_ToFILE(fp), "%s\n", mybrd[i]))
				break;
		}
		fclose(fp);
	}
	return i;
}

void
SaveMyBrdsToGoodBrd(char *userid,
		    char mybrd[GOOD_BRC_NUM][80], int mybrdnum)
{
	FILE *fp;
	char fname[STRLEN];
	int i;

	sprintf(fname, "home/%c/%c/%s/.goodbrd", toupper(userid[0]), toupper(userid[1]), userid);
	if ((fp = fopen(fname, "w")))
	{
		for (i = 0; i < mybrdnum; i++)
			fprintf(fp, "%s\n", mybrd[i]);
		fclose(fp);
	}

}

//add by mudboy 4 »ØÊÕÕ¾
void
post_article2(char *board, char *title, char *file, char *id,
	      unsigned int currid, unsigned int reid)
{
	FILE *fp;
	char buf[1024];
	struct fileheader header;
//      char temptitle[STRLEN];

//      memset(temptitle, 0x00, STRLEN);
	//strcpy(temptitle, header.title)
	bzero(&header, sizeof(header));

	strcpy(header.owner, id);
	header.id = currid;
	header.reid = reid;

	sprintf(header.filename, "%s", file);

//debug by rwzmm @ sjtubbs
//    strsncpy(header.title, title, 60);
	sprintf(header.title, "%-32.32s - %s", title, currentuser.userid);
//      strncpy(header.title, temptitle, STRLEN);

	header.accessed[0] = FILE_DELETED;
	sprintf(buf, "boards/%s/.RETURNDIR", board);
	fp = fopen(buf, "a");
	fwrite(&header, sizeof(header), 1, fp);
	fclose(fp);
}



void
strtolower(char *dst, char *src)
{
	for (; *src; src++)
		*dst++ = tolower(*src);
	*dst = '\0';
}

void
log_usies(char *mode, char *mesg)
{

	g_message("%s %s %s", currentuser.userid, mode, mesg);
}

//added by hongliang on Nov.21th, 2003 18:10 for
//Return a post from recycle bin
int
return_del_article(char *file, int size, int num)
{
	FILE *fp;
	int total, i;
	char buf[4096];

	if (size < 1 || size > 4096)
		return 0;
	total = file_size(file) / size;
	if (total < 1 || total > 1000000)
		return 0;
	fp = fopen(file, "r+");
	if (fp == 0)
		return 0;
	flock(fileno(fp), LOCK_EX);
	for (i = num + 1; i <= total - 1; i++)
	{
		fseek(fp, i * size, SEEK_SET);
		if (fread(buf, size, 1, fp) <= 0)
			break;
		fseek(fp, (i - 1) * size, SEEK_SET);
		fwrite(buf, size, 1, fp);
	}
	ftruncate(fileno(fp), (total - 1) * size);
	flock(fileno(fp), LOCK_UN);
	fclose(fp);
	return 1;
}





//created by rwzmm @ sjtubbs. ÐÂ×¢²áID 24Ð¡Ê±ÄÚ²»ÄÜPOST
int
too_young(struct userec *user, char *board)
{
	if (strcmp(board, "BBSHelp") == 0)
		return 0;

	if (strcmp(board, "sysop") == 0)
		return 0;

	if ((time(0) - user->firstlogin) > 86400)
		return 0;


	return 1;
}



	  // this function is for debug only, by rwzmm @ sjtubbs
	  // it must be annotated in release version.

	  //void tracerecord(char* buf, char* filename)
	  //{
	  //    FILE* fp;
	  //    char path[256];
	  //
	  //    sprintf(path, "/home/rwzmm/%s", filename);
	  //    fp = fopen(path, "w");
	  //    fwrite(buf, 256, 1, fp);
	  //    fclose(fp);
	  //}

	  //add end

int
kick_web_user(struct user_info *user)
{
	int stay = 0;
	FILE *fp;
	struct userec u;
	if (!user->active)
		return -1;
	stay = abs(time(0) - user->logintime);
	if (stay > 3600 * 24 * 3)
		stay = 3600 * 24 * 3;
	else if (stay < 0)
		stay = 0;

	fp = fopen(".PASSWDS", "r+");
	fseek(fp, (user->uid - 1) * sizeof(struct userec), SEEK_SET);
	fread(&u, sizeof(struct userec), 1, fp);
	if (strcasecmp(u.userid, user->userid))
	{
		fclose(fp);
		memset(user, 0, sizeof(struct user_info));
		return -2;
	}
	u.stay += stay;
	u.lastlogout = time(0);
	fseek(fp, -sizeof(struct userec), SEEK_CUR);
	fwrite(&u, sizeof(struct userec), 1, fp);
	fclose(fp);
	memset(user, 0, sizeof(struct user_info));
	return 0;
}


//added by littlesword@SJTUbbs
//show dm and bm

void
show_dm(char *userid)
{
	int dist = 12;

	dist = WHICHDIST(userid);
	if ((dist < 12) && (dist > 0))
	{
		printf(" [<font class=c31>");
		if (dist < 10)
			printf("%d", dist);
		else
		{
			if (dist == 10)
				printf("%c", 'A');
			if (dist == 11)
				printf("%c", 'B');
		}

		//      printf("%c" , 'A' + dist - 10);
		printf(" <font class=c32>ÇøÇø³¤<font class=c37>]");
	}
	else
		printf(" [<font class=c32>±¾Õ¾Çø³¤<font class=c37>]");
	return;
}


void
show_bm(char firstchar, char *userid)
{
	FILE *stream;
	int i = 0;
	int f = 0;
	char bm_file_path[64], buffer[64], *ch;
	sprintf(bm_file_path, "home/%c/%c/%s/.bmfile", firstchar, toupper(userid[1]), userid);

	if ((stream = fopen(bm_file_path, "r")) == NULL)
		printf(" [<font class=c32>±¾Õ¾°åÖ÷<font class=c37>]");
	else
	{

		while (fgets(buffer, 32, stream) && i < 4)
		{
			if ((ch = strchr(buffer, 10)))
				*ch = '\0';
			if (strncmp(buffer, "District", 8))
			{
				if (f == 0)
					printf(" [<font class=c31>");
				printf("%s ", buffer);
				f = 1;
			}
			i++;
		}
		fclose(stream);
		if (f)
			printf("<font class=c32>°å°åÖ÷<font class=c37>]");
	}
	return;
}



static struct
{
	struct fileheader *add;
	size_t len;
	int size;
	int offset;
} _map_dir =
{ 0, 0, -1,0};
void *
map_board_dir(char *board, char *file)
{
	char buf[1024];
	int fd;
	sprintf(buf, "boards/%s/%s", board, file);
	_map_dir.len = file_size(buf);
	if (_map_dir.len == 0)
		return 0;
	fd = open(buf, O_RDONLY);
	if (fd == -1)
		return 0;
	_map_dir.add =
		(struct fileheader *) mmap(NULL, _map_dir.len, PROT_READ,
					   MAP_PRIVATE, fd, 0);
	_map_dir.size = _map_dir.len / sizeof(struct fileheader);
	if (_map_dir.add == MAP_FAILED)
	{
		_map_dir.add = 0;
		_map_dir.len = 0;
	}
	close(fd);
	return _map_dir.add;
}

int
unmap_board_dir(void)
{
	if (_map_dir.add == 0)
		return 0;
	if (munmap(_map_dir.add, _map_dir.len) != -1)
	{
		_map_dir.len = 0;
		_map_dir.add = 0;
		_map_dir.size = 0;
		_map_dir.offset = -1;
		return 1;
	}
	else
		return 0;
}
struct fileheader *
find_in_map_dir(char *filename)
{
	int size, i;
	struct fileheader *fh;
	if (0 == _map_dir.add)
		return NULL;
	fh = _map_dir.add;
	size = _map_dir.size;
	for (i = size - 1; i >= 0; --i)
	{
		if (!strcmp(fh[i].filename, filename))
		{
			_map_dir.offset = i;
			break;
		}
	}
	if (i >= 0)
		return &fh[i];
	else
	{
		_map_dir.offset = -1;
		return NULL;
	}
}
struct fileheader *
pre_in_map_dir(void)
{
	if (0 == _map_dir.add || _map_dir.offset <= 0)
		return NULL;
	else
		return &_map_dir.add[_map_dir.offset - 1];
}
struct fileheader *
next_in_map_dir(void)
{
	if (0 == _map_dir.add || -1 == _map_dir.offset
	    || _map_dir.size == _map_dir.offset + 1)
		return NULL;
	else
		return &_map_dir.add[_map_dir.offset + 1];
}

int
get_offset_of_mapdir(void)
{
	return _map_dir.offset;
}

int
set_offset_of_mapdir(int offset)
{
	if (0 == _map_dir.add || _map_dir.size <= offset || 0 > offset)
		return 0;
	_map_dir.offset = offset;
	return 1;
}

int
get_size_of_mapdir(void)
{
	return _map_dir.size;
}

int
print_style(void)
{
	printf("<link rel=stylesheet type=text/css href='"
	       UPLOAD_SERVER_HEADER "/blue.css'>\n");
	return 0;
}

static int is_head_printed = 0;

static CALLBACK_RETURN reset_kernel(CALLBACK_ID callback_id, void *data, void *userdata)
{
	loginok = 0;
	silence = 0;
	bzero(&currentuser, sizeof(currentuser));
	u_info = NULL;
	bzero(fromhost, sizeof(fromhost));
	my_link_mode = 0;
	my_def_mode = 0;
	my_font_mode = 0;
	my_fontn_mode = 1;
	my_ansisize_mode = 1;
	my_ansifont_mode = 0;
	my_t_lines = 20;
	my_theme_mode = 1;
	friendnum = 0;
	friend_inited = 0;
	bzero(fff, sizeof(fff));
	badnum = 0;
	bzero(bbb, sizeof(bbb));
	is_normal_board = 0;
	bzero(denyuser, sizeof(denyuser));
	denynum = 0;
	is_head_printed = 0;
	last_modified_time = 0;
	cache_age = HEADER_NO_CACHE;
	return CALLBACK_CONTINUE;
}
static CALLBACK_RETURN bbswww_final(CALLBACK_ID callback_id, void *data, void *userdata)
{
	CALLBACK_ID *cid = (CALLBACK_ID *) userdata;
	remove_callback(*cid);
	g_free(cid);
	return CALLBACK_CONTINUE;
}
gboolean bbswww_init(EVENT_ID unload_event, EVENT_ID enter_event, EVENT_ID exit_event)
{
	CALLBACK_ID *cid = g_malloc(sizeof(CALLBACK_ID *));
	*cid = register_callback(enter_event, reset_kernel, NULL);
	register_callback(unload_event, bbswww_final, (void *) cid);
	return TRUE;
}

int
show_bm_words(char *board, int width)
{
	FILE *fp;
	char buf[256], buf2[256] = "";

	sprintf(buf, "vote/%s/note2", board);
	fp = fopen(buf, "r");
	if (!fp)
		return -1;
	fgets(buf2, 80, fp);
	buf2[80] = 0;
	fclose(fp);
	if (strlen(buf2) == 0)
		return -1;
	if (loginok && has_BM_perm(&currentuser, board))
	{
		printf("<table width=%d><tr style='height:14px'><td><b><a href=bbsmnote3?board=%s>°åÖ÷¼ÄÓï</a></b>: <font color='#308080'>%s</font></table>", width, board, nohtml(buf2));
	}
	else
	{
		printf("<table width=%d><tr style='height:14px'><td><b>°åÖ÷¼ÄÓï</b>: <font color='#308080'>%s</font></table>", width, nohtml(buf2));
	}
	return 0;
}

#define BRC_MAXSIZE     70000
#define BRC_MAXNUM      60
#define BRC_STRLEN      15
#define BRC_ITEMSIZE    (BRC_STRLEN + 1 + BRC_MAXNUM * sizeof( int ))
static char brc_buf[BRC_MAXSIZE];
static int brc_size;
static char brc_name[BRC_STRLEN];
static int brc_list[BRC_MAXNUM], brc_num;

char *
brc_getrecord(char *ptr, char *name, int *pnum, int *list)
{
	int num;
	char *tmp;

	strncpy(name, ptr, BRC_STRLEN);
	ptr += BRC_STRLEN;
	num = (*ptr++) & 0xff;
	tmp = ptr + num * sizeof(int);
	if (num > BRC_MAXNUM)
		num = BRC_MAXNUM;
	*pnum = num;
	memcpy(list, ptr, num * sizeof(int));
	return tmp;
}

char *
brc_putrecord(char *ptr, char *name, int num, int *list)
{
	if (num > 0)
	{
		if (num > BRC_MAXNUM)
		{
			num = BRC_MAXNUM;
		}
		strncpy(ptr, name, BRC_STRLEN);
		ptr += BRC_STRLEN;
		*ptr++ = num;
		memcpy(ptr, list, num * sizeof(int));
		ptr += num * sizeof(int);
	}
	return ptr;
}

void
brc_update(char *userid, char *board)
{
	char dirfile[STRLEN], *ptr;
	char tmp_buf[BRC_MAXSIZE - BRC_ITEMSIZE], *tmp;
	char tmp_name[BRC_STRLEN];
	int tmp_list[BRC_MAXNUM], tmp_num;
	int fd, tmp_size;

	ptr = brc_buf;
	if (brc_num > 0)
	{
		ptr = brc_putrecord(ptr, brc_name, brc_num, brc_list);
	}
	if (1)
	{
		sethomefile(dirfile, userid, ".boardrc");
		if ((fd = open(dirfile, O_RDONLY)) != -1)
		{
			tmp_size = read(fd, tmp_buf, sizeof(tmp_buf));
			close(fd);
		}
		else
		{
			tmp_size = 0;
		}
	}
	tmp = tmp_buf;
	while (tmp < &tmp_buf[tmp_size] && (*tmp >= ' ' && *tmp <= 'z'))
	{
		tmp = brc_getrecord(tmp, tmp_name, &tmp_num, tmp_list);
		if (strncmp(tmp_name, board, BRC_STRLEN) != 0)
		{
			ptr = brc_putrecord(ptr, tmp_name, tmp_num, tmp_list);
		}
	}
	brc_size = (int) (ptr - brc_buf);
	if ((fd = open(dirfile, O_WRONLY | O_CREAT, 0644)) != -1)
	{
		ftruncate(fd, 0);
		write(fd, brc_buf, brc_size);
		close(fd);
	}
}

int
brc_init(char *userid, char *boardname)
{
	FILE *fp;
	char dirfile[STRLEN], *ptr;

	sethomefile(dirfile, userid, ".boardrc");
	if ((fp = fopen(dirfile, "r")))
	{
		brc_size = fread(brc_buf, 1, BRC_MAXSIZE, fp);
		fclose(fp);
	}
	else
	{
		brc_size = 0;
	}
	ptr = brc_buf;
	while (ptr < &brc_buf[brc_size] && (*ptr >= ' ' && *ptr <= 'z'))
	{
		ptr = brc_getrecord(ptr, brc_name, &brc_num, brc_list);
		if (!strncasecmp(brc_name, boardname, BRC_STRLEN))
			return brc_num;
	}
	strncpy(brc_name, boardname, BRC_STRLEN);
	brc_list[0] = 1;
	brc_num = 1;
	return 0;
}

int
brc_add_read(char *filename)
{
	int ftime, n, i;

	if (!loginok)
		return -1;
	if (filename[0] != 'M' && filename[0] != 'G')
		return -1;
	ftime = atoi(&filename[2]);
	if (brc_num <= 0)
	{
		brc_list[brc_num++] = ftime;
		return -1;
	}
	for (n = 0; n < brc_num; n++)
	{
		if (ftime == brc_list[n])
		{
			return -1;
		}
		else if (ftime > brc_list[n])
		{
			if (brc_num < BRC_MAXNUM)
				brc_num++;
			for (i = brc_num - 1; i > n; i--)
			{
				brc_list[i] = brc_list[i - 1];
			}
			brc_list[n] = ftime;
			return -1;
		}
	}
	if (brc_num < BRC_MAXNUM)
		brc_list[brc_num++] = ftime;
	return 0;
}

int
brc_un_read(char *file)
{
	int ftime, n;

	if (file[0] != 'M' && file[0] != 'G')
		return 0;
	ftime = atoi(file + 2);
	if (brc_num <= 0)
		return 1;
	for (n = 0; n < brc_num; n++)
	{
		if (ftime > brc_list[n])
			return 1;
		if (ftime == brc_list[n])
			return 0;
	}
	return 0;
}

int
brc_clear(void)
{
	int i;
	char filename[20];

	if (!loginok)
		return -1;
	for (i = 0; i < 60; i++)
	{
		sprintf(filename, "M.%d.A", (int) (time(0) - i));
		brc_add_read(filename);
	}
	return 0;
}

int
brc_has_read(char *file)
{
	return !brc_un_read(file);
}

int
do_cache(char *filename, char *cachefilename)
{
	FILE *fp, *fpcache;
	char buf[512], *ptr;
	int swap = 0;
	fp = fopen(filename, "r");
	if(fp==NULL) return -1;
	fpcache = fopen(cachefilename, "w");
	if(fpcache==NULL)
	{
		fclose(fp);
		return -2;
	}
	while (1)
	{
		char *id, *s;
		if (fgets(buf, 512, fp) == 0)
			break;
		if (!strncmp(buf, "·¢ÐÅÈË: ", 8))
		{
			ptr = strdup(buf);
			id = strtok(ptr + 8, " ");
			s = strtok(0, "");
			if (id == 0)
				id = " ";
			if (s == 0)
				s = "\n";
			if (strlen(id) < 13 && getuser(id))
			{
				fprintf(fpcache, "·¢ÐÅÈË: %s",
					userid_str(id));
				hfprintf(fpcache, "%s", s);
				free(ptr);
				continue;
			}
			free(ptr);
		}
		if (!strncmp(buf, "--", 2))
			pic_mod = 1;
		if (!strncmp(buf, ": ", 2))
		{
			fprintf(fpcache, "<font color=808080>");
			swap = pic_mod;
			pic_mod = 0;
		}
		hhfprintf(fpcache, "%s", void1(buf));
		if (!strncmp(buf, ": ", 2))
		{
			fprintf(fpcache, "</font>");
			pic_mod = swap;
		}
	}
	fclose(fp);
	fclose(fpcache);
	return 0;
}

/*
p:no post form outside
a:cf@yssy
t:20050404
*/

unsigned long
ipintonum(char *ip)
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
	if( !strchr(buf,'.'))
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

int
ipinrange(unsigned long ipaddr, unsigned long network, unsigned long mask)
{
	if ((ipaddr & mask) == network)
		return 1;
	else
		return 0;
}

int
insjtunetwork(char *ip)
{
	struct ip_range
	{
		unsigned long network;
		unsigned long mask;
	} ip_range[10];
	int i;

	ip_range[0].network = 0xca780000UL;	/* 202.120.0.0   255.255.192.0/18   64  */
	ip_range[0].mask =    0xffffc000UL;
	ip_range[1].network = 0xd3502000UL;	/* 211.80.32.0   255.255.224.0/19   32  */
	ip_range[1].mask =    0xffffe000UL;
	ip_range[2].network = 0xd3505000UL;	/* 211.80.80.0   255.255.240.0/20   16  */
	ip_range[2].mask =    0xfffff000UL;
	ip_range[3].network = 0xca79b000UL;	/* 202.121.176.0 255.255.248.0/21    8  */
	ip_range[3].mask =    0xfffff800UL;
	ip_range[4].network = 0xdac1b000UL;	/* 218.193.176.0 255.255.240.0/20   16  */
	ip_range[4].mask =    0xfffff000UL;
	ip_range[5].network = 0xdbe46000UL;	/* 219.228.96.0  255.255.224.0/19   32  */
	ip_range[5].mask =    0xffffe000UL;
	ip_range[6].network = 0xca701a00UL;	/* 202.112.26.0  255.255.255.0/24    1  */
	ip_range[6].mask =    0xffffff00UL;
	ip_range[7].network = 0x3b4e0000UL;	/* 59.78.0     ~ 59.78.63    255.255.192.0/18 */
	ip_range[7].mask =    0xffffc000UL;
	ip_range[8].network = 0xca788000UL;	/* 202.120.128 ~ 202.120.143 255.255.240.0/20  */
	ip_range[8].mask =    0xfffff000UL;
	ip_range[9].network = 0x3ac48000UL;	/* 58.196.128.0~ 58.196.191  255.255.192.0/18 */
	ip_range[9].mask =    0xffffc000UL;
	

	for (i = 0; i <= 10; i++)
	{
		if (ipinrange
		    (ipintonum(ip), ip_range[i].network, ip_range[i].mask))
		{
			return 1;
		}

	}
	return 0;
}


int
noregist(void)
{
	FILE *fp = NULL;
	fp = fopen("NOREGISTER", "r");
	if (fp)
	{
		fclose(fp);
		return 1;
	}
	else
		return 0;
}

#ifdef LOGHANDLER

/* loghandler log file fd */
static int loghandler_fd = -1;
/* timeindex of current log file fd */
static int loghandler_timeindex = -1;
static int
loghandler_open_new(void)
{
	int randomindex,timeindex;
	char path[1024];
	randomindex = getpid()%LOGHANDLER_RANDOMINDEX_NUM;
	timeindex = \
		time(NULL)/LOGHANDLER_ROTATING_TIME%LOGHANDLER_TIMEINDEX_NUM;
	snprintf(path,1024,LOGHANDLER_PATH_FORMT,timeindex,randomindex);
	if(loghandler_fd > -1)
	{
		close(loghandler_fd);
	}
	loghandler_fd = open(path,O_CREAT|O_WRONLY|O_APPEND);
	if(loghandler_fd > -1)
	{
		loghandler_timeindex = timeindex;
	}
	return loghandler_fd;
}
int
loghandler_get_fd(void)
{
	int timeindex;
	if(loghandler_fd < 0)
	{
		return loghandler_open_new();
	}	
	timeindex = \
		time(NULL)/LOGHANDLER_ROTATING_TIME%LOGHANDLER_TIMEINDEX_NUM;
	if(timeindex != loghandler_timeindex)
	{
		return loghandler_open_new();
	}
	return loghandler_fd;
}
int
loghander_write(char *line, int len)
/*line: must be ended with '\n'*/
{
	int fd,ret;
	
	if ('\n' != line[len-1]) return -2;
	fd = loghandler_get_fd();
	if(fd<0)
		return -1;
	else
	{
		ret = write(fd,line,len);
		return ret;
	}
}

int
loghandler_writev(int linenum, ...)
/* write multi-line logs at a time by an atomic way
 * linenum: how many lines to write
 * following format:
 * char * line, int len (,char * line, int len)+
 * line: the line to write
 * len: len of the line, must be ended with '\n'
 * return number of bytes written, -1 on ERROR
 */
{
	va_list ap;
	struct iovec *pvec;
	int i, ret=0, fd, len;
	char *line;
	fd = loghandler_get_fd();
	if(fd<0) return -1;
	pvec = (struct iovec *) malloc(linenum*sizeof(struct iovec));
	if(!pvec) return -1;
	va_start(ap,linenum);
	for(i=0;i<linenum;++i)
	{
		line = va_arg(ap,char *);
		len = va_arg(ap,int);
		if (line[len-1] != '\n')
		{
			ret = -1;
			break;
		}
		pvec[i].iov_base = line;
		pvec[i].iov_len = len;
	}
	va_end(ap);
	if(ret<0)
	{ 
		free(pvec);
		return ret;
	}
	ret = writev(fd,pvec,linenum);
	free(pvec);
	return ret;
}


int
mysql_escape_string(char *dst, char *src, int len)
{
	int i;
	char *tmpdst = dst;
	for(i=0;i<len;i++)
	{
		switch(src[i])
		{
			case '\n':
				*tmpdst++='\\'; *tmpdst++='n'; break;
			case '\\':
				*tmpdst++='\\'; *tmpdst++='\\'; break;
			case '\r':
				*tmpdst++='\\'; *tmpdst++='r'; break;
			case '\0':
				*tmpdst++='\\'; *tmpdst++='0'; break;
			case '\'':
				*tmpdst++='\\'; *tmpdst++='\''; break;
			case '\"':
				*tmpdst++='\\'; *tmpdst++='\"'; break;
			case '\b':
				*tmpdst++='\\'; *tmpdst++='b'; break;
			case '\t':
				*tmpdst++='\\'; *tmpdst++='t'; break;
			case 0x1A: /* ctrl+Z */
				*tmpdst++='\\'; *tmpdst++='Z'; break;
			case '%':
				*tmpdst++='\\'; *tmpdst++='%'; break;
			case '_':
				*tmpdst++='\\'; *tmpdst++='_'; break;
			default:
				*tmpdst++=src[i]; break;
		}
	}
	*tmpdst++='\0';
	return tmpdst - dst -1;
}
#endif

char* url_quote(char *url) 
{ 
	static char qurl[1024]; 
	int i = 0; 
	while(i<1020&&(*url)) 
	{ 
		if(isalnum(*url)) 
		{ 
			qurl[i++]=*url; 
		} 
		else 
		{ 
			i+=sprintf(qurl+i,"%%%02X",*url); 
		} 
		++url; 
	} 
	qurl[i]=0; 
	return qurl; 
}

int
is_bad_id(char *userid)
{
	FILE *fp;
	char buf[STRLEN], ptr2[IDLEN + 2], *ptr;

	if ((fp = fopen(".badname", "r")) != NULL)
	{
		strtolower(ptr2, userid);
		while (fgets(buf, STRLEN, fp) != NULL)
		{
			ptr = strtok(buf, " \n\t\r");
			if (ptr != NULL && *ptr != '#')
			{
				if ((ptr[0] == '*'
				     && strstr(ptr2, &ptr[1]) != NULL)
				    || (ptr[0] != '*' && !strcmp(ptr2, ptr)))
				{
					fclose(fp);
					return 1;
				}
			}
		}
		fclose(fp);
	}
	/*
	   fp=fopen(".badname2", "r");
	   if(fp) {
	   while(1) {
	   if(fscanf(fp, "%12s", buf)<=0) break;
	   if(!strcasecmp(buf, s)) {
	   fclose(fp);
	   return 1;
	   }
	   }
	   }
	 */
	return 0;
}

long get_num_records(char *filename, int size)
{
    char *s,buf[64];
    int num;
    struct stat st;
    if (stat(filename, &st) == -1)
        return 0;
    num=st.st_size;
    strcpy(buf,filename);
    s=strrchr(buf,'/')+1;
    strcpy(s,TOPFILE_DIR);
    if(stat(buf,&st)==-1)
        return num/size;
    else
        return (num+st.st_size)/size;
}

/**
 * used for cgi_head(), plz set before call init_all()
 * if cache_age >0, will print http header "Cache-control: public,max-age=cache_age"
 * if cache_age == HEADER_NO_STORE, will print http header "Cache-control: no-store"
 * if cache_age == HEADER_NO_CACHE, will print http header "Cache-control: no-cache"
 * if cache_age == HEADER_PRIVATE, will print http header "Cache-control: private"
 **/
int cache_age = HEADER_NO_STORE;

/** 
 * default http header Last-Modified is time(0)
 * if u want to send a different one, plz set this before call init_all()
 **/
time_t last_modified_time = 0;



void
cgi_head(void)
{
	char buf[256];
	if (is_head_printed)
		return;
	if (last_modified_time == 0)
	{
		snprintf(buf, 256, "Last-Modified: %s\r\n", Gtime(time(0)));
		cgiHeaderString(buf);
	}
	else
	{
		snprintf(buf, 256, "Last-Modified: %s\r\n",
			 Gtime(last_modified_time));
		cgiHeaderString(buf);
	}
	switch (cache_age)
	{
	case HEADER_NO_CACHE:
		cgiHeaderString("Cache-control: no-cache\r\n");
		break;
	case HEADER_NO_STORE:
		cgiHeaderString("Cache-control: no-store\r\n");
		break;
	case HEADER_PRIVATE:
		cgiHeaderString("Cache-control: private\r\n");
		break;
	default:
		if (cache_age <= 0)
			break;	/* something wrong, skip it */
		snprintf(buf, 256, "Cache-control: max-age=%d\r\n",
			 cache_age);
		cgiHeaderString(buf);
	}
	cgiHeaderContentType("text/html; charset=" CHARSET);
	is_head_printed = 1;
}

void
http_quit(void)
{
	printf("\n</html>\n");
	longjmp(QUIT_JMP, 1);
	return;
}
void
http_quit_print(char *str)
{
	printf("<br>ERROR: %s<br><br>\n<a href=\"javascript:history.go(-1)\">¿ìËÙ·µ»Ø</a></html>\n",str);
	longjmp(QUIT_JMP, 1);
	return;
}
void
http_fatal(char *fmt, ...)
{
	char buf[1024];
	va_list ap;

	cgi_head();
	va_start(ap, fmt);
	vsnprintf(buf, 1023, fmt, ap);
	va_end(ap);
	buf[1023] = 0;
	printf("´íÎó! %s! <br><br>\n", buf);
	printf("<a href=\"javascript:history.go(-1)\">¿ìËÙ·µ»Ø</a>");
	printf("</html>\n");
	fflush(stdout);
	longjmp(fatalJmp, 1);
	return;
}

void
setcookie(char *s, char *b)
{
	//cgiHeaderCookieSetString(s, b, COOKIE_LIVE, "/", BBSHOST)
	printf("<script>document.cookie='%s=%s'</script>\n", s, b);
}

void
redirect(char *x)
{
	//cgiHeaderLocation(x);
	printf("<meta http-equiv='Refresh' content='0; url=%s'>\n", x);
}

void
refreshto(char *x, int t)
{
	printf("<meta http-equiv='Refresh' content='%d; url=%s'>\n", t, x);
}

//add by clist at sjtubbs
char* getfolderstr(char* filename)
{
	static char str[4] = {'\0','\0','\0','\0'};
	int len = strlen(filename);
	str[0] = filename[len-5]>'4'?filename[len-5]-5:filename[len-5];
	str[1] = filename[len-4];
	str[2] = filename[len-3];
	return str;
}

//°ÑfilenameÖÐµÚidÌõ¼ÇÂ¼Ìæ»»ÎªrptrËùÖ¸ÏòµÄÄÚÈÝ
int substitute_record(char *filename, void *rptr, int size, int id)
{
    int     fd;
    //add by cf
    struct stat st;
    if(stat(filename, &st) == -1 || st.st_size / size < id)
        return -1;
    //end

    if(id < 1)
    {
        g_message("substitue_record(...) id <= 0");
        return -1;
    }
    if(id > 120000)
    { // if id too big, maybe error!
        g_message("substitue_record(...) id > 120000");
        return -1;
    }
    if((fd=open(filename,O_WRONLY|O_CREAT,0644))==-1)
        return -1;
    flock(fd, LOCK_EX);
    if(lseek(fd, (off_t) (size * (id - 1)), SEEK_SET) == -1)
    {
        g_message("subrec seek err");
    }
    else
    {
        if (safewrite(fd, rptr, size) != size)
            g_message("subrec write err");
    }
    flock(fd, LOCK_UN);
    close(fd);
    return 0;
}

//±È½ÏÎÄ¼þÃû
int
cmpvfilename (char *filename, void *vfn)
{
	struct fileheader *fn = (struct fileheader *)vfn;
    return !strcmp (filename, fn->filename);
}

//ÔÚfilenameÖÐÑ°ÕÒrptrËùÖ¸ÏòµÄ¹Ø¼ü×ÖÎªfargµÄ¼ÇÂ¼£¬
int
search_record(char *filename, char *rptr, int size, int (*fptr)(), char *farg)
{
    int     fd;
    int     id = 1;
    if ((fd = open(filename, O_RDONLY, 0)) == -1)
        return 0;
    while (read(fd, rptr, size) == size)
    {
        if ((*fptr) (farg, rptr))
        {
            close(fd);
            return id;
        }
        id++;
    }
    close(fd);
    return 0;
}

int change_title(char *fname, char *title)
{
    FILE   *fp, *out;
    char    buf[256], outname[STRLEN];
    int		newtitle = 0 ;

	g_message("%s", fname);
    if ((fp = fopen(fname, "r")) == NULL)
        return 0;
	
    sprintf(outname, "tmp/editpost.%s.%05d", currentuser.userid, u_info->pid);
	g_message("%s", outname);
    if ((out = fopen(outname, "w")) == NULL)
        return 0;
	while ((fgets(buf, 256, fp)) != NULL)  
	{  
	g_message("equal: %d", !strncmp(buf, "±ê  Ìâ: ", 8));
	   if (!strncmp(buf, "±ê  Ìâ: ", 8) && newtitle ==0)  
	   {  
		   fprintf(out, "±ê  Ìâ: %s\n", title);  
		   newtitle = 1;  
		   continue;  
	   }  
	   fputs(buf, out);  
	}
    fclose(fp);
    fclose(out);
    rename(outname, fname);
    chmod(fname, 0644);
    return 1;
}

