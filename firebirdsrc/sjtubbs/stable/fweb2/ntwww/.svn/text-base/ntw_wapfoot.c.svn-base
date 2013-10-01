#include "nt.h"
#include "wapfoot.tpl.h"


int ntp_bbswapfoot(void)
{
    char msg_value[2] = { 'n', '\0' };
    time_t login_time;
    Tdict dict;
    Tpl tpl;

    if (!tpl_load(&tpl, WAPFOOT_TPL))
    {
        ntw_error_headers();
        return 1;
    }
    dict = ntw_default_tdict_init(WAPFOOT_TPL);

    tdict_set_fmt_value(dict, k_wapfoot_time_now, "%16.16s",
                        nt_ctime(ntw_now));

    if (ntw_haslogin)
    {
        login_time = abs(ntw_now - ntw_puinfo->logintime) / 60;
        if (ntw_notguest)
        {
            if (nt_msg_www_has_new(ntw_curuser.userid))
            {
                msg_value[0] = 'y';
            }
        }
        tdict_set_int_value(dict, k_wapfoot_stay_hour, login_time / 60);
        tdict_set_int_value(dict, k_wapfoot_stay_minute, login_time % 60);
    }
    else
    {
        tdict_set_int_value(dict, k_wapfoot_stay_hour, 0);
        tdict_set_int_value(dict, k_wapfoot_stay_minute, 0);
    }
    tdict_set_value(dict, k_wapfoot_msg_value, msg_value);

    ntw_normal_headers();
    tpl_output(tpl, dict);
    return 0;
}
