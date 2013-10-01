# 本程序用于改变.BOARDS的结构，使用前请记得备份
#include <stdio.h>
#include <string.h>

#define STRLEN	80
#define BM_LEN	60

struct boardheader
{            /* This structure is used to hold data in */
    char filename[STRLEN];      /* the BOARDS files */
    char owner[STRLEN - BM_LEN];
	char BM[BM_LEN - 4];
	char flag2[3];
	//modified end
    char flag;
    char title[STRLEN];
    unsigned level;
	unsigned char accessed[12];
};


void main()
{

	FILE* fp = NULL;
	FILE* fp2 = NULL;
	struct boardheader header = {0};
	fp = fopen("/home/bbs/bbshome/.BOARDS", "rb");
	fp2 = fopen("/home/bbs/bbshome/BOARDS_new", "wb");

	if ((fp == NULL) || (fp2 == NULL))
	{
		printf("can not open file\n");
		return;
	}

	while(fread(&header, sizeof(struct boardheader), 1, fp)>0)
	{
		header.flag2[0] = 0;
		header.flag2[1] = 0;
		header.flag2[2] = 0;
		fwrite(&header, sizeof(struct boardheader), 1, fp2);
		memset(&header, 0x00, sizeof(struct boardheader));
	}

	fclose(fp);
	fclose(fp2);

	printf("finished!\n");
	return;
}

