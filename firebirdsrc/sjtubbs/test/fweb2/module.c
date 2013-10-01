#include <string.h>
#include <gmodule.h>
#include "fweb.h"
#include "config_reader.h"
#include "module.h"

typedef enum
{
    FLIB_FLAG_NONE = 0,
    FLIB_FLAG_GLOBAL = 1,
    FLIB_FLAG_LAZY = 1 << 1,
} FLIB_FLAG;

typedef struct
{
    GModule *mptr;
    char *path;
    char *name;
    LIB_INIT_F init;
    EVENT_ID unload_event;
    /* enter_event and exit event only take effect in page module */
    EVENT_ID enter_event;
    EVENT_ID exit_event;
    FLIB_FLAG flag;
} FLIB;

typedef struct
{
    char *url;
    char *name;
    FLIB *lib;
    PAGE_PROCESS_F func;
    unsigned int count;
    unsigned int errcount;
} FPAGE;

static gboolean fweb_dlsym(FLIB * lib, const char *func_name, void **pptr)
{
    gboolean result;

    result = g_module_symbol(lib->mptr, func_name, pptr);
    if (result)
        return TRUE;
    g_warning("can't load func %s at module %s, %s", func_name, lib->name,
              g_module_error());
    return FALSE;
}

static FLIB *fweb_dlopen(const char *dir, const char *name, FLIB_FLAG flag)
{
    GModuleFlags gflag = 0;
    FLIB *lib;
    char buf[1024];

    /* init the FLIB struct */
    lib = g_malloc(sizeof(FLIB));
    lib->name = g_strdup(name);
    g_snprintf(buf, 1024, "unload_module_%s", name);
    lib->unload_event = create_event(buf,
                                     EVENT_FLAG_REVERSE_ORDER |
                                     EVENT_FLAG_IGNORE_BREAK |
                                     EVENT_FLAG_IGNORE_FAILED);
    g_snprintf(buf, 1024, "enter_module_%s", name);
    lib->enter_event = create_event(buf, EVENT_FLAG_NONE);
    g_snprintf(buf, 1024, "exit_module_%s", name);
    lib->exit_event = create_event(buf, EVENT_FLAG_REVERSE_ORDER);

    lib->flag = flag;
    if (!(flag & FLIB_FLAG_GLOBAL))
        gflag |= G_MODULE_BIND_LOCAL;
    if (flag & FLIB_FLAG_LAZY)
        gflag |= G_MODULE_BIND_LAZY;

    /* load the shared library */
    lib->path = g_module_build_path(dir, name);
    lib->mptr = g_module_open(lib->path, gflag);
    if (NULL == lib->mptr)
    {
        g_warning("can't load module %s at path %s, %s", name, lib->path,
                  g_module_error());
        goto LOAD_MODULE_ERROR;
    }

    /* load init func of the module */
    g_snprintf(buf, 1024, "%s_init", name);
    if (!fweb_dlsym(lib, buf, (void **) &lib->init))
    {
        g_warning("can't load init func from module %s", name);
        goto LOAD_FUNC_ERROR;
    }

    /* call the init func */
    if (!lib->init(lib->unload_event, lib->enter_event, lib->exit_event))
    {
        g_warning("init func of module %s failed", name);
        goto FUNC_ERROR;
    }

    return lib;

  FUNC_ERROR:
    send_event(lib->unload_event, NULL);
  LOAD_FUNC_ERROR:
    if (!g_module_close(lib->mptr))
    {
        g_warning("unload module %s failed, %s", lib->name,
                  g_module_error());
    }
  LOAD_MODULE_ERROR:
    destroy_event(lib->unload_event, TRUE);
    destroy_event(lib->enter_event, TRUE);
    destroy_event(lib->exit_event, TRUE);
    g_free(lib->path);
    g_free(lib->name);
    g_free(lib);
    return NULL;
}

static void fweb_dlclose(FLIB * lib)
{
    send_event(lib->unload_event, NULL);
    if (!g_module_close(lib->mptr))
    {
        g_warning("unload module %s failed, %s", lib->name,
                  g_module_error());
    }
    destroy_event(lib->unload_event, TRUE);
    destroy_event(lib->enter_event, TRUE);
    destroy_event(lib->exit_event, TRUE);
    g_free(lib->name);
    g_free(lib->path);
    g_free(lib);
    return;
}

static char *global_module_dir = NULL;
static char *page_module_dir = NULL;

static CALLBACK_RETURN free_module(CALLBACK_ID callback_id, void *data,
                                   void *userdata)
{
    FLIB *lib = (FLIB *) userdata;

    fweb_dlclose(lib);
    return CALLBACK_CONTINUE;
}
static CALLBACK_RETURN free_hash_table(CALLBACK_ID callback_id, void *data,
                                       void *userdata)
{
    GHashTable *table = (GHashTable *) userdata;

    g_hash_table_destroy(table);
    return CALLBACK_CONTINUE;
}
static GHashTable *global_module_table = NULL;
static GHashTable *page_module_table = NULL;

#define _GLOBAL_MODULE 0U
#define _PAGE_MODULE 1U
static gboolean load_module(CONFIG * config, CONF_DATA * data)
{
    unsigned int i, num, module_type = (unsigned int) config->ptr;
    gboolean ret = TRUE;
    GHashTable *module_table;
    FLIB_FLAG flag = FLIB_FLAG_NONE;
    char *moddir = NULL, *desc = NULL;

    switch (module_type)
    {
    case _GLOBAL_MODULE:
        if (data == NULL)
            return TRUE;
        flag = FLIB_FLAG_GLOBAL;
        moddir = global_module_dir;
        desc = "global";
        break;
    case _PAGE_MODULE:
        flag = FLIB_FLAG_LAZY;
        moddir = page_module_dir;
        desc = "page";
        break;
    }

    num = data->c_str_l.num;
    module_table = g_hash_table_new(g_str_hash, g_str_equal);
    register_callback(event_program_exiting, free_hash_table,
                      module_table);
    for (i = 0; i < num; i++)
    {
        FLIB *lib;
        char *modname, *orig_modname;

        orig_modname = g_strdup(data->c_str_l.str_arr[i]);
        modname = g_strstrip(orig_modname);
        if (g_hash_table_lookup(module_table, modname))
        {
            g_warning("load duplicated %s module %s", desc, modname);
            g_free(orig_modname);
            ret = FALSE;
            break;
        }
        lib = fweb_dlopen(moddir, modname, flag);
        if (lib == NULL)
        {
            g_warning("load %s module failed", desc);
            g_free(orig_modname);
            ret = FALSE;
            break;
        }
        g_hash_table_insert(module_table, lib->name, lib);
        g_free(orig_modname);
        register_callback(event_program_exiting, free_module, lib);
    }
    if (ret)
    {
        switch (module_type)
        {
        case _GLOBAL_MODULE:
            global_module_table = module_table;
            break;
        case _PAGE_MODULE:
            page_module_table = module_table;
            break;
        }
    }
    return ret;
}

static GHashTable *page_table;

static void free_page(gpointer data)
{
    FPAGE *page = (FPAGE *) data;

    g_free(page->url);
    g_free(page->name);
    g_free(page);
}
static gboolean get_mod_and_func(const char *name, char **mod, char **func)
{
    char **strv;

    if (strlen(name) == 0)
        return FALSE;
    strv = g_strsplit_set(name, ":", 2);

    /* there must be 2 pieces */
    if (strv[1] == NULL)
    {
        g_strfreev(strv);
        return FALSE;
    }
    *mod = g_strdup(strv[0]);
    *func = g_strdup(strv[1]);
    g_strfreev(strv);
    return TRUE;
}
static gboolean load_page(const char *url, const char *name)
{
    char *modname, *funcname;
    FPAGE *page;
    FLIB *lib;

    if (g_hash_table_lookup(page_table, url))
    {
        g_warning("duplicated url: '%s'", url);
        return FALSE;
    }
    if (!get_mod_and_func(name, &modname, &funcname))
    {
        g_warning("unknown page processor '%s' for url '%s'", name, url);
        return FALSE;
    }
    lib = g_hash_table_lookup(page_module_table, modname);
    if (!lib)
    {
        g_warning("unknown page module '%s' for url '%s'", modname, url);
        goto UNKNOWN_MODULE_ERROR;
    }
    page = g_malloc(sizeof(FPAGE));
    page->lib = lib;
    page->count = 0;
    page->errcount = 0;
    if (!fweb_dlsym(lib, funcname, (void **) &(page->func)))
    {
        g_warning("can't load func '%s' at module '%s' for url '%s'",
                  funcname, modname, url);
        goto UNKNOWN_FUNC_ERROR;
    }
    page->url = g_strdup(url);
    page->name = g_strdup(name);

    g_hash_table_insert(page_table, page->url, page);
    g_free(modname);
    g_free(funcname);

    return TRUE;

  UNKNOWN_FUNC_ERROR:
    g_free(page);
  UNKNOWN_MODULE_ERROR:
    g_free(modname);
    g_free(funcname);
    return FALSE;
}
static gboolean load_pages(CONFIG * config, CONF_DATA * data)
{
    unsigned int i, num;

    page_table =
        g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free_page);
    register_callback(event_program_exiting, free_hash_table, page_table);
    num = data->c_all_l.num;
    for (i = 0; i < num; i++)
    {
        if (!load_page(data->c_all_l.keys[i], data->c_all_l.values[i]))
        {
            g_warning("load pages failed");
            return FALSE;
        }
    }
    return TRUE;
}

static const char *group_pages = "pages";
gboolean module_init(void)
{
    CONFIG config[] = {
        {group_fweb, "global_module_dir", CONF_TYPE_STRING,
         CONF_FLAG_REQURIED, set_dir, &global_module_dir},
        {group_fweb, "page_module_dir", CONF_TYPE_STRING,
         CONF_FLAG_REQURIED, set_dir, &page_module_dir},
        {group_fweb, "global_module", CONF_TYPE_STR_LIST, CONF_FLAG_NONE,
         load_module, (void *) _GLOBAL_MODULE},
        {group_fweb, "page_module", CONF_TYPE_STR_LIST, CONF_FLAG_REQURIED,
         load_module, (void *) _PAGE_MODULE},
        {group_pages, NULL, CONF_TYPE_ALL_LIST, CONF_FLAG_REQURIED,
         load_pages, NULL},
    };

    if (!config_parse(config, G_N_ELEMENTS(config)))
    {
        return FALSE;
    }
    return TRUE;
}

jmp_buf QUIT_JMP;

gboolean invoke_page_processor(const char *url)
{
    FPAGE *page;

    page = g_hash_table_lookup(page_table, url);
    if (!page)
    {
        g_warning("no page processor for url '%s'", url);
        return FALSE;
    }
    page->count++;

    if (!send_event(page->lib->enter_event, NULL))
    {
        g_warning("send enter_event for module %s failed",
                  page->lib->name);
        send_event(page->lib->exit_event, NULL);
        return FALSE;
    }
    if (setjmp(QUIT_JMP))
    {
        goto EXIT_EVENT;
    }
    if (0 != page->func())
    {
        page->errcount++;
    }
  EXIT_EVENT:
    if (!send_event(page->lib->exit_event, NULL))
    {
        g_warning("send exit_event for module %s failed", page->lib->name);
        return FALSE;
    }

    return TRUE;
}
