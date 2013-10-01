
#ifndef _H_CONFIG_READER
#define _H_CONFIG_READER

#include <glib.h>

/**
 *
 * This file provide a config file parsing mechanism.
 * The config file format is like windows ini file.
 * Config file consists of groups of key-value pairs.
 * Line start with '#' is considered as comment.
 * For example:
 * [group1]
 * key1=value1
 * # I'm a comment
 * key2=value2
 * [group2]
 * # I'm comment, too
 * key3=value3
 *
 * see the comment of function config_parse for how-to
 */
G_BEGIN_DECLS
/**
 * config type flag, used to define the type of config value
 *
 * @see config_parse
 * @see CONFIG
 * @see CONF_DATA
 */
    typedef enum
{
        /**
	 * is char* 
	 * example:
	 * abc=edf
	 */
    CONF_TYPE_STRING,
        /**
	 * is gint64
	 * example:
	 * abc=-1234
	 * edf=1234
	 */
    CONF_TYPE_INT,
        /**
	 * is guint64
	 * example:
	 * abc=1234
	 */
    CONF_TYPE_UINT,
        /**
	 * is gboolean
	 * example:
	 * abc=true
	 * edf=false
	 */
    CONF_TYPE_BOOLEAN,
        /**
	 * is gdouble
	 * example:
	 * abc=1.234
	 * edf=-1.234
	 */
    CONF_TYPE_DOUBLE,
        /**
	 * is char* array
	 * example:
	 * abc=edf;ghi;jkl
	 * will be parsed as char* array[] = {"edf","ghi","jkl"}
	 */
    CONF_TYPE_STR_LIST,
        /**
	 * is gint64 array
	 * example:
	 * abc=12;34;-56
	 * will be parsed as gint64 array[] = {12,34,-56}
	 */
    CONF_TYPE_INT_LIST,
        /**
	 * is guint64 array
	 * example:
	 * abc=12;34;56
	 * will be parsed as guint64 array[] = {12,34,56}
	 */
    CONF_TYPE_UINT_LIST,
        /**
	 * is gdouble array
	 * example:
	 * abc=1.2;3.4;-5.6
	 * will be parsed as gdouble array[] = {1.2,3.4,-5.6}
	 */
    CONF_TYPE_DOUBLE_LIST,
        /**
	 * is gboolean array
	 * example:
	 * abc=true;false;false
	 * will be parsed as gboolean array[] = {TRUE,FALSE,FALSE}
	 */
    CONF_TYPE_BOOLEAN_LIST,
        /**
	 * is char* array
	 * it operate on group instead of key-value
	 * example:
	 * [abc]
	 * d=1234
	 * e=text
	 * c=-12.34
	 * will be parsed as two arrays
	 * char *key[] = {"d","e","f"}
	 * char *value[] = {"1234","text","-12.34"}
	 */
    CONF_TYPE_ALL_LIST,
} CONF_TYPE;

/**
 * config flag
 */
typedef enum
{
    CONF_FLAG_NONE = 0,         /* no flag */
    CONF_FLAG_REQURIED = 1 << 0,        /* this config must be provided */
} CONF_FLAG;

/**
 * this union is used to store parsed config value
 * the value will be set according to CONF_TYPE 
 */
typedef union
{
    /* set if type is CONF_TYPE_STRING */
    char *c_str;
    /* set if type is CONF_TYPE_INT */
    gint64 c_int;
    /* set If type is CONF_TYPE_UINT */
    guint64 c_uint;
    /* set if type is CONF_TYPE_BOOL */
    gboolean c_bool;
    /* set if type is CONF_TYPE_DOUBLE */
    gdouble c_double;
        /**
	 * set if type is CONF_TYPE_STR_LIST
	 * num is the char* array length, str_arr is char* array
	 */
    struct
    {
        unsigned int num;
        char **str_arr;
    } c_str_l;
        /**
	 * set if type is CONF_TYPE_INT_LIST
	 * num is the gint64 array length, int_arr is gint64 array
	 */
    struct
    {
        unsigned int num;
        gint64 *int_arr;
    } c_int_l;
        /**
	 * set if type is CONF_TYPE_UNIT_LIST
	 * num is the guint64 array length, uint_arr is guint64 array
	 */
    struct
    {
        unsigned int num;
        guint64 *uint_arr;
    } c_uint_l;
        /**
	 * set if type is CONF_TYPE_BOOL_LIST
	 * num is the gboolean array length, bool_arr is gboolean array
	 */
    struct
    {
        unsigned int num;
        gboolean *bool_arr;
    } c_bool_l;
        /**
	 * set if type is CONF_TYPE_DOUBLE_LIST
	 * num is the gdouble array length, double_arr is gdouble array
	 */
    struct
    {
        unsigned int num;
        gdouble *double_arr;
    } c_double_l;
        /**
	 * set if type is CONF_TYPE_ALL_LIST
	 * num is the char* array length( keys and values array are the same length),
	 * keys and values are char* array
	 */
    struct
    {
        unsigned int num;
        char **keys;
        char **values;
    } c_all_l;
} CONF_DATA;

struct _CONFIG;

/**
 * config callback sef_func prototype
 *
 * @param config the CONFIG struct associated with this set_func,
 *		the most useful data maybe ptr in the struct. it can be used
 *		as user specified data pointer.
 * @param data the CONF_DATA union associated with this set_func
 *
 * @return return TRUE on success, FALSE on error. if return FALSE, it will cause 
 *		config_parse stop parsing and return failed
 *
 * @see CONFIG
 * @see config_parse
 */
typedef gboolean(*CONF_SET_F) (struct _CONFIG * config, CONF_DATA * data);

typedef struct _CONFIG
{
    /* config group */
    const char *group;
    /* config key, if type is CONF_TYPE_ALL_LIST, set key to NULL */
    const char *key;
    /* the config type */
    CONF_TYPE type;
    /* the config flag */
    CONF_FLAG flag;
    /* callback to set the config value */
    CONF_SET_F set_func;
    /* store any data, if needed */
    void *ptr;
} CONFIG;

/**
 * parse the config file using config[] 
 * If get the config value successfully, call config->set_func function with parameters
 * 'config' set to the relative CONFIG and 'data' set to the config value
 * The provided data will be freed automatically after set_func return, so
 * don't set any pointer to the data's value
 * If the config is not exist and flag is set to CONF_NONE, call set_func
 * with config set to the relative CONFIG, data set to NULL
 * If the config is not exist and flag is set to CONF_REQURIED, return FALSE
 * If the set_func return FALSE, config_parse will stop parse and return FALSE
 * The parse order is the same as the  provided config[] order
 *
 * @param config the config array to be parse
 * @param num the length of config array
 *
 * @return: on success, return TRUE
 *          on error, return FALSE 
 * 
 * @see CONFIG
 **/
extern gboolean config_parse(CONFIG config[], int num);

/**
 * the main group name
 */
extern const char *group_fweb;

/**
 * initialize the config subsystem
 *
 * @param config_path the config file path
 *
 * @return TRUE on success, FALSE on error
 */
extern gboolean config_init(const char *config_path);

/**
 * some utility functions
 */

/**
 * transfer string to gint64
 *
 * @param str the string
 * @param integer OUT: the result integer, if failed, the result is undefined
 *
 * @return TRUE on success, FALSE on error
 */
extern gboolean strtoint64(const char *str, gint64 * integer);

/**
 * transfer string to guint64
 *
 * @param str the string
 * @param integer OUT: the result integer, if failed, the result is undefined
 *
 * @return TRUE on success, FALSE on error
 */
extern gboolean strtouint64(const char *str, guint64 * integer);

/**
 * check a path if it is a directory and readable
 *
 * @param dir the path
 *
 * @return TRUE if dir is directory and readable, FALSE if not
 */
extern gboolean check_dir(const char *dir);

/**
 * this is a CONF_SET_F function.
 * check config value is directory and readable,
 * then set config->ptr (it must be a char**) to the value (the
 * value is g_malloc memory, and it will be free while program existing).
 * the config->type must be CONF_TYPE_STRING
 * 
 * @param config the config
 * @param data   the config value
 * 
 * @return TRUE on sucess, FALSE on error
 * @example 
 * char *path; 
 * CONFIG config[] = { 
 * {"mygroup","mykey",CONF_TYPE_STRING,CONF_FLAG_REQUIRED,set_dir,&path}, 
 * }; 
 * 
 * if(config_parse(config,1))
 *    ...use path...
 * else
 *    ...deal with error
 */
extern gboolean set_dir(CONFIG * config, CONF_DATA * data);

G_END_DECLS
#endif
/*end _H_CONFIG_READER */
