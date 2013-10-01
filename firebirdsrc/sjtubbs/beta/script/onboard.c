#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#define STRLEN 80
#define BBSHOME "/home/bbs/bbshome"

struct fileheader
{
    char filename[STRLEN];
    char owner[STRLEN];
    char title[STRLEN];
    unsigned level;
    unsigned char accessed[4];
    unsigned int id;
    unsigned int reid;
};


int main(int argc, char* argv[])
{
    FILE* fp = NULL;
    char dir_path[STRLEN];
    struct fileheader data;
    unsigned int reid;
    int num=0;

    if (argc != 3)
    {
        printf("argc error\n");
        return 0;
    }

    reid = atoi(argv[2]);
    sprintf(dir_path, "%s/boards/%s/.DIR", BBSHOME,argv[1]);
    /*printf("%s\n",dir_path);*/
    fp = fopen(dir_path, "rb");
    if(fp == NULL)
    {
        printf("open dir file error\n");
        return 0;
    }
    while(fread(&data, sizeof(struct fileheader), 1, fp)>0)
    {
        if(reid == data.reid)
        {
		num++;
        }
    }
	printf("%d",num);
    return 0;
    
        
}

