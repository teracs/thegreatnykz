#include <string.h>
#include "dispatcher.h"
#include "cgic.h"
#include "fwebhttp.h"
#include "fwebio.h"
#include "fweb.h"

static int default_response_code = 404;

int set_default_response_code(int num)
{
    int old = default_response_code;

    default_response_code = num;
    return old;
}

int check_method(void)
{
    cgiHeaderString("X-Powered-By: " FWEB_ID "\r\n");
    if (strcasecmp(cgiRequestMethod, "GET")
        && strcasecmp(cgiRequestMethod, "POST"))
    {
        cgiHeaderString("Allow: GET, POST\r\n");
        cgiHeaderStatus(405, "Method Not Allowed");
        return 1;
    }
    return 0;

}

int output_default_header(void)
{
    cgiHeaderString("Content-type: text/html\r\n");
    switch (default_response_code)
    {
    case 404:
        cgiHeaderStatus(404, "PAGE NOT FOUND");
        fwebio_printf("404 PAGE NOT FOUND");
        break;
    case 500:
        cgiHeaderStatus(500, "INTERNAL ERROR");
        fwebio_printf("500 INTERNAL ERROR");
        break;
    default:
        cgiHeaderStatus(default_response_code, "UNKNOW ERROR");
        fwebio_printf("%d UNKNOW ERROR", default_response_code);
    }
    return 0;
}

CALLBACK_RETURN reset_http(CALLBACK_ID callback_id, void *data,
                           void *userdata)
{

    default_response_code = 404;
    return CALLBACK_CONTINUE;
}
static CALLBACK_ID reset_cid;
CALLBACK_RETURN fwebhttp_final(CALLBACK_ID callback_id, void *data,
                               void *userdata)
{
    remove_callback(reset_cid);
    return CALLBACK_CONTINUE;
}

gboolean fwebhttp_init(void)
{
    reset_cid = register_callback(event_new_request, reset_http, NULL);
    register_callback(event_program_exiting, fwebhttp_final, NULL);
    return TRUE;
}

/*
static char *days[] = {
	"Sun",
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat"
};

static char *months[] = {
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};

static inline char *asc_gtime(time_t t,char *buf)
{
	struct tm *gt;
	gt = gmtime(&t);
	sprintf(buf,
		"%s, %02d-%s-%04d %02d:%02d:%02d GMT",
		days[gt->tm_wday],
		gt->tm_mday,
		months[gt->tm_mon],
		gt->tm_year + 1900, gt->tm_hour, gt->tm_min, gt->tm_sec);
	return buf;
}

*/
