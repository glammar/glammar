
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 
*/
#include "ge1.h"
#ifdef LOOP
void loop_line_mode (rroot)
void (*rroot) ();
{
  affix *raf = af;
  char *rc = c;
  if (!ll_mode)
  {
    (*rroot) ();
    return;
  }
  ip = input;
  do
  {
    *ip++ = getc (inputfile);
    nrofchars += 1;
    if (memorizing)
    {
      if (nrofchars > runtime_input_size - 4)
      {
        fprintf (stderr, "%s: Input size too large (max %d chars)\n", arg_zero, runtime_input_size - 4);
        exit (1);
      }
      clear_memo_tab ();
    }
    if (*(ip - 1) == '\n')
    {
      int pcount = parsecount;
      limitip = ip;
      mip = input;
      *(ip - 1) = '\0';
      *ip = '\n';
      ip = input;
      (*rroot) ();
      if (parsecount == pcount)
        fprintf (stderr, "%s: error on line %d, input = `%s' \n", arg_zero, set_line_num, input);
      ip = input;
      nrofchars = 0;
      set_line_num += 1;
      if ((set_line_num == 2) && (ll_count != 0))
      {
        fprintf (stderr, "root = %x' \n", rroot);
        /* rroot = uloop; */
        fprintf (stderr, "root = %x' \n", rroot);
      }
      q -= 1;
      c = rc;
      af = raf;
    }
  }
  while (!feof (inputfile));
  exit (0);
}
#endif

#ifdef ERMSG
int errline (b, e)
char *b, *e;
{
  if (b > e)
  {
    return 0;
  }
  if (e > (b + 75))
  {
    if (e > mip + 35)
      e = mip + 35;
    if (b < mip - 35)
      b = mip - 35;
    if (e > b + 75)
      b = e - 75;
    fprintf (stderr, "@");
  }
  while (b <= e)
    fprintf (stderr, "%c", *b++);
  fprintf (stderr, "\n", *b++);
}

int underline (b, e)
char *b, *e;
{
  if (!e || !b)
  {
    fprintf (stderr, "%s: Internal error while generating error message fragment\n", arg_zero);
    exit (1);
  }
  if (b > e)
  {
    fprintf (stderr, "\n^(012)\n");
    return 0;
  }
  if (e > b + 75)
    if (e > mip + 35)
      e = mip + 35;
  if (b < mip - 35)
    b = mip - 35;
  if (e > b + 75)
    b = e - 76;
  for (; b <= e; b++)
  {
    if (b == mip)
    {
      int o = *mip;
      o &= 255;
      fprintf (stderr, "^(%o)\n", o);
      break;
    }
    else if (*b == '\t')
      fprintf (stderr, "\t");
    else
      fprintf (stderr, "-");
  }

}

extern affix a_scriptname;
extern char *pntname;
int errmsg ()
{
  char *beginmsg, *endmsg;
  int linenumber = 1;

  if (emsg_count > 0)
    return (1);

  if (parsecount != 0)
  {
    if (output_to_stdout)
    {
      fclose (stdout);
    }
    else
    {

/*         putc('\012',output); */
      fclose (output);
    }
    return (exit_code);
  }

  /* say file */

  if (*current_file_name != '\0')
    fprintf (stderr, "\n*** %s *** File: %s\n", arg_zero, current_file_name);

  if (rmax > 0)
    fprintf (stderr,
             "\n*** %s *** Out of branches near \"%s\" called from \"%s\"\n", a_scriptname.t, sv_error_msg, pntname);

  if (*mip == '\0')
  {
    fprintf (stderr, "*** %s *** Stopped.\n", a_scriptname.t);
    return (1);
  }

  /* say line number */
  {
    char *count;
    if ((mip > limitip) || (mip < input))
    {
      fprintf (stderr, "%s: *** glammar compiler error in \"errmsg\"\n\
     failed to compute linenumber\n", arg_zero);
      exit (1);
    }
    linenumber = set_ip_start_num;
    for (count = set_ip_start_pos; count < mip;)
      if (*count++ == '\n')
        linenumber += 1;
    if (*count == '\n')
      linenumber += 1;

    fprintf (stderr, "*** %s *** Syntax error on line %d:\n", a_scriptname.t, linenumber);
  }
  /* print line before erroneous line, max 76 chars */

  if (linenumber > 1)
  {
    for (beginmsg = mip; *beginmsg-- != '\n';);
    endmsg = beginmsg;
    for (; *beginmsg-- != '\n';);
    beginmsg += 2;
    errline (beginmsg, endmsg);
    beginmsg = endmsg + 2;
  }
  else
    beginmsg = input;


  /* print errline */
  endmsg = mip;
  while (*endmsg != '\0')
    if (*endmsg++ == '\n')
    {
      endmsg -= 2;
      break;
    }

  errline (beginmsg, endmsg);
  underline (beginmsg, endmsg);

  /* print line after error line */
  endmsg += 2;
  beginmsg = endmsg;
  if (*endmsg != '\0')
  {
    while (*endmsg)
      if (*endmsg++ == '\n')
      {
        endmsg -= 2;
        break;
      }
  }
  errline (beginmsg, endmsg);
  return (1);
}

void result ()
{
  if (emsg_count > 0)
    return;
  if (report_stacksize)
  {
    long long stack_size;
    char stack_top;
    long long cur_afx_heap_size = (long long) af - (long long) affix_heap;
    long long cur_char_heap_size = (long long) c - (long long) cstore;
    long long max_afx_heap_size = (long long) afhsize * sizeof (affix);
    long long max_char_heap_size = (long long) cssize;

    stack_size = (long long) stack_bottum - (long long) &stack_top;
    fprintf (stderr, "\nHere is how much storage glammar used:\n");
    fprintf (stderr, "eval count = %d\n", eval_count);
    fprintf (stderr,
             "Backtrack stack size = %ldMiB (out of ?), increase with csh command limit()\n", (stack_size >> 20) + 1);
    fprintf (stderr,
             "Char stack size  = %.1fMB (out of %.1fMB), increase with -cn (-c1 = 1MB)\n",
             ((float) cur_char_heap_size) / 1000000.0, ((float) max_char_heap_size) / 1000000.0);
    fprintf (stderr,
             "Affix stack size  = %.1fMB (out of %.1fMB), increase with -hn (-h1 = %dMB)\n",
             ((float) cur_afx_heap_size) / 1000000.0, ((float) max_afx_heap_size) / 1000000.0, sizeof (affix));
  }
  if (output == 0)
  {
    if (output_to_stdout)
      output = stdout;
    else
    {
      output = fopen (out_file_name, "w");
      if (!output)
        fprintf (stderr, "%s: cannot write to `%s'\n", arg_zero, out_file_name);
    }
  }
  printa (output, q->a);
  fflush (output);
}
#endif

#ifdef DREPORT

/*  report   */
int Dreport (A, B)
register AFFIX A, B;
{
  if (output == 0)
  {
    if (output_to_stdout)
      output = stdout;
    else
    {
      output = fopen (out_file_name, "w");
      if (!output)
        fprintf (stderr, "%s: cannot write to `%s'\n", arg_zero, out_file_name);
    }
  }
  printa (output, A);
  fflush (output);
  return true;
}
#endif

#ifdef UREPORT
void Ureport ()
{
  register cont *rq = q;
  register AFFIX A_0 = (rq + -1)->a;
  if (Dreport (A_0))
  {
    q = rq + -2;
    (*(rq + -1)->q) ();
    rq = q - -1;
  }
  (rq)->a = A_0;
  (rq + 1)->q = Ureport;
  q = rq + 1;
}
#endif

#ifdef PRINTA
void printa (outdir, afx)
register AFFIX afx;
FILE *outdir;
{
  register char *cp;
tailrecel:
  if (afx == nil)
    return;
  cp = afx->t;
  if (cp == fast_list_acces)
  {
    fprintf (outdir, "!<LNODE_%d>!", afx->r);
    return;
  }
  if (cp == undefined)
  {
    fprintf (outdir, "!@!");
    return;
  }
  if (cp != empty)
  {
    while (*cp)
    {
      if ((cp[0] == '\001') && (cp[9] == '\001'))
      {
        fputs ("[*]", outdir);
        cp += 10;
      }
      else
        fputc (*cp, outdir);
      cp++;
    }
  }
  printa (outdir, afx->l);

/*  printa(outdir, afx->r); */
  afx = afx->r;
  goto tailrecel;
}

void sprinta (afx)
register AFFIX afx;
{
  register char *cp, *cp0, *rc;
tailrecel:
  if (afx == nil)
    return;
  cp = cp0 = afx->t;
  if (cp == fast_list_acces)
  {
    fprintf (stderr, "%s: system error: evaluating list node\n", arg_zero);
    exit (1);
  }
  if (cp == undefined)
  {
    if (rmax > 0)
      fprintf (stderr, "==> %d :\n", rmax);
    fprintf (stderr, "%s: Glammar message:\nIn sprinta (\"%s\"): trying to evaluate uninstantiated affix.\n", arg_zero,
             cp);
    parsecount = 0;
    errmsg ();
    exit (1);

  }
  if (cp != empty)
  {
    char *ic;
    eval_count += 1;

    for (rc = c; *cp != '\0';)
    {
      if (rc + 12 > cstore_top)
        cstore_overflow ();
      if (*cp == '\001' || *cp == '\002')
      {
        *rc = *cp;
        *(rc + 1) = *(cp + 1);
        *(rc + 2) = *(cp + 2);
        *(rc + 3) = *(cp + 3);
        *(rc + 4) = *(cp + 4);
        *(rc + 5) = *(cp + 5);
#ifdef EIGHT_BYTE_ADDR
        *(rc + 6) = *(cp + 6);
        *(rc + 7) = *(cp + 7);
        *(rc + 8) = *(cp + 8);
        *(rc + 9) = *(cp + 9);
        cp += 4;
        rc += 4;
#endif
        cp += 6;
        rc += 6;

      }
      else
      {
        if (cp == c)
        {
          fprintf (stderr,
                   "%s: Glammar internal error:\nIn sprinta (\"%.773s\"): strcpy overlap %p\n", arg_zero, cp0, c);
          exit (1);
        }
        *rc++ = *cp++;
      }

    }

    c = rc;
  }
  if (afx->l)
    sprinta (afx->l);

/*   sprinta((*afx).r);   tail recursion eliminations */
  afx = afx->r;
  goto tailrecel;

}
#endif
