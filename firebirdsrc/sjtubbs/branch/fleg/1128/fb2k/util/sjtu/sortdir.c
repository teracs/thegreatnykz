//合并完版面后，需要对文章进行重新排序
//下面程序只对.DIR进行排序
//用法：sortdir brdname
//brdname为需要对.DIR进行排序的版面名称，区分大小写
//
#include <unistd.h>
#include <sys/mman.h>

#ifdef _POSIX_MAPPED_FILES

void *mmap(void *start, size_t length, int prot, int
           flags, int fd, off_t offset);

int munmap(void *start, size_t length);

#endif

#include "bbs.h"

int compare(a, b)
struct fileheader *a, *b;
{
    return (atoi(a->filename + 2) - atoi(b->filename + 2));
}

int main(int argc, char **argv)
{
    int fd, size;
    char file[256];
    struct stat buf;
    char *mem;

    if (argc < 2)
    {
        printf("Usage: sortdir board.\n");
        exit(0);
    }
    size = sizeof(struct fileheader);
    //        sprintf(file, "%s/boards/%s/%s", BBSHOME, argv[1], DOT_DIR);
    sprintf(file, "%s/boards/%s/.DIR", BBSHOME, argv[1]);
    fd = open(file, O_RDWR, 0644);
    if (fd == -1)
    {
        printf("Cannot open %s.\n", file);
        exit(0);
    }
    flock(fd, LOCK_EX);
    fstat(fd, &buf);
    mem = mmap(0, buf.st_size, PROT_READ | PROT_WRITE,
               MAP_FILE | MAP_SHARED, fd, 0);
    qsort(mem, buf.st_size / size, size, compare);
    flock(fd, LOCK_UN);
    close(fd);
}
