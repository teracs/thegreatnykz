#include "nt.h"
#include "topmenu.tpl.h"

extern int ntp_bbstopmenu(void);

int ntp_bbstopmenu(void)
{
	char buf[16];
    Tdict dict;
    Tpl tpl;

    if (!tpl_load(&tpl, TOPMENU_TPL))
    {
        ntw_error_headers();
        return 1;
    }
    dict = ntw_default_tdict_init(TOPMENU_TPL);

	ntw_haslogin=1;
	ntw_notguest=1;

    if (ntw_haslogin)
    {
        if (ntw_notguest)
        {
		strcpy(buf, "Œ¥◊¢≤·”√ªß");

        if (ntw_curuser.userlevel & PERM_LOGINOK)
            strcpy(buf, "∆’Õ®’æ”—");
        if (ntw_curuser.userlevel & PERM_BOARDS)
            strcpy(buf, "∞Â÷˜");
        if (ntw_curuser.userlevel & PERM_XEMPT)
            strcpy(buf, "”¿æ√’ ∫≈");
        if (ntw_curuser.userlevel & PERM_CHATCLOAK)
            strcpy(buf, "±æ’æ÷«ƒ“");
        if (ntw_curuser.userlevel & PERM_SYSOP)
            strcpy(buf, "±æ’æ’æ≥§");
	if ( !(ntw_curuser.userlevel & PERM_LOGINOK) && !nt_has_fill_form(ntw_curuser.userid))
		tdict_show_sec(dict,k_topmenu_fillform);
	

		tdict_show_sec(dict, k_topmenu_loginin);
        	tdict_set_value(dict, k_topmenu_user_id, ntw_curuser.userid);
        	tdict_set_value(dict, k_topmenu_user_level, buf);
		tdict_show_sec(dict, k_topmenu_fav);
		tdict_show_sec(dict, k_topmenu_mail);
		tdict_show_sec(dict, k_topmenu_tbox);
		tdict_show_sec(dict, k_topmenu_sn);
        }
    }
    else
    {
	tdict_show_sec(dict, k_topmenu_guest);
    }

    ntw_normal_headers();
    tpl_output(tpl, dict);
    return 0;
}
