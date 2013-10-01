#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <glib.h>
#include "const.h"
#include "log.h"


#define L_ERROR 0
#define L_WARNING 1
#define L_MESSAGE 2
#define L_DEBUG 3
static FILE *logFiles[4] = { NULL, NULL, NULL, NULL };

static pid_t pid;


static void
log(const gchar * level, const gchar * message, int filepos)
{
	time_t now = time(NULL);
	fprintf(logFiles[filepos], "%s - %d - %.24s - %s\n",
		level, pid, ctime(&now), message);
	fflush(logFiles[filepos]);
}
static void
error_handler(const gchar * log_domain, GLogLevelFlags log_level,
	      const gchar * message, gpointer user_data)
{
#ifdef	L_ERROR_FILE
	log("ERROR", message, L_ERROR);
#endif
}

static void
warning_handler(const gchar * log_domain, GLogLevelFlags log_level,
		const gchar * message, gpointer user_data)
{
#ifdef	L_WARNING_FILE
	log("WARNING", message, L_WARNING);
#endif
}

static void
message_handler(const gchar * log_domain, GLogLevelFlags log_level,
		const gchar * message, gpointer user_data)
{
#ifdef 	L_MESSAGE_FILE
	log("MESSAGE", message, L_MESSAGE);
#endif
}

static void
debug_handler(const gchar * log_domain, GLogLevelFlags log_level,
	      const gchar * message, gpointer user_data)
{
#ifdef	L_DEBUG_FILE
	log("DEBUG", message, L_DEBUG);
#endif
}

static void
close_files(void)
{
	if (logFiles[L_ERROR])
		fclose(logFiles[L_ERROR]);
	if (logFiles[L_MESSAGE])
		fclose(logFiles[L_MESSAGE]);
	if (logFiles[L_WARNING])
		fclose(logFiles[L_WARNING]);
	if (logFiles[L_DEBUG])
		fclose(logFiles[L_DEBUG]);
}

int
log_init(void)
{

	pid = getpid();
#ifdef	L_ERROR_FILE
	logFiles[L_ERROR] = fopen(L_ERROR_FILE, "a");
	if (!logFiles[L_ERROR])
	{
		close_files();
		return 1;
	}
#endif

#ifdef	L_WARNING_FILE
	logFiles[L_WARNING] = fopen(L_WARNING_FILE, "a");
	if (!logFiles[L_WARNING])
	{
		close_files();
		return 2;
	}
#endif

#ifdef 	L_MESSAGE_FILE
	logFiles[L_MESSAGE] = fopen(L_MESSAGE_FILE, "a");
	if (!logFiles[L_MESSAGE])
	{
		close_files();
		return 3;
	}
#endif

#ifdef	L_DEBUG_FILE
	logFiles[L_DEBUG] = fopen(L_DEBUG_FILE, "a");
	if (!logFiles[L_DEBUG])
	{
		close_files();
		return 4;
	}
#endif
	g_log_set_handler(NULL, G_LOG_LEVEL_ERROR, error_handler, NULL);
	g_log_set_handler(NULL, G_LOG_LEVEL_WARNING, warning_handler, NULL);
	g_log_set_handler(NULL, G_LOG_LEVEL_MESSAGE, message_handler, NULL);
	g_log_set_handler(NULL, G_LOG_LEVEL_DEBUG, debug_handler, NULL);

	return 0;
}

int
log_final(void)
{
	close_files();
	return 0;
}
