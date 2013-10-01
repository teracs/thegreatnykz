#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include <glib/gstdio.h>
#include "config_reader.h"
#include "dispatcher.h"
#include "fweb.h"

const char *group_fweb = "fweb";
static GKeyFile *fweb_config = NULL;

static CALLBACK_RETURN config_final(CALLBACK_ID callback_id, void *data,
                                    void *userdata)
{
    if (fweb_config)
    {
        g_key_file_free(fweb_config);
        fweb_config = NULL;
    }
    else
    {
        g_warning("fweb config hasn't been initialized, but call final");
    }
    return CALLBACK_CONTINUE;
}

gboolean config_init(const char *config_path)
{
    GError *err = NULL;

    fweb_config = g_key_file_new();

    if (TRUE == g_key_file_load_from_file(fweb_config,
                                          config_path,
                                          G_KEY_FILE_NONE, &err))
    {
        register_callback(event_program_exiting, config_final, NULL);
        return TRUE;
    }

    g_warning("can't read fweb config file at %s: %s",
              config_path, err->message);
    g_error_free(err);
    g_key_file_free(fweb_config);
    fweb_config = NULL;
    return FALSE;
}

gboolean check_dir(const char *dir)
{
    struct stat st;

    if ((0 == stat(dir, &st))
        && (0 == access(dir, R_OK | X_OK)) && S_ISDIR(st.st_mode))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static CALLBACK_RETURN free_dir(CALLBACK_ID callback_id, void *data,
                                void *userdata)
/* userdata is a pointer to (char *), which store a g_malloc string pointer */
{
    char **pdir = (char **) userdata;

    g_free(*pdir);
    *pdir = NULL;
    return CALLBACK_CONTINUE;
}

gboolean set_dir(CONFIG * config, CONF_DATA * data)
/* config->ptr is a pointer to (char *), which is used to store a g_malloc string pointer */
{
    if (!check_dir(data->c_str))
    {
        g_warning("can't access dir %s", data->c_str);
        return FALSE;
    }
    else
    {
        char **pdir = config->ptr;

        *pdir = g_strdup(data->c_str);
        register_callback(event_program_exiting, free_dir, pdir);
        return TRUE;
    }
}

gboolean strtouint64(const char *str, guint64 * integer)
{
    guint64 i64;
    char *endptr;

    errno = 0;
    i64 = strtoull(str, &endptr, 0);
    if (errno == ERANGE)
        return FALSE;
    else if (errno == EINVAL)
        return FALSE;
    else if (endptr == str)
        return FALSE;
    else if (*endptr != '\0')
        return FALSE;
    else
    {
        *integer = i64;
        return TRUE;
    }
}

gboolean strtoint64(const char *str, gint64 * integer)
{
    gint64 i64;
    char *endptr;

    errno = 0;
    i64 = strtoll(str, &endptr, 0);
    if (errno == ERANGE)
        return FALSE;
    else if (errno == EINVAL)
        return FALSE;
    else if (endptr == str)
        return FALSE;
    else if (*endptr != '\0')
        return FALSE;
    else
    {
        *integer = i64;
        return TRUE;
    }
}
static gboolean error_check(CONFIG config, GError * err)
{
    gboolean ret;

    if (err->code == G_KEY_FILE_ERROR_KEY_NOT_FOUND
        || err->code == G_KEY_FILE_ERROR_GROUP_NOT_FOUND)
    {
        if (config.flag & CONF_FLAG_REQURIED)
        {
            g_warning("a requried config can't be found: [%s]->'%s'",
                      config.group, config.key);
            ret = FALSE;
        }
        else
        {
            ret = (*config.set_func) (&config, NULL);
        }
    }
    else
    {
        g_warning("parse [%s]->'%s' failed: %s",
                  config.group, config.key, err->message);
        ret = FALSE;
    }
    g_error_free(err);
    return ret;
}
static gboolean parse_string(CONFIG config)
{
    GError *err = NULL;
    char *val;
    gboolean ret;
    CONF_DATA data;

    val =
        g_key_file_get_string(fweb_config, config.group, config.key, &err);
    if (err == NULL)
    {
        data.c_str = val;
        ret = (*config.set_func) (&config, &data);
        g_free(val);
        return ret;
    }
    else
    {
        return error_check(config, err);
    }

}
static gboolean parse_int(CONFIG config)
{
    GError *err = NULL;
    char *val;
    gboolean ret;
    CONF_DATA data;

    val =
        g_key_file_get_string(fweb_config, config.group, config.key, &err);
    if (err == NULL)
    {
        gint64 i64;

        if (strtoint64(val, &i64))
        {
            data.c_int = i64;
            ret = (*config.set_func) (&config, &data);
        }
        else
        {
            g_warning("can't convert %s to integer, [%s]->'%s'",
                      val, config.group, config.key);
            ret = FALSE;
        }
        g_free(val);
        return ret;
    }
    else
    {
        return error_check(config, err);
    }
}

static gboolean parse_uint(CONFIG config)
{
    GError *err = NULL;
    char *val;
    gboolean ret;
    CONF_DATA data;

    val =
        g_key_file_get_string(fweb_config, config.group, config.key, &err);
    if (err == NULL)
    {
        guint64 i64;

        if (strtouint64(val, &i64))
        {
            data.c_int = i64;
            ret = (*config.set_func) (&config, &data);
        }
        else
        {
            g_warning("can't convert %s to unsigned integer, [%s]->'%s'",
                      val, config.group, config.key);
            ret = FALSE;
        }
        g_free(val);
        return ret;
    }
    else
    {
        return error_check(config, err);
    }
}
static gboolean parse_bool(CONFIG config)
{
    GError *err = NULL;
    gboolean val;
    gboolean ret;
    CONF_DATA data;

    val =
        g_key_file_get_boolean(fweb_config, config.group, config.key,
                               &err);
    if (err == NULL)
    {
        data.c_bool = val;
        ret = (*config.set_func) (&config, &data);
        return ret;
    }
    else
    {
        return error_check(config, err);
    }

}
static gboolean parse_double(CONFIG config)
{
    GError *err = NULL;
    gdouble val;
    gboolean ret;
    CONF_DATA data;

    val =
        g_key_file_get_double(fweb_config, config.group, config.key, &err);
    if (err == NULL)
    {
        data.c_double = val;
        ret = (*config.set_func) (&config, &data);
        return ret;
    }
    else
    {
        return error_check(config, err);
    }

}
static gboolean parse_str_list(CONFIG config)
{
    GError *err = NULL;
    char **val;
    unsigned int num;
    gboolean ret;
    CONF_DATA data;

    val =
        g_key_file_get_string_list(fweb_config, config.group, config.key,
                                   &num, &err);
    if (err == NULL)
    {
        data.c_str_l.num = num;
        data.c_str_l.str_arr = val;
        ret = (*config.set_func) (&config, &data);
        g_strfreev(val);
        return ret;
    }
    else
    {
        return error_check(config, err);
    }

}
static gboolean parse_bool_list(CONFIG config)
{
    GError *err = NULL;
    gboolean *val;
    unsigned int num;
    gboolean ret;
    CONF_DATA data;

    val =
        g_key_file_get_boolean_list(fweb_config, config.group, config.key,
                                    &num, &err);
    if (err == NULL)
    {
        data.c_bool_l.num = num;
        data.c_bool_l.bool_arr = val;
        ret = (*config.set_func) (&config, &data);
        g_free(val);
        return ret;
    }
    else
    {
        return error_check(config, err);
    }

}

static gboolean parse_double_list(CONFIG config)
{
    GError *err = NULL;
    gdouble *val;
    unsigned int num;
    gboolean ret;
    CONF_DATA data;

    val =
        g_key_file_get_double_list(fweb_config, config.group, config.key,
                                   &num, &err);
    if (err == NULL)
    {
        data.c_double_l.num = num;
        data.c_double_l.double_arr = val;
        ret = (*config.set_func) (&config, &data);
        g_free(val);
        return ret;
    }
    else
    {
        return error_check(config, err);
    }

}
static gboolean parse_int_list(CONFIG config)
{
    GError *err = NULL;
    char **val;
    unsigned int num;
    gboolean ret;
    CONF_DATA data;

    val =
        g_key_file_get_string_list(fweb_config, config.group, config.key,
                                   &num, &err);
    if (err == NULL)
    {
        gint64 *pi64;
        unsigned int i;

        pi64 = g_malloc(num * sizeof(gint64));
        ret = TRUE;
        for (i = 0; i < num; i++)
        {
            if (!strtoint64(val[i], &pi64[i]))
            {
                g_warning("can't convert %s to integer, [%s]->'%s'",
                          val[i], config.group, config.key);
                ret = FALSE;
                break;
            }
        }
        if (ret)
        {
            data.c_int_l.num = num;
            data.c_int_l.int_arr = pi64;
            ret = (*config.set_func) (&config, &data);
        }
        g_free(pi64);
        g_strfreev(val);
        return ret;
    }
    else
    {
        return error_check(config, err);
    }

}
static gboolean parse_uint_list(CONFIG config)
{
    GError *err = NULL;
    char **val;
    unsigned int num;
    gboolean ret;
    CONF_DATA data;

    val =
        g_key_file_get_string_list(fweb_config, config.group, config.key,
                                   &num, &err);
    if (err == NULL)
    {
        guint64 *pi64;
        unsigned int i;

        pi64 = g_malloc(num * sizeof(guint64));
        ret = TRUE;
        for (i = 0; i < num; i++)
        {
            if (!strtouint64(val[i], &pi64[i]))
            {
                g_warning
                    ("can't convert %s to unsigned integer, [%s]->'%s'",
                     val[i], config.group, config.key);
                ret = FALSE;
                break;
            }
        }
        if (ret)
        {
            data.c_uint_l.num = num;
            data.c_uint_l.uint_arr = pi64;
            ret = (*config.set_func) (&config, &data);
        }
        g_free(pi64);
        g_strfreev(val);
        return ret;
    }
    else
    {
        return error_check(config, err);
    }

}

static gboolean parse_all_list(CONFIG config)
{
    GError *err = NULL;
    char **key;
    unsigned int num;
    gboolean ret;
    CONF_DATA data;

    key = g_key_file_get_keys(fweb_config, config.group, &num, &err);
    if (err == NULL)
    {
        char **val;
        unsigned int i;
        val = g_malloc(num * sizeof(char *));
        bzero(val, num * sizeof(char *));
        ret = TRUE;
        for (i = 0; i < num; i++)
        {
            val[i] =
                g_key_file_get_string(fweb_config, config.group, key[i],
                                      &err);
            if (err != NULL)
            {
                g_warning("get config [%s]->'%s' failed: %s",
                          config.group, key[i], err->message);
                ret = FALSE;
                g_error_free(err);
                break;
            }
        }
        if (ret)
        {
            data.c_all_l.num = num;
            data.c_all_l.keys = key;
            data.c_all_l.values = val;
            ret = (*config.set_func) (&config, &data);
        }
        for (i = 0; i < num; i++)
        {
            if (val[i])
                g_free(val[i]);
        }
        g_free(val);
        g_strfreev(key);
        return ret;
    }
    else
    {
        return error_check(config, err);
    }

}

gboolean config_parse(CONFIG config[], int num)
{
    int i;
    gboolean ret;

    for (i = 0; i < num; i++)
    {
        switch (config[i].type)
        {
        case CONF_TYPE_STRING:
            ret = parse_string(config[i]);
            break;
        case CONF_TYPE_INT:
            ret = parse_int(config[i]);
            break;
        case CONF_TYPE_UINT:
            ret = parse_uint(config[i]);
            break;
        case CONF_TYPE_BOOLEAN:
            ret = parse_bool(config[i]);
            break;
        case CONF_TYPE_DOUBLE:
            ret = parse_double(config[i]);
            break;
        case CONF_TYPE_STR_LIST:
            ret = parse_str_list(config[i]);
            break;
        case CONF_TYPE_INT_LIST:
            ret = parse_int_list(config[i]);
            break;
        case CONF_TYPE_UINT_LIST:
            ret = parse_uint_list(config[i]);
            break;
        case CONF_TYPE_BOOLEAN_LIST:
            ret = parse_bool_list(config[i]);
            break;
        case CONF_TYPE_DOUBLE_LIST:
            ret = parse_double_list(config[i]);
            break;
        case CONF_TYPE_ALL_LIST:
            ret = parse_all_list(config[i]);
            break;
        default:
            g_warning("unknown CONF_TYPE, [%s]->'%s'",
                      config[i].group, config[i].key);
            ret = FALSE;
            break;
        }
        if (ret == FALSE)
            return FALSE;
    }
    return TRUE;
}
