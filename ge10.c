
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 
*/
#include "ge1.h"

#define EVAL(a,b)   b = c;\
                        sprinta (a);\
                        *c++ = '\0';

#ifdef DUNIXCON
#define MAX_CON 32
int talkto ();

FILE *Unix_in[MAX_CON], *Unix_out[MAX_CON];

int UnixConnectionSetUp[MAX_CON] = {
  false, false, false, false, false, false, false, false,
  false, false, false, false, false, false, false, false,
  false, false, false, false, false, false, false, false,
  false, false, false, false, false, false, false, false,
};

int connection_count = 0;



int Dsetupconnectiontounix (B, C)       /* setupconnectiontounix */
register AFFIX B, C;
{
  char *A;
  if (connection_count < MAX_CON - 1)
  {
    EVAL (B, A);
    if (!talkto (A))
      return false;
    UnixConnectionSetUp[connection_count] = true;

    C->t = c;
    (void) sprintf (c, "%d\0", connection_count++);
    c += 4;

    return true;
  }
  return false;
}

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>


int Dtalktounix (B, A_0, A_1)   /* talktounix */
register AFFIX B, A_0, A_1;
{
  int connection = atoi (B->t);
  FILE *in = Unix_in[connection];
  FILE *out = Unix_out[connection];
  int in_fildes = fileno (in);
  struct timeval tv;

  fd_set rfds;
  int retval;

  /* Watch Unix_in to see when it has input. */


  if (UnixConnectionSetUp[connection] && !feof (in))
  {
    register char *rc = c;
    A_1->t = rc;
    A_1->l = nil;
    A_1->r = nil;
    fflush (out);
    fflush (in);

/*      fseek (in, 0L, SEEK_END); */
    printa (stderr, A_0);
    printa (out, A_0);
    fflush (out);
#ifdef NEVER
    *--rc = '\0';
#endif
    FD_ZERO (&rfds);
    FD_SET (in_fildes, &rfds);

    /* Wait up to five seconds. */
    tv.tv_sec = 2;
    tv.tv_usec = 0;

#ifdef never

    retval = select (in_fildes + 1, &rfds, NULL, NULL, &tv);
    /* FD_ISSET(in_fildes, &rfds) will be true if retval. */
    if (retval != 1)
    {

      fprintf (stderr, "%s: talk to unix: select() failed, err = %s\n", arg_zero, strerror ((errno)));
      return false;
    }
#endif
    do
    {
      do
      {
        *rc = getc (in);
        if (*rc != '\r')
          rc++;
      }
      while ((*(rc - 1) != '\n') && (!feof (in)));

      if (feof (in))
        break;

      FD_ZERO (&rfds);
      FD_SET (in_fildes, &rfds);

      tv.tv_sec = 0;
      tv.tv_usec = 100;

      retval = select (in_fildes + 1, &rfds, NULL, NULL, &tv);
    }
    while (retval == 1 && (!feof (in)));
    if (*(rc - 1) == '\n')
      rc--;
    *rc++ = '\0';
    c = rc;
    if (rc > cstore_top)
      cstore_overflow ();
    return true;
  }
  return false;
}


int talkto (cmd)
char *cmd;
{
  int to_child[2], to_parent[2], pid, cnt = 1;
  char *args = cmd;

  pipe (to_child);
  pipe (to_parent);

  argv[0] = cmd;

  while (*args++ != '\0')
  {
    if ((*args == ' ') && (args == cmd || *(args - 1) != '\\'))
    {
      *args = '\0';
      while (*++args == ' ');
      argv[cnt++] = args;
      if (cnt == 102)
      {
        fprintf (stderr, "%s: set up connection to unix: too many args (>100)\n", arg_zero);
        return false;
      }
    }
    else if (*args == ' ' && (args != cmd && *(args - 1) == '\\'))
    {
      char *a = args--;
      do
        *(a - 1) = *a;
      while (*a++);
    }
  }
  argv[cnt] = NULL;


  if (pid = vfork (), pid == 0)
  {
    int err = 0;

    close (0);
    dup (to_child[0]);
    close (1);
    dup (to_parent[1]);
    close (to_child[0]);
    close (to_child[1]);
    close (to_parent[1]);
    close (to_parent[0]);
    err = execvp (cmd, argv);
    fprintf (stderr, "%s: execvp: (%d) No such command %s\n", arg_zero, err, cmd);
    exit (1);
  }
  else if (pid > 0)
  {                             /* in the parent */

    Unix_in[connection_count] = fdopen (to_parent[0], "r");
    Unix_out[connection_count] = fdopen (to_child[1], "w");

    if (Unix_in[connection_count] == NULL)
      return false;
    close (to_child[0]);
    close (to_parent[1]);


  }
  else
  {
    fprintf (stderr, "%s: Couldnt fork process %s\n", arg_zero, cmd);
    return false;
  }
  return true;
}

int Dgetfromunix (A_0, A_1)
register AFFIX A_1;
register AFFIX A_0;
{
  char *A;
  register char *rc;
  EVAL (A_0, A);
  if (talkto (A))
  {
    FILE *in = Unix_in[connection_count];
    FILE *out = Unix_out[connection_count];
    rc = c;
    A_1->t = rc;
    A_1->l = nil;
    A_1->r = nil;
    while (!feof (in))
      *rc++ = getc (in);
    *(rc - 1) = '\0';
    c = rc;
    if (rc > cstore_top)
      cstore_overflow ();
    fclose (in);
    fclose (out);
    return true;
  }
  return false;
}

int Dwritefile (A_0, A_1)
register AFFIX A_1;
register AFFIX A_0;
{
  char *A;
  register char *rc;
  FILE *out;
  EVAL (A_0, A);
  if (out = fopen (A, "w"))
  {
    printa (out, A_1);
    fflush (out);
    fclose (out);
    return true;
  }
  return false;
}

int Dsystem (A_0)
register AFFIX A_0;
{
  char *A;
  EVAL (A_0, A);
  if (!system (A))
    return true;
  return false;
}
#endif

#ifdef UUNIXCON
void Usetupconnectiontounix ()
{
  register cont *rq = q;
  register AFFIX A_1 = rq->a;
  register AFFIX A_0 = (rq - 1)->a;
  char *rc = c;
  if (Dsetupconnectiontounix (A_0, A_1))
  {
    q = rq - 3;
    (*(rq - 2)->q) ();
    rq = q + 2;
  }
  (rq - 1)->a = A_0;
  rq->a = A_1;
  (rq + 1)->q = Usetupconnectiontounix;
  q = rq + 1;
  c = rc;
}

void Utalktounix ()
{
  register cont *rq = q;
  register AFFIX A_2 = rq->a;
  register AFFIX A_1 = (rq - 1)->a;
  register AFFIX A_0 = (rq - 2)->a;
  char *rc = c;
  if (Dtalktounix (A_0, A_1, A_2))
  {
    q = rq - 4;
    (*(rq - 3)->q) ();
    rq = q + 3;
  }
  (rq - 2)->a = A_0;
  (rq - 1)->a = A_1;
  rq->a = A_2;
  (rq + 1)->q = Utalktounix;
  q = rq + 1;
  c = rc;
}

void Ugetfromunix ()
{
  register cont *rq = q;
  register AFFIX A_1 = rq->a;
  register AFFIX A_0 = (rq - 1)->a;
  char *rc = c;
  if (Dgetfromunix (A_0, A_1))
  {
    q = rq - 3;
    (*(rq - 2)->q) ();
    rq = q + 2;
  }
  (rq - 1)->a = A_0;
  rq->a = A_1;
  (rq + 1)->q = Ugetfromunix;
  q = rq + 1;
  c = rc;
}

GLAMMAR_Q1 (Dsystem, Usystem)
void Uwritefile ()
{
  register cont *rq = q;
  register AFFIX A_1 = rq->a;
  register AFFIX A_0 = (rq - 1)->a;
  char *rc = c;
  if (Dwritefile (A_0, A_1))
  {
    q = rq - 3;
    (*(rq - 2)->q) ();
    rq = q + 2;
  }
  (rq - 1)->a = A_0;
  rq->a = A_1;
  (rq + 1)->q = Uwritefile;
  q = rq + 1;
  c = rc;
}
#endif
#ifdef DTOUTC

#include <stdio.h>
#include <time.h>

int Dtoutc (A_0, A_1)
register AFFIX A_1;
register AFFIX A_0;
{
  char *A;
  register char *rc;

  struct tm tmi, *tmt;

  time_t tt;
  int msec;
  int year;
  EVAL (A_0, A);
  sscanf (A, "%d/%d/%d %d:%d:%d.%d\n", &year, &tmi.tm_mon, &tmi.tm_mday, &tmi.tm_hour, &tmi.tm_min, &tmi.tm_sec, &msec);
  tmi.tm_isdst = -1;
  tmi.tm_year = year - 1900;
  --tmi.tm_mon;
  tt = mktime (&tmi);
  tmt = gmtime (&tt);
  rc = c;
  A_1->t = rc;
  A_1->l = nil;
  A_1->r = nil;
  rc += sprintf (rc, "%02d/%02d/%04d %02d:%02d:%02d.%03d",
                 tmt->tm_mon + 1, tmt->tm_mday, tmt->tm_year + 1900, tmt->tm_hour, tmt->tm_min, tmt->tm_sec, msec);
  c = rc + 1;
  if (rc > cstore_top)
    cstore_overflow ();
  return true;
}
#endif


#ifdef UTOUTC
GLAMMAR_Q2 (Dtoutc, Utoutc)
#endif
