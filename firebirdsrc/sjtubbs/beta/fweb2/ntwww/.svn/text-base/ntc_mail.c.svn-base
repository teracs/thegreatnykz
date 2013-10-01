#include <glib.h>

#include "ntcore.h"
#include "ntcinternal.h"


unsigned int nt_mail_get_num(const char* userid)
{
	char path[1024];
	
	nt_setmailfile(path,sizeof(path),userid, ".DIR");
	return nt_stat_size(path)/sizeof(struct fileheader);
}

unsigned int nt_mail_get_unread_num(const char* userid)
{
	char path[1024];
	unsigned int num;
	GMappedFile *map_dir;
	struct fileheader *fheader, *fheader_end;
	
	nt_setmailfile(path,sizeof(path),userid, ".DIR");
	map_dir = g_mapped_file_new(path,FALSE,NULL);

	if(map_dir==NULL)
		return 0;

	fheader = (struct fileheader *) g_mapped_file_get_contents(map_dir);
	fheader_end = fheader + g_mapped_file_get_length(map_dir)/sizeof(struct fileheader);
	for(num=0;fheader != fheader_end;fheader++)
	{
		if(!(fheader->accessed[0] & FILE_READ)) num++;
	}
	g_mapped_file_free(map_dir);
	return num;
}
