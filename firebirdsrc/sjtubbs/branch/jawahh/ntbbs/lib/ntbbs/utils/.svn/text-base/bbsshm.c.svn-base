#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "bbsshm.h"


char* c_bbsshmat(int key, int size, int to_create, int use_hugetlb)
{
    int shmid, flags = 0;
    void *shmptr;
   
    if(to_create)
    {
        flags = flags | IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR;
    }

    if(use_hugetlb)
    {
        flags = flags | SHM_HUGETLB;
    }

    shmid = shmget(key, (size_t) size, flags);
    if(shmid<0)
    {
        return NULL;
    }

    shmptr = shmat(shmid, NULL, 0);
    if (shmptr==(void *)-1)
    {
        return NULL;
    }

    if(to_create)
    {
        memset(shmptr, 0, (size_t) size);
    }

    return (char *)shmptr;

}

void c_bbsshmdt(char *shmptr)
{
    shmdt(shmptr);
}

/* vim: ts=4 sw=4 sts=4 et: */
