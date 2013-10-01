//created by clist

#include "bbs.h"

#define BOARDS_FILE	BBSHOME"/.BOARDS"
#define INDEX_DIR	".DIR"
#define INDEX_DIGEST	".DIGEST"
#define INDEX_DELETEDIR	".RETURNDIR"
#define INDEX_TOPFILE ".TOPFILE"

char* getfolderstring(char* filename)
{
	static char str[4] = {'\0','\0','\0','\0'};
	int len = strlen(filename);
	str[0] = filename[len-5]>'4'?filename[len-5]-5:filename[len-5];
	str[1] = filename[len-4];
	str[2] = filename[len-3];
	return str;
}

int main()
{
	FILE *fpboards;
	int nfpdir = 0;
	struct boardheader b_head;
	struct fileheader file_head;
	char f[100], buf[256], buf2[256];
	int i, count, countf;

	sprintf(buf, "%s/cache", BBSHOME);
	mkdir(buf, 0755);

	fpboards = fopen (BOARDS_FILE, "r");

	count = 0;
	while (fread (&b_head, sizeof (b_head), 1, fpboards) > 0)
	{
		printf("%4d : BOARD \"%s\" is processing.\n", count, b_head.filename);

		//mkdir of cache
		sprintf(buf, "%s/cache/%s", BBSHOME, b_head.filename);
		mkdir(buf, 0755);

		for (i = 0; i <MAX_FOLDERNUM; i++){
			sprintf(buf, "%s/cache/%s/%03d", BBSHOME, b_head.filename, i);
			mkdir(buf, 0755);

			sprintf(buf, "%s/boards/%s/%03d", BBSHOME, b_head.filename, i);
			mkdir(buf, 0755);
		}

		count ++;
		countf = 0;
		sprintf (f, "%s/boards/%s/%s", BBSHOME, b_head.filename, INDEX_DIR);
		if ((nfpdir = open (f, O_RDONLY)) != -1)
		{
			flock(nfpdir, LOCK_EX);
			while (read(nfpdir, &file_head, sizeof(file_head)) > 0)
			{
				sprintf(buf, "%s/boards/%s/%s", BBSHOME, b_head.filename, file_head.filename);
				sprintf(buf2, "%s/boards/%s/%s/%s", BBSHOME, b_head.filename, getfolderstring(file_head.filename), file_head.filename);
				rename(buf, buf2);
				countf ++;
			}
			flock(nfpdir, LOCK_UN);		
			close(nfpdir);
		} else
			perror(INDEX_DIR);

		sprintf (f, "%s/boards/%s/%s", BBSHOME, b_head.filename, INDEX_DIGEST);
		if ((nfpdir = open (f, O_RDONLY)) != -1)
		{
			flock(nfpdir, LOCK_EX);
			while (read(nfpdir, &file_head, sizeof(file_head)) > 0)
			{
				sprintf(buf, "%s/boards/%s/%s", BBSHOME, b_head.filename, file_head.filename);
				sprintf(buf2, "%s/boards/%s/%s/%s", BBSHOME, b_head.filename, getfolderstring(file_head.filename), file_head.filename);
				rename(buf, buf2);
				countf ++;
			}
			flock(nfpdir, LOCK_UN);			
			close(nfpdir);
		} else
			perror(INDEX_DIGEST);

		sprintf (f, "%s/boards/%s/%s", BBSHOME, b_head.filename, INDEX_TOPFILE);
		if ((nfpdir = open (f, O_RDONLY)) != -1)
		{
			flock(nfpdir, LOCK_EX);
			while (read(nfpdir, &file_head, sizeof(file_head)) > 0)
			{
				sprintf(buf, "%s/boards/%s/%s", BBSHOME, b_head.filename, file_head.filename);
				sprintf(buf2, "%s/boards/%s/%s/%s", BBSHOME, b_head.filename, getfolderstring(file_head.filename), file_head.filename);
				if (rename(buf, buf2) != 0)
					perror(buf);
				countf ++;
			}
			flock(nfpdir, LOCK_UN);			
			close(nfpdir);

		} else
			perror(INDEX_TOPFILE);

		sprintf (f, "%s/boards/%s/%s", BBSHOME, b_head.filename, INDEX_DELETEDIR);
		if ((nfpdir = open (f, O_RDONLY)) != -1)
		{
			flock(nfpdir, LOCK_EX);
			while (read(nfpdir, &file_head, sizeof(file_head)) > 0)
			{
				sprintf(buf, "%s/boards/%s/%s", BBSHOME, b_head.filename, file_head.filename);
				sprintf(buf2, "%s/boards/%s/%s/%s", BBSHOME, b_head.filename, getfolderstring(file_head.filename), file_head.filename);
				rename(buf, buf2);
				countf ++;
			}
			flock(nfpdir, LOCK_UN);
			close(nfpdir);

			printf("%4d : BOARD \"%s\" has %d files.\n", count, b_head.filename, countf);
		}
		else{
			perror(INDEX_DELETEDIR);
			printf("%4d : BOARD \"%s\" can not be read.\n", count, b_head.filename);
		}

	}
	printf("%d boards has been processed.", count);

	return 0;
}
