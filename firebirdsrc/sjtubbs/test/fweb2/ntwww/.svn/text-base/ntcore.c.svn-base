#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <glib.h>

#include "ntcore.h"
#include "ntcinternal.h"

gboolean nt_init(void)
{
    unsigned int r;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    r = (unsigned int) (tv.tv_sec);
    r ^= ((unsigned int) tv.tv_usec) << 16;
    r ^= ((unsigned int) getppid()) << 8;
    r ^= ((unsigned int) getpid());
    srand(r);
    if (ntcuser_init() && ntcboard_init())
        return TRUE;
    else
        return FALSE;
}

void nt_final(void)
{
    ntcboard_final();
    ntcuser_final();
    return;
}

void nt_reset(void)
{
    ntcuser_reset();
    ntcboard_reset();
    return;
}

#define BAD_HOST                BBSHOME"/.bad_host"

gboolean nt_is_bad_host(const char *remoteaddr)
{
        /**
	 * BAD_HOST file format: 
	 *  1.1.1.1 : if ip match 1.1.1.1, bad host
	 *  -1.1.1.1 : if ip match 1.1.1.1, not bad host
	 *  1.1.1. : if ip in net 1.1.1.0/24, bad host
	 *  
     */
    FILE *list;
    char buf[256], *ptr;

    if ((list = fopen(BAD_HOST, "r")))
    {
        while (fgets(buf, sizeof(buf), list))
        {
            ptr = strtok(buf, " \n\t\r");
            if (ptr != NULL && ptr[0] != '#')
            {
                if (!strcmp(ptr, remoteaddr))
                {
                    fclose(list);
                    return TRUE;
                }
                if (ptr[0] == '-' && !strcmp(remoteaddr, &ptr[1]))
                {
                    fclose(list);
                    return FALSE;
                }
                if (ptr[strlen(ptr) - 1] == '.'
                    && !strncmp(ptr, remoteaddr, strlen(ptr) - 1))
                {
                    fclose(list);
                    return TRUE;
                }
            }
        }
        fclose(list);
    }
    return FALSE;
}

gboolean nt_inc_login_count(void)
{
    int fd, c;
    FILE *fp;

    fd = nt_open_lock_file(".dellock", O_RDWR | O_CREAT | O_APPEND);
    if (fd < 0)
        return FALSE;
    if ((fp = fopen("counts.dat", "r")) == NULL)
    {
        c = 0;
    }
    else
    {
        fscanf(fp, "%d", &c);
        c++;
        fclose(fp);
    }
    fp = fopen("counts.dat", "w");
    fprintf(fp, "%d", c);
    fclose(fp);
    nt_close_lock_file(fd);
    return TRUE;
}

//
//gboolean nt_post_mail(char *userid, char *nickname, char *fromip, int signature,
//              char *toid, char *title, char *content)
//      
////gboolean nt_post_mail(char *userid, char *title, char *file, char *id,
//        //char *nickname, char *ip, int sig)
//{
//      FILE *fp, *fp2;
//      char buf3[256], dir[256];
//      struct fileheader header;
//      int t, i;
//
//      bzero(&header, sizeof(header));
//      strcpy(header.owner, id);
//      for (i = 0; i < 100; i++)
//      {
//              t = time(0) + i;
//              sprintf(buf3, "mail/%c/%s/M.%d.A", toupper(userid[0]), userid,
//                      (int) (i + time(0)));
//              if (!file_exist(buf3))
//                      break;
//      }
//      if (i >= 99)
//              return -1;
//      sprintf(header.filename, "M.%d.A", t);
//      strsncpy(header.title, title, 60);
//      fp = fopen(buf3, "w");
//      if (fp == 0)
//              return -2;
//      fp2 = fopen(file, "r");
//      getdatestring(time(0), NA);
//      fprintf(fp, "ºƒ–≈»À: %s (%s)\n", id, nickname);
//      fprintf(fp, "±Í  Ã‚: %s\n", title);
//      fprintf(fp, "∑¢–≈’æ: %s (%s)\n", BBSNAME, datestring);
//      fprintf(fp, "¿¥  ‘¥: %s\n\n", ip);
//      if (fp2)
//      {
//              while (1)
//              {
//                      if (fgets(buf3, 256, fp2) <= 0)
//                              break;
//                      fprintf2(fp, buf3);
//              }
//              fclose(fp2);
//      }
//      fprintf(fp, "\n--\n");
//      sig_append(fp, id, sig);
//      fprintf(fp, "\n\n[1;%dm°˘ ¿¥‘¥:°§%s http://%s°§[FROM: %.20s][m\n",
//              31 + rand() % 7, BBSNAME, BBSHOST, ip);
//      fclose(fp);
//      sprintf(dir, "mail/%c/%s/.DIR", toupper(userid[0]), userid);
//      fp = fopen(dir, "a");
//      if (fp == 0)
//              return -1;
//      fwrite(&header, sizeof(header), 1, fp);
//      fclose(fp);
//      return 0;
//}
//
//gboolean nt_post_article(char *userid, char *nickname, char *fromip, int signature, 
//              char *board, char *title, char *content, unsigned int reid)
////gboolean post_article(char *board, char *title, char *file, char *id,
////         char *nickname, char *ip, int sig, int autocr,
////         unsigned int currid, unsigned int reid)
//{
//      FILE *fp, *fp2;
//      char buf3[1024];
//      struct shortfile *brd;
//      struct fileheader header;
//      int t = 0, count=0,filesd;
//
//      brd = getbcache(board);
//      if (brd == 0)
//              return -1;
//      board = brd->filename;
//      bzero(&header, sizeof(header));
//      strcpy(header.owner, id);
//
///*    header.id = currid;
//      header.reid = reid;*/
//      //add by heack 2005-06-13
//      t = time(0);
//      sprintf(buf3, "boards/%s/M.%d.A", board,t);
//      
//      while ((filesd = open(buf3, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1)
//      {
//              t++;
//              sprintf(buf3, "boards/%s/M.%d.A", board,t);
//              if (count++ > MAX_POSTRETRY)
//              {
//                      return -1;
//              }
//      }
//      fchmod(filesd, 0644);
//      close(filesd);
//      if(0 == currid)//modified by chaser 2006.05.25
//      {
//              header.id=t;//reply
//              header.reid=reid;
//      }
//      else
//      {
//              header.id=t;//new article
//              header.reid=t;
//      }//modify end
//      
//      /*for (i = 0; i < 100; i++)
//      {
//              t = time(0) + i;
//              sprintf(buf3, "boards/%s/M.%d.A", board, t);
//              if (!file_exist(buf3))
//                      break;
//      }
//      if (i >= 99)
//              return -1;*/
//      sprintf(header.filename, "M.%d.A", t);
//      strsncpy(header.title, title, 60);
//      fp = fopen(buf3, "w");
//      fp2 = fopen(file, "r");
//      getdatestring(time(0), NA);
//      fprintf(fp,
//              "∑¢–≈»À: %s (%s), –≈«¯: %s\n±Í  Ã‚: %s\n∑¢–≈’æ: %s (%s)\n\n",
//              id, nickname, board, title, BBSNAME, datestring);
//      if (fp2 != 0)
//      {
//              while (1)
//              {
//                      if (fgets(buf3, 1000, fp2) <= 0)
//                              break;
//                      if (autocr)
//                      {
//                              fprintf2(fp, buf3);
//                      }
//                      else
//                      {
//                              fprintf(fp, "%s", buf3);
//                      }
//              }
//              fclose(fp2);
//      }
//      fprintf(fp, "\n--\n");
//      sig_append(fp, id, sig);
//      fprintf(fp, "\n[1;%dm°˘ ¿¥‘¥:°§%s %s°§[FROM: %.20s][m\n",
//              31 + rand() % 7, BBSNAME, BBSHOST, ip);
//      fclose(fp);
//      sprintf(buf3, "boards/%s/.DIR", board);
//      fp = fopen(buf3, "a");
//      fwrite(&header, sizeof(header), 1, fp);
//      fclose(fp);
//      return t;
//}
//
