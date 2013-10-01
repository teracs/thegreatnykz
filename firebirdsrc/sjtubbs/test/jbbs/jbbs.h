#ifndef _H_JBBS
#define _H_JBBS

extern int jbbs_check_session(const char *id,const char *fromhost, int num,int key);
extern int jbbs_has_perm(unsigned int perm);



extern int jbbs_request_init(void);
extern void jbbs_request_shutdown(void);
extern void jbbs_module_shutdown(void);
extern int jbbs_module_init(void);

#endif
