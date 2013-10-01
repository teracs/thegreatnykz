#include <stdio.h>
#include "../../include/bbs.h"

main(argc, argv)
int argc;
char *argv[];
{
	int i;
	FILE *inf;
	FILE *outf;
	char MYPASSFILE[160];
	struct userec urec;

	sprintf(MYPASSFILE, "%s/.PASSWDS", BBSHOME);
    inf = fopen(MYPASSFILE, "rb");
	outf = fopen(BBSHOME "/etc/showrewardeduser.result", "w");
	
	for (i = 0;; i++)
    {
        if (fread(&urec, sizeof(urec), 1, inf) <= 0)
            break;
		if ((urec.userlevel & PERM_XEMPT)
            || strcmp (urec.userid, "SYSOP") == 0
            || strcmp (urec.userid, "guest") == 0)
		{
			fprintf(outf, "%s\t999\n", urec.userid);
		}
		else if (urec.userlevel & PERM_666LIFE)
		{
			fprintf(outf, "%s\t666\n", urec.userid);
		}
	}
	
	fclose(inf);
    fclose(outf);

	return 0;
}
