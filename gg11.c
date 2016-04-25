
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 

/* file coder2 (for deterministic prefixes) */
#include "gg1.h"
#include "gg2.h"

extern int mems_lifted, highest_tempory, nraffixes;
extern int tail_recursion_elm_count;


/* 
 * affix offset count 
 */
int oCount = 0;

det_rule_head ()
{
  fprintf (output, "\n\n/*  %s   */\n", FREPR (ulhs));
  if (MARKED (ulhs, external))
    fprintf (output, "int D%s(", REPR (ulhs));
  else
    fprintf (output, "static int D_%d(", ulhs);
  det_get_affixes (AFFIXDEF (SON (ulhs)));

  if (fprintf (output, " {\n register AFFIX raf = af;\n") == EOF)
  {
    fprintf (stderr, "glammar: Write failed (try again later)\n");
    exit (12);
  }
  if (BROTHER (SON (ulhs)) != nil)
  {
    fprintf (output, "  char *rc = c,  *rip = ip;\n");
  }
  if (lookahead_in_alt (SON (ulhs)))
    fprintf (output, "  char *lkh;\n");
  if (BROTHER (SON (ulhs)) != nil)
    fprintf (output, "  int cut_set = 0;\n");
  fprintf (output, "  int ltr=level++;\n");
  if (trace_flag)
    fprintf (output, " char *pntnamesv = pntname; char *ntname = \"%s\";\n", FREPR (ulhs));
  else if (errormsg_flag)
  {
    int mem = SON (SON (ulhs));
    if (mem != nil && DEF (mem) == equal)
      fprintf (output, "  char *pntnamesv = pntname; char *ntname = \"%s\"; int lsave = level+10;\n", FREPR (ulhs));
    else
      fprintf (output, "  char *pntnamesv = pntname; char *ntname = \"%s\"; int lsave = level++;\n", FREPR (ulhs));
  }

  if (stat_flag)
  {
    fprintf (output, "  int s1 = stat_count++;\n");
  }
  save_lattice_afx ();
  if (tail_recursion_elimination ())
    fprintf (output, " label_%d:\n\n", ulhs);

}

get_last_but_cut_mem (mem)
{
  if (mem == nil)
    return nil;

  if (BROTHER (mem) == nil)
    return mem;

  for (; BROTHER (mem) != nil && BROTHER (BROTHER (mem)) != nil; mem = BROTHER (mem));


  if (DEF (BROTHER (mem)) == cut)
    return mem;

  return BROTHER (mem);
}

tail_recursion_elimination ()
{
  int alt;
  int tre = 0;

  if (no_recursion_elm_flag)
    return false;

  for (alt = SON (ulhs); alt != nil; alt = BROTHER (alt))
  {
    int mem = SON (alt);
    if (mem == nil)
      continue;
    if (FLAG_MARKED (alt, no_tail_recursion_opt_f))
      continue;
    mem = get_last_but_cut_mem (mem);
    if (DEF (mem) == ulhs)
    {
      FLAG_SET (alt, tail_recursion_opt_f);
      FLAG_SET (mem, tail_recursion_opt_f);
      tre = 1;
    }
  }
  return tre;
}

det_rule_tail ()
{
  int last_alt;

  for (last_alt = SON (ulhs); BROTHER (last_alt) != nil; last_alt = BROTHER (last_alt));
  if (SON (last_alt) == nil)
  {
    fprintf (output, "  \n}\n");
    return;
  }

  if (trace_flag)
  {
    if (errormsg_flag)
      fprintf (output, "  set_errmsg(ntname);\n");
    fprintf (output, "  endtrace(parsecount,pntnamesv,ntname); level=ltr;");
  }
  else if (errormsg_flag)
  {
    fprintf (output, "  set_errmsg(ntname);\n");
    fprintf (output, " level = lsave;\n");
  }

/*   
 */
  if (stat_flag)
    fprintf (output, "  stat_tab[%ld] += stat_count - s1;\n", COUNT (ulhs));


  fprintf (output, "  return false;\n}\n");

}

det_get_affixes (afx)
int afx;
{
  int affix = afx, count = 0;

  if (afx == nil)
  {
    fprintf (output, ")\n");
    return;
  }

  for (; affix != nil; affix = BROTHER (affix))
    if (BROTHER (affix) != nil)
      fprintf (output, "A_%d,", count++);
    else
      fprintf (output, "A_%d)\n", count++);

  count = 0;
  for (affix = afx; affix != nil; affix = BROTHER (affix))
    fprintf (output, "  register AFFIX A_%d;\n", count++);
}


det_rule_alts (alt)
{
  int affix = (AFFIXDEF (alt)), rule = ulhs;

  if (trace_flag)
    trace_code (MARKED (rule, built_aliased), affix);

  for (; alt != nil; alt = BROTHER (alt))
  {

    fprintf (output, "  {\n");
    det_alt_code (alt);
    fprintf (output, "  }\n");
    if (BROTHER (alt) != nil)
    {
      fprintf (output, " ip = rip; c = rc; af = raf;\n");
      if (trace_flag)
        fprintf (output, " level=ltr+1;\n");
      restore_lattice_afx ();
      if (trace_flag)
      {
        fprintf (output, "  if (cut_set) {\n");
        if (stat_flag)
          fprintf (output, "  stat_tab[%d] += stat_count-s1;\n", COUNT (ulhs));
        fprintf (output, "  endtrace(parsecount,pntnamesv,ntname);");
        if (errormsg_flag)
          fprintf (output, "  set_errmsg(ntname);\n");
        fprintf (output, " return false;\n}\n");
      }
      else
      {
        if (errormsg_flag)
          fprintf (output, " if (cut_set) { level=lsave;set_errmsg(ntname);return false;}\n");
        if (stat_flag)
          fprintf (output, " if (cut_set) { stat_tab[%d] += stat_count-s1;return false;}\n", COUNT (ulhs));
        else
          fprintf (output, " if (cut_set) return false;\n");
      }
    }
  }
}


det_define_all_derived_affixes (alt)
int alt;
{
  int affix, mem;

  for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
    for (affix = AFFIXTREE (mem); affix != nil; affix = BROTHER (affix))
    {
      int term = SON (affix);

      if (NODENAME (affix) == derived)
        if ((APPLY_BOUND_AFFIX (term)) && (!IS_LEFTDEF (term)))
          fprintf (output, "  AFFIX  a_%s = (raf+%d);\n", REPR (term), ++oCount);
      if ((LATTICE (affix)) && (!IS_LEFTDEF (term)))
        fprintf (output, "  AFFIX  a_%s = (raf+%d);\n", REPR (term), ++oCount);

    }
}

det_initialize_all_derived_affixes (alt)
int alt;
{
  int affix, mem;

  for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
    for (affix = AFFIXTREE (mem); affix != nil; affix = BROTHER (affix))
    {
      int term = SON (affix);

      if (NODENAME (affix) == derived)
      {
        if ((IS_LEFTDEF (term)) && (NODENAME (term) == affixnt))
        {
          if (ISNT_DONTCARE (term))
            fprintf (output, "   A_%ld->t = undefined;\n", LEFTDEF (term));
        }
        else
          fprintf (output, "   a_%s->t = undefined;\n", REPR (term));
      }
    }
}


det_nice_lift_tree (lt, rt)
int lt, rt;
{
  int term;

  if (lt == rt)
    return;
  term = BROTHER (lt);
  if (term == rt)
  {
    if (NODENAME (lt) == affixtm)
    {
      aCount += 1;
      fprintf (output, "(raf+%d)->t = \"%s\",(raf+%d)->l = nil,(raf+%d)->r = nil,", aCount, REPR (lt), aCount, aCount);
    }
    return;
  }
  else if ((BROTHER (term) == rt) && (NODENAME (term) != affixtm))
  {
    aCount += 1;
    if (NODENAME (lt) == affixtm)
    {
      if (IS_LEFTDEF (term))
        fprintf (output,
                 "(raf+%d)->t = \"%s\",(raf+%d)->l = nil,(raf+%d)->r = A_%ld,",
                 aCount, REPR (lt), aCount, aCount, LEFTDEF (term));
      else if (META (term))
        fprintf (output, "(raf+%d)->t = \"%s\",(raf+%d)->l = nil,(raf+%d)->r = &a_%s,",
                 aCount, REPR (lt), aCount, aCount, REPR (term));
      else
        fprintf (output, "(raf+%d)->t = \"%s\",(raf+%d)->l = nil,(raf+%d)->r = a_%s,",
                 aCount, REPR (lt), aCount, aCount, REPR (term));
    }
    else if (IS_LEFTDEF (lt))
    {
      if (IS_LEFTDEF (term))
        fprintf (output, "(raf+%d)->t = empty,(raf+%d)->l = A_%ld,(raf+%d)->r = A_%d,",
                 aCount, aCount, LEFTDEF (lt), aCount, LEFTDEF (term));
      else if (META (term))
        fprintf (output, "(raf+%d)->t = empty,(raf+%d)->l = A_%d,(raf+%d)->r = &a_%s,",
                 aCount, aCount, LEFTDEF (lt), aCount, REPR (term));
      else
        fprintf (output, "(raf+%d)->t = empty,(raf+%d)->l = A_%ld,(raf+%d)->r = a_%s,",
                 aCount, aCount, LEFTDEF (lt), aCount, REPR (term));
    }
    else if (META (lt))
    {
      if (IS_LEFTDEF (term))
        fprintf (output, "(raf+%d)->t = empty,(raf+%d)->l = &a_%s,(raf+%d)->r = A_%ld,",
                 aCount, aCount, REPR (lt), aCount, LEFTDEF (term));
      else if (META (term))
        fprintf (output, "(raf+%d)->t = empty,(raf+%d)->l = &a_%s,(raf+%d)->r = &a_%s,",
                 aCount, aCount, REPR (lt), aCount, REPR (term));
      else
        fprintf (output, "(raf+%d)->t = empty,(raf+%d)->l = &a_%s,(raf+%d)->r = a_%s,",
                 aCount, aCount, REPR (lt), aCount, REPR (term));
    }
    else if (IS_LEFTDEF (term))
      fprintf (output, "(raf+%d)->t = empty,(raf+%d)->l = a_%s,(raf+%d)->r = A_%ld,",
               aCount, aCount, REPR (lt), aCount, LEFTDEF (term));
    else if (META (term))
      fprintf (output, "(raf+%d)->t = empty,(raf+%d)->l = a_%s,(raf+%d)->r = &a_%s,",
               aCount, aCount, REPR (lt), aCount, REPR (term));
    else
      fprintf (output, "(raf+%d)->t = empty,(raf+%d)->l = a_%s,(raf+%d)->r = a_%s,",
               aCount, aCount, REPR (lt), aCount, REPR (term));
    return;
  }
  if (NODENAME (lt) != affixtm)
  {
    aCount += 1;
    if (IS_LEFTDEF (lt))
      fprintf (output, "(raf+%d)->t = empty,(raf+%d)->l = A_%ld,(raf+%d)->r = (raf+%d),",
               aCount, aCount, LEFTDEF (lt), aCount, aCount + 1);
    else if (META (lt))
      fprintf (output, "(raf+%d)->t = empty,(raf+%d)->l = /* */ &a_%s,(raf+%d)->r = (raf+%d),",
               aCount, aCount, REPR (lt), aCount, aCount + 1);
    else
      fprintf (output, "(raf+%d)->t = empty,(raf+%d)->l = a_%s,(raf+%d)->r = (raf+%d),",
               aCount, aCount, REPR (lt), aCount, aCount + 1);
    det_nice_lift_tree (BROTHER (lt), rt);
  }
  else
  {
    aCount += 1;
    fprintf (output, "(raf+%d)->t = \"%s\",(raf+%d)->l = nil,(raf+%d)->r = (raf+%d),",
             aCount, REPR (lt), aCount, aCount, aCount + 1);
    det_nice_lift_tree (BROTHER (lt), rt);
  }
}


tail_recursion_elm_mem_affix_code (affix)
int affix;
{

  int position;
  for (position = 0; affix != nil; affix = BROTHER (affix), ++position)
  {
    int term = SON (affix);

    if (NODENAME (affix) == inherited)
    {
      if (BROTHER (term) != nil)
      {
        int this_a_count = aCount + 1;

        fprintf (output, " A_%d = (", position);
        det_nice_lift_tree (term, nil);
        fprintf (output, "  (raf+%d));", this_a_count);
      }
      else if (NODENAME (term) == affixtm)
      {
        aCount += 1;
        fprintf (output,
                 " A_%d =  ((raf+%d)->t = \"%s\", (raf+%d)->l = nil, (raf+%d)->r = nil,(raf+%d));",
                 position, aCount, REPR (term), aCount, aCount, aCount);
      }
      else if (IS_LEFTDEF (term))
      {
        if (position != LEFTDEF (term))
          fprintf (output, "  A_%d = A_%ld;", position, LEFTDEF (term));
      }
      else if (META (term))
        fprintf (output, "  A_%d = & a_%s;", position, REPR (term));
      else
        fprintf (output, "  A_%d = a_%s;", position, REPR (term));
    }
    else if (NODENAME (affix) == derived)
    {
      if (IS_LEFTDEF (term))
      {
        if (position != LEFTDEF (term))
          fprintf (output, "  A_%d = A_%ld;", position, LEFTDEF (term));
      }
      else
        fprintf (output, " A_%d =  a_%s;", position, REPR (term));
    }
  }
}

det_code_lifted_affixes (affix)
int affix;
{
  fprintf (output, "(");
  for (; affix != nil; affix = BROTHER (affix))
  {
    int term = SON (affix);

    if (NODENAME (affix) == inherited)
    {
      if (BROTHER (term) != nil)
      {
        int this_a_count = aCount + 1;

        fprintf (output, "(");
        det_nice_lift_tree (term, nil);
        fprintf (output, "  (raf+%d))", this_a_count);
      }
      else if (NODENAME (term) == affixtm)
      {
        aCount += 1;
        fprintf (output,
                 "  ((raf+%d)->t = \"%s\", (raf+%d)->l = nil, (raf+%d)->r = nil,(raf+%d))",
                 aCount, REPR (term), aCount, aCount, aCount);
      }
      else if (IS_LEFTDEF (term))
        fprintf (output, "  A_%ld", LEFTDEF (term));
      else if (META (term))
        fprintf (output, "  & a_%s", REPR (term));
      else
        fprintf (output, "  a_%s", REPR (term));
    }
    else
    {
      if (IS_LEFTDEF (term))
        fprintf (output, "  A_%ld", LEFTDEF (term));
      else if (META (term))
        fprintf (output, "  & a_%s", REPR (term));
      else
        fprintf (output, "  a_%s", REPR (term));
    }
    if (BROTHER (affix) != nil)
      fprintf (output, ",");
  }
  fprintf (output, ")");
}


det_result_code (alt)
int alt;
{
  int affix, count = 0;

  for (affix = AFFIXDEF (alt); affix != nil; affix = BROTHER (affix))
  {
    if (NODENAME (affix) == derived)
    {
      int term = SON (affix);

      if (BROTHER (term) == nil)
      {
        if (NODENAME (term) == affixtm)
          fprintf (output, "  A_%d->t = \"%s\"; A_%d->l = nil; A_%d->r = nil;\n", count, REPR (term), count, count);
        else if (IS_LEFTDEF (term) && LEFTDEF(term) != count)
          fprintf (output, "MAKE(A_%d,A_%ld);\n ", count, LEFTDEF (term));
        else if (FREE_AFFIX (term))
        {
#ifndef NO_COPY_STRUCT
          fprintf (output, "  *A_%d  = a_%s;", count, REPR (term));
#else
          fprintf (output, "  A_%d->t = a_%s.t;", count, REPR (term));
          fprintf (output, "A_%d->r = a_%s.r;", count, REPR (term));
          fprintf (output, "A_%d->l = a_%s.l;\n", count, REPR (term));
#endif
        }
      }
      else
      {
        int this_a_count = aCount + 1, bterm = BROTHER (term);

        if (BROTHER (bterm) == nil)
        {
          if ((NODENAME (term) == affixtm) && (NODENAME (bterm) == affixtm))
          {
            aCount += 1;
            fprintf (output, "  (raf+%d)->t = \"%s\";", this_a_count, REPR (bterm));
            fprintf (output, "(raf+%d)->l = nil;", this_a_count);
            fprintf (output, "(raf+%d)->r = nil;\n", this_a_count);
            fprintf (output, "  A_%d->t = \"%s\";", count, REPR (term));
            fprintf (output, "A_%d->r = (raf+%d);", count, this_a_count);
            fprintf (output, "A_%d->l = nil;", count);
          }
          else if ((NODENAME (term) != affixtm) && (NODENAME (bterm) == affixtm))
          {
            aCount += 1;
            fprintf (output, "  (raf+%d)->t = \"%s\";", this_a_count, REPR (bterm));
            fprintf (output, "(raf+%d)->l = nil;", this_a_count);
            fprintf (output, "(raf+%d)->r = nil;\n", this_a_count);
            if (IS_LEFTDEF (term))
              fprintf (output, "  A_%d->l = A_%ld;", count, LEFTDEF (term));
            else if (META (term))
              fprintf (output, "  A_%d->l = &a_%s;", count, REPR (term));
            else
              fprintf (output, "  A_%d->l = a_%s;", count, REPR (term));
            fprintf (output, "A_%d->r = (raf+%d);", count, this_a_count);
            fprintf (output, "A_%d->t = empty;\n", count);
          }
          else if ((NODENAME (term) == affixtm) && (NODENAME (bterm) != affixtm))
          {
            fprintf (output, "  A_%d->t = \"%s\";", count, REPR (term));
            if (IS_LEFTDEF (bterm))
              fprintf (output, "A_%d->r = A_%ld;", count, LEFTDEF (bterm));
            else if (META (bterm))
              fprintf (output, "  A_%d->r = &a_%s;", count, REPR (bterm));
            else
              fprintf (output, "A_%d->r = a_%s;", count, REPR (bterm));
            fprintf (output, "A_%d->l = nil;\n", count);
          }
          else
          {
            fprintf (output, "  A_%d->t = empty;", count);
            if (IS_LEFTDEF (term))
              fprintf (output, "A_%d->l = A_%ld;", count, LEFTDEF (term));
            else if (META (term))
              fprintf (output, "A_%d->l = &a_%s;", count, REPR (term));
            else
              fprintf (output, "A_%d->l = a_%s;", count, REPR (term));
            if (IS_LEFTDEF (bterm))
              fprintf (output, "A_%d->r = A_%d;\n", count, LEFTDEF (bterm));
            else if (META (bterm))
              fprintf (output, "A_%d->r = /* */ &a_%s;\n", count, REPR (bterm));
            else
              fprintf (output, "A_%d->r = a_%s;\n", count, REPR (bterm));
          }
        }
        else if (NODENAME (term) != affixtm)
        {
          det_nice_tree (bterm, nil);
          fprintf (output, "  A_%d->t = empty;", count);
          fprintf (output, "A_%d->r = (raf+%d);", count, this_a_count);
          if (IS_LEFTDEF (term))
            fprintf (output, "A_%d->l = A_%ld;\n", count, LEFTDEF (term));
          else if (META (term))
            fprintf (output, "A_%d->l = & a_%s;\n", count, REPR (term));
          else
            fprintf (output, "A_%d->l = a_%s;\n", count, REPR (term));
        }
        else
        {
          det_nice_tree (bterm, nil);
          fprintf (output, "  A_%d->t = \"%s\";", count, REPR (term));
          fprintf (output, "A_%d->r = (raf+%d);", count, this_a_count);
          fprintf (output, "A_%d->l = nil;\n", count);
        }
      }
    }
    else if (LATTICE (affix))
      if (NODENAME (SON (affix)) == affixtm)
      {
        int term = SON (affix);
        fprintf (output, " A_%d->t = (char *) 0X%x;\n", count, NODENAME (LATTICE_DEF (term)));
      }
    count += 1;
  }
}


det_alt_code (alt)
int alt;
{
  int mem;

  oCount = 0;
  det_alt_intermediate_defs (alt);
  aCount = 0;
  det_define_all_derived_affixes (alt);

  if (FLAG_MARKED (alt, freestacks_f))
  {
    fprintf (output, "  char *svc;\n");
    fprintf (output, "  AFFIX sva;\n");
  }
  det_initialize_all_derived_affixes (alt);
  if (oCount > 0)
  {
    fprintf (output, " if ((af+%d) > afx_top ) afx_overflow();\n", oCount);
    fprintf (output, " af = (raf+%d);\n", oCount);
  }
  if (FLAG_MARKED (alt, freestacks_f))
  {
    fprintf (output, "  svc = c; sva = af;\n");
  }
  det_lift_code (alt);
  fprintf (output, "   {\n");
  det_result_code (alt);

  /* tail recursion elimination */
  if (BROTHER (SON (ulhs)) != nil)
    if (FLAG_MARKED (alt, tail_recursion_opt_f))
    {
      if (FLAG_MARKED (alt, freestacks_f))
      {
        /* if no results are used affix and sting stacks can be popped to the beginning of the computation */
        fprintf (output, "   af = raf; c = rc; cut_set = 0; rip = ip;\n");
      }
      else
      {
        /* we can do tail recursion elimination but no affix and string space can be saved. */
        fprintf (output, "   raf = af; rc = c; cut_set = 0; rip = ip;\n");
      }
      tail_recursion_elm_mem_affix_code (AFFIXTREE (get_last_but_cut_mem (SON (alt))));
      fprintf (output, "   goto label_%d;\n    }\n", ulhs);
      return;
    }
  /* end tail recursion elimination */

  if (FLAG_MARKED (alt, freestacks_f))
  {
    fprintf (output, "  c = svc; af = sva;\n");
  }
  if (trace_flag)
    fprintf (output, "    pntname=pntnamesv;\n");
  fprintf (output, "   return true;\n    }\n");
}


det_lift_code (alt)
int alt;
{
  int mem = SON (alt);

  mems_lifted = false;
  if ((mem == nil) || (!lift_element (mem)))
    return;
  for (mem = SON (alt); (mem != nil) && (lift_element (mem)); mem = BROTHER (mem))
  {
    if (LKH (mem))
      fprintf (output, "   if ((lkh = ip))\n");
    if (TERMINAL (mem))
      det_code_lifted_terminal (mem, AFFIXTREE (mem));
    else
    {
      if (DEF (mem) == cut)
      {
        if (BROTHER (mem) != nil)
          fprintf (output, "   if ((cut_set = 1))\n");
      }
      else if (FLAG_MARKED (mem, tail_recursion_opt_f))
        return;                 /* this is a member optimized for tail recursion */
      else if (DEF (mem) == unpair || DEF (mem) == where || DEF (mem) == pair)
      {
        char uc[12];
        char *s, *d;

        for (s = REPR (mem), d = uc; *s;)
          *d++ = toupper ((unsigned char) *s++);
        *d = '\0';
        fprintf (output, "   if (%s", uc);
        det_code_lifted_affixes (AFFIXTREE (mem));
        fprintf (output, ")\n");
      }
      else if (!MARKED (DEF (mem), external))
      {
        fprintf (output, "   if ( D_%d", DEF (mem));
        det_code_lifted_affixes (AFFIXTREE (mem));
        fprintf (output, ")\n");
      }
      else
      {
        fprintf (output, "   if ( D%s", REPR (mem));
        det_code_lifted_affixes (AFFIXTREE (mem));
        fprintf (output, ")\n");
      }
    }
    if (LKH (mem))
      fprintf (output, "   if ((ip = lkh))\n");
  }
}


det_code_lifted_terminal (termi, afx)
int termi, afx;
{
  int trm;

  if (STRING (termi))
    return fprintf (output, "  if (Dterminal_(\"%s\"))\n", REPR (termi));
  trm = SON (afx);
  if (IS_DONTCARE (trm))
    return fprintf (output, "  if ( D_%ld())\n", termi);
  if (IS_LEFTDEF (trm))
    return fprintf (output, "  if ( D_%ld(A_%ld))\n", termi, LEFTDEF (trm));
  fprintf (output, "  if ( D_%ld(a_%s))\n", termi, REPR (trm));
}


det_alt_intermediate_defs (alt)
int alt;

{
  int mem = SON (alt), count;

  aCount = 0;
  highest_tempory = 0;
  result_intermediate_defs (alt);
  if (mem == nil);
  else if (BROTHER (mem) == nil)
    affix_intermediate_defs (mem);
  else
  {
    mems_intermediate_defs (BROTHER (mem));
    affix_intermediate_defs (mem);
  }
  if (highest_tempory > aCount)
    aCount = highest_tempory;
  oCount = aCount;
}

det_nice_tree (lt, rt)
int lt, rt;
{
  int term;

  if (lt == rt)
    return;
  term = BROTHER (lt);
  if (term == rt)
  {
    if (NODENAME (lt) == affixtm)
    {
      aCount += 1;
      fprintf (output, "  (raf+%d)->t = \"%s\"; (raf+%d)->l = nil; (raf+%d)->r = nil;\n",
               aCount, REPR (lt), aCount, aCount);
    }
    return;
  }
  else if ((BROTHER (term) == rt) && (NODENAME (term) != affixtm))
  {
    aCount += 1;
    if (NODENAME (lt) == affixtm)
    {
      if (IS_LEFTDEF (term))
        fprintf (output, "  (raf+%d)->t = \"%s\"; (raf+%d)->l = nil; (raf+%d)->r = A_%ld;\n",
                 aCount, REPR (lt), aCount, aCount, LEFTDEF (term));
      else if (META (term))
        fprintf (output, "  (raf+%d)->t = \"%s\"; (raf+%d)->l = nil; (raf+%d)->r = &a_%s;\n",
                 aCount, REPR (lt), aCount, aCount, REPR (term));
      else
        fprintf (output, "  (raf+%d)->t = \"%s\"; (raf+%d)->l = nil; (raf+%d)->r = a_%s;\n",
                 aCount, REPR (lt), aCount, aCount, REPR (term));
    }
    else if (IS_LEFTDEF (lt))
    {
      if (IS_LEFTDEF (term))
        fprintf (output, "  (raf+%d)->t = empty; (raf+%d)->l = A_%d; (raf+%d)->r = A_%ld;\n",
                 aCount, aCount, LEFTDEF (lt), aCount, LEFTDEF (term));
      else if (META (term))
        fprintf (output, "  (raf+%d)->t = empty; (raf+%d)->l = A_%d; (raf+%d)->r = &a_%s;\n",
                 aCount, aCount, LEFTDEF (lt), aCount, REPR (term));
      else
        fprintf (output, "  (raf+%d)->t = empty; (raf+%d)->l = A_%d; (raf+%d)->r = a_%s;\n",
                 aCount, aCount, LEFTDEF (lt), aCount, REPR (term));
    }
    else if (IS_LEFTDEF (term))
    {
      if (META (lt))
        fprintf (output, "  (raf+%d)->t = empty; (raf+%d)->l = &a_%s; (raf+%d)->r = A_%ld;\n",
                 aCount, aCount, REPR (lt), aCount, LEFTDEF (term));
      else
        fprintf (output, "  (raf+%d)->t = empty; (raf+%d)->l = a_%s; (raf+%d)->r = A_%ld;\n",
                 aCount, aCount, REPR (lt), aCount, LEFTDEF (term));
    }
    else if (META (term))
    {
      if (META (lt))
        fprintf (output, "  (raf+%d)->t = empty; (raf+%d)->l = &a_%s; (raf+%d)->r = &a_%s;\n",
                 aCount, aCount, REPR (lt), aCount, REPR (term));
      else
        fprintf (output, "  (raf+%d)->t = empty; (raf+%d)->l = a_%s; (raf+%d)->r = &a_%s;\n",
                 aCount, aCount, REPR (lt), aCount, REPR (term));
    }
    else
    {
      if (META (lt))
        fprintf (output, "  (raf+%d)->t = empty; (raf+%d)->l = &a_%s; (raf+%d)->r = a_%s;\n",
                 aCount, aCount, REPR (lt), aCount, REPR (term));
      else
        fprintf (output, "  (raf+%d)->t = empty; (raf+%d)->l = a_%s; (raf+%d)->r = a_%s;\n",
                 aCount, aCount, REPR (lt), aCount, REPR (term));
    }
    return;
  }
  if (NODENAME (lt) != affixtm)
  {
    aCount += 1;
    if (IS_LEFTDEF (lt))
      fprintf (output, "  (raf+%d)->t = empty;(raf+%d)->l = A_%ld;(raf+%d)->r = (raf+%d);\n",
               aCount, aCount, LEFTDEF (lt), aCount, aCount + 1);
    else if (META (lt))
      fprintf (output, "  (raf+%d)->t = empty;(raf+%d)->l = &a_%s;(raf+%d)->r = (raf+%d);\n",
               aCount, aCount, REPR (lt), aCount, aCount + 1);
    else
      fprintf (output, "  (raf+%d)->t = empty;(raf+%d)->l = a_%s;(raf+%d)->r = (raf+%d);\n",
               aCount, aCount, REPR (lt), aCount, aCount + 1);
    det_nice_tree (BROTHER (lt), rt);
  }
  else
  {
    aCount += 1;
    fprintf (output, "  (raf+%d)->t = \"%s\";(raf+%d)->l = nil;(raf+%d)->r = (raf+%d);\n",
             aCount, REPR (lt), aCount, aCount, aCount + 1);
    det_nice_tree (BROTHER (lt), rt);
  }
}


nondet_code ()
{
  if (MARKED (ulhs, external))
    fprintf (output, "void U%s () { register cont *rq = q;\n", REPR (ulhs));
  else
    fprintf (output, "static void U%d(){register cont *rq = q;\n", ulhs);
  fprintf (output, "char *rc =c , *rip = ip;\n");
  get_affixes (AFFIXDEF (SON (ulhs)), 0);
  qCount = nraffixes;
  if (MARKED (ulhs, external))
    fprintf (output, "  if (D%s(", REPR (ulhs));
  else
    fprintf (output, "  if (D_%d(", ulhs);
  det_call (AFFIXDEF (SON (ulhs)));
  if (memo_flag)
    fprintf (output, "  { q=rq+%d;(*(rq+%d)->q)();rq=q- %d;}\n", qCount - 1, qCount, qCount);
  else
    fprintf (output, "  { q = rq+%d;(*(rq+%d)->q)();}\n", qCount - 1, qCount);
  fprintf (output, "c  = rc; ip =rip;\n");
  push_affixes (AFFIXDEF (SON (ulhs)), 0);
  if (MARKED (ulhs, external))
    fprintf (output, "  (rq+1)-> q = U%s; q = rq+1; }\n", REPR (ulhs));
  else
    fprintf (output, "  (rq+1)-> q = U%d; q = rq+1; }\n", ulhs);
}

det_call (afx)
int afx;
{
  int affix = afx, count = 0;

  if (afx == nil)
  {
    fprintf (output, "))");
    return;
  }

  for (; affix != nil; affix = BROTHER (affix))
    if (BROTHER (affix) != nil)
      fprintf (output, "A_%d,", count++);
    else
      fprintf (output, "A_%d))\n", count++);
}

nondet_builtin_code ()
{
  int ulhs;
  for (ulhs = laststdpred; ulhs != nil; ulhs = BROTHER (ulhs))
  {
    fprintf (output, "void U%s () { register cont *rq = q;\n", REPR (ulhs));
    get_affixes (AFFIXDEF (SON (ulhs)), 0);
    qCount = nraffixes;
    fprintf (output, "  if ( D%s (", REPR (ulhs));
    det_call (AFFIXDEF (SON (ulhs)));
    fprintf (output, "  { \n");
    fprintf (output, "  q = rq+ %d;(*(rq+%d)->q)(); rq = q- %d;\n", qCount - 1, qCount, qCount);
    fprintf (output, "  } \n");
    push_affixes (AFFIXDEF (SON (ulhs)), 0);
    fprintf (output, "  (rq+1)-> q = U%s; q = rq+1; }\n\n", REPR (ulhs));
  }
}

statistic_table ()
{
  int ulhs, rule;
  NtCount = 0;
  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
    COUNT (rule) = NtCount++;
  if (!MARKED (root, docompile))
    fprintf (output, "extern ");
  fprintf (output, "int stat_tab[%d], stat_count;\n", NtCount);
  if (!MARKED (root, docompile))
    return;
  fprintf (output, "char * name_tab[%d] = { \"%s\"", NtCount, FREPR (root));
  for (ulhs = BROTHER (root); ulhs != laststdpred; ulhs = BROTHER (ulhs))
  {
    fprintf (output, ",\n  \"%s\"", FREPR (ulhs));
  }
  fprintf (output, "  \n};\n");

  fprintf (output, "print_stat() { \n int nr = 0;\n\
  for (;nr <= %d; nr++) {\n\
  printf(\"%%d.%%s->%%d\\n\",nr,name_tab[nr], stat_tab[nr]);}\n}", NtCount - 1);
}

save_lattice_afx ()
{
  int affix = AFFIXDEF (SON (ulhs)), count = 0;
  for (; affix != nil; count++, affix = BROTHER (affix))
    if (LATTICE (affix))
      fprintf (output, "  char *LA_%d = A_%d->t;\n", count, count);
}

restore_lattice_afx ()
{
  int affix = AFFIXDEF (SON (ulhs)), count = 0;
  for (; affix != nil; count++, affix = BROTHER (affix))
    if (LATTICE (affix))
      fprintf (output, "  A_%d->t = LA_%d;\n", count, count);
}


choice_switch_statement_coder ()
{
  int rule, alt, mem;
  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
    if (MARKED (rule, docompile))
      for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
        for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
        {
          char *r = REPR (mem);
          char choice_trace_txt[255];
          char *s;
          char *t;
          int rep, comp, noargs, always_succeeds;
          if (!TERMINAL (mem))
            continue;
          if (STRING (mem))
            continue;
          rep = EXCLAMATIONSTARCHOICE (mem) || EXCLAMATIONPLUSCHOICE (mem);

          comp = COMPLEMENT (mem);
          noargs = IS_DONTCARE (SON (AFFIXTREE (mem)));
          always_succeeds = EXCLAMATIONSTARCHOICE (mem);
          for (s = REPR (mem), t = &choice_trace_txt[0]; *s;)
          {
            *t++ = *s;
            if (*s == '\\')
            {
              if (*(s + 1) != '"')
                *t++ = *s;
            }
            s++;
          }
          *t = '\0';
          fprintf (output, "\n/* %s %c{%s}%c */",
                   FREPR (rule), comp ? '^' : ' ', REPR (mem), always_succeeds ? '*' : rep ? '+' : ' ');
          fprintf (output, "\nstatic inline int D_%ld", mem);
          if (!noargs)
            fprintf (output, "(A0)\nAFFIX A0;\n{");
          else
            fprintf (output, "()\n{");
          if (!always_succeeds)
            fprintf (output, "\n char   *sip = ip;");
          if (trace_flag)
          {
            fprintf (output, " char *pntnamesv = pntname; char *ntname = \"%c{%s}%c\";int ltr=level++;\n",
                     comp ? '^' : ' ', choice_trace_txt, always_succeeds ? '*' : rep ? '+' : ' ');
            if (!always_succeeds)
            {
              fprintf (output, "   begin1_trace(ntname);\n");
              fprintf (output, "   begin2_trace(ntname);\n");
            }
          }

          if (!noargs)
            fprintf (output, "\n A0->t = c; A0->l = nil;A0->r = nil;");
          if (rep)
            fprintf (output, "\nagain_%ld:", mem);
          fprintf (output, "\n  switch (*ip)\n  {");
          fprintf (output, "\n   case '\\0':");
          if (*r)
            fprintf (output, " break;");
          while (*r)
          {

            fprintf (output, "\n   case '");
            if (*r == '\\')
            {
              if (*++r != '"')
                fprintf (output, "\\");
            }
            else if (*r == '\'')
            {
              fprintf (output, "\\");
            }
            fprintf (output, "%c':", *r++);
          }
          if (comp)
          {
            if (noargs)
              fprintf (output, "break;\n   default: ip++;\n");
            else
              fprintf (output, "break;\n   default: *c++ = *ip++;\n");
            if (rep)
              fprintf (output, "    goto again_%d;\n", mem);
          }
          else
          {
            if (noargs)
              fprintf (output, "ip++;\n");
            else
              fprintf (output, "*c++ = *ip++;\n");
            if (rep)
              fprintf (output, "    goto again_%d;\n", mem);
            fprintf (output, "\n   default: break;\n");
          }
          fprintf (output, "\n  }");
          if (!always_succeeds)
          {
            if (trace_flag)
              fprintf (output, "\n if (ip == sip) { \n\
         endtrace(parsecount,pntnamesv,ntname); level=ltr; return false;\n }\n");
            else
              fprintf (output, "\n if (ip == sip) { \n return false;\n }\n");
          }
          if (!noargs)
            fprintf (output, "\n  *c++ = '\\0'; if (c > cstore_top) cstore_overflow();");
          fprintf (output, "\n if ((mip < ip) && (ip < limitip) && (ip > input)) \
            {\n  mip = ip;change_line_file = true;\n   %s}\n return true;\n}", always_succeeds ? "" : trace_flag ? "ntname=pntnamesv;\n  " : "");
        }
}
