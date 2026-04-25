
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 
*/
#ifdef LINUX64
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include "ge1.h"
#ifdef SET_AND_TEST_LIMITS
void afx_overflow () 
{
  fprintf (stderr, "%s: Affix heap overflow.\n\
 Hard Limit was %ld bytes.\n\
 Increase with -hn (n=1 = 6Mb, n=2 =12Mb, ...);\n", arg_zero, (char *) afx_top - (char *) affix_heap);
  exit (1);
}

void need_more_qstack () 
{
  fprintf (stderr, "%s: Continuation stack overflow.\n\
 Hard Limit was %ld bytes.\n\
 Increase with -qn (n=1 = 100Kb, n=2 =200Kb, ...);\n", arg_zero, q_size);
  exit (1);
}

void cstore_overflow () 
{
  fprintf (stderr, "%s: Char store overflow.\n\
 Hard Limit was %ld bytes.\n\
 Increase with -cn (n=1 = 512Kb, n=2 =1Mb, ...);\n", arg_zero, cstore_top - input);
  exit (1);
}

void set_and_test_limits (int stk_size, int memo_flag) 
{
#ifdef LINUX64 
  {
    struct rlimit lmts;

    if (getrlimit (RLIMIT_STACK, &lmts) != 0)
    {
      fprintf (stderr, "%s: Cannot read stack limits\n", arg_zero);
      exit (1);
    }

    lmts.rlim_cur = lmts.rlim_max;

    if (setrlimit (RLIMIT_STACK, &lmts) != 0)
    {
      fprintf (stderr, "%s: Cannot set new stack limits\n", arg_zero);
      exit (1);
    }
  }
#endif
  /* test inputsize */
  if (memo_flag)
    if (nrofchars > runtime_input_size - 4)
    {
      fprintf (stderr, "%s: Input size too large (max %ld chars)\n", arg_zero, runtime_input_size - 4);
      exit (1);
    }
}
#endif

#ifdef TRACE
char *nt_cbuf[128];
int nt_idx = 0;

void trc_linenr(void)
{
 if (ip <= limitip && ip >= input)
 {
  register char *rip = set_line_pos;
  int line = set_line_num;

  while (rip < ip)
    if (*rip++ == '\n')
      line += 1;
  while (rip > ip)
    if (*rip-- == '\n')
      line -= 1;
  fprintf( stdout, ": %d\n", line);
  set_line_num =  line;
  set_line_pos =  rip;
 }
 else 
    fputs("(input redirected)\n", stdout); 
}
void traceinput(int end);

int nt_name_push (char *nt_name) 
{
  if (nt_cbuf[(nt_idx) & 127] == nt_name)
    return 0;
  nt_cbuf[++nt_idx & 127] = nt_name;
  nt_cbuf[(nt_idx + 1) & 127] = 0;
  return 1;
}

int nt_name_ipop (int pushed )
{
  if (pushed && nt_cbuf[(nt_idx & 127)])
    nt_cbuf[(nt_idx--) & 127] = 0;
  return 1;
}

char *nt_name_pop (int pushed )
{
  char *r;
  if ((r = nt_cbuf[(nt_idx & 127)]))
    nt_cbuf[(nt_idx--) & 127] = 0;
  return r;
}

int last_trace_index;
char *spaces = "                                                                                                                                ";
int begin1_trace (char *ntname) 
{
  if (interesting_level_number != -1 && level >= interesting_level_number)
  {
    int id = (q - q_stack) >> 3;
    char *ids = spaces + 128 - (id*2 & 127);
    if (id > 0)
      fprintf (stdout, "%d ", id);
    if (pntname != lastpntname)
    {
      fprintf (stdout, "%ld %s%.60s<-%.12s ", level, ids, ntname, pntname);
      lastpntname = pntname;
    }
    else
      fprintf (stdout, "%ld %s%.60s ", level, ids, ntname);
    if (!memcmp (ntname, "setinputptrto_", sizeof ("setinputptrto_") - 1))
      fprintf (stdout, "%p ", c);

    fflush (stdout);
  }
  pntname = ntname;
 return 0;
}

void begin2_trace ()
{
  char lb[100];
  if (interesting_level_number == -1)
    return;
  if (level > interesting_level_number)
  {
    int i;
    if ((ip > limitip) || (ip < input))
    {
      fputc ('$', stdout);
    }
    else if (*ip != '\0')
    {
      fprintf (stdout, "%ld ", ip - input);
    }
    else
    {
      fprintf (stdout, "EOF\n");
      return;
    }
    traceinput(10);
    trc_linenr();
    fflush (stdout);
  }
}

void traceinput(int end)
{ 
    int i;
    fputc ('\'', stdout);
    for (i = 0; i < end  && ip[i]; i++)
    {
      if (isprint (ip[i]))
      {
        fputc (ip[i], stdout);
        continue;
      }
      if (!ip[i])
      {
        fputc ('\\', stdout);
        fputc ('0', stdout);
        break;
      }
      if (ip[i] == '\n')
      {
        fputc ('\\', stdout);
        fputc ('n', stdout);
        continue;
      }
      if (ip[i] == '\r')
      {
        fputc ('\\', stdout);
        fputc ('r', stdout);
        continue;
      }
      if (ip[i] == '\t')
      {
        fputc ('\\', stdout);
        fputc ('t', stdout);
        continue;
      }
      fprintf (stdout, "\\%3o", ip[i]);
    }
    fputc ('\'', stdout);
}
int try_trace_list_affix (int, int, AFFIX);

void trace_lattice (AFFIX afx) 
{

  if (interesting_level_number == -1)
    return;
  if (afx == nil)
    return;
  if (level > interesting_level_number)
  {
    printf ("{%lX}", (long)afx->t);
  }
}

void trace_affix (int builtin_affix, AFFIX afx) 
{
  int trace_level_affix = builtin_affix ? 3 : 9;

  if (interesting_level_number == -1)
    return;
  if (afx == nil)
    return;
  if (level > interesting_level_number)
  {
    printf ("(");
    if (!try_trace_list_affix (0, trace_level_affix, afx))
      printa (stdout, afx);
    printf (") ");
  }
}

void int_trace_affix (int paren, int n, AFFIX afx) 
{
  if (afx == nil)
    return;

  if (!try_trace_list_affix (paren, n, afx))
    printa (stdout, afx);
}

int try_trace_list_affix (int paren, int n, AFFIX A) 
{
  affix *cell = (affix *) - 1;

  if (A == 0)
  {
    printf ("(null)");
    return true;
  }
  GET_CELL_ADDR (cell, (A->t));
  if (cell)
  {
    if (n <= 0)
    {
      printf ("..");
      return true;
    }

    if (paren)
      printf ("{");
    int_trace_affix (!paren, n - 1, cell->l);
    printf ("*");
    int_trace_affix (0, n - 2, cell->r);
    if (paren)
      printf ("}");
    return true;
  }
  return false;
}


int traceterm (char *term, int rc, char *pre, char *post) 
{

  if (interesting_level_number == -1)
    return rc;
  if (level > interesting_level_number)
  {
    int id = (q - q_stack) >> 3;
    char *ids = spaces + 128 - (id*2 & 127);
    char eterm[2001];
    char *ec = eterm;
    char *tc = term;
    char *eec=ec+2000;
    *ec = 0;
    while (*tc && ec < eec)
    {
      
      if (isprint (*tc))
      {
        if (*tc == '"')
          *ec++ = '\\';
        *ec++ = *tc++;
      }
      else if (*tc == '\n')
      {
        *ec++ = '\\';
        *ec++ = 'n';
        tc++;
      }
      else if (*tc == '\t')
      {
        *ec++ = '\\';
        *ec++ = 't';
        tc++;
      }
    }
    *ec = 0;

    if (id > 0)
      fprintf (stdout, "%d ", id);
    if (rc)
    {
      fprintf (stdout, "%ld +%s%s%s%s\n", level, ids, pre, eterm, post);
    }
    else
    {
      int end = strlen(eterm);
      fprintf (stdout, "%ld -%s%s%s%s <", level, ids, pre, eterm, post);
      traceinput(end>10?end:10);
      trc_linenr();
       
    }
    fflush (stdout);
  }
  return rc;
}

long endtrace (int pct, char *pntsave, char *ntname) 
{
  if (interesting_level_number == -1)
  {
    if (level >= rmax && pct == parsecount)
    {
      if (level == rmax)
      {
        int x = strlen (sv_error_msg);
        if (x < 500)
        {
          char xx[512];
          sprintf (xx, "%.100s <- %.100s", ntname, pntsave);
          if (!strstr (sv_error_msg, xx))
            sprintf (sv_error_msg + x, "\n     OR %s", xx);
        }
      }
      else
      {
        rmax = level;
        sprintf (sv_error_msg, "%.100s <- %.100s", ntname, pntsave);
      }
    }
    pntname = pntsave;
    return 0;
  }
  if (level > interesting_level_number)
  {
    int id = (q - q_stack) >> 3;
    char *ids = spaces + 128 - (id*2 & 127);
    char *pt = pntsave;
    if (id > 0)
      fprintf (stdout, "%d ", id);
    if (pct == parsecount)
    {
      if ((ip > limitip) || (ip < input))
        fprintf (stdout, "%ld %s<-- %s.%s \n", level, ids, ntname, pt);
      else if (*ip != '\0')
        fprintf (stdout, "%ld %s<-- %s.%s %ld,%ld\n", level, ids, ntname, pt, mip - input, ip - input);
      else
        fprintf (stdout, "%ld %s<-- %s.%s EOF\n", level, ids, ntname, pt);
    }
    else
    {
      if ((ip > limitip) || (ip < input))
        fprintf (stdout, "%ld %s<++ %s.%s META \n", level, ids, ntname, pt);
      else if (*ip != '\0')
        fprintf (stdout, "%ld %s<++ %s.%s %ld,%ld \n", level, ids, ntname, pt, mip - input, ip - input);
      else
        fprintf (stdout, "%ld %s<++ %s.%s EOF\n", level, ids, ntname, pt);
    }
    fflush (stdout);
  }
  pntname = pntsave;
  return 0;
}

void ambiguous_trace (int *pct, int *act, char *ntname) 
{
  if (*pct < parsecount)
  {
    if (*act != 0)
      fprintf (stderr, "%s: runtime message: `%s' ambiguous, %ld, '%c'\n", arg_zero, ntname, ip - input, *ip);
    else
      *act += 1;
    *pct = parsecount;
  }
}
#endif

void result ();
#ifdef FOUND
void found ()
{
  if (*ip == '\0')
  {
    parsecount += 1;
    result ();
    if (abq_level && parsecount >= abq_level)
      exit (0);
  }
  q += 1;
}
#endif
