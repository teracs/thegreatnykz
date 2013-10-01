/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu
    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@bbs.cs.ccu.edu.tw
                        Peng Piaw Foong, ppfoong@csie.ncu.edu.tw
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/
/*
$Id: boards.c,v 1.3 2004/07/04 07:09:39 bbs Exp $
*/

#include "bbs.h"

#define BBS_PAGESIZE    (t_lines - 4)

// Ö§³Ö290¸ö°å, 290*256=74240, Ôö¼Ó°åÃæºó×öµ÷Õû
#define BRC_MAXSIZE     75000
#define BRC_MAXNUM      60
#define BRC_STRLEN      BDNAME_LEN-1
#define BRC_ITEMSIZE    (BRC_STRLEN + 1 + BRC_MAXNUM * sizeof( int ))
#define GOOD_BRC_NUM	20

char brc_buf[BRC_MAXSIZE];
int brc_size, brc_changed = 0;
char brc_name[BRC_STRLEN];
int brc_list[BRC_MAXNUM], brc_num;

//void anywhere_lockscreen();


char *sysconf_str ();
extern time_t login_start_time;
extern int numboards;
extern struct shortfile *bcache;
extern struct BCACHE *brdshm;

struct newpostdata
{
    char *name, *title, *BM;
    char flag;
    int pos, total;
    char unread, zap;
    char status;
}
*nbrd;

struct newgood
{
	char subdirsfile[80];
	char subdirname[80];
	char boards[80];
	int isdir;// 1 dir 0 boards
};
struct goodboard
{
    char ID[GOOD_BRC_NUM][BRC_STRLEN];
    int num;
}
GoodBrd;

struct colboard//collectiong board
{	
	char name[BRC_STRLEN];
};

struct coldir
{
	char name[80];
	char file[80];
};

int *zapbuf;
int brdnum, yank_flag = 0;
char *boardprefix;

void
EGroup (cmd)
char *cmd;
{
    char buf[STRLEN];

    sprintf (buf, "EGROUP%c", *cmd);
    GoodBrd.num = 0;
    boardprefix = sysconf_str (buf);
    choose_board (DEFINE (DEF_NEWPOST) ? 1 : 0);
}

void
Boards ()
{
    boardprefix = NULL;
    GoodBrd.num = 0;
    choose_board (0);
}

void
GoodBrds ()
{
    if (!strcmp (currentuser.userid, "guest"))
        return;
    //modified by rwzmm@sjtubbs
    modify_user_mode(FAVOURITE);
    GoodBrd.num = 9999;
    boardprefix = NULL;
    //  choose_board (1);
    choose_board_good ("main.hck");
}

int removefile(char *file)
{	
	char filename[80];
	snprintf(filename,80,"home/%c/%s/%s",toupper(currentuser.userid[0]),currentuser.userid,file);
	if(remove(filename)==0)
		return 0;
	return -1;
	
}


int
f_replace_file(struct boardcollection sstruct)
{
	int sd;
	char file[80];
	snprintf(file,80,"home/%c/%s/%s",toupper(currentuser.userid[0]),currentuser.userid,sstruct.dirname);

	//int num; ±£ÏÕÓÃ
	if((sd=open(file,O_RDWR|O_CREAT,0644))==-1)
		return -1;
	write(sd,&sstruct,sizeof(struct boardcollection));
	close(sd);
	return 0;
}

int getfilename()
{
	char buf3[80];
	time_t t;
	int filesd,count=0;;
	t = time(0);
    	snprintf(buf3,80,"home/%c/%s/%d",toupper(currentuser.userid[0]),currentuser.userid,t);
	while ((filesd = open(buf3, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1)
    	{
        	t++;
        	snprintf(buf3,80,"home/%c/%s/%d",toupper(currentuser.userid[0]),currentuser.userid,t);

        	if (count++ > MAX_POSTRETRY)
        	{
        		close(filesd);
            		return -1;
        	}
    	}
    	fchmod(filesd, 0644);
    	close(filesd);
	return t;
	
		
}


int
f_get_file(char *file,struct boardcollection *sstruct, int n)
{
	int sd;
	char currentfilename[80];
	snprintf(currentfilename,80,"home/%c/%s/%s",toupper(currentuser.userid[0]),currentuser.userid,file);
	if((sd=open(currentfilename,O_RDONLY))==-1)
		return -1;
	if(lseek(sd,(n-1)*sizeof(struct boardcollection),SEEK_CUR)==-1)
	{
		close(sd);
		return -3;
	}
	if(read(sd,sstruct,sizeof(struct boardcollection))!=sizeof(struct boardcollection))
	{
		close(sd);
		return -2;
	}
	close(sd);
	return 0;
}
int do_changedir(struct boardcollection dirr,char* olddir,char*newdir)
{
	int i;
	int booldel=0;
	while(booldel!=1)
	{
		for(i=0;i<dirr.subnum;i++)
		{
			if(!strcmp(dirr.subdirname[i],olddir))
			{
				snprintf(dirr.subdirname[i],80,"%s",newdir);
				f_replace_file(dirr);
				booldel=1;
				break;
			}
		}
		if(booldel==0)
		{
			if(dirr.extends[0]==0)
				return -1;
			f_get_file(dirr.extends, &dirr, 1);
		}
	}
	return 0;
}
int do_adddir(char *boardfile,char* newdir)
{
	char boardfilename[80];
	struct boardcollection currentdir;
	struct boardcollection newstruct;
	time_t now;
	snprintf(boardfilename,80,"home/%c/%s/%s",toupper(currentuser.userid[0]),currentuser.userid,boardfile);
	f_get_file(boardfile, &currentdir, 1);
	if(currentdir.subnum<20)
	{
		now=getfilename();
		currentdir.subnum++;
		snprintf(currentdir.subdirsfile[currentdir.subnum-1],80,"%d",now);
		snprintf(currentdir.subdirname[currentdir.subnum-1],80,"%s",newdir);
		f_replace_file(currentdir);
		init_dir(&newstruct,currentdir.subdirsfile[currentdir.subnum-1],boardfile);
		f_replace_file(newstruct);
	}
	else
	{
		if(currentdir.extends[0]==0)
		{
			now=getfilename();
			snprintf(currentdir.extends,80,"%d",now);
			f_replace_file(currentdir);
			init_dir(&newstruct,currentdir.extends,boardfile);
			newstruct.isextends=1;
			f_replace_file(newstruct);
			do_adddir(currentdir.extends,newdir);
		}
		else
		{
			do_adddir(currentdir.extends,newdir);
		}
	}
	return 0;
	
}

int init_dir(struct boardcollection *dirr,char * dirname, char* updir)
{
	int i=0;
	dirr->subnum	=	0;
	dirr->boardnum	=	0;
	dirr->isextends	=	0;
	snprintf(dirr->dirname,80,dirname,toupper(currentuser.userid[0]),currentuser.userid);
	snprintf(dirr->updir,80,updir,toupper(currentuser.userid[0]),currentuser.userid);
	for(i=0;i<=19;i++)
	{
		dirr->subdirname[i][0]=0;
		dirr->subdirsfile[i][0]=0;
		dirr->boards[i][0]=0;
	}
	dirr->extends[0]=0;
	return 1;
}

int do_addboard(char *boardfile,char* newboard)
{
	char boardfilename[80];
	struct boardcollection currentdir;
	struct boardcollection newstruct;
	time_t now;
	snprintf(boardfilename,80,"home/%c/%s/%s",toupper(currentuser.userid[0]),currentuser.userid,boardfile);
	f_get_file(boardfile, &currentdir, 1);
	if(currentdir.boardnum<20)
	{
		currentdir.boardnum++;
		snprintf(currentdir.boards[currentdir.boardnum-1],80,"%s",newboard);
		f_replace_file(currentdir);
	}
	else
	{
		if(currentdir.extends[0]==0)
		{
			now=getfilename();
			snprintf(currentdir.extends,80,"%d",now);
			f_replace_file(currentdir);
			init_dir(&newstruct,currentdir.extends,boardfile);
			newstruct.isextends=1;
			f_replace_file(newstruct);
			do_addboard(currentdir.extends,newboard);
		}
		else
		{
			do_addboard(currentdir.extends,newboard);
		}
	}
	return 0;
	
}
int struct_del_dir(struct boardcollection *dirr, char *deldir)
{
	int i;
	int booldel=0;
	while(booldel!=1)
	{
		for(i=0;i<dirr->subnum;i++)
		{
			if(!strcasecmp(dirr->subdirsfile[i],deldir))
			{
				snprintf(dirr->subdirsfile[i],80,"%s",dirr->subdirsfile[dirr->subnum-1]);
				snprintf(dirr->subdirname[i],80,"%s",dirr->subdirname[dirr->subnum-1]);
				dirr->subdirsfile[dirr->subnum-1][0]=0;
			
				dirr->subnum--;
				booldel=1;
				break;
			}
		}
		if(booldel==0)
		{
			if(dirr->extends[0]==0)
				return -1;
			f_get_file(dirr->extends, dirr, 1);
		}
	}	
	if(booldel==1)
		return 0;
	return -1;
}
int struct_del_board(struct boardcollection *dirr, char *delboard)
{
	int i;
	int booldel=0;
	while(booldel!=1)
	{
		for(i=0;i<dirr->boardnum;i++)
		{
			if(!strcasecmp(dirr->boards[i],delboard))
			{
				snprintf(dirr->boards[i],80,"%s",dirr->boards[dirr->boardnum-1]);
				dirr->boards[dirr->boardnum-1][0]=0;
				dirr->boardnum--;
				booldel=1;
				break;
			}
		}
		if(booldel==0)
		{
			if(dirr->extends[0]==0)
				return -1;
			f_get_file(dirr->extends, dirr, 1);
		}
	}
	if(booldel==1)
		return 0;
	return -1;
}
int do_del_nocare(struct boardcollection dirr)
{
	int i;
	struct boardcollection temp;
	for(i=0;i<dirr.subnum;i++)
	{
		f_get_file(dirr.subdirsfile[i], &temp, 1);
		do_del_nocare(temp);
	}
	if(dirr.extends[0]!=0)
	{
		f_get_file(dirr.extends, &temp, 1);
		do_del_nocare(temp);
	}
	removefile(dirr.dirname);
}
int do_deldir(struct boardcollection updir,char *deldir)
{
	struct boardcollection dirr;
	struct boardcollection upupdir;
	f_get_file(deldir, &dirr,1);
	do_del_nocare(dirr);
	struct_del_dir(&updir,deldir);
	if(updir.isextends==1&&updir.boardnum==0&&updir.subnum==0)
	{
		if(updir.extends[0]==0)
		{
			f_get_file(updir.updir,&upupdir,1);
			upupdir.extends[0]=0;
			f_replace_file(upupdir);
		}
		else
		{
			f_get_file(updir.updir,&upupdir,1);
			snprintf(upupdir.extends,80,"%s",updir.extends);
			f_replace_file(upupdir);
		}
		removefile(updir.dirname);
	}
	else
	{
		f_replace_file(updir);
	}
	return 0;
	
}
int do_delboard(struct boardcollection updir,char *delboard)
{
	struct boardcollection upupdir;
	struct_del_board(&updir,delboard);
	if(updir.isextends==1&&updir.boardnum==0&&updir.subnum==0)
	{
		if(updir.extends[0]==0)
		{
			f_get_file(updir.updir,&upupdir,1);
			upupdir.extends[0]=0;
			f_replace_file(upupdir);
		}
		else
		{
			f_get_file(updir.updir,&upupdir,1);
			snprintf(upupdir.extends,80,"%s",updir.extends);
			f_replace_file(upupdir);
		}
		removefile(updir.dirname);
	}
	else
	{
		f_replace_file(updir);
	}
	
}
int getgood_dirnum(struct boardcollection currentdir)
{
	int i=0;
	i+=currentdir.subnum;
	while(currentdir.extends[0]!=0)
	{
		f_get_file(currentdir.extends, &currentdir, 1);
		i+=currentdir.subnum;
	}
	return i;
}
	
int getgood_brdnum(struct boardcollection currentdir)
{
	int i=0;
	i+=currentdir.boardnum;
	while(currentdir.extends[0]!=0)
	{
		f_get_file(currentdir.extends, &currentdir, 1);
		i+=currentdir.boardnum;
	}
	return i;
}
int get_pos(char *boardname)
{
	
	int i;
	for (i=0; i<MAXBOARD; i++)
       {
             	if (!strcasecmp(brdshm->bcache[i].filename, boardname))
             	{
        		return i;       		
                    	
             	}
				
	}
	return -1;
}

int
cmpboard (brd, tmp)
struct newpostdata *brd, *tmp;
{
    register int type = 0;

    if (!(currentuser.flags[0] & BRDSORT_FLAG))
    {
        type = brd->title[0] - tmp->title[0];
        if (type == 0)
            type = ci_strncmp (brd->title + 1, tmp->title + 1, 6);

    }
    if (type == 0)
        type = ci_strcmp (brd->name, tmp->name);
    return type;
}

int choose_board_good(char *filename,int newflag)
{
	struct newpostdata *ptr;
	struct newpostdata *ntbrd;
	struct boardcollection currentstruct;
	struct newgood tmpgood;
	FILE *fp;
	int pos,boolread=1;
	char bname[STRLEN],buf[STRLEN];
       char askynbuf[STRLEN];
	struct boardheader fh;
	char dname[80],bboard[80],goodboard[80];
	int brdnum,dirnum,totalnum,num=0/*,i*/;
	int page, ch, tmp, number=0;
    	int loop_mode = 0;
	
	modify_user_mode(FAVOURITE);
	//dir_collection=(struct coldir*)calloc(sizeof(struct coldir),dirnum);
	clear();
	page=-1;
	brdnum=-1;
	while(1)
	{
		if(brdnum==-1)
		{
			if(f_get_file(filename,&currentstruct,1)==-1)//file not exists
			{
				if(!strcmp(filename,"main.hck"))
				{
					init_dir(&currentstruct,"main.hck","main.hck");
					snprintf(goodboard,80,"home/%c/%s/.goodbrd",toupper(currentuser.userid[0]),currentuser.userid);
					fp = fopen(goodboard, "r+");
					if (fp == NULL)
					{
						f_replace_file(currentstruct);
					}
					else
					{
						while (fgets(bboard, 80, fp) != NULL)
						{
							strstr(bboard,"\n")[0]=0;		
							snprintf(currentstruct.boards[currentstruct.boardnum++],80,"%s",bboard);
						}
						fclose(fp);
						f_replace_file(currentstruct);
					}
				}
				else 
				{
					prints("read file error");
					exit(0);
				}
			}
			brdnum=getgood_brdnum(currentstruct);
			dirnum=getgood_dirnum(currentstruct);
			ntbrd=(struct newpostdata *)calloc(sizeof(struct newpostdata),brdnum);
			if(load_goodboards(currentstruct, ntbrd, dirnum, brdnum)!=0)
			{
				brdnum=-1;
				continue;
			}
			qsort (ntbrd, brdnum, sizeof (ntbrd[0]), cmpboard);
			totalnum=brdnum+dirnum;
		}
		if (num < 0)
            		num = 0;
		if (num >= totalnum)
            		num = totalnum - 1;
		if (page < 0)
        	{
                	tmp = num;
                	while (num <totalnum)
                	{
                		if(num<dirnum)
                		{
					if(boolread==0)
					{
						boolread=1;
						break;
					}
					num++;
					continue;
                		}
                		ptr = &ntbrd[num-dirnum];
		             	if (ptr->total == -1)
              	       	check_newpost (ptr);
                    		if (ptr->unread)
                    		{
                    			break;
                    		}
                    		num++;
                	}
                	if (num >= totalnum)
                    	num = tmp;
            		
            		page = (num / BBS_PAGESIZE) * BBS_PAGESIZE;
			
            		showgood_brdlist (page, 1, newflag,currentstruct,dirnum,brdnum,ntbrd);
            		update_endline ();
        	}
		if (num < page || num >= page + BBS_PAGESIZE)
        	{
            		page = (num / BBS_PAGESIZE) * BBS_PAGESIZE;
            		showgood_brdlist (page, 1, newflag,currentstruct,dirnum,brdnum,ntbrd);
            		update_endline ();
        	}
		if(dirnum+brdnum<=0)move(3,0);
		else
			move (3 + num - page, 0);
		
        	prints (">", number);
        	if (loop_mode == 0)
        	{
            		ch = egetch ();
        	}
        	move (3 + num - page, 0);
        	prints (" ");
        	if (ch == 'q' || ch == 'e' || ch == KEY_LEFT || ch == EOF)
            		return 0;
        	switch (ch)
        	{
        		case 'P':
        		case 'b':
        		case Ctrl ('B'):
                    	case KEY_PGUP:
                            if (num == 0)
                            	num = totalnum - 1;
                    		else
                        		num -= BBS_PAGESIZE;
            		break;
        		case 'C':
        		case 'c':
            			
            			break;
        		case 'L':
            			m_read ();
            			page = -1;
            		break;
        		case Ctrl('v'):
                        	anywhere_lockscreen();
            			update_endline();
            			break;
        		case 'M':
            			m_new ();
            			page = -1;
            			break;
        		case 'u':
            			modify_user_mode (QUERY);
            			t_query ();
            			page = -1;
            			break;
        		case 'H':
    			{
                		//show_help ("0Announce/bbslist/day");
                		show_top_ten(0);
                		page = -1;
                		break;
            		}
            		case 'x':
				if(load_goodboards(currentstruct, ntbrd, dirnum, brdnum)!=0)
				{
					brdnum=-1;
					break;
				}
				qsort (ntbrd, brdnum, sizeof (ntbrd[0]), cmpboard);
				page=-1;
				break;
        		case 'l':
            			show_allmsgs ();
            			page = -1;
            			break;
        		case 'N':
        		case ' ':
        		case Ctrl ('F'):
                    	case KEY_PGDN:
                            if (num == totalnum - 1)
	                            num = 0;
                    		else
                        		num += BBS_PAGESIZE;
            			break;
        		case 'p':
        		case 'k':
        		case KEY_UP:
            			if (num-- <= 0)
			       	num = totalnum - 1;
            			break;
        		case 'n':
        		case 'j':
        		case KEY_DOWN:
            			if (++num >= totalnum)
                			num = 0;
            			break;
        		case '$':
            			num = totalnum - 1;
            			break;
        		case '!':		/* youzi leave */
            			return Goodbye ();
            			break;
        		case 'h':
            			show_help ("help/boardreadhelp");
            			page = -1;
            			break;
						
        		case 's':
				 currentuser.flags[0] ^= BRDSORT_FLAG;
            			qsort (ntbrd, brdnum, sizeof (ntbrd[0]), cmpboard);
            			page = 999;
            			break;
			case 't':
			case 'T':
				{
					if(num>=dirnum)
						break;
					if(getdata (0, 0, "ÇëÊäÈëĞÂÄ¿Â¼Ãû³Æ: ",dname, IDLEN + 1, DOECHO, YEA))
					{
						getname_byn(currentstruct, num,&tmpgood, dirnum, brdnum);
						do_changedir( currentstruct, tmpgood.subdirname, dname);
						f_get_file(currentstruct.dirname, &currentstruct, 1);
					}
					page=-1;
					break;
				}
			case 'A':		//add a new dir to my favourate. Added by heack@sjtubbs
				
				
				if(getdata (0, 0, "ÇëÊäÈëÄ¿Â¼Ãû³Æ: ",dname, 20 + 1, DOECHO, YEA))
				{
					do_adddir(filename, dname);
					
					
				}
				page=-1;
				brdnum=-1;
				break;
			case 'a':
				{			
                		if (gettheboardname(1, "ÊäÈëÌÖÂÛÇøÃû (°´¿Õ°×¼ü×Ô¶¯ËÑÑ°): ", &pos, &fh, bname))
                		{
                    			if (!inGoodBrds (bname))
                    			{
                        			do_addboard(filename, bname);
						
					
                    			}
                		}
                		page = -1;
				brdnum=-1;
				break;
				}
        		case 'd':
				
				if(num<dirnum)
				{
					getname_byn(currentstruct, num,&tmpgood, dirnum, brdnum);
					sprintf(askynbuf, "È·¶¨ÒªÉ¾³ıÄ¿Â¼%s", tmpgood.subdirname);
					move(t_lines - 1, 0);
                    			saveline(t_lines -1, 0);
                    			clrtoeol();
                    			if(askyn(askynbuf, YEA, YEA) == YEA)
                    			{
						do_deldir(currentstruct, tmpgood.subdirsfile);
						
						page=-1;
						brdnum=-1;
						break;
					}
					saveline(t_lines-1, 1);
					
				}
				else if(num<totalnum)
				{
					ptr=&ntbrd[num-dirnum];
					sprintf(askynbuf, "È·¶¨ÒªÉ¾³ıÌÖÂÛÇø%s", ptr->name);
					move(t_lines - 1, 0);
                    			saveline(t_lines -1, 0);
                    			clrtoeol();
					if(askyn(askynbuf, YEA, YEA) == YEA)
                    			{
						do_delboard(currentstruct, ptr->name);
						
						page=-1;
						brdnum=-1;
						break;
					}
					saveline(t_lines-1, 1);
				}
				break;
            			
        		case KEY_HOME:
            			num = 0;
            			break;
			case KEY_TAB:
				tmp=num;
				if(num<dirnum)
					num=dirnum-1;
				num++;
				while(num<=totalnum-1&&num>=dirnum)
				{
					ptr=&ntbrd[num-dirnum];
					if(ptr->unread)
						break;
					num++;
				}
				if(num==totalnum)
				{
					if(tmp<dirnum)
					{
						num=tmp;
						break;
					}
					num=dirnum;
					while(num<tmp&&num>=dirnum)
					{
						ptr=&ntbrd[num-dirnum];
						if(ptr->unread)
							break;
						num++;
					}
				}
				break;
				
        		case KEY_END:
            			num = totalnum - 1;
				break;
        		case '\n':
        		case '\r':
          			 if (number > 0)
           			 {
               			 num = number - 1;
                			break;
            			}
            /* fall through */
       		 case KEY_RIGHT:
            		{
				if(dirnum+brdnum<=0)break;
				getname_byn(currentstruct, num,&tmpgood, dirnum, brdnum);
				if(tmpgood.isdir)
				{
					choose_board_good(tmpgood.subdirsfile,newflag);
					boolread=0;
				}
				else
				{
					ptr = &ntbrd[num-dirnum];
                			brc_initial (ptr->name);
					if(0==strcasecmp("master",tmpgood.boards)) 
					{
						report("enter board master");
					}
	
                			memcpy (currBM, ptr->BM, BM_LEN - 1);
                			if (DEFINE (DEF_FIRSTNEW))
                			{
                    				setbdir (buf, tmpgood.boards);
                    				tmp = unread_position (buf, ptr);
                    				page = tmp - t_lines / 2;
                    				getkeep (buf, page > 1 ? page : 1, tmp + 1);
                			}
                			Read ();

                			if (zapbuf[ptr->pos] > 0 && brc_num > 0)
                			{
                    				zapbuf[ptr->pos] = brc_list[0];
                			}
                			ptr->total  = -1;
                			
                		
				}
				page=-1;
				break;
        		}
        		case 'S':		/* sendmsg ... youzi */
            			if (HAS_PERM (PERM_MESSAGE))
            			{
              			  s_msg ();
               			 page = -1;
           			 }

           			 break;
       		 case 'f':		/* show friends ... youzi */
           			 if (!HAS_PERM (PERM_BASIC))
              			  break;
            			t_friends ();
            			page = -1;
					
            			break;
        		default:
            		;
        }
		
	}
	
}

void
New ()
{
    if (heavyload ())
    {
        clear ();
        prints ("±§Ç¸£¬Ä¿Ç°ÏµÍ³¸ººÉ¹ıÖØ£¬Çë¸ÄÓÃ Boards Ö¸ÁîÔÄÀÀÌÖÂÛÇø...");
        pressanykey ();
        return;
    }
    boardprefix = NULL;
    GoodBrd.num = 0;
    choose_board (1);
}

int
inGoodBrds (char *bname)
{
    int i;

    for (i = 0; i < GoodBrd.num && i < GOOD_BRC_NUM; i++)
        if (!strcasecmp (bname, GoodBrd.ID[i]))
            return i + 1;
    return 0;
}

void
load_zapbuf ()
{
    char fname[STRLEN];
    int fd, size, n;

    size = MAXBOARD * sizeof (int);
    zapbuf = (int *) malloc (size);
    for (n = 0; n < MAXBOARD; n++)
        zapbuf[n] = 1;
    setuserfile (fname, ".lastread");
    if ((fd = open (fname, O_RDONLY, 0600)) != -1)
    {
        size = numboards * sizeof (int);
        read (fd, zapbuf, size);
        close (fd);
    }
}

void
load_GoodBrd ()
{
    int i;
    char fname[STRLEN];
    FILE *fp;

    GoodBrd.num = 0;
    setuserfile (fname, ".goodbrd");
    if ((fp = fopen (fname, "r")))
    {
        for (i = 0; i < GOOD_BRC_NUM; i++)
            if (fscanf (fp, "%s\n", GoodBrd.ID[i]) != EOF)
            {
                if (getbnum (GoodBrd.ID[i]))
                    GoodBrd.num++;
                else
                    i--;
            }
            else
                break;
        fclose (fp);
    }
    if (GoodBrd.num == 0)
    {
        GoodBrd.num++;
        if (getbcache (DEFAULTBOARD))
            strcpy (GoodBrd.ID[0], DEFAULTBOARD);
        else
            strcpy (GoodBrd.ID[0], currboard);
    }
}

void
save_GoodBrd ()
{
    int i;
    FILE *fp;
    char fname[STRLEN];

    if (GoodBrd.num <= 0)
    {
        GoodBrd.num = 1;
        if (getbcache (DEFAULTBOARD))
            strcpy (GoodBrd.ID[0], DEFAULTBOARD);
        else
            strcpy (GoodBrd.ID[0], currboard);
    }
    setuserfile (fname, ".goodbrd");
    if ((fp = fopen (fname, "wb+")) != NULL)
    {
        for (i = 0; i < GoodBrd.num; i++)
            fprintf (fp, "%s\n", GoodBrd.ID[i]);
        fclose (fp);
    }
}

void
save_zapbuf ()
{
    char fname[STRLEN];
    int fd, size;

    setuserfile (fname, ".lastread");
    if ((fd = open (fname, O_WRONLY | O_CREAT, 0600)) != -1)
    {
        size = numboards * sizeof (int);
        write (fd, zapbuf, size);
        close (fd);
    }
}
int load_goodboards(struct boardcollection currentstruct, struct newpostdata *nbrd,int dirnum,int brdnum)
{
	struct shortfile *bptr;
	struct newpostdata *ptr;
	struct newgood ndir;
	int n,pos;
	char control_buffer[256] = {0};
	if (zapbuf == NULL)
    	{
       	load_zapbuf ();
    	}
	for (n = 0; n < brdnum; n++)
    	{
    		getname_byn(currentstruct, n+dirnum, &ndir, dirnum, brdnum);
		if(ndir.isdir)continue;
		pos=get_pos(ndir.boards);
		if(-1==pos)//used when a board is deleted
		{
			nbrd=NULL;
			do_delboard(currentstruct, ndir.boards);
			return -1;
		}
			
		bptr=&brdshm->bcache[pos];
		setbfile(control_buffer, bptr->filename , "board.allow");
		ptr = &nbrd[n];
            	ptr->name = bptr->filename;
            	ptr->title = bptr->title;
            	ptr->BM = bptr->BM;
            	ptr->flag =
                	bptr->flag | ((bptr->level & PERM_NOZAP) ? NOZAP_FLAG : 0);
            	ptr->pos = pos;
            	ptr->total = -1;
            	ptr->zap = (zapbuf[pos] == 0);
            	if (bptr->level & PERM_POSTMASK)
                	ptr->status = 'p';
            	else if (bptr->level & PERM_NOZAP)
                	ptr->status = 'z';
            	else if (ptr->flag & NOREPLY_FLAG)
                	ptr->status = 'x';
            	else if ((bptr->level & ~PERM_POSTMASK) != 0)
                	ptr->status = 'r';
				else if(dashf(control_buffer))
					ptr->status = 'k';
            	else
                	ptr->status = ' ';
	}
    
    return 0;
}
int
load_boards ()
{
    struct shortfile *bptr;
    struct newpostdata *ptr;
    int n, addto = 0, goodbrd = 0;
	char control_buffer[256] = {0};
    resolve_boards ();
    if (zapbuf == NULL)
    {
        load_zapbuf ();
    }
    brdnum = 0;
    if (GoodBrd.num == 9999)
    {
        load_GoodBrd ();
        goodbrd = 1;
    }
    for (n = 0; n < numboards; n++)
    {
        bptr = &bcache[n];
        if (!(bptr->filename[0]))
            continue;		/* Òş²Ø±»É¾³ıµÄ°åÃæ */
		setbfile(control_buffer, bptr->filename , "board.allow");
        if (goodbrd == 0)
        {
            if (!(bptr->level & PERM_POSTMASK) && !HAS_PERM (bptr->level)
                    && !(bptr->level & PERM_NOZAP))
                continue;
            if (boardprefix != NULL
                    && strchr (boardprefix, bptr->title[0]) == NULL
                    && boardprefix[0] != '*')
                continue;
            if (boardprefix != NULL && boardprefix[0] == '*')
            {
                if (!strstr (bptr->title, "¡ñ")
                        && !strstr (bptr->title, "¡Ñ") && bptr->title[0] != '*')
                    continue;
            }
            if (boardprefix == NULL && bptr->title[0] == '*')
                continue;
            addto = yank_flag || zapbuf[n] != 0 || (bptr->level & PERM_NOZAP);
        }
        else
            addto = inGoodBrds (bptr->filename);

		if (!dashf(control_buffer)||seek_in_file(control_buffer,currentuser.userid))//add by pax 061004
		{
        if (addto)
        {
            ptr = &nbrd[brdnum++];
            ptr->name = bptr->filename;
            ptr->title = bptr->title;
            ptr->BM = bptr->BM;
            ptr->flag =
                bptr->flag | ((bptr->level & PERM_NOZAP) ? NOZAP_FLAG : 0);
            ptr->pos = n;
            ptr->total = -1;
            ptr->zap = (zapbuf[n] == 0);
			
            if (bptr->level & PERM_POSTMASK)
                ptr->status = 'p';
            else if (bptr->level & PERM_NOZAP)
                ptr->status = 'z';
            else if (ptr->flag & NOREPLY_FLAG)
                ptr->status = 'x';
            else if ((bptr->level & ~PERM_POSTMASK) != 0)
                ptr->status = 'r';
			else if(dashf(control_buffer))
				ptr->status = 'k';
            else
                ptr->status = ' ';
        }
		}
    }
    if (brdnum == 0 && !yank_flag && boardprefix == NULL)
    {
        if (goodbrd)
        {
            GoodBrd.num = 0;
            save_GoodBrd ();
            GoodBrd.num = 9999;
        }
        brdnum = -1;
        yank_flag = 1;
        return -1;
    }
    return 0;
}

int
search_board (num)
int *num;
{
    static int i = 0, find = YEA;
    static char bname[STRLEN];
    int n, ch, tmpn = NA;

    if (find == YEA)
    {
        bzero (bname, sizeof (bname));
        find = NA;
        i = 0;
    }
    while (1)
    {
        move (t_lines - 1, 0);
        clrtoeol ();
        prints ("ÇëÊäÈëÒªÕÒÑ°µÄ board Ãû³Æ£º%s", bname);
        ch = egetch ();

        if (isprint2 (ch))
        {
            bname[i++] = ch;
            for (n = 0; n < brdnum; n++)
            {
                if (!ci_strncmp (nbrd[n].name, bname, i))
                {
                    tmpn = YEA;
                    *num = n;
                    if (!strcasecmp (nbrd[n].name, bname))
                        return 1	/* ÕÒµ½ÀàËÆµÄ°å£¬»­ÃæÖØ»­
                               				 */ ;
                }
            }
            if (tmpn)
                return 1;
            if (find == NA)
            {
                bname[--i] = '\0';
            }
            continue;
        }
        else if (ch == Ctrl ('H') || ch == KEY_LEFT || ch == KEY_DEL ||
                 ch == '\177')
        {
            i--;
            if (i < 0)
            {
                find = YEA;
                break;
            }
            else
            {
                bname[i] = '\0';
                continue;
            }
        }
        else if (ch == '\t')
        {
            find = YEA;
            break;
        }
        else if (ch == '\n' || ch == '\r' || ch == KEY_RIGHT)
        {
            find = YEA;
            break;
        }
        bell (1);
    }
    if (find)
    {
        move (t_lines - 1, 0);
        clrtoeol ();
        return 2 /* ½áÊøÁË */ ;
    }
    return 1;
}

int
check_newpost (ptr)
struct newpostdata *ptr;
{
    struct fileheader fh;
    struct stat st;
    char filename[STRLEN];
    int fd, offset, total;

    ptr->total = ptr->unread = 0;
    setbdir (genbuf, ptr->name);
    if ((fd = open (genbuf, O_RDWR)) < 0)
        return 0;
    fstat (fd, &st);
    total = st.st_size / sizeof (fh);
    if (total <= 0)
    {
        close (fd);
        return 0;
    }
    ptr->total = total;
    if (!brc_initial (ptr->name))
    {
        ptr->unread = 1;
    }
    else
    {
        offset = (int) ((char *) &(fh.filename[0]) - (char *) &(fh));
        lseek (fd, (off_t) (offset + (total - 1) * sizeof (fh)), SEEK_SET);
        if (read (fd, filename, STRLEN) > 0 && brc_unread (filename))
        {
            ptr->unread = 1;
        }
    }
    close (fd);
    return 1;
}

int
unread_position (dirfile, ptr)
char *dirfile;
struct newpostdata *ptr;
{
    struct fileheader fh;
    char filename[STRLEN];
    int fd, offset, step, num;

    num = ptr->total + 1;
    if (ptr->unread && (fd = open (dirfile, O_RDWR)) > 0)
    {
        if (!brc_initial (ptr->name))
        {
            num = 1;
        }
        else
        {
            offset = (int) ((char *) &(fh.filename[0]) - (char *) &(fh));
            num = ptr->total - 1;
            step = 4;
            while (num > 0)
            {
                lseek (fd, (off_t) (offset + num * sizeof (fh)), SEEK_SET);
                if (read (fd, filename, STRLEN) <= 0 || !brc_unread (filename))
                    break;
                num -= step;
                if (step < 32)
                    step += step / 2;
            }
            if (num < 0)
                num = 0;
            while (num < ptr->total)
            {
                lseek (fd, (off_t) (offset + num * sizeof (fh)), SEEK_SET);
                if (read (fd, filename, STRLEN) <= 0 || brc_unread (filename))
                    break;
                num++;
            }
        }
        close (fd);
    }
    if (num < 0)
        num = 0;
    return num;
}

void
show_brdlist (page, clsflag, newflag)
int page, clsflag, newflag;
{
    struct newpostdata *ptr;
    int n,i,inboard;
    char tmpBM[BM_LEN - 1];
    //extern struct BCACHE *brdshm;
    if (clsflag)
    {
        clear ();
        docmdtitle ("[ÌÖÂÛÇøÁĞ±í]",
                    " [mÖ÷Ñ¡µ¥[[1;32m¡û[m,[1;32me[m] ÔÄ¶Á[[1;32m¡ú[m,[1;32mRtn[m] Ñ¡Ôñ[[1;32m¡ü[m,[1;32m¡ı[m] ÁĞ³ö[[1;32my[m] ÅÅĞò[[1;32ms[m] ËÑÑ°[[1;32m/[m] ÇĞ»»[[1;32mc[m] ÇóÖú[[1;32mh[m]\n");
        prints
        ("[1;44;37m %s ÌÖÂÛÇøÃû³Æ     V  Àà±ğ  ×ª %-25s   S °å  Ö÷      [m\n",
         newflag ? "È«²¿  Î´" : "  ±àºÅ  ", "ÖĞ  ÎÄ  Ğğ  Êö      ÔÚÏß"/*,
         	 newflag ? "" : "   "*/);
    }
    move (3, 0);
    for (n = page; n < page + BBS_PAGESIZE; n++)
    {
        if (n >= brdnum)
        {
            prints ("\n");
            continue;
        }
        ptr = &nbrd[n];
        if (!newflag)
            prints ("  %4d %c ", n + 1, ptr->zap
                    && !(ptr->flag & NOZAP_FLAG) ? '-' : ' ');
        else
        {
            if (ptr->total == -1)
            {
                refresh ();
                check_newpost (ptr);
            }
            for (i=0; i<MAXBOARD; i++)
                if (!strcasecmp(brdshm->bcache[i].filename, ptr->name))
                {
                    inboard =brdshm->inboard[i];
                    //ĞŞÕı°åÃæÔÚÏßÈËÊı-1µÄbug
                    if(inboard < 0)
                    {
                        inboard = 0;
                        //brdshm->inboard[i] = 0;
                    }
                    break;
                }
            prints (" %5d %s", ptr->total, ptr->unread ? "¡ô" : "¡ó");
        }
        strcpy (tmpBM, ptr->BM);
        //ĞŞÕıÔÚÏßÈËÊı²»ÄÜ¶ÔÆëµÄbug, modified by rwzmm @ sjtubbs

        prints ("%c%-14s %s %-30s %3d    %c %-12s\n",
                (ptr->zap&& !(ptr->flag & NOZAP_FLAG)) ? '*' : ' ',
                ptr->name,
                (ptr->flag & VOTE_FLAG) ? "[1;31mV[m" : " ",
                ptr->title + 1,
                inboard,
                HAS_PERM (PERM_POST) ? ptr->status : ' ',
                ptr->BM[0] <= ' ' ? "³ÏÕ÷°åÖ÷ÖĞ" : strtok (tmpBM, " "));
    }
    refresh ();
}
int getptr_byn(struct boardcollection tmpstruct,int n,struct newpostdata *ptr,int dirnum,int brdnum)
{
	struct newgood ndir;
	struct shortfile *bptr;
	int pos;
	char control_buffer[256] = {0};
	if(n>=dirnum)return 0;
	getname_byn(tmpstruct, n, &ndir, dirnum, brdnum);
	pos=get_pos(ndir.boards);
	bptr=&brdshm->bcache[pos];
	if (zapbuf == NULL)
    	{
       	load_zapbuf ();
    	}
	setbfile(control_buffer, bptr->filename , "board.allow");
	ptr->name = bptr->filename;
       ptr->title = bptr->title;
       ptr->BM = bptr->BM;
       ptr->flag = bptr->flag | ((bptr->level & PERM_NOZAP) ? NOZAP_FLAG : 0);
       ptr->pos = pos;
       ptr->total = -1;
       ptr->zap = (zapbuf[pos] == 0);
       if (bptr->level & PERM_POSTMASK)
                ptr->status = 'p';
       else if (bptr->level & PERM_NOZAP)
                ptr->status = 'z';
       else if (ptr->flag & NOREPLY_FLAG)
                ptr->status = 'x';
       else if ((bptr->level & ~PERM_POSTMASK) != 0)
                ptr->status = 'r';
	   else if(dashf(control_buffer))
				ptr->status = 'k';	   
       else
                ptr->status = ' ';
	return 1;
}

int getname_byn(struct boardcollection tmpstruct,int n,struct newgood *ndir,int dirnum,int brdnum)
{
	if(n<=dirnum-1&&n>=0)
	{
		ndir->isdir=1;
		while(n+1-tmpstruct.subnum>0)
		{	
			n-=tmpstruct.subnum;
			if(tmpstruct.extends[0]!=0)
			{
				f_get_file(tmpstruct.extends, &tmpstruct, 1);
			}
			
		}
		snprintf(ndir->subdirname,80,"%s",tmpstruct.subdirname[n]);
		snprintf(ndir->subdirsfile,80,"%s",tmpstruct.subdirsfile[n]);
		
	}
	else if(n<=dirnum+brdnum-1&&n>=dirnum)
	{
		ndir->isdir=0;
		n-=dirnum;
		while(n+1-tmpstruct.boardnum>0)
		{	
			n-=tmpstruct.boardnum;
			if(tmpstruct.extends[0]!=0)
			{
				f_get_file(tmpstruct.extends, &tmpstruct, 1);
			}
		}
		snprintf(ndir->boards,80,"%s",tmpstruct.boards[n]);
	}
	else ndir->isdir=-1;
	return 1;
}

int
showgood_brdlist (page, clsflag, newflag,currentstruct,dirnum,brdnum,nbrd)
int page, clsflag, newflag,dirnum,brdnum;
struct boardcollection currentstruct;
struct newpostdata *nbrd;
{
    	struct newpostdata *ptr;
	struct newgood tmpgood;
    int n,i,inboard;
    char tmpBM[BM_LEN - 1];
    //extern struct BCACHE *brdshm;
    if (clsflag)
    {
        clear ();
        docmdtitle ("[ÌÖÂÛÇøÁĞ±í]",
                    " [mÖ÷Ñ¡µ¥[[1;32m¡û[m,[1;32me[m] ÔÄ¶Á[[1;32m¡ú[m,[1;32mRtn[m] ¸ÄÃû[[1;32mT[m,[1;32mt[m] Ë¢ĞÂ[[1;32mx[m] Ìí¼Ó[[1;32mA,a[m] É¾³ı[[1;32md[m] ÇĞ»»[[1;32mc[m] ÇóÖú[[1;32mh[m]\n");
        prints
        ("[1;44;37m %s ÌÖÂÛÇøÃû³Æ     V  Àà±ğ  ×ª %-25s   S °å  Ö÷      [m\n",
         newflag ? "È«²¿  Î´" : "  ±àºÅ  ", "ÖĞ  ÎÄ  Ğğ  Êö      ÔÚÏß"/*,
         	 newflag ? "" : "   "*/);
    }
    move (3, 0);
	if(dirnum+brdnum<=0)//impossible if <0
	{
		prints("       £­  <¿ÕÄ¿Â¼>\n");
	}
	else
	{
    for (n = page; n < page + BBS_PAGESIZE; n++)
    {
    	getname_byn(currentstruct, n, &tmpgood, dirnum, brdnum);
    	if(tmpgood.isdir==1)
    	{
    		prints("       £« %-17s",tmpgood.subdirname);
		prints("<Ä¿Â¼>\n");
		
    	}
	else if(tmpgood.isdir==0)
	{
		ptr = &nbrd[n-dirnum];
            	if (ptr->total == -1)
            	{
                	refresh ();
                	check_newpost (ptr);
            	}
            	for (i=0; i<MAXBOARD; i++)
              if (!strcasecmp(brdshm->bcache[i].filename, ptr->name))
              {
                    inboard =brdshm->inboard[i];
                    if(inboard < 0)
                    {
                        inboard = 0;
                    }
                    break;
                }
            	prints (" %5d %s", ptr->total, ptr->unread ? "¡ô" : "¡ó");
        
        	strcpy (tmpBM, ptr->BM);

        	prints ("%c%-14s %s %-30s %3d    %c %-12s\n",
                	(ptr->zap&& !(ptr->flag & NOZAP_FLAG)) ? '*' : ' ',
                	brdshm->bcache[i].filename,
                	(ptr->flag & VOTE_FLAG) ? "[1;31mV[m" : " ",
                	ptr->title + 1,
                	inboard,
                	HAS_PERM (PERM_POST) ? ptr->status : ' ',
                	ptr->BM[0] <= ' ' ? "³ÏÕ÷°åÖ÷ÖĞ" : strtok (tmpBM, " "));
		}
	else if (n >= brdnum+dirnum)
       {
       	prints ("\n");
       	continue;
       }
       }
		}
	refresh ();
	return 1;
}


int
choose_board (newflag)
int newflag;
{
    static int num;
    struct newpostdata newpost_buffer[MAXBOARD];
    struct newpostdata *ptr;
    int page, ch, tmp, number, tmpnum;
    int loop_mode = 0;

    if (!strcmp (currentuser.userid, "guest"))
        yank_flag = 1;
    nbrd = newpost_buffer;
    if(newflag == 2)
        modify_user_mode(FAVOURITE);
    else
        modify_user_mode (newflag ? READNEW : READBRD);
    brdnum = number = 0;
    clear ();
    //show_brdlist(0, 1, newflag);
    while (1)
    {
        if (brdnum <= 0)
        {
            if (load_boards () == -1)
                continue;
            qsort (nbrd, brdnum, sizeof (nbrd[0]), cmpboard);
            page = -1;
            if (brdnum <= 0)
                break;
        }
        if (num < 0)
            num = 0;
        if (num >= brdnum)
            num = brdnum - 1;
        if (page < 0)
        {
            if (newflag)
            {
                tmp = num;
                while (num < brdnum)
                {
                    ptr = &nbrd[num];
                    if (ptr->total == -1)
                        check_newpost (ptr);
                    if (ptr->unread)
                        break;
                    num++;
                }
                if (num >= brdnum)
                    num = tmp;
            }
            page = (num / BBS_PAGESIZE) * BBS_PAGESIZE;
            show_brdlist (page, 1, newflag);
            update_endline ();
        }
        if (num < page || num >= page + BBS_PAGESIZE)
        {
            page = (num / BBS_PAGESIZE) * BBS_PAGESIZE;
            show_brdlist (page, 0, newflag);
            update_endline ();
        }
        move (3 + num - page, 0);
        prints (">", number);
        if (loop_mode == 0)
        {
            ch = egetch ();
        }
        move (3 + num - page, 0);
        prints (" ");
        if (ch == 'q' || ch == 'e' || ch == KEY_LEFT || ch == EOF)
            break;
        switch (ch)
        {
        case 'P':
        case 'b':
        case Ctrl ('B'):
                    case KEY_PGUP:
                            if (num == 0)
                                num = brdnum - 1;
                    else
                        num -= BBS_PAGESIZE;
            break;
        case 'C':
        case 'c':
            if (newflag == 1)
                newflag = 0;
            else
                newflag = 1;
            show_brdlist (page, 1, newflag);
            break;
        case 'L':
            m_read ();
            page = -1;
            break;
        case Ctrl('v'):
                        anywhere_lockscreen();
            update_endline();
            break;
        case 'M':
            m_new ();
            page = -1;
            break;
        case 'u':
            modify_user_mode (QUERY);
            t_query ();
            page = -1;
            break;
        case 'H':
    {
    		//show_help ("0Announce/bbslist/day");
    		show_top_ten(0);
    		page = -1;
    		break;
            }
            /*		case 'R': {
                    		char buf[200],path[80],ans[4],*t;
            	        	sprintf(buf, "[1;5;31mÁ¢¼´¶ÏÏß[m: [1;33mÒÔ±ã»Ö¸´ÉÏ´ÎÕı³£Àë¿ª±¾Õ¾Ê±µÄÎ´¶Á±ê¼Ç (Y/N) ? [[1;32mN[m[1;33m]£º[m");
            		        getdata(22, 0, buf, ans, 3, DOECHO, YEA);
            			if (ans[0] == 'Y' || ans[0] == 'y' ) {
                    			setuserfile(path, ".lastread");
            			        t = strstr(path,".");
            				*t = '\0';
            			        sprintf(buf,"cp %s/.lastread.backup %s/.lastread",path,path);
            			        system(buf);
            			        sprintf(buf,"cp %s/.boardrc.backup %s/.boardrc",path,path);
            			        system(buf);
            				move(23,0);clrtoeol();
            				move(22,0);clrtoeol();
            				prints("[1;33mÒÑ¾­»Ö¸´ÉÏ´ÎÕı³£Àë¿ª±¾Õ¾Ê±µÄÎ´¶Á±ê¼Ç[m\n[1;32mÇë°´ Enter ¼ü[1;31mÁ¢¼´¶ÏÏß[m[1;32m, È»ºóÖØĞÂµÇÂ½±¾Õ¾ [m");
            				egetch();
            				exit(0);
            			}
            			page = -1;
            			break;	
            	        }
            */
        case 'l':
            show_allmsgs ();
            page = -1;
            break;
        case 'N':
        case ' ':
        case Ctrl ('F'):
                    case KEY_PGDN:
                            if (num == brdnum - 1)
                                num = 0;
                    else
                        num += BBS_PAGESIZE;
            break;
        case 'p':
        case 'k':
        case KEY_UP:
            if (num-- <= 0)
                num = brdnum - 1;
            break;
        case 'n':
        case 'j':
        case KEY_DOWN:
            if (++num >= brdnum)
                num = 0;
            break;
        case '$':
            num = brdnum - 1;
            break;
        case '!':		/* youzi leave */
            return Goodbye ();
            break;
        case 'h':
            show_help ("help/boardreadhelp");
            page = -1;
            break;
        case '/':
            move (3 + num - page, 0);
            prints (">", number);
            tmpnum = num;
            tmp = search_board (&num);
            move (3 + tmpnum - page, 0);
            prints (" ", number);
            if (tmp == 1)
                loop_mode = 1;
            else
    {
                loop_mode = 0;
                update_endline ();
            }
            break;
        case 's':		/* sort/unsort -mfchen */
            currentuser.flags[0] ^= BRDSORT_FLAG;
            qsort (nbrd, brdnum, sizeof (nbrd[0]), cmpboard);
            page = 999;
            break;
        case 'y':
            if (GoodBrd.num)
                break;
            yank_flag = !yank_flag;
            brdnum = -1;
            break;
        case 'z':
            if (GoodBrd.num)
                break;
            if (HAS_PERM (PERM_BASIC) && !(nbrd[num].flag & NOZAP_FLAG))
            {
                ptr = &nbrd[num];
                ptr->zap = !ptr->zap;
                ptr->total = -1;
                zapbuf[ptr->pos] = (ptr->zap ? 0 : login_start_time);
                page = 999;
            }
            break;
        case 'a':
        case 'A':
            /*add boards to my favourate. modified by rwzmm@sjtubbs*/
#ifdef SPECIALID

            if (strcmp (currentuser.userid, SPECIALID1) == 0
                    || strcmp (currentuser.userid, SPECIALID2) == 0)
                break;
#endif
		if (uinfo.mode == READNEW || uinfo.mode == READBRD)
            	{
                //		  int GoodBoardNumber = 0;
                char bname[STRLEN];
                char askynbuf[STRLEN];

           

                strcpy(bname, nbrd[num].name);

                
                    sprintf(askynbuf, "½« %s °åÌí¼Óµ½¸öÈËÊÕ²Ø¼Ğ", bname);
                    move(t_lines - 1, 0);
                    saveline(t_lines -1, 0);
                    clrtoeol();
                    if(askyn(askynbuf, YEA, YEA) == YEA)
                    {
                        do_addboard("main.hck", bname);
                    }
                    saveline(t_lines-1, 1);
                    break;
                
                //here may cause a bug
                
               
            }

            break;

        
        case KEY_HOME:
            num = 0;
            break;
        case KEY_END:
            num = brdnum - 1;
            break;
        case '\n':
        case '\r':
            if (number > 0)
            {
                num = number - 1;
                break;
            }
            /* fall through */
        case KEY_RIGHT:
            {
                char buf[STRLEN];

                ptr = &nbrd[num];
                brc_initial (ptr->name);
				/*add addtional security to board master,log all enter*/
				if(0==strcmp("master",currboard)) 
				{
					report("enter board master");
				}
	
                memcpy (currBM, ptr->BM, BM_LEN - 1);
                if (DEFINE (DEF_FIRSTNEW))
                {
                    setbdir (buf, currboard);
                    tmp = unread_position (buf, ptr);
                    page = tmp - t_lines / 2;
                    getkeep (buf, page > 1 ? page : 1, tmp + 1);
                }
                Read ();

                if (zapbuf[ptr->pos] > 0 && brc_num > 0)
                {
                    zapbuf[ptr->pos] = brc_list[0];
                }
                ptr->total = page = -1;
                break;
            }
        case 'S':		/* sendmsg ... youzi */
            if (HAS_PERM (PERM_MESSAGE))
            {
                s_msg ();
                page = -1;
            }

            break;
        case 'f':		/* show friends ... youzi */
            if (!HAS_PERM (PERM_BASIC))
                break;
            t_friends ();
            page = -1;
            break;
        default:
            ;
        }
        if (newflag ==2)
            modify_user_mode(FAVOURITE);
        else
            modify_user_mode (newflag ? READNEW : READBRD);
        if (ch >= '0' && ch <= '9')
        {
            number = number * 10 + (ch - '0');
            ch = '\0';
        }
        else
        {
            number = 0;
        }
    }
    clear ();
    save_zapbuf ();
    return -1;
}

char *
brc_getrecord (ptr, name, pnum, list)
char *ptr, *name;
int *pnum, *list;
{
    int num;
    char *tmp;

    strncpy (name, ptr, BRC_STRLEN);
    ptr += BRC_STRLEN;
    num = (*ptr++) & 0xff;
    tmp = ptr + num * sizeof (int);
    if (num > BRC_MAXNUM)
    {
        num = BRC_MAXNUM;
    }
    *pnum = num;
    memcpy (list, ptr, num * sizeof (int));
    return tmp;
}

char *
brc_putrecord (ptr, name, num, list)
char *ptr, *name;
int num, *list;
{
    if (num > 0)
    {
        if (num > BRC_MAXNUM)
        {
            num = BRC_MAXNUM;
        }
        strncpy (ptr, name, BRC_STRLEN);
        ptr += BRC_STRLEN;
        *ptr++ = num;
        memcpy (ptr, list, num * sizeof (int));
        ptr += num * sizeof (int);
    }
    return ptr;
}

void
brc_update ()
{
    char dirfile[STRLEN], *ptr;
    char tmp_buf[BRC_MAXSIZE - BRC_ITEMSIZE], *tmp;
    char tmp_name[BRC_STRLEN];
    int tmp_list[BRC_MAXNUM], tmp_num;
    int fd, tmp_size;

    if (brc_changed == 0)
    {
        return;
    }
    ptr = brc_buf;
    if (brc_num > 0)
    {
        ptr = brc_putrecord (ptr, brc_name, brc_num, brc_list);
    }
    if (1)
    {
        setuserfile (dirfile, ".boardrc");
        if ((fd = open (dirfile, O_RDONLY)) != -1)
        {
            tmp_size = read (fd, tmp_buf, sizeof (tmp_buf));
            close (fd);
        }
        else
        {
            tmp_size = 0;
        }
    }
    tmp = tmp_buf;
    while (tmp < &tmp_buf[tmp_size] && (*tmp >= ' ' && *tmp <= 'z'))
    {
        tmp = brc_getrecord (tmp, tmp_name, &tmp_num, tmp_list);
        if (strncmp (tmp_name, currboard, BRC_STRLEN) != 0)
        {
            ptr = brc_putrecord (ptr, tmp_name, tmp_num, tmp_list);
        }
    }
    brc_size = (int) (ptr - brc_buf);

    if ((fd = open (dirfile, O_WRONLY | O_CREAT, 0644)) != -1)
    {
        ftruncate (fd, 0);
        write (fd, brc_buf, brc_size);
        close (fd);
    }
    brc_changed = 0;
}

int
brc_initial (boardname)		//deardragon0912
char *boardname;
{
    char dirfile[STRLEN], *ptr;
    int fd;

    if (strcmp (currboard, boardname) == 0)
    {
        return brc_num;
    }
    brc_update ();
    strcpy (currboard, boardname);
    brc_changed = 0;
    if (brc_buf[0] == '\0')
    {
        setuserfile (dirfile, ".boardrc");
        if ((fd = open (dirfile, O_RDONLY)) != -1)
        {
            brc_size = read (fd, brc_buf, sizeof (brc_buf));
            close (fd);
        }
        else
        {
            brc_size = 0;
        }
    }
    ptr = brc_buf;
    while (ptr < &brc_buf[brc_size] && (*ptr >= ' ' && *ptr <= 'z'))
    {
        ptr = brc_getrecord (ptr, brc_name, &brc_num, brc_list);
        if (strncmp (brc_name, currboard, BRC_STRLEN) == 0)
        {
            return brc_num;
        }
    }
    strncpy (brc_name, boardname, BRC_STRLEN);
    brc_list[0] = 1;
    brc_num = 1;
    return 0;
}

void
brc_addlist (filename)
char *filename;
{
    int ftime, n, i;

    if (!strcmp (currentuser.userid, "guest"))
        return;
    ftime = atoi (&filename[2]);
    if ((filename[0] != 'M' && filename[0] != 'G') || filename[1] != '.')
    {
        return;
    }
    if (brc_num <= 0)
    {
        brc_list[brc_num++] = ftime;
        brc_changed = 1;
        return;
    }
    for (n = 0; n < brc_num; n++)
    {
        if (ftime == brc_list[n])
        {
            return;
        }
        else if (ftime > brc_list[n])
        {
            if (brc_num < BRC_MAXNUM)
                brc_num++;
            for (i = brc_num - 1; i > n; i--)
            {
                brc_list[i] = brc_list[i - 1];
            }
            brc_list[n] = ftime;
            brc_changed = 1;
            return;
        }
    }
    if (brc_num < BRC_MAXNUM)
    {
        brc_list[brc_num++] = ftime;
        brc_changed = 1;
    }
}

int
brc_unread (filename)
char *filename;
{
    int ftime, n;

    ftime = atoi (&filename[2]);
    if ((filename[0] != 'M' && filename[0] != 'G') || filename[1] != '.')
    {
        return 0;
    }
    if (brc_num <= 0)
        return 1;
    for (n = 0; n < brc_num; n++)
    {
        if (ftime > brc_list[n])
        {
            return 1;
        }
        else if (ftime == brc_list[n])
        {
            return 0;
        }
    }
    return 0;
}
/*
<<<<<<< boards.c
 
 
 
 
 
=======
*/



//add chat admin's deny list. by rwzmm @ sjtubbs

int
addtodenytalk (char *uident, char *msg, int ischange)
{
    char buf[50], strtosave[256], buf2[50];
    int fAutoUndeny = 1;
    time_t nowtime;
    int day, seek;
    char *tmp;

    seek = SeekInNameList (uident);
    if (seek && !ischange)
    {
        move (3, 0);
        prints (" %s ÒÑ¾­ÔÚÃûµ¥ÖĞ¡£", uident);
        pressanykey ();
        return -1;
    }
    if (ischange && !seek)
    {
        move (3, 0);
        prints (" %s ²»ÔÚÃûµ¥ÖĞ¡£", uident);
        pressreturn ();
        return -1;
    }
    while (1)
    {
        getdata (3, 0, "ÊäÈëËµÃ÷: ", buf, 40, DOECHO, YEA);
        if (killwordsp (buf) == 0)
            continue;
        tmp = buf;
        while (*tmp != '\0')
        {
            if (*tmp == ' ')
                *tmp = '.';
            tmp++;
        }
        break;
    }
    nowtime = time (0);
    getdatestring (nowtime, NA);
    getdata (4, 0,
             "ÊäÈë·â½ûÊ±¼ä[È±Ê¡Îª 1 Ìì, 0 Îª·ÅÆú, ÊÖ¶¯½â·âÔÚÊ±¼äÇ°¼Ó'.']: ",
             buf2, 4, DOECHO, YEA);
    if (buf2[0] == '\0')
        day = 1;
    else if (buf2[0] == '.')
    {
        fAutoUndeny = 0;
        day = atoi (buf2 + 1);
    }
    else
    {
        fAutoUndeny = 1;
        day = atoi (buf2);
    }
    if (day <= 0)
        return -1;
    if (day >= 30)
        day = 30;
    nowtime += day * 86400;
    getdatestring (nowtime, NA);
    sprintf (strtosave, "%-12s %-40s %s%16.16sµã½â", uident, buf,
             fAutoUndeny ? "(a) " : "(n) ", datestring);
    if (!ischange)
    {
        sprintf (msg,
                 "\n  %s ÍøÓÑ: \n\n"
                 "\tÎÒºÜ±§Ç¸µØ¸æËßÄú£¬ÄúÒÑ¾­±»È¡Ïû ½øÈëÁÄÌìÊÒ µÄÈ¨Àû\n\n"
                 "\tÄú±»·â½ûµÄÔ­ÒòÊÇ£º [%s]\n\n"
                 "\tÒòÎªÄúµÄĞĞÎª£¬¾ö¶¨Í£Ö¹Äã½øÈëÁÄÌìÊÒµÄÈ¨Àû [%d] Ìì\n\n"
                 "\tÇëÄúÓÚ [%14.14s] Ïò %s ·¢ĞÅÉêÇë½â·â¡£\n\n",
                 uident, buf, day, datestring, currentuser.userid);
    }
    else
    {
        sprintf (msg,
                 "\n  %s ÍøÓÑ: \n\n"
                 "\t¹ØÓÚÄã½øÈëÁÄÌìÊÒÈ¨ÏŞ±»È¡ÏûµÄÎÊÌâ£¬ÏÖ±ä¸üÈçÏÂ:\n\n"
                 "\t·â½ûµÄÔ­Òò£º [%s]\n\n"
                 "\t´ÓÏÖÔÚ¿ªÊ¼£¬Í£Ö¹¸ÃÈ¨ÀûÊ±¼ä£º [%d] Ìì\n\n"
                 "\tÇëÄúÓÚ [%14.14s] Ïò %s ·¢ĞÅÉêÇë½â·â¡£\n\n",
                 uident, buf, day, datestring, currentuser.userid);
    }
    sprintf (genbuf, "boards/deny_talk");

    if (ischange)
        del_from_file (genbuf, uident);
    return add_to_file (genbuf, strtosave);
}

/* Modify End. */

int
deldenytalk (char *uident)
{
    char fn[STRLEN];

    sprintf (fn, "boards/deny_talk");
    return del_from_file (fn, uident);
}





