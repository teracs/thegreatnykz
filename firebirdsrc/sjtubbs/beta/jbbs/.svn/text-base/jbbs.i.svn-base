%module jbbs

%{
extern int jbbs_request_init(void);
extern void jbbs_request_shutdown(void);
extern void jbbs_module_shutdown(void);
extern int jbbs_module_init(void);
%}


#define PERM_BASIC          000001
#define PERM_CHAT           000002
#define PERM_PAGE           000004
#define PERM_POST           000010
#define PERM_LOGINOK        000020
#define PERM_666LIFE        000040
#define PERM_CLOAK          000100
#define PERM_SEECLOAK       000200
#define PERM_XEMPT          000400
#define PERM_WELCOME        001000
#define PERM_BOARDS         002000
#define PERM_ACCOUNTS       004000
#define PERM_CHATCLOAK      010000
#define PERM_OVOTE          020000
#define PERM_SYSOP          040000
#define PERM_POSTMASK       0100000
#define PERM_ANNOUNCE       0200000
#define PERM_OBOARDS        0400000
#define PERM_ACBOARD        01000000
#define PERM_NOZAP          02000000
#define PERM_FORCEPAGE      04000000
#define PERM_EXT_IDLE       010000000
#define PERM_MESSAGE        020000000
#define PERM_ACHATROOM      040000000
#define PERM_ARBITRATOR     0100000000
#define PERM_MAILINFINITY   0200000000
#define PERM_PERSONAL       0400000000
#define PERM_SPECIAL6       01000000000
#define PERM_SPECIAL7       02000000000
#define PERM_SPECIAL8       04000000000
#define PERM_SPECIAL9       010000000000
#define PERM_SPECIALT       020000000000     


extern int jbbs_check_session(const char *id,const char *fromhost, int num,int key);
extern unsigned int jbbs_has_perm(unsigned int perm);
