/* 
 * fcgi_stdio.h --
 *
 *      FastCGI-stdio compatibility package
 *
 *
 * Copyright (c) 1996 Open Market, Inc.
 *
 * See the file "LICENSE.TERMS" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * $Id: fcgi_stdio.h,v 1.5 2001/06/22 13:21:15 robs Exp $
 */

#ifndef _FWEBIO_H
#define _FWEBIO_H 1

#include <stdio.h>
#include <sys/types.h>
#include <fcgiapp.h>
#include <glib.h>

G_BEGIN_DECLS
#ifndef DLLAPI
#define DLLAPI
#endif
/*
 * Wrapper type for FILE
 */
    typedef struct
{
    FILE *stdio_stream;
    FCGX_Stream *fcgx_stream;
} fweb_FILE;

/* special api for fweb */
/**
 * initialze the fwebio subsystem
 *
 * @return TRUE on success, FALSE on error
 */
DLLAPI gboolean fwebio_init(void);

/**
 * read size length byte from http server and write to ptr
 *
 * @param ptr OUT: the content read will write to it
 * @param size length byte to read
 *
 * @return how many byte read, return -1 on error
 */
DLLAPI size_t fwebio_read(void *ptr, size_t size);

/**
 * write size length byte to http server
 *
 * @param ptr the content to write
 * @param size length byte to write
 *
 * @return how many byte written, return -1 on error
 */
DLLAPI size_t fwebio_write(const void *ptr, size_t size);

/**
 * write formated content to http server
 *
 * @param fmt printf like format string
 * 
 * @return how many byte written, return -1 on error
 */
DLLAPI int fwebio_printf(const char *fmt, ...) G_GNUC_PRINTF(1, 2);

/* special api for fweb */

/**
 * accept new request, return >=0 on success, others if no new request
 * It will call fweb_Finish if last request is not finished
 */
DLLAPI int fweb_Accept(void);

/**
 * finish a request, flush all data to http server
 */
DLLAPI void fweb_Finish(void);
DLLAPI int fweb_StartFilterData(void);

DLLAPI void fweb_SetExitStatus(int status);

#define fweb_ToFILE(fcgi_file) (fcgi_file->stdio_stream)
#define fweb_ToFcgiStream(fcgi_file) (fcgi_file->fcgx_stream)

/*
 * Wrapper stdin, stdout, and stderr variables, set up by fweb_Accept()
 */

DLLAPI extern fweb_FILE _fcgi_sF[];

#define fweb_stdin	(&_fcgi_sF[0])
#define fweb_stdout	(&_fcgi_sF[1])
#define fweb_stderr	(&_fcgi_sF[2])

/*
 * Wrapper function prototypes, grouped according to sections
 * of Harbison & Steele, "C: A Reference Manual," fourth edition,
 * Prentice-Hall, 1995.
 */

DLLAPI void fweb_perror(const char *str);

DLLAPI fweb_FILE *fweb_fopen(const char *path, const char *mode);
DLLAPI int fweb_fclose(fweb_FILE * fp);
DLLAPI int fweb_fflush(fweb_FILE * fp);
DLLAPI fweb_FILE *fweb_freopen(const char *path, const char *mode,
                               fweb_FILE * fp);

DLLAPI int fweb_setvbuf(fweb_FILE * fp, char *buf, int bufmode,
                        size_t size);
DLLAPI void fweb_setbuf(fweb_FILE * fp, char *buf);

DLLAPI int fweb_fseek(fweb_FILE * fp, long offset, int whence);
DLLAPI int fweb_ftell(fweb_FILE * fp);
DLLAPI void fweb_rewind(fweb_FILE * fp);
DLLAPI int fweb_fgetpos(fweb_FILE * fp, fpos_t * pos);
DLLAPI int fweb_fsetpos(fweb_FILE * fp, const fpos_t * pos);
DLLAPI int fweb_fgetc(fweb_FILE * fp);
DLLAPI int fweb_getchar(void);
DLLAPI int fweb_ungetc(int c, fweb_FILE * fp);

DLLAPI char *fweb_fgets(char *str, int size, fweb_FILE * fp);
DLLAPI char *fweb_gets(char *str);

/*
 * Not yet implemented
 *
 * int        fweb_fscanf(fweb_FILE *fp, const char *format, ...);
 * int        fweb_scanf(const char *format, ...);
 *
 */

DLLAPI int fweb_fputc(int c, fweb_FILE * fp);
DLLAPI int fweb_putchar(int c);

DLLAPI int fweb_fputs(const char *str, fweb_FILE * fp);
DLLAPI int fweb_puts(const char *str);

DLLAPI int fweb_fprintf(fweb_FILE * fp, const char *format,
                        ...) G_GNUC_PRINTF(2, 3);
DLLAPI int fweb_printf(const char *format, ...) G_GNUC_PRINTF(1, 2);

DLLAPI int fweb_vfprintf(fweb_FILE * fp, const char *format, va_list ap);
DLLAPI int fweb_vprintf(const char *format, va_list ap);

DLLAPI size_t fweb_fread(void *ptr, size_t size, size_t nmemb,
                         fweb_FILE * fp);
DLLAPI size_t fweb_fwrite(const void *ptr, size_t size, size_t nmemb,
                          fweb_FILE * fp);

DLLAPI int fweb_feof(fweb_FILE * fp);
DLLAPI int fweb_ferror(fweb_FILE * fp);
DLLAPI void fweb_clearerr(fweb_FILE * fp);

DLLAPI fweb_FILE *fweb_tmpfile(void);

DLLAPI int fweb_fileno(fweb_FILE * fp);
DLLAPI fweb_FILE *fweb_fdopen(int fd, const char *mode);
DLLAPI fweb_FILE *fweb_popen(const char *cmd, const char *type);
DLLAPI int fweb_pclose(fweb_FILE *);

/*
 * The remaining definitions are for application programs,
 * not for fcgi_stdio.c
 */

#ifdef FWEB_STDIO_DEFINES

/*
 * Replace standard types, variables, and functions with FastCGI wrappers.
 * Use undef in case a macro is already defined.
 */
#define FCGI_ToFILE(fcgi_file) (fcgi_file->stdio_stream)

#undef  FILE
#define	FILE     fweb_FILE

#undef  stdin
#define	stdin    fweb_stdin
#undef  stdout
#define	stdout   fweb_stdout
#undef  stderr
#define	stderr   fweb_stderr

#undef  perror
#define	perror   fweb_perror

#undef  fopen
#define	fopen    fweb_fopen
#undef  fclose
#define	fclose   fweb_fclose
#undef  fflush
#define	fflush   fweb_fflush
#undef  freopen
#define	freopen  fweb_freopen

#undef  setvbuf
#define	setvbuf  fweb_setvbuf
#undef  setbuf
#define	setbuf   fweb_setbuf

#undef  fseek
#define fseek    fweb_fseek
#undef  ftell
#define ftell    fweb_ftell
#undef  rewind
#define rewind   fweb_rewind
#undef  fgetpos
#define fgetpos  fweb_fgetpos
#undef  fsetpos
#define fsetpos  fweb_fsetpos

#undef  fgetc
#define	fgetc    fweb_fgetc
#undef  getc
#define getc     fweb_fgetc
#undef  getchar
#define	getchar  fweb_getchar
#undef  ungetc
#define ungetc   fweb_ungetc

#undef  fgets
#define fgets    fweb_fgets
#undef  gets
#define	gets     fweb_gets

#undef  fputc
#define fputc    fweb_fputc
#undef  putc
#define putc     fweb_fputc
#undef  putchar
#define	putchar  fweb_putchar

#undef  fputs
#define	fputs    fweb_fputs
#undef  puts
#define	puts     fweb_puts

#undef  fprintf
#define	fprintf  fweb_fprintf
#undef  printf
#define	printf   fweb_printf

#undef  vfprintf
#define vfprintf fweb_vfprintf
#undef  vprintf
#define vprintf  fweb_vprintf

#undef  fread
#define	fread    fweb_fread
#undef  fwrite
#define fwrite   fweb_fwrite

#undef  feof
#define	feof     fweb_feof
#undef  ferror
#define ferror   fweb_ferror
#undef  clearerr
#define	clearerr fweb_clearerr

#undef  tmpfile
#define tmpfile  fweb_tmpfile

#undef  fileno
#define fileno   fweb_fileno
#undef  fdopen
#define fdopen   fweb_fdopen
#undef  popen
#define popen    fweb_popen
#undef  pclose
#define	pclose   fweb_pclose

#endif                          /* FWEB_STDIO_DEFINES */

G_END_DECLS
#endif                          /* _FWEBIO_H */
