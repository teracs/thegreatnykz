#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <glib.h>
#include "fweblog.h"
#include "config_reader.h"
#include "dispatcher.h"
#include "fweb.h"

#define L_DEBUG 0
#define L_MESSAGE 1
#define L_WARNING 2
#define L_ERROR 3
#define LOG_NUM 4

static FILE *logFiles[LOG_NUM] = { NULL, NULL, NULL, NULL };
static const char *group_log = "log";
static const char *key_levels[LOG_NUM] =
    { "debug", "message", "warning", "error" };
static const char *level_desc[LOG_NUM] =
    { "DEBUG", "MESSAGE", "WARNING", "ERROR" };

static pid_t pid;

static void
log_handler(const char *log_domain, GLogLevelFlags log_level,
            const gchar * message, gpointer user_data)
{
    int level;

    switch (log_level)
    {
    case G_LOG_LEVEL_ERROR:
    case G_LOG_LEVEL_CRITICAL:
        level = L_ERROR;
        break;
    case G_LOG_LEVEL_WARNING:
        level = L_WARNING;
        break;
    case G_LOG_LEVEL_MESSAGE:
    case G_LOG_LEVEL_INFO:
        level = L_MESSAGE;
        break;
    case G_LOG_LEVEL_DEBUG:
        level = L_DEBUG;
        break;
    default:
        level = L_ERROR;
        break;
    }

    if (logFiles[level])
    {
        time_t now = time(NULL);

        if (NULL != log_domain)
            fprintf(logFiles[level], "%s - %d - %.24s - (%s) %s\n",
                    level_desc[level], pid, ctime(&now), log_domain,
                    message);
        else
            fprintf(logFiles[level], "%s - %d - %.24s - (unknown) %s\n",
                    level_desc[level], pid, ctime(&now), message);

    }
}
static void close_files(void)
{
    int i;

    for (i = 0; i < LOG_NUM; i++)
    {
        if (logFiles[i] != NULL)
        {
            fclose(logFiles[i]);
            logFiles[i] = NULL;
        }
    }
}

static gboolean set_log_file(CONFIG * config, CONF_DATA * data)
{
    int i;
    char *log_filepath;

    if (NULL == data)
        return TRUE;
    log_filepath = data->c_str;
    i = (int) config->ptr;
    FILE *openfile;

    openfile = fopen(log_filepath, "a");
    if (NULL == openfile)
    {
        g_warning("can not open log file at %s", log_filepath);
        return FALSE;
    }
    setlinebuf(openfile);
    logFiles[i] = openfile;
    return TRUE;
}
static CALLBACK_RETURN
log_final(CALLBACK_ID callback_id, void *data, void *userdata)
{
    close_files();
    return CALLBACK_CONTINUE;
}

gboolean log_init(void)
{
    CONFIG config[] = {
        {group_log, key_levels[L_DEBUG], CONF_TYPE_STRING, CONF_FLAG_NONE,
         set_log_file, (void *) L_DEBUG},
        {group_log, key_levels[L_MESSAGE], CONF_TYPE_STRING,
         CONF_FLAG_NONE, set_log_file, (void *) L_MESSAGE},
        {group_log, key_levels[L_WARNING], CONF_TYPE_STRING,
         CONF_FLAG_NONE, set_log_file, (void *) L_WARNING},
        {group_log, key_levels[L_ERROR], CONF_TYPE_STRING, CONF_FLAG_NONE,
         set_log_file, (void *) L_ERROR},
    };
    if (!config_parse(config, G_N_ELEMENTS(config)))
    {
        close_files();
        return FALSE;
    }
    pid = getpid();
    g_log_set_default_handler(log_handler, NULL);
    register_callback(event_program_exiting, log_final, NULL);
    return TRUE;
}
