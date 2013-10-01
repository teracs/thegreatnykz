#include "bbs.h"

int safewrite(int fd, void *buf, int size)
{
    int cc, sz = size, origsz = size;
    char *bp = (char *) buf;

    do
    {
        cc = write(fd, bp, sz);
        if ((cc < 0) && (errno != EINTR))
        {
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

int main(int argc, char **argv)
{
    FILE *rec, *mail_fp;
    struct userec user;
    struct fileheader fileinfo;
    char *delmail_userid;
    char mail_folder[STRLEN], maildir_filename[STRLEN], mail_filename[STRLEN];
    int fd, pos, nfpdir;
    int usercount = 0;
    int affected_usercount = 0;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s userid\n", argv[0]);
        return -1;
    }
    delmail_userid = argv[1];

    rec = fopen(BBSHOME"/.PASSWDS", "rb");

    while (1)
    {
        if (fread(&user, sizeof(user), 1, rec) <= 0)
            break;
        
        usercount ++;
        if (usercount % 1000 == 0)
            printf("processed %d\n", usercount);

        sprintf(mail_folder, "%s/mail/%c/%c/%s", BBSHOME, toupper(user.userid[0]), toupper(user.userid[1]), user.userid);
		sprintf(maildir_filename, "%s/.DIR", mail_folder);

		if ((nfpdir = open (maildir_filename, O_RDONLY)) != -1)
		{
            pos = 0;
			while (read(nfpdir, &fileinfo, sizeof(fileinfo)) > 0)
            {
                pos ++;
                if (strcmp(fileinfo.owner, delmail_userid) == 0)
                {
                    affected_usercount ++;
/*
                    if ((fd = open(maildir_filename, O_WRONLY | O_CREAT, 0644)) == -1)
                        continue;
                    flock(fd, LOCK_EX);
                    if (lseek(fd, (off_t) (sizeof(struct fileheader) * (pos - 1)), SEEK_SET) != -1)
                    {
                        sprintf(fileinfo.title, "--SPAM--");    
                        safewrite(fd, &fileinfo, sizeof(struct fileheader));

                        sprintf(mail_filename, "%s/%s", mail_folder, fileinfo.filename);
                        mail_fp = fopen(mail_filename, "w");
                        fprintf(mail_fp, "--SPAM--");
                        fclose(mail_fp);
                    }
                    flock(fd, LOCK_UN);
                    close(fd);
                    */
                }
            }
            close(nfpdir);
        }
    }

    fclose(rec);

    printf("Affected user count: %d\n", affected_usercount);

    return 0;
}
