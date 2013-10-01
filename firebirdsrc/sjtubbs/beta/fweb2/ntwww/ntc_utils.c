#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <utime.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <glib.h>


#include "ntcore.h"
#include "ntcinternal.h"




static int open_lock_file(const char *path, int open_flags, int lock_flags)
{
	int fd;
	fd = open(path, open_flags);
	if(fd<0)
	{
		g_debug("can't open file at '%s'",path);
		return -1;
	}
	if( -1 == flock(fd, lock_flags))
	{
		close(fd);
		return -1;
	}
	return fd;
}

int nt_open_lock_file(const char *path, int flags)
{
	return open_lock_file(path, flags, LOCK_EX);
}
int nt_open_lock_file_nb(const char *path, int flags)
{
	return open_lock_file(path, flags, LOCK_EX|LOCK_NB);
}

void nt_close_lock_file(int fd)
{
	flock(fd, LOCK_UN);
	close(fd);	
}


void nt_sleep(int sec)
{
	struct timeval t;
	t.tv_sec = sec;
	t.tv_usec = 0;
	select(0,0,0,0,&t);
}

gboolean nt_file_append(char *file, char *buf)
{
	int fd;

	fd = open(file, O_WRONLY|O_APPEND);
	if(fd<0)
	{
		g_warning("can't not open file at '%s'",file);
		return FALSE;
	}
	write(fd,buf,strlen(buf));
	close(fd);
	return TRUE;
}


time_t nt_stat_mtime(const char *path)
{
	struct stat buf;

	if(stat(path, &buf)==-1)
		return 0;
	else
		return buf.st_mtime;

}

off_t nt_stat_size(const char *path)
{
	struct stat buf;
	if(stat(path, &buf)==-1)
		return 0;
	else if(! S_ISREG(buf.st_mode))
		return 0;
	else
		return buf.st_size;
}

gboolean nt_touch_file(const char *path)
{
	int fd;
	if(!g_file_test(path, G_FILE_TEST_EXISTS))
	{
		fd = open(path, O_CREAT|O_RDWR);
		if(fd<0)
			return FALSE;
		close(fd);
	}
	return -1 != utime(path,NULL);
}

gboolean nt_file_has_word(const char *file, const char *word)
{
	FILE *fp;
	char buf[256], buf2[256];

	fp = fopen(file, "r");
	if (fp == 0)
	{
		g_warning("can't open file at '%s'",file);
		return FALSE;
	}
	while (1)
	{
		if (fgets(buf, sizeof(buf), fp) == 0)
			break;
		sscanf(buf, "%s", buf2);
		if (!strcasecmp(buf2, word))
		{
			fclose(fp);
			return TRUE;
		}
	}
	fclose(fp);
	return FALSE;
}

char *nt_clear_ansi(char *buf)
{
	int i = 0,j=0, mode = 0;

	while (buf[i])
	{
		if (mode == 0)
		{
			if (buf[i] == 27)
			{
				mode = 1;
			}
			else
			{
				buf[j++] = buf[i];
			}
		}
		else
		{
			if (buf[i]==';' 
				|| buf[i]=='['
				|| (buf[i]>='0' && buf[i]<='9'))
				mode = 0;
		}
		i++;
	}
	buf[j] = 0;
	return buf;
}

char *nt_clear_htmltag(char *buf)
{
	int i = 0, j=0, mode = 0;

	while (buf[i])
	{
		if (mode == 0)
		{
			if (buf[i] == '<')
			{
				mode = 1;
			}
			else
			{
				buf[j++] = buf[i];
			}
		}
		else
		{
			if (buf[i] == '>')
				mode = 0;
		}
		i++;
	}
	buf[j] = '\0';
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

static char *weeknum[] = { "天", "一", "二", "三", "四", "五", "六" };


char *nt_gtime(time_t t)
{
	struct tm *gt;
	static char buf[32];
	gt = gmtime(&t);
	sprintf(buf,
		"%s, %02d-%s-%04d %02d:%02d:%02d GMT",
		days[gt->tm_wday],
		gt->tm_mday,
		months[gt->tm_mon],
		gt->tm_year + 1900, gt->tm_hour, gt->tm_min, gt->tm_sec);
	return buf;
}

char *nt_ctime(time_t t)
{
	static char buf[32];
	memcpy(buf,ctime(&t),24);
	buf[24] = '\0';
	return buf;
}

char *nt_ctime_cn(time_t t)
{
	static char buf[64];
	struct tm *tm;

	tm = localtime(&t);
	sprintf(buf, "%4d年%02d月%02d日%02d:%02d:%02d 星期%2s",
		tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
		tm->tm_hour, tm->tm_min, tm->tm_sec, weeknum[tm->tm_wday]);
	return buf;
}
