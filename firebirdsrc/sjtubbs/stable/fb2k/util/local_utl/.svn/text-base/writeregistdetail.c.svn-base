//created by clist
#include <stdio.h>
#include "bbs.h"

#define MAXREGIST (1500)

struct registdetail
{
	char id[IDLEN + 1];
	char email[REAL_NAME_EMAIL_LEN];
	int num;
	int gender;
	int bus;	// 0: ·ñ; 1: À´, 2: »Ø; 3: both
};

int main(argc, argv)
int argc;
char **argv;
{
	char* id;
	int num; 
	int bus; 
	char* email; 
	int gender;

	char filename[] = BBSHOME"/vote/regist";
	int fd;
	struct registdetail detail;

	if (argc < 6)
	{
		printf("Usage: %s id num bus email gender\n", argv[0]);
        exit(-1);
	}

	id = argv[1];
	num = atoi(argv[2]);
	bus = atoi(argv[3]);
	email = argv[4];
	gender = atoi(argv[5]);

	strncpy(detail.id, id, IDLEN + 1);
	if(id[0] == '\0' || strlen(id)==0)
	{
		detail.num = 0;
	}
	else
	{
		detail.num = num;
	}
	strncpy(detail.email, email, REAL_NAME_EMAIL_LEN);
	detail.bus = bus;
	detail.gender = gender;
	if((fd = open(filename, O_WRONLY)) == -1)
	{
		exit(-1);
	}
	lseek(fd, (num - 1) * sizeof(struct registdetail), SEEK_SET);
	flock(fd, LOCK_EX);
	write(fd, &detail, sizeof(struct registdetail));
	flock(fd, LOCK_UN);
	close(fd);

	return 0;
}
