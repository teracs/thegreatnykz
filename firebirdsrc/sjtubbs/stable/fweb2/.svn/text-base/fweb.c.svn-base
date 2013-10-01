/******************************************************************
 * program: fweb
 * author: jawahh
 * version: 2
 * email: jawahh at gmail.com
 * fweb is a web interface for fb2k that is based on fastcgi and uses
 * glib (http://developer.gnome.org/doc/API/2.0/glib/index.html), 
 * ctemplate(http://goog-ctemplate.sourceforge.net/) and
 * cgic (http://www.boutell.com/cgic/) library. And the most of program
 * source is taken from njuwebbsd 0.90. Thanks for the original authors.
 * This program was written for my favorite BBS bbs.sjtu.edu.cn:)
 *******************************************************************/

/*******************************************************************
 *  1. fweb v2新增加了一个配置文件fweb.ini。配置文件的具体参数意义请
 *  参考自带的fweb.ini中的解析。fweb.ini的路径可以通过调用程序的第一个
 *  参数传入。如果没有传入，就会去默认位置寻找（由宏FWEB_CONF_PATH定义）
 *
 *  2. fweb提供在运行期间装载两种模块，全局模块和页面模块。全局模块负责
 *  提供一些页面模块中要使用的功能；一来避免在各个模块中实现重复功能，二
 *  来避免在各个页面模块中多次初始化同一功能。页面模块负责输出页面内容。
 *  URL和页面处理函数之间的映射由配置文件确定。所有模块装载后不会自动
 *  检查更新，必须重启fweb以更新模块。
 *
 *  3. fweb现在接受SIGTERM和SIGUSR1信号，SIGTERM通知fweb退出，SIGUSR1通知
 *  fweb检查并装载所有更新的模板文件。注意，fweb利用了SIGALARM检测运行时间，
 *  以避免过长处理一个连接请求。所以，程序的其他部分请谨慎使用sleep相关的
 *  调用，最好使用select之类的调用模拟代替。
 *  
 *  4. 各个文件的功能：
 *		fweb.c：主程序，负责初始化以及主循环。
 *
 *		fwebio.c：fastcgi协议的wrapper，其中提供了两部分内容。一是
 *		基本通讯API，包括fwebio_read,fwebio_write,fwebio_printf；在
 *		所有新写的程序中，请仅使用这三个函数。二是旧API兼容，提供了
 *		对stdio库的包装，不推荐在新写的程序中再次使用。要打开旧API
 *		兼容模式，请在包含fwebio.h文件前定义宏FWEB_STDIO_DEFINES
 *
 *		fwebtpl.c：对ctemplate库的包装。因为ctemplate是基于c++的，所以
 *		fweb中对ctemplate库再包装一次，输出一个在C中方便使用的API。API
 *		使用方式可以参考ctemplate的文档。基本上tpl_前缀的函数是关于Template
 *		类的，tdict_前缀函数是关于Dictinary类。
 *
 *		mempool.c: 基于GNU libc中的obstack实现的内存池。因为使用模板
 *		很容易涉及到大量数据存储于内存中，然后用模板一次性输出所有内容。
 *		如果还是基于malloc/free来管理动态内存，很容易造成各种内存问题。
 *		基于HTTP协议的特性，这个内存池设计为每当一个连接处理完毕，会自动
 *		释放所有在这个连接处理过程中申请的内存。所以，这个内存池没有对应
 *		的free函数，所申请的内存也不适合用于保存生存期超过单个HTTP连接生存
 *		期的数据。
 *
 *		fweblog.c: 基于libglib实现的一个日志输出API。在程序中，直接使用
 *		libglib提供的g_error，g_warning，g_message，g_debug函数输出日志。
 *
 *		fwebhttp.c: 提供一些关于HTTP协议的工具函数。
 *
 *		cgic.c: 对cgic原始库的一个简单修改版本。所做的主要修改是使之适应
 *		fwebio.c提供的fastcgi接口。同时增加了cgiFormFilePath函数，用于直接
 *		获得上传文件的临时文件路径，由此避开cgicFormFileOpen/Read/Close这几
 *		个函数。其他所有函数都和原始cgic库保持一致。可以通过这个库解析HTTP 1.1
 *		协议，并获得由web server传过来的各种参数和环境变量。
 *
 *		config_reader.c: 提供一个基于回调函数解析配置文件的界面。
 *
 *		dispatcher.c: 提供一个基于事件回调模型的框架。各个模块可以在其中
 *		注册自己感兴趣的事件，在事件发生时，会自动调用所有回调函数。主要
 *		的事件包括
 *			event_program_exiting
 *			event_new_request
 *			event_before_processing
 *			event_after_processing
 *			event_finish_request
 *		当然，程序各个部分也可以创建自己的事件。在module.c中就使用了这个框架。
 *
 *		module.c: 负责动态装载全局模块和页面模块。其中全局模块所有导出的
 *		函数都会导出到全局符号表中（相当于dlopen的时候使用了RTLD_GLOBAL）。
 *		而页面模块不会把导出函数导出到全局符号表（相当于使用了RTLD_LOCAL）。
 *		module.c会先装载全局模块，再装载页面模块。装载模块的时候，会首先调用
 *		里面的(module name)_init函数（原型请参考module.h文件）。比如，装载
 *		libbbsshm.so，就会调用bbsshm_init函数
 *		传入的参数是unload_event, enter_event, exit_event。unload_event发生于
 *		模块将要卸载之前；enter_event发生于要进入模块调用page processor之前；
 *		enter_event发生于退出模块page processor之后。其中enter_event和
 *		unload_event对于全局模块没有任何意义，任何时候都不会被调用。
 *
 *	5. 退出状态
 *		0 正常退出
 *		1 处理请求出错
 *		2 处理请求执行时间超时
 *		3 CPU使用时间超时
 *		6 初始化错误
 *		128 无法请求内存
 *
 *
 *******************************************************************/

/********************************************************************
注: 下面这些说明全部基于fweb v1, 可能已经不适用了

   新的框架基本保持跟以前的一致，许多以前的代码可以直接使用，但还有以下需要
注意的地方

1，每个页面都是单独的一个so文件，当访问到这个url时，会去打开同名的的so文件
   比如访问/bbsdoc?board=bbsdev，就会去打开pages/下面的bbsdoc.so，然后从里
   面读出3个函数，分别是bbsdoc_main(),bbsdoc_init(),bbsdoc_final()，其中
   bbsdoc_main()是一定要有的，功能跟以前一样。bbsdoc_init()和bbsdoc_final()
   是可选的，其中bbsdoc_init()会在读入so文件的时候调用一次，他的返回值一定
   是int类型，而且任何返回非0值都认为是init出错，此时将认为打开so文件出错。
   而_final在卸载so文件的时候调用一次，返回值无关重要。系统会自动判断so文件
   是否更新过，如果更新过，将自动关闭原来的so handle，打开新的handle。

2，使用了cgic解析http头，完全支持http1.1协议。原来的getparm依然存在，功能一
   样。不过推荐使用cgic的api，这样可以使用更多的完整功能。
   http://www.boutell.com/cgic/

3，使用了glib做log，同时glib里面还有很多功能，推荐使用。
   http://developer.gnome.org/doc/API/2.0/glib/index.html
   可以方便使用g_debug,g_message，g_warning等函数输出log。
   现在的分配是g_message输出一些关于bbs的重要log，比如，发文，删贴。
   g_debug输出debug信息，正式使用时关闭。
   g_warning输出程序运行的错误信息，如打开文件失败，分配内存失败等。
   如果出现编译是出现g_debug未找到的问题,请更换glib >= 2.6的版本

4，增加一个新的页面方法。比如是bbsxxx(推荐使用bbs前缀),然后建立一个bbsxxx.c
   文件在pages/里面，如果不需要连接特殊的库，直接make就可以了，不用修改
   Makefile.
   如果要链接特殊的库，就需要修改pages/Makefile。比如showfile.c需要链接
   libgdbm.so,在21行
   sofiles := $(filter-out showfile.so bbssnd.so,$(sofiles))
                           ~~~~~~~~~~~这里增加showfile.so
   然后增加
   showfile.so:showfile.c
        $(CC) -shared -fpic $(CFLAGS) $(LIBS) -lgdbm -o $@ $<
   这样，编译的时候就会去链接libgdbm.so

5，使用了fastcgi作为框架，这个框架修改了stdio.h。有两个函数不兼容
    scanf和fscanf,使用这两个函数会编译出错，需要特殊修改，比如
    fscanf(fp, "%s %s", buf1, buf2)
    要修改为
    fscanf(FCGI_ToFILE(fp), "%s %s", buf1, buf2)
    而scanf( "%s %s", buf1, buf2)要改为
    fscanf(FCGI_ToFILE(stdin), "%s %s", buf1, buf2)
    
***************************************************************************/
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <pwd.h>
#include <glib.h>
#include <limits.h>
#include <errno.h>

#include "bbs.h"
#include "fweb.h"
#include "config_reader.h"
#include "const.h"
#include "cgic.h"
#include "fweblog.h"
#include "fwebhttp.h"
#include "module.h"
#include "fwebtpl.h"
#include "mempool.h"
#include "fwebio.h"

static int max_requests = 0;

static void timesig_handler(int sig)
{
    fweb_Finish();
    if (SIGALRM == sig)
    {
        g_warning("execution time exceeded");
        exit(2);
    }
    else if (SIGPROF == sig)
    {
        g_warning("cpu time exceeded");
        exit(3);
    }
}
static gboolean stop_running = FALSE;
static void termsig_handler(int sig)
{
    stop_running = TRUE;
    return;
}
static gboolean set_user(CONFIG * config, CONF_DATA * data)
{
    struct passwd *pa;
    char *user;

    if (!data)
        return TRUE;
    if (getuid() != 0)
    {
        /* if I'm not root, don't change user */
        g_warning("user is set in config, but not run by root");
        return TRUE;
    }
    user = data->c_str;
    pa = getpwnam(user);
    if (NULL == pa)
    {
        g_warning("no such user %s from passwd", user);
        return FALSE;
    }
    if (0 != setregid(pa->pw_gid, pa->pw_gid))
    {
        g_warning("call setregid(%d) failed", pa->pw_gid);
        return FALSE;
    }
    if (0 != setreuid(pa->pw_uid, pa->pw_uid))
    {
        g_warning("call setreuid(%d) failed", pa->pw_uid);
        return FALSE;
    }
    return TRUE;
}

static gboolean set_working_dir(CONFIG * config, CONF_DATA * data)
{
    char *dir;

    dir = data->c_str;
    if (!(check_dir(dir) && (0 == chdir(dir))))
    {
        g_warning("can't change dir to %s", dir);
        return FALSE;
    }
    return TRUE;
}
static gboolean set_cpu_time(CONFIG * config, CONF_DATA * data)
{
    long sec;
    struct itimerval cputime;

    sec = (long) data->c_int;
    if (sec < 1)
    {
        g_error("get negtive number for cpu time: %ld", sec);
        return FALSE;
    }
    sec = (long) data->c_int;
    cputime.it_interval.tv_sec = 0;
    cputime.it_interval.tv_usec = 0;
    cputime.it_value.tv_sec = sec;
    cputime.it_value.tv_usec = 0;
    if (setitimer(ITIMER_PROF, &cputime, 0) == -1)
    {
        g_error("can't set cpu timer");
        return FALSE;
    }
    return TRUE;
}
static gboolean set_max_requests(CONFIG * config, CONF_DATA * data)
/* set max requests */
{
    max_requests = (int) data->c_int;
    if (max_requests < 1)
    {
        g_error("get negative number for max requests: %d", max_requests);
        max_requests = 0;
        return FALSE;
    }
    return TRUE;
}

long execution_time = 0;
static gboolean set_execution_time(CONFIG * config, CONF_DATA * data)
{
    execution_time = (long) data->c_int;
    if (execution_time < 1)
    {
        g_error("get negative number for execution time: %ld",
                execution_time);
        execution_time = 0;
        return FALSE;
    }
    return TRUE;
}

EVENT_ID event_program_exiting;
EVENT_ID event_new_request;
EVENT_ID event_finish_request;
EVENT_ID event_before_processing;
EVENT_ID event_after_processing;
static void destroy_general_event(void)
{
    send_event(event_program_exiting, NULL);
    destroy_event(event_new_request, TRUE);
    destroy_event(event_finish_request, TRUE);
    destroy_event(event_before_processing, TRUE);
    destroy_event(event_after_processing, TRUE);
    destroy_event(event_program_exiting, TRUE);
    return;
}
static void setup_general_event(void)
{
    event_program_exiting = create_event("program existing",
                                         EVENT_FLAG_REVERSE_ORDER |
                                         EVENT_FLAG_IGNORE_BREAK |
                                         EVENT_FLAG_IGNORE_FAILED);
    event_new_request = create_event("new request", EVENT_FLAG_NONE);
    event_finish_request =
        create_event("finish request", EVENT_FLAG_REVERSE_ORDER);
    event_before_processing =
        create_event("before processing", EVENT_FLAG_NONE);
    event_after_processing =
        create_event("after processing", EVENT_FLAG_REVERSE_ORDER);
    return;
}
static gboolean init(int argc, char *argv[])
{
    char *config_path;
    struct sigaction intr_act;
    CONFIG config_step1[] = {
        {group_fweb, "user", CONF_TYPE_STRING, CONF_FLAG_NONE, set_user,
         NULL},
        {group_fweb, "working_dir", CONF_TYPE_STRING, CONF_FLAG_REQURIED,
         set_working_dir, NULL},
    };
    CONFIG config_step2[] = {
        {group_fweb, "max_cpu_time", CONF_TYPE_INT, CONF_FLAG_REQURIED,
         set_cpu_time, NULL},
        {group_fweb, "max_requests", CONF_TYPE_INT, CONF_FLAG_REQURIED,
         set_max_requests, NULL},
        {group_fweb, "max_execution_time", CONF_TYPE_INT,
         CONF_FLAG_REQURIED, set_execution_time, NULL},
    };

    setup_general_event();

    if (argc == 2)
        config_path = argv[1];
    else if (argc == 1)
        config_path = FWEB_CONF_PATH;
    else
    {
        g_warning("unknown options. usage: fweb [config_path]");
        return FALSE;
    }
    if (0 != access(config_path, R_OK))
    {
        g_warning("can not access config file at %s", config_path);
        return FALSE;
    }
    if (!config_init(config_path))
    {
        g_warning("initialize config system failed, used file: %s",
                  config_path);
        return FALSE;
    }

    if (!config_parse(config_step1, G_N_ELEMENTS(config_step1)))
        return FALSE;

    if (!log_init())
    {
        g_warning("can't initialized log subsystem");
        return FALSE;
    }

    intr_act.sa_handler = termsig_handler;
    sigemptyset(&intr_act.sa_mask);
    intr_act.sa_flags = 0;      /* don't need SA_RESTART */

    if ((signal(SIGALRM, timesig_handler) == SIG_ERR)
        || (signal(SIGPROF, timesig_handler) == SIG_ERR)
        || (sigaction(SIGTERM, &intr_act, NULL) < 0))
    {
        g_error("can't install signal handler");
        return FALSE;
    }
    if (!config_parse(config_step2, G_N_ELEMENTS(config_step2)))
        return FALSE;

    if (mp_init()
        && fwebtpl_init()
        && fwebio_init() && module_init() && fwebhttp_init())
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}

static void final(void)
{
    destroy_general_event();
}

static gboolean accept_and_process(void)
{
    int count = 0;
    int ret;
    gboolean retval = TRUE, need_finish = FALSE;
    sigset_t newmask, oldmask;

    sigprocmask(SIG_SETMASK, NULL, &newmask);

    while (1)
    {
        if (stop_running)
        {
            break;
        }
        if (fweb_Accept() >= 0)
        {
            need_finish = TRUE;
            if (!sigismember(&newmask, SIGTERM))
            {
                sigaddset(&newmask, SIGTERM);
            }
            /* block SIGTERM while serving a request */
            sigprocmask(SIG_SETMASK, &newmask, &oldmask);
            if (!send_event(event_new_request, NULL))
            {
                g_warning("send event new request failed");
                send_event(event_finish_request, NULL);
                retval = FALSE;
                break;
            }
            ret = serv_request();
            if (!send_event(event_finish_request, NULL))
            {
                g_warning("send event finish request failed");
                retval = FALSE;
                break;
            }
            if (ret == -1)
            {
                /* it means cgic can't parse request, eg, request from hacker.
                   it's not a fatal error, we can recover from it */
                g_warning("invalid http request. uri:'%s',remote:%s",
                          getenv("SCRIPT_NAME"), getenv("REMOTE_ADDR"));
            }
            else if (ret)
            {
                g_warning("serv_request failed, %d", ret);
                retval = FALSE;
                break;
            }
            fweb_Finish();
            need_finish = FALSE;
            count++;
            if (count >= max_requests)
            {
                break;
            }
            /* serving finish, unblock SIGTERM */
            sigprocmask(SIG_SETMASK, &oldmask, &newmask);
        }
        else if (errno != EINTR)
        {
            break;
        }
    }

    if (need_finish)
        fweb_Finish();
    return retval;
}

int main(int argc, char *argv[])
{
    int ret;

    if (!init(argc, argv))
    {
        return 6;
    }
    if (accept_and_process())
    {
        ret = 0;
    }
    else
    {
        ret = 1;
    }
    final();
    return ret;
}

static void failed_process(void)
{
    output_default_header();
}

static void process_request(void)
{

    if (invoke_page_processor(cgiScriptName) == FALSE)
    {
        set_default_response_code(404);
        failed_process();
    }
    return;
}

jmp_buf fatalJmp;
int cgiMain(void)
{
    struct itimerval exectime;

    if (check_method())
        return 0;

    exectime.it_interval.tv_sec = 0;
    exectime.it_interval.tv_usec = 0;
    exectime.it_value.tv_sec = execution_time;
    exectime.it_value.tv_usec = 0;
    if (setitimer(ITIMER_REAL, &exectime, 0) == -1)
    {
        g_warning("setitimer ITIMER_REAL failed");
        return 1;
    }
    if (!send_event(event_before_processing, NULL))
    {
        g_warning("send event before_processing failed");
        send_event(event_after_processing, NULL);
        return 1;
    }
    if (setjmp(fatalJmp))
    {
        g_warning("fatal longjmp called");
        send_event(event_after_processing, NULL);
        return 1;
    }
    process_request();
    if (!send_event(event_after_processing, NULL))
    {
        g_warning("send event after processing failed");
        return 1;
    }
    exectime.it_value.tv_sec = 0;
    if (setitimer(ITIMER_REAL, &exectime, 0) == -1)
    {
        g_warning("cancel setitimer ITIMER_REAL failed");
        return 1;
    }
    return 0;
}
