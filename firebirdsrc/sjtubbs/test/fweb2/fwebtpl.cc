#include <string>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <google/template.h>
#include <glib.h>
#include <glib/gprintf.h>
#include "fwebtpl.h"
#include "fwebio.h"
#include "config_reader.h"
#include "fweb.h"





using namespace std;
using namespace google;

static ofstream *errfile;
static streambuf *old_sbuf;
static char *template_root_dir = NULL;
static char *default_error_tpl = NULL;

static CALLBACK_RETURN clean_error_output_file(CALLBACK_ID callback_id, void *data, void *userdata)
{
	cerr.rdbuf(old_sbuf);
	delete errfile;
	return CALLBACK_CONTINUE;
}
static gboolean set_error_output_file(CONFIG *config, CONF_DATA *data)
{
	errfile = new ofstream;
	errfile->open(data->c_str,ios::out|ios::app);
	if(!errfile->is_open())
	{
		delete errfile;
		g_warning("can't open cerr log: %s", data->c_str);
		return FALSE;
	}
	old_sbuf = cerr.rdbuf();
	cerr.rdbuf(errfile->rdbuf());
	register_callback(event_program_exiting, clean_error_output_file, NULL);
	return TRUE;
}

static CALLBACK_RETURN clear_tpl_cache(CALLBACK_ID callback_id, void *data, void *userdata)
{
	Template::ClearCache();
	return CALLBACK_CONTINUE;
}

static CALLBACK_RETURN free_default_error_tpl(CALLBACK_ID callback_id, void *data, void *userdata)
{
	g_free(default_error_tpl);
	return CALLBACK_CONTINUE;
}
static gboolean set_default_error_tpl(CONFIG *config, CONF_DATA *data)
{
	Tpl tpl;
	if(!tpl_load(&tpl, data->c_str))
	{
		g_warning("can't load default error template: %s", data->c_str);
		return FALSE;
	}
	default_error_tpl = g_strdup(data->c_str);
	register_callback(event_program_exiting, free_default_error_tpl, NULL);
	return TRUE;
}

static gboolean reload_all_tpl = FALSE;

static void usr1sig_handler(int sig)
{
	reload_all_tpl = TRUE;
}

extern "C" {


gboolean fwebtpl_init(void)
{
	CONFIG config1[] = {
		{group_fweb, "template_dir", CONF_TYPE_STRING, CONF_FLAG_REQURIED, set_dir, &template_root_dir},
	};
	CONFIG config2[] = {
		{group_fweb, "template_error_output_file", CONF_TYPE_STRING, CONF_FLAG_REQURIED, set_error_output_file, NULL},
		{group_fweb, "template_default_error_tpl", CONF_TYPE_STRING, CONF_FLAG_REQURIED, set_default_error_tpl, NULL},
	};
	if(signal(SIGUSR1,usr1sig_handler)==SIG_ERR)
	{
		g_warning("set SIGUSR! error");
		return FALSE;
	}
	if(!config_parse(config1, G_N_ELEMENTS(config1)))
	{
		g_warning("template init failed");
		return FALSE;
	}
	Template::SetTemplateRootDirectory(template_root_dir);
	if(!config_parse(config2, G_N_ELEMENTS(config2)))
	{
		g_warning("template init failed");
		return FALSE;
	}
	register_callback(event_program_exiting, clear_tpl_cache, NULL);
	return TRUE;
}


gboolean tpl_load(Tpl *ptpl, const char *tpl_fn)
{
	Template *tptr = NULL;
	if(reload_all_tpl)
	{
		reload_all_tpl = FALSE;
		Template::ReloadAllIfChanged();
	}
	tptr = Template::GetTemplate(tpl_fn, DO_NOT_STRIP);
	if(!tptr)
	{
		g_warning("can't load template %s", tpl_fn);
		return FALSE;
	}

	if(tptr->state() != TS_READY)
	{
		g_warning("template %s syntax error", tpl_fn);
		return FALSE;
	}
	
	ptpl->tptr = tptr;
	return TRUE;
}

gboolean tpl_print_err(const char *fmt, ...)
{
	Tpl tpl;
	Tdict dict;
	va_list ap;

	dict = tdict_init("error");
	va_start(ap,fmt);
	tdict_set_fmtv_value(dict,"ERRMSG",fmt,ap);
	va_end(ap);

	if(!tpl_load(&tpl, default_error_tpl))
	{
		tdict_final(dict);
		return FALSE;
	}
	if(tpl_output(tpl,dict)<1)
	{	
		tdict_final(dict);
		return FALSE;
	}
	tdict_final(dict);
	return TRUE;
}

size_t tpl_output(Tpl tpl, Tdict dict)
{
	Template *tptr = static_cast <Template *> (tpl.tptr);
	TemplateDictionary *dptr = static_cast <TemplateDictionary *> (dict.dptr);
	string output;
	if( tptr->Expand(&output, dptr) )
		return fwebio_write(output.data(),output.size());
	else
		return 0;
}

Tdict tdict_init(const char *name)
{
	Tdict dict;
	TemplateDictionary *dptr = new TemplateDictionary(name);
	dict.dptr = dptr;
	return dict;
}

void tdict_final(Tdict dict)
{
	TemplateDictionary *dptr = \
		static_cast<TemplateDictionary *> (dict.dptr);
	delete dptr;
}

void tdict_set_value_len_esc(Tdict dict, const char *key, const char *val,int vallen, ESCAPE esc)
{
	TemplateDictionary *dptr = \
		static_cast<TemplateDictionary *> (dict.dptr);
	switch (esc)
	{
		case ESC_NONE:
			dptr->SetValue(key,TemplateString(val,vallen));
			break;
		case ESC_HTML:
			dptr->SetEscapedValue(key,TemplateString(val,vallen),TemplateDictionary::html_escape);
			break;
		case ESC_XML:
			dptr->SetEscapedValue(key,TemplateString(val,vallen),TemplateDictionary::xml_escape);
			break;
		case ESC_JS:
			dptr->SetEscapedValue(key,TemplateString(val,vallen),TemplateDictionary::javascript_escape);
			break;
		case ESC_JSON:
			dptr->SetEscapedValue(key,TemplateString(val,vallen),TemplateDictionary::json_escape);
			break;
	}
	return;

}


void tdict_set_value(Tdict dict, const char *key, const char *val)
{
	tdict_set_value_len_esc(dict,key,val,strlen(val),ESC_NONE);
	return;
}

void tdict_set_value_len(Tdict dict, const char *key, const char *val, int vallen)
{
	tdict_set_value_len_esc(dict,key,val,vallen,ESC_NONE);
	return;
}

void tdict_set_value_esc(Tdict dict, const char *key, const char *val, ESCAPE esc)
{
	tdict_set_value_len_esc(dict,key,val,strlen(val),esc);
	return;
}

void tdict_set_int_value(Tdict dict, const char *key, int val)
{
    TemplateDictionary *dptr = \
		static_cast<TemplateDictionary *> (dict.dptr);
	dptr->SetIntValue(key,val);
	return;
}

void tdict_set_uint_value(Tdict dict, const char *key, unsigned int val)
{
	tdict_set_fmt_value(dict, key, "%u", val);
	return;
}
void tdict_set_fmtv_value_esc(Tdict dict, const char *key, ESCAPE esc, const char *fmt, va_list ap)
{
	char *str;
	int len;

	len = g_vasprintf(&str, fmt, ap);
	tdict_set_value_len_esc(dict, key, str, len, esc);
	g_free(str);

	return;
}

void tdict_set_fmtv_value(Tdict dict, const char *key, const char *fmt, va_list ap)
{
	tdict_set_fmtv_value_esc(dict, key, ESC_NONE, fmt, ap);
}

void tdict_set_fmt_value_esc(Tdict dict, const char *key, ESCAPE esc, const char *fmt, ... )
{
	va_list ap;

	va_start(ap, fmt);
	tdict_set_fmtv_value_esc(dict, key, esc, fmt, ap);
	va_end(ap);

	return;
}


void tdict_set_fmt_value(Tdict dict, const char *key, const char *fmt, ...)
{	
	va_list ap;
	
	va_start(ap,fmt);
	tdict_set_fmtv_value_esc(dict, key, ESC_NONE, fmt, ap);
	va_end(ap);

	return;
}

void tdict_set_gbl_value_len(Tdict dict, const char *key, const char *val, int vallen)
{
    TemplateDictionary *dptr = \
	        static_cast<TemplateDictionary *> (dict.dptr);
	
	dptr->SetTemplateGlobalValue(key, TemplateString(val,vallen));
	return;
}

void tdict_set_gbl_value(Tdict dict, const char *key, const char *val)
{
	tdict_set_gbl_value_len(dict,key,val,strlen(val));
	return;
}

void tdict_set_all_gbl_value_len(const char *key, const char *val, int vallen)
{
	TemplateDictionary::SetGlobalValue(key, TemplateString(val,vallen));
	return;
}



void tdict_set_all_gbl_value(const char *key, const char *val)
{
	tdict_set_all_gbl_value_len(key,val,strlen(val));
	return;
}

Tdict tdict_add_sec(Tdict dict, const char *name)
{
	Tdict newdict;
	TemplateDictionary *newdptr;
    TemplateDictionary *dptr = \
	        static_cast<TemplateDictionary *> (dict.dptr);
	newdptr = dptr->AddSectionDictionary(name);
	
	newdict.dptr = newdptr;
	return newdict;
}

void tdict_show_sec(Tdict dict, const char *name)
{
	TemplateDictionary *dptr = \
			static_cast<TemplateDictionary *> (dict.dptr);
	dptr->ShowSection(name);
	return;
}

void tdict_show_sec_value_len_esc(Tdict dict, const char *name,
	const char *key, const char *val, int vallen, ESCAPE esc)
{
	TemplateDictionary *dptr = \
		static_cast<TemplateDictionary *> (dict.dptr);
	
	switch(esc)
	{
		case ESC_NONE:
			dptr->SetValueAndShowSection(key,TemplateString(val,vallen),name);
			break;
		case ESC_HTML:
			dptr->SetEscapedValueAndShowSection(key,TemplateString(val,vallen),
				TemplateDictionary::html_escape, name);
			break;
		case ESC_XML:
			dptr->SetEscapedValueAndShowSection(key,TemplateString(val,vallen),
				TemplateDictionary::xml_escape, name);
			break;
		case ESC_JS:
			dptr->SetEscapedValueAndShowSection(key,TemplateString(val,vallen),
				TemplateDictionary::javascript_escape, name);
			break;
		case ESC_JSON:
			dptr->SetEscapedValueAndShowSection(key,TemplateString(val,vallen),
				TemplateDictionary::json_escape, name);
			break;
	}
	return;
}


void tdict_show_sec_value(Tdict dict, const char *name,
	const char *key, const char *val)
{
	tdict_show_sec_value_len_esc(dict, name, key, val, strlen(val), ESC_NONE);
	return;
}

void tdict_show_sec_value_len(Tdict dict, const char *name,
	const char *key, const char *val, int vallen)
{
	tdict_show_sec_value_len_esc(dict, name, key, val, vallen, ESC_NONE);
	return;
}

Tdict tdict_add_include(Tdict dict, const char *name, const char *filename)
{
	Tdict newdict;
	TemplateDictionary *newdptr;
	TemplateDictionary *dptr = \
	        static_cast<TemplateDictionary *> (dict.dptr);
	newdptr = dptr->AddIncludeDictionary(name);
	newdptr->SetFilename(filename);
	newdict.dptr = newdptr;
	return newdict;
}
/*
void tdict_set_annotate(Tdict dict, gboolean annotate)
{
	TemplateDictionary *dptr = \
	            static_cast<TemplateDictionary *> (dict.dptr);
	if(annotate)
		dptr->SetAnnotateOutput(template_root_dir);
	else
		dptr->SetAnnotateOutput(NULL);
	return ;
}*/

void tdict_dump(Tdict dict)
{
	TemplateDictionary *dptr = \
	            static_cast<TemplateDictionary *> (dict.dptr);
	string out;
	dptr->DumpToString(&out,2);
	fwebio_write(out.data(),out.size());
	return;
}

} //end extern "C"
