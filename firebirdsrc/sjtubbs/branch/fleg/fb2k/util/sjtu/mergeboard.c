//用法：merge oldbrd newbrd
//将oldbrd合并到newbrd中，名称区分大小写
//比如 merge sysop syssecurity，将sysop的文章合并到syssecurity

#include "bbs.h"

int dashf(char *fname)
{
    struct stat st;

    return (stat(fname, &st) == 0 && S_ISREG(st.st_mode));
}

void merge(char *brd1, char *brd2, char *fname)
{
    int fdr, fdw, size, now;
    char buf[STRLEN], buf2[STRLEN];
    struct fileheader fh;

    size = sizeof(struct fileheader);
    sprintf(buf, "%s/boards/%s/%s", BBSHOME, brd1, fname);
    fdr = open(buf, O_RDONLY);
    sprintf(buf, "%s/boards/%s/%s", BBSHOME, brd2, fname);
    fdw = open(buf, O_WRONLY | O_CREAT, 0644);
    if (fdr == -1 || fdw == -1)
        return;
    flock(fdw, LOCK_EX);
    lseek(fdw, 0, SEEK_END);
    while (read(fdr, &fh, size) == size)
    {
        now = atoi(fh.filename + 2);
        do
        {
            buf[0] = fh.filename[0];
            buf[1] = '\0';
            sprintf(buf2, "%s.%d.A", buf, now);
            sprintf(buf, "%s/boards/%s/%s", BBSHOME, brd2, buf2);
            now++;
        }
        while (dashf(buf));
        sprintf(buf2, "%s/boards/%s/%s", BBSHOME, brd1, fh.filename);
        link(buf2, buf);
        buf[0] = fh.filename[0];
        buf[1] = '\0';
        sprintf(fh.filename, "%s.%d.A", buf, now - 1);
        write(fdw, &fh, size);
    }
    flock(fdw, LOCK_UN);
    close(fdw);
    close(fdr);
}

main(int argc, char **argv)
{
    merge(argv[1], argv[2], ".DIR");
    merge(argv[1], argv[2], ".DIGEST");
    merge(argv[1], argv[2], ".DELETEDDIR");
    //这里视实际情况添加或者删除
}
