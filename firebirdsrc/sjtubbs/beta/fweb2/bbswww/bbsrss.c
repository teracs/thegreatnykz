#include "www.h"
#include <time.h>

int
bbsrss_main()
{
	FILE *fp, *fpcache;
	char board[80], dir[80];
	struct shortfile *x1;
	struct fileheader x;
	int i, start, total, count = 0, cache = 1;
	char buff[20][2048];

	char boardLink[128], boardDes[128], buf[1024], filename[80],
		cachename[80];
	struct stat latestfile, cachefile;

	silence = 1;
	init_all();
	printf("Connection: close\n");
	printf("Content-type: text/xml;charset=GB2312\n\n");
	fflush(0);
	count = 20;

	strsncpy(board, getparm("board"), 32);
	x1 = getbcache(board);
	if (x1 == 0)
		http_quit_print("错误的讨论区1");
	strcpy(board, x1->filename);
	if (!has_read_perm(&currentuser, board))
		http_quit_print("错误的讨论区2");

	sprintf(dir, "boards/%s/.DIR", board);
	total = file_size(dir) / sizeof(x);
	if (total == 0)
		http_quit_print("讨论区暂无文章");
	start = total - count;
	if (start < 0)
	{
		start = 0;
		count = total;
	}
	sprintf(cachename, "boards/%s/rss2.xml", board);
	if (stat(cachename, &cachefile) == -1)
		cache = 0;
	else
	{
		if (stat(dir, &latestfile) == -1)
			http_quit_print("fatal error1");
		if (cachefile.st_mtime >= latestfile.st_mtime)
			cache = 1;
		else
			cache = 0;

	}

	if (cache == 0)
	{
		fpcache = fopen(cachename, "w");
		flock(fileno(fpcache), LOCK_EX);
		sprintf(boardLink, "http://"BBSHOST"/bbsdoc?board=%s",
			board);
		strcpy(boardDes, x1->title + 11);
		fprintf(fpcache,
			"<?xml version=\"1.0\" encoding=\"GB2312\"?>\n<!-- generator=\"fweb/0.1\" -->\n<rss version=\"2.0\">\n	<channel>\n		<title>%s</title>\n		<description> %s </description>\n		<link>%s</link>\n		<generator>%s</generator>\n		",
			board, boardDes, boardLink, BBSHOST);
		fp = fopen(dir, "r");
		if (fseek(fp, (start * sizeof(x)), SEEK_SET) == -1)
		{
			fclose(fp);
			http_quit_print("seek error");
		}
		for (i = 0; i < count; i++)
		{
			char date[128];
			if (fread(&x, sizeof(x), 1, fp) <= 0)
				break;

			sprintf(filename, "boards/%s/%s/%s", board, getfolderstr(x.filename), x.filename);
			stat(filename, &cachefile);
			strftime(date, 128, "%a,%d %b %Y %H:%M:%S %z",
				 localtime(&cachefile.st_mtime));
			sprintf(buff[i],
				"<item>	\n			<title> %s </title>	\n			<link> <![CDATA[http://"BBSHOST"/bbscon?board=%s&file=%s]]> </link>\n			<author> %s </author>	\n			<pubDate> %s </pubDate>	\n		</item>\n",
				(void1(nohtml2(x.title))),
				board,
				x.filename,  x.owner, date);
		}
		fclose(fp);
		for (i = 19; i >= 0; i--)
		{
			fprintf(fpcache, "%s", buff[i]);
		}
		fprintf(fpcache, "%s", "</channel></rss>\n");
		flock(fileno(fpcache), LOCK_UN);
		fclose(fpcache);
	}

	fpcache = fopen(cachename, "r");
	while (1)
	{
		int r;
		r = fread(buf, 1, 512, fpcache);
		if (r <= 0)
			break;
		fwrite(buf, 1, r, stdout);

	}
	fclose(fpcache);



	fflush(0);
	return 0;
}
