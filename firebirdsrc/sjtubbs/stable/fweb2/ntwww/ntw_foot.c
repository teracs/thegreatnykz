#include "nt.h"
#include "foot.tpl.h"

extern int ntp_bbsfoot(void);

int ntp_bbsfoot(void)
{
    char board[32], tip[512], msg_value[2] = { 'n', '\0' };
    unsigned int mail_num = 0, mail_unread = 0;
    time_t login_time;
    Tdict dict;
    Tpl tpl;

    if (!tpl_load(&tpl, FOOT_TPL))
    {
        ntw_error_headers();
        return 1;
    }
    dict = ntw_default_tdict_init(FOOT_TPL);

    tdict_set_uint_value(dict, k_foot_user_num,
                         ntw_get_cached_online_num());

    tdict_set_fmt_value(dict, k_foot_time_now, "%16.16s",
                        nt_ctime(ntw_now));

    if (nt_board_get_tip(board, sizeof(board), tip, sizeof(tip)))
    {
        tdict_show_sec(dict, k_foot_tip);
        tdict_set_value(dict, k_foot_tip_board, board);
        tdict_set_value(dict, k_foot_tip_content, tip);
    }

    if (ntw_haslogin)
    {
        login_time = abs(ntw_now - ntw_puinfo->logintime) / 60;
        tdict_set_value(dict, k_foot_user_id, ntw_curuser.userid);
        if (ntw_notguest)
        {
            if (nt_msg_www_has_new(ntw_curuser.userid))
            {
                msg_value[0] = 'y';
            }
            mail_num = nt_mail_get_num(ntw_curuser.userid);
            mail_unread = nt_mail_get_unread_num(ntw_curuser.userid);
        }
        tdict_set_int_value(dict, k_foot_stay_hour, login_time / 60);
        tdict_set_int_value(dict, k_foot_stay_minute, login_time % 60);
    }
    else
    {
        tdict_set_value(dict, k_foot_user_id, "guest");
        tdict_set_int_value(dict, k_foot_stay_hour, 0);
        tdict_set_int_value(dict, k_foot_stay_minute, 0);
    }
    tdict_set_value(dict, k_foot_msg_value, msg_value);
    if (mail_num > 0)
    {
        tdict_add_sec(dict, k_foot_mail);
        tdict_set_uint_value(dict, k_foot_mail_total, mail_num);
        if (mail_unread > 0)
        {
            tdict_show_sec(dict, k_foot_mail_new);
            tdict_set_uint_value(dict, k_foot_mail_unread, mail_unread);
        }
    }

    ntw_normal_headers();
    tpl_output(tpl, dict);
    return 0;
}
