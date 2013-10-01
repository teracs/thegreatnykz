#include "nt.h"
#include "waphead.tpl.h"

int ntp_bbswaphead(void)
{
    unsigned int mail_num = 0, mail_unread = 0;
    Tdict dict;
    Tpl tpl;

    if (!tpl_load(&tpl, WAPHEAD_TPL))
    {
        ntw_error_headers();
        return 1;
    }
    dict = ntw_default_tdict_init(WAPHEAD_TPL);

    if (ntw_haslogin)
    {
        tdict_set_value(dict, k_waphead_user_id, ntw_curuser.userid);
        tdict_set_value(dict, k_waphead_login,"<a href=\"/bbslogout\" target=_top>×¢Ïú</a>");
        if (ntw_notguest)
        {
            mail_num = nt_mail_get_num(ntw_curuser.userid);
            mail_unread = nt_mail_get_unread_num(ntw_curuser.userid);
        }
    }
    else
    {
        tdict_set_value(dict, k_waphead_user_id, "guest");
        tdict_set_value(dict, k_waphead_login,"<a href=\"/file/bbs/mobile/index.htm\" target=_top>·µ»ØÊ×Ò³µÇÂ¼</a>");
    }
    if (mail_num > 0)
    {
        tdict_add_sec(dict, k_waphead_mail);
        tdict_set_uint_value(dict, k_waphead_mail_total, mail_num);
        if (mail_unread > 0)
        {
            tdict_show_sec(dict, k_waphead_mail_new);
            tdict_set_uint_value(dict, k_waphead_mail_unread, mail_unread);
        }
    }

    ntw_normal_headers();
    tpl_output(tpl, dict);
    return 0;
}
