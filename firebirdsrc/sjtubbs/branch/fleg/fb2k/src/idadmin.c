#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define STRLEN		80
#define ADMIN_NS	8
#define BBSHOME		"/home/bbs/bbshome"
#define FILE_PATH	"0Announce/groups/GROUP_0/IDAdmin/idadmin"
#define BLOCK_SIZE	50
//#define BLOCK_SIZE	1	//for debug

struct id_statistic
{
	int accept;
	int reject;
	int del;
};


int cal_date(char* datestr, time_t now)
{
    struct tm *tm;
	
    tm = localtime(&now);
    sprintf(datestr, "%4d年%02d月%02d日%02d:%02d:%02d",
		tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
		tm->tm_hour, tm->tm_min, tm->tm_sec);
    return (tm->tm_sec % 10);
}


int get_file_size(char* fullpath)
{
	int size = 0;
	FILE* fp = NULL;
	fp = fopen(fullpath, "r");
	if(fp == NULL)
		return 0;
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fclose(fp);
	return size;
}

void draw_bar(char* buf, int number)
//color: 1: red, 1: 
{
	int count;
	int i;
	//int addtail = 0;
	char block[2] = {"■"};
	count = (number + BLOCK_SIZE - 1) / BLOCK_SIZE;
	if(count>120)
		count = 120;
	for(i=0; i<count;i++)
	{
		memcpy(buf+i*2, block, 2);
	}
	return;
}

int main()
{
	chdir(BBSHOME);
	char mode[3][10] = 
	{
		"accept",
		"reject",
		"del"
	};
	
	char id_table[ADMIN_NS][16] = 
	{
		"supermaltose",
		"yisuoyinyu",
		"yymomo",
		"darkfighter",
		"starboy",
		"altheimer",
		"flyeagle",
		"tsai"
	};
	struct id_statistic data[ADMIN_NS];
	int i = 0;
	char path[STRLEN];
	FILE* fp = NULL;
	int time_value = 0;
	char date_str1[STRLEN];
	char date_str2[STRLEN];
	char bar[255];//条形统计图用

	memset(data, 0x00, ADMIN_NS * sizeof(struct id_statistic));
	for(i=0; i<ADMIN_NS; i++)
	{
		sprintf(path, "etc/idadmin.%s.%s", id_table[i], mode[0]);
		data[i].accept = (get_file_size(path)) / 2;
		if(data[i].accept > 0)
		{
			unlink(path);
		}
		sprintf(path, "etc/idadmin.%s.%s", id_table[i], mode[1]);
		data[i].reject = (get_file_size(path)) / 2;
		if(data[i].reject > 0)
		{
			unlink(path);
		}
		sprintf(path, "etc/idadmin.%s.%s", id_table[i], mode[2]);	
		data[i].del = (get_file_size(path)) / 2;
		if(data[i].del > 0)
		{
			unlink(path);
		}
	}
	
	fp = fopen(FILE_PATH, "w");
	if(fp == NULL)
		return 0;

	time_value = time(0);
	cal_date(date_str1, time_value - 86400*7);
	cal_date(date_str2, time_value);
	
	fprintf(fp, "\t\t\t\t\t\t帐号管理员工作情况统计\n\n");
	fprintf(fp, "\t\t\t%s - %s\n\n", date_str1, date_str2);
	for(i=0; i<ADMIN_NS; i++)
	{
		fprintf(fp, "\033[1;35m☆──────────────────────────────────────☆\033[m\n\n");
		fprintf(fp, "%s:\n\n", id_table[i]);
		fprintf(fp, "通过：\t");
		memset(bar, 0x00, 255);
		draw_bar(bar, data[i].accept);
		fprintf(fp, "\033[1;32m%s\033[m %d人\n", bar, data[i].accept);

		fprintf(fp, "拒绝：\t");
		memset(bar, 0x00, 255);
		draw_bar(bar, data[i].reject);
		fprintf(fp, "\033[1;31m%s\033[m %d人\n", bar, data[i].reject);

		fprintf(fp, "删除：\t");
		memset(bar, 0x00, 255);
		draw_bar(bar, data[i].del);
		fprintf(fp, "\033[1;33m%s\033[m %d人\n\n", bar, data[i].del);
	}
	
	
	fclose(fp);
	return 1;

}
