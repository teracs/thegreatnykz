/*
 * fcgi_stdio.c --
 *
 *      FastCGI-stdio compatibility package
 *
 *
 * Copyright (c) 1996 Open Market, Inc.
 *
 * See the file "LICENSE.TERMS" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

#include <errno.h>              /* for errno */
#include <stdarg.h>             /* for va_arg */
#include <stdlib.h>             /* for malloc */
#include <string.h>             /* for strerror */

#include <unistd.h>

#include <fcgiapp.h>
#include <fcgios.h>
#include <fcgimisc.h>

#include "fwebio.h"

extern char **environ;

#include <stdio.h>
#include "fweb.h"
#include "dispatcher.h"

fweb_FILE _fcgi_sF[3];

#define FCGI_DEFAULT_LISTEN_FD 0
static int fwebio_init_called = 0;
static FCGX_Request fwebio_request;
static CALLBACK_RETURN fwebio_final(CALLBACK_ID callback_id, void *data,
                                    void *userdata)
{
    if (!fwebio_init_called)
    {
        return CALLBACK_CONTINUE;
    }
    FCGX_Free(&fwebio_request, 1 /*close the socket */ );
    fwebio_init_called = 0;
    return CALLBACK_CONTINUE;
}

gboolean fwebio_init(void)
{
    if (fwebio_init_called)
    {
        return TRUE;
    }
    if (FCGX_Init())
    {
        /*FCGX_Init error */
        return FALSE;
    }
    if (FCGX_InitRequest
        (&fwebio_request, FCGI_DEFAULT_LISTEN_FD,
         FCGI_FAIL_ACCEPT_ON_INTR))
    {
        return FALSE;
    }
    fwebio_init_called = 1;
    register_callback(event_program_exiting, fwebio_final, NULL);
    return TRUE;
}

size_t fwebio_read(void *ptr, size_t size)
{
    return fweb_fread(ptr, 1, size, fweb_stdin);
}

size_t fwebio_write(const void *ptr, size_t size)
{
    return fweb_fwrite(ptr, 1, size, fweb_stdout);
}
int fwebio_printf(const char *fmt, ...)
{
    int result;
    va_list ap;

    va_start(ap, fmt);
    result = fweb_vprintf(fmt, ap);
    va_end(ap);
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * fweb_Accept --
 *
 *      Accepts a new request from the HTTP server and creates
 *      a conventional execution environment for the request.
 *
 *      If the application was invoked as a FastCGI server,
 *      the first call to fweb_Accept indicates that the application
 *      has completed its initialization and is ready to accept
 *      a request.  Subsequent calls to fweb_Accept indicate that
 *      the application has completed its processing of the
 *      current request and is ready to accept a new request.
 *
 *      If the application was invoked as a CGI program, the first
 *      call to fweb_Accept is essentially a no-op and the second
 *      call returns EOF (-1).
 *
 * Results:
 *    0 for successful call, -1 for error (application should exit).
 *
 * Side effects:
 *      If the application was invoked as a FastCGI server,
 *      and this is not the first call to this procedure,
 *      fweb_Accept first performs the equivalent of fweb_Finish.
 *
 *      On every call, fweb_Accept accepts the new request and
 *      reads the fweb_PARAMS stream into an environment array,
 *      i.e. a NULL-terminated array of strings of the form
 *      ``name=value''.  It assigns a pointer to this array
 *      to the global variable environ, used by the standard
 *      library function getenv.  It creates new fweb_FILE *s
 *      representing input from the HTTP server, output to the HTTP
 *      server, and error output to the HTTP server, and assigns these
 *      new files to stdin, stdout, and stderr respectively.
 *
 *      DO NOT mutate or retain pointers to environ or any values
 *      contained in it (e.g. to the result of calling getenv(3)),
 *      since these are freed by the next call to fweb_Finish or
 *      fweb_Accept.  In particular do not use setenv(3) or putenv(3)
 *      in conjunction with fweb_Accept.
 *
 *----------------------------------------------------------------------
 */
static gboolean acceptCalled = FALSE;
static gboolean isCGI = FALSE;

int fweb_Accept(void)
{
    if (!acceptCalled)
    {
        acceptCalled = TRUE;
        atexit(&fweb_Finish);
        isCGI = FCGX_IsCGI();
    }
    else if (isCGI)
    {
        /*
         * Not first call to fweb_Accept and running as CGI means
         * application is done.
         */
        return -1;
    }
    if (isCGI)
    {
        fweb_stdin->stdio_stream = stdin;
        fweb_stdin->fcgx_stream = NULL;
        fweb_stdout->stdio_stream = stdout;
        fweb_stdout->fcgx_stream = NULL;
        fweb_stderr->stdio_stream = stderr;
        fweb_stderr->fcgx_stream = NULL;
    }
    else
    {
        int acceptResult = FCGX_Accept_r(&fwebio_request);

        if (acceptResult < 0)
        {
            return acceptResult;
        }
        fweb_stdin->stdio_stream = NULL;
        fweb_stdin->fcgx_stream = fwebio_request.in;
        fweb_stdout->stdio_stream = NULL;
        fweb_stdout->fcgx_stream = fwebio_request.out;
        fweb_stderr->stdio_stream = NULL;
        fweb_stderr->fcgx_stream = fwebio_request.err;
        environ = fwebio_request.envp;
    }
    return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * fweb_Finish --
 *
 *      Finishes the current request from the HTTP server.
 *
 * Side effects:
 *
 *      Flushes any buffered output to the HTTP server.  Then frees
 *      all storage allocated by the previous call, including all
 *      storage reachable from the value of environ set by the previous
 *      call to fweb_Accept.
 *
 *      DO NOT use stdin, stdout, stderr, or environ between calling
 *      fweb_Finish and calling fweb_Accept.
 *
 *      DO NOT mutate or retain pointers to environ or any values
 *      contained in it (e.g. to the result of calling getenv(3)),
 *      since these are freed by the next call to fweb_Finish or
 *      fweb_Accept.  In particular do not use setenv(3) or putenv(3)
 *      in conjunction with fweb_Accept.
 *
 *----------------------------------------------------------------------
 */
void fweb_Finish(void)
{
    if (!acceptCalled || isCGI)
    {
        return;
    }
    FCGX_Finish_r(&fwebio_request);
    fweb_stdin->fcgx_stream = NULL;
    fweb_stdout->fcgx_stream = NULL;
    fweb_stderr->fcgx_stream = NULL;
    environ = NULL;
}

/*
 *----------------------------------------------------------------------
 *
 * fweb_StartFilterData --
 *
 *
 *      The current request is for the filter role, and stdin is
 *      positioned at EOF of fweb_STDIN.  The call repositions
 *      stdin to the start of fweb_DATA.
 *      If the preconditions are not met (e.g. fweb_STDIN has not
 *      been read to EOF), the call sets the stream error code to
 *      FCGX_CALL_SEQ_ERROR.
 *
 * Results:
 *      0 for a normal return, < 0 for error
 *
 *----------------------------------------------------------------------
 */
int fweb_StartFilterData(void)
{
    if (fweb_stdin->stdio_stream)
    {
        return -1;
    }
    else
    {
        return FCGX_StartFilterData(fweb_stdin->fcgx_stream);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * fweb_SetExitStatus --
 *
 *      Sets the exit status for the current request. The exit status
 *      is the status code the request would have exited with, had
 *      the request been run as a CGI program.  You can call
 *      fweb_SetExitStatus several times during a request; the last call
 *      before the request ends (by calling fweb_Accept) determines the
 *      value.
 *
 *----------------------------------------------------------------------
 */
void fweb_SetExitStatus(int status)
{
    if (fweb_stdin->fcgx_stream)
    {
        FCGX_SetExitStatus(status, fweb_stdin->fcgx_stream);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * fweb_perror --
 *
 *       Wrapper for function defined in H&S Section 11.2
 *
 *----------------------------------------------------------------------
 */
void fweb_perror(const char *str)
{
    fweb_fputs(str, fweb_stderr);
    fweb_fputs(": ", fweb_stderr);
    fweb_fputs(strerror(OS_Errno), fweb_stderr);
    return;
}

/*
 *----------------------------------------------------------------------
 *
 * fweb_OpenFromFILE --
 *
 *      Constructs a new fweb_FILE * from the FILE *stream.
 *
 * Results:
 *	NULL if stream == NULL or storage could not be allocated,
 *      otherwise the new fweb_FILE *.
 *
 *----------------------------------------------------------------------
 */
static fweb_FILE *fweb_OpenFromFILE(FILE * stream)
{
    fweb_FILE *fp;

    if (stream == NULL)
        return NULL;

    fp = (fweb_FILE *) malloc(sizeof(fweb_FILE));
    if (fp != NULL)
    {
        fp->stdio_stream = stream;
        fp->fcgx_stream = NULL;
    }

    return fp;
}

/*
 *----------------------------------------------------------------------
 *
 * fweb_fopen, fweb_fclose, fweb_fflush, fweb_freopen --
 *
 *       Wrappers for functions defined in H&S Section 15.2
 *
 *----------------------------------------------------------------------
 */
fweb_FILE *fweb_fopen(const char *path, const char *mode)
{
    FILE *file = fopen(path, mode);
    fweb_FILE *fcgi_file = fweb_OpenFromFILE(file);

    if (file && !fcgi_file)
        fclose(file);

    return fcgi_file;
}

int fweb_fclose(fweb_FILE * fp)
{
    int n = EOF;

    if (fp->stdio_stream)
    {
        n = fclose(fp->stdio_stream);
        fp->stdio_stream = NULL;
    }
    else if (fp->fcgx_stream)
    {
        n = FCGX_FClose(fp->fcgx_stream);
        fp->fcgx_stream = NULL;
    }
    if ((fp != fweb_stdin) && (fp != fweb_stdout) && (fp != fweb_stderr))
    {
        free(fp);
    }
    return n;
}

int fweb_fflush(fweb_FILE * fp)
{
    if (fp == NULL)
        return fflush(NULL);
    if (fp->stdio_stream)
        return fflush(fp->stdio_stream);
    else if (fp->fcgx_stream)
        return FCGX_FFlush(fp->fcgx_stream);
    return EOF;
}

fweb_FILE *fweb_freopen(const char *path, const char *mode, fweb_FILE * fp)
{
    if (fp->stdio_stream)
    {
        if (freopen(path, mode, fp->stdio_stream) == NULL)
            return NULL;
        else
            return fp;
    }
    else if (fp->fcgx_stream)
    {
        (void) FCGX_FClose(fp->fcgx_stream);
        fp->stdio_stream = fopen(path, mode);
        if (fp->stdio_stream == NULL)
            return NULL;
        else
        {
            fp->fcgx_stream = NULL;
            return fp;
        }
    }
    return NULL;
}

/*
 *----------------------------------------------------------------------
 *
 * fweb_setvbuf, fweb_setbuf --
 *
 *       Wrappers for functions defined in H&S Section 15.3
 *
 *----------------------------------------------------------------------
 */
int fweb_setvbuf(fweb_FILE * fp, char *buf, int bufmode, size_t size)
{
    if (fp->stdio_stream)
        return setvbuf(fp->stdio_stream, buf, bufmode, size);
    else
    {
        return -1;
    }
}

void fweb_setbuf(fweb_FILE * fp, char *buf)
{
    if (fp->stdio_stream)
        setbuf(fp->stdio_stream, buf);
}

/*
 *----------------------------------------------------------------------
 *
 * fweb_fseek, fweb_ftell, fweb_rewind, fweb_fgetpos, fweb_fsetpos --
 *
 *       Wrappers for functions defined in H&S Section 15.5
 *
 *----------------------------------------------------------------------
 */
int fweb_fseek(fweb_FILE * fp, long offset, int whence)
{
    if (fp->stdio_stream)
        return fseek(fp->stdio_stream, offset, whence);
    else
    {
        OS_SetErrno(ESPIPE);
        return -1;
    }
}

int fweb_ftell(fweb_FILE * fp)
{
    if (fp->stdio_stream)
        return ftell(fp->stdio_stream);
    else
    {
        OS_SetErrno(ESPIPE);
        return -1;
    }
}

void fweb_rewind(fweb_FILE * fp)
{
    if (fp->stdio_stream)
        rewind(fp->stdio_stream);
    else
        OS_SetErrno(ESPIPE);
}

int fweb_fgetpos(fweb_FILE * fp, fpos_t * pos)
{
    if (fp->stdio_stream)
        return fgetpos(fp->stdio_stream, pos);
    else
    {
        OS_SetErrno(ESPIPE);
        return -1;
    }
}

int fweb_fsetpos(fweb_FILE * fp, const fpos_t * pos)
{
    if (fp->stdio_stream)
        return fsetpos(fp->stdio_stream, pos);
    else
    {
        OS_SetErrno(ESPIPE);
        return -1;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * fweb_fgetc, fweb_getchar, fweb_ungetc --
 *
 *       Wrappers for functions defined in H&S Section 15.6
 *
 *       XXX: getc and getchar are generally defined as macros
 *            for performance reasons
 *
 *----------------------------------------------------------------------
 */
int fweb_fgetc(fweb_FILE * fp)
{
    if (fp->stdio_stream)
        return fgetc(fp->stdio_stream);
    else if (fp->fcgx_stream)
        return FCGX_GetChar(fp->fcgx_stream);
    return EOF;
}

int fweb_getchar(void)
{
    return fweb_fgetc(fweb_stdin);
}

int fweb_ungetc(int c, fweb_FILE * fp)
{
    if (fp->stdio_stream)
        return ungetc(c, fp->stdio_stream);
    else if (fp->fcgx_stream)
        return FCGX_UnGetChar(c, fp->fcgx_stream);
    return EOF;
}

/*
 *----------------------------------------------------------------------
 *
 * fweb_fgets, fweb_gets --
 *
 *       Wrappers for functions defined in H&S Section 15.7
 *
 *----------------------------------------------------------------------
 */
char *fweb_fgets(char *str, int size, fweb_FILE * fp)
{
    if (fp->stdio_stream)
        return fgets(str, size, fp->stdio_stream);
    else if (fp->fcgx_stream)
        return FCGX_GetLine(str, size, fp->fcgx_stream);
    return NULL;
}

/*
 * The gets() function reads characters from the standard input stream
 * into the array pointed to by str until a newline character is read
 * or an end-of-file condition is encountered.  The newline character
 * is discarded and the string is terminated with a null character. 
 */
char *fweb_gets(char *str)
{
    char *s;
    int c;

    for (s = str; ((c = fweb_getchar()) != '\n');)
    {
        if (c == EOF)
        {
            if (s == str)
                return NULL;
            else
                break;
        }
        else
            *s++ = (char) c;
    }
    *s = 0;
    return str;
}

/*
 *----------------------------------------------------------------------
 *
 *       Wrappers for functions defined in H&S Section 15.8
 *
 *       XXX: missing: fscanf, scanf
 *
 *----------------------------------------------------------------------
 */

/*
 *----------------------------------------------------------------------
 *
 * fweb_fputc, fweb_putchar --
 *
 *       Wrappers for functions defined in H&S Section 15.9
 *
 *       XXX: putc and putchar are generally defined as macros
 *            for performance reasons
 *
 *----------------------------------------------------------------------
 */
int fweb_fputc(int c, fweb_FILE * fp)
{
    if (fp->stdio_stream)
        return fputc(c, fp->stdio_stream);
    else if (fp->fcgx_stream)
        return FCGX_PutChar(c, fp->fcgx_stream);
    else
        return EOF;
}

int fweb_putchar(int c)
{
    return fweb_fputc(c, fweb_stdout);
}

/*
 *----------------------------------------------------------------------
 *
 * fweb_fputs, fweb_puts
 *
 *       Wrappers for functions defined in H&S Section 15.10
 *
 *----------------------------------------------------------------------
 */
int fweb_fputs(const char *str, fweb_FILE * fp)
{
    if (fp->stdio_stream)
        return fputs(str, fp->stdio_stream);
    else if (fp->fcgx_stream)
        return FCGX_PutS(str, fp->fcgx_stream);
    return EOF;
}

int fweb_puts(const char *str)
{
    int n;

    if (fweb_stdout->stdio_stream)
    {
        n = fputs(str, fweb_stdout->stdio_stream);
        if (n < 0)
            return n;
        else
            return fputc('\n', fweb_stdout->stdio_stream);
    }
    else if (fweb_stdout->fcgx_stream)
    {
        n = FCGX_PutS(str, fweb_stdout->fcgx_stream);
        if (n < 0)
            return n;
        else
            return FCGX_PutChar('\n', fweb_stdout->fcgx_stream);
    }
    return EOF;
}

/*
 *----------------------------------------------------------------------
 *
 * fweb_fprintf, fweb_printf --
 *
 *       Wrappers for functions defined in H&S Section 15.11
 *
 *----------------------------------------------------------------------
 */
int fweb_fprintf(fweb_FILE * fp, const char *format, ...)
{
    va_list ap;
    int n = 0;

    va_start(ap, format);
    if (fp->stdio_stream)
        n = vfprintf(fp->stdio_stream, format, ap);
    else if (fp->fcgx_stream)
        n = FCGX_VFPrintF(fp->fcgx_stream, format, ap);
    va_end(ap);
    return n;
}

int fweb_printf(const char *format, ...)
{
    va_list ap;
    int n;

    va_start(ap, format);
    n = fweb_vfprintf(fweb_stdout, format, ap);
    va_end(ap);
    return n;
}

/*
 *----------------------------------------------------------------------
 *
 * fweb_vfprintf, fweb_vprintf --
 *
 *       Wrappers for functions defined in H&S Section 15.12
 *
 *----------------------------------------------------------------------
 */
int fweb_vfprintf(fweb_FILE * fp, const char *format, va_list ap)
{
    if (fp->stdio_stream)
        return vfprintf(fp->stdio_stream, format, ap);
    else if (fp->fcgx_stream)
        return FCGX_VFPrintF(fp->fcgx_stream, format, ap);
    return EOF;
}

int fweb_vprintf(const char *format, va_list ap)
{
    if (fweb_stdout->stdio_stream)
        return vfprintf(fweb_stdout->stdio_stream, format, ap);
    else if (fweb_stdout->fcgx_stream)
        return FCGX_VFPrintF(fweb_stdout->fcgx_stream, format, ap);
    return EOF;
}

/*
 *----------------------------------------------------------------------
 *
 * fweb_fread, fweb_fwrite --
 *
 *       Wrappers for functions defined in H&S Section 15.13
 *
 *----------------------------------------------------------------------
 */
size_t fweb_fread(void *ptr, size_t size, size_t nmemb, fweb_FILE * fp)
{
    int n;

    if (fp->stdio_stream)
        return fread(ptr, size, nmemb, fp->stdio_stream);
    else if (fp->fcgx_stream)
    {
        if ((size * nmemb) == 0)
        {
            return 0;
        }
        n = FCGX_GetStr((char *) ptr, size * nmemb, fp->fcgx_stream);
        return (n / size);
    }
    return (size_t) EOF;
}

size_t fweb_fwrite(const void *ptr, size_t size, size_t nmemb,
                   fweb_FILE * fp)
{
    int n;

    if (fp->stdio_stream)
        return fwrite(ptr, size, nmemb, fp->stdio_stream);
    else if (fp->fcgx_stream)
    {
        if ((size * nmemb) == 0)
        {
            return 0;
        }
        n = FCGX_PutStr((char *) ptr, size * nmemb, fp->fcgx_stream);
        return (n / size);
    }
    return (size_t) EOF;
}

/*
 *----------------------------------------------------------------------
 *
 * fweb_feof, fweb_ferror, fweb_clearerr --
 *
 *       Wrappers for functions defined in H&S Section 15.14
 *
 *----------------------------------------------------------------------
 */
int fweb_feof(fweb_FILE * fp)
{
    if (fp->stdio_stream)
    {
        return feof(fp->stdio_stream);
    }
    else if (fp->fcgx_stream)
    {
        return FCGX_HasSeenEOF(fp->fcgx_stream);
    }
    return -1;

}

int fweb_ferror(fweb_FILE * fp)
{
    if (fp->stdio_stream)
    {
        return ferror(fp->stdio_stream);
    }
    else if (fp->fcgx_stream)
    {
        return FCGX_GetError(fp->fcgx_stream);
    }
    return -1;
}

void fweb_clearerr(fweb_FILE * fp)
{
    if (fp->stdio_stream)
    {
        clearerr(fp->stdio_stream);
    }
    else if (fp->fcgx_stream)
    {
        FCGX_ClearError(fp->fcgx_stream);
    }
    return;
}

/*
 *----------------------------------------------------------------------
 *
 * fweb_tmpfile --
 *
 *       Wrappers for function defined in H&S Section 15.16
 *
 *----------------------------------------------------------------------
 */
fweb_FILE *fweb_tmpfile(void)
{
    FILE *file = tmpfile();
    fweb_FILE *fcgi_file = fweb_OpenFromFILE(file);

    if (file && !fcgi_file)
        fclose(file);

    return fcgi_file;
}

/*
 *----------------------------------------------------------------------
 *
 * fweb_fileno, fweb_fdopen, fweb_popen, fweb_pclose --
 *
 *       Wrappers for POSIX, X/OPEN functions not in ISO C
 *
 *----------------------------------------------------------------------
 */
int fweb_fileno(fweb_FILE * fp)
{
    if (fp->stdio_stream)
        return fileno(fp->stdio_stream);
    else
        return -1;
}

fweb_FILE *fweb_fdopen(int fd, const char *mode)
{
    FILE *file = fdopen(fd, mode);
    fweb_FILE *fcgi_file = fweb_OpenFromFILE(file);

    if (file && !fcgi_file)
        fclose(file);

    return fcgi_file;
}

fweb_FILE *fweb_popen(const char *cmd, const char *type)
{
    FILE *file = popen(cmd, type);
    fweb_FILE *fcgi_file = fweb_OpenFromFILE(file);

    if (file && !fcgi_file)
        pclose(file);

    return fcgi_file;
}

int fweb_pclose(fweb_FILE * fp)
{
    int n = EOF;

    if (fp->stdio_stream)
    {
        n = pclose(fp->stdio_stream);
        fp->stdio_stream = NULL;
    }
    else if (fp->fcgx_stream)
    {
        /*
         * The caller is deeply confused; don't free the storage.
         */
        return EOF;
    }
    if ((fp != fweb_stdin) && (fp != fweb_stdout) && (fp != fweb_stderr))
    {
        free(fp);
    }
    return n;
}
