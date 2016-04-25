
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 

/* file: coder 1 */
#include "gg1.h"
#include "gg2.h"
int nraffixes, alt_nr = 0, nr_of_mems = 0,
                                        no_inherited_afx,
                                        highest_tempory, memo_alt, mems_lifted, lift_count = 0, total_nr_of_mems = 0;

char *rrrt = "";

code ()
{
  alt_nr = 0, nr_of_mems = 0, lift_count = 0, total_nr_of_mems = 0;
  if (output != stdout)
  {
    if (syntax_flag && !separate_comp_flag)
    {
      outputfile[strlen (outputfile) - 3] = '.';
      outputfile[strlen (outputfile) - 2] = 'c';
      outputfile[strlen (outputfile) - 1] = '\0';
    }
    output = fopen (outputfile, "w");
  }
  if (output == NULL)
  {
    fprintf (stderr, "glammar: cannot write to %s. Bye!\n", outputfile);
    exit (1);
  }
  fprintf (output, "\n\n/*\n * Output generated for `%s'\n */\n\n", ex_filename);

/* lattice element names */
  if (max_group > 0)
    conv_table ();
  else
    fprintf (output, "char **groups; /* not used */\n");

#ifdef ATARI
  fprintf (output, "const long _stksize = %ldl;\n", runtime_stksize);
#endif

  if (separate_comp_flag && !MARKED (root, docompile))
    def_extern_meta_affixes ();
  else
  {
    fprintf (output, "int backtrace = %d;\n", backtrace_mark);
    def_meta_affixes ();
  }
  if (memo_flag)
  {
    if (nr_of_memo_alts)
      fprintf (output, "#define MEMOIZE %d\n", nr_of_memo_alts + 1);
    else
      fprintf (stderr, "no alternatives to MEMOIZE.\n");
  }
  UNSET (init_one_star, nondeterministic);
  SET (init_one_star, deterministic);
  SET (initmeta, deterministic);
  UNSET (initmeta, nondeterministic);
  start_code ();
  for (ulhs = root; ulhs != laststdpred; ulhs = BROTHER (ulhs))
  {
    if (!MARKED (ulhs, docompile))
    {
      if (memo_flag)
      {
        int alt;
        for (alt = SON (ulhs); alt != nil; alt = BROTHER (alt))
          if (MEMOIZE (alt))
            alt_nr++;
      }
    }
    else if (MARKED (ulhs, nondeterministic))
    {

      if (MARKED (ulhs, external))
        fprintf (output, "void U%s() {", REPR (ulhs));
      else
        fprintf (output, "static void U%d() {   /* %s */ \n", ulhs, FREPR (ulhs));
      if (trace_flag)
        fprintf (output, "  char *pntnamesv = pntname; char  *ntname = \"%s\"; int pct= parsecount;\n ", FREPR (ulhs));
      if (fprintf (output, "  register cont *rq = q;\n") == EOF)
      {
        fprintf (stderr, "glammar: Write failed (try again later)\n");
        exit (12);
      }
      fprintf (output, "  char *rc = c,*rip = ip;\n AFFIX raf = af;\n");
      fprintf (output, "  int ltr = level+1;\n");
      if ((memo_flag) && (memo_in_alt (SON (ulhs))))
        fprintf (output, "  MEMTEMPS;\n");
      if (lookahead_in_alt (SON (ulhs)))
        fprintf (output, "  char *lkh;\n");
      if (cut_in_alt (SON (ulhs)))
        fprintf (output, "  int cut_set = 0;\n");
      if (ambiguous_flag)
        fprintf (output, "  int aq=0,pq=parsecount;\n");
      alts_code (ulhs, SON (ulhs));
      if (backtrace_mark > 0)
        fprintf (output, " rcount -= 1;\n");
      if (trace_flag)
      {
        if (!MARKED (ulhs, external))
          fprintf (output, " (rq+1)->q = U%d;q= rq+1;level=ltr;endtrace(pct,pntnamesv, ntname);\n }\n", ulhs);
        else
          fprintf (output, " (rq+1)->q = U%s;q= rq+1;level=ltr;endtrace(pct,pntnamesv, ntname);\n }\n", REPR (ulhs));
      }
      else if (!MARKED (ulhs, external))
        fprintf (output, "  (rq+1)-> q = U%d; q = rq+1; }\n", ulhs);
      else
        fprintf (output, "  (rq+1)-> q = U%s; q = rq+1; }\n", REPR (ulhs));
    }
    else
    {
      det_rule_head ();
      det_rule_alts (SON (ulhs));
      det_rule_tail ();
      if ((!det_flag) || (ulhs == root))
        nondet_code ();
    }
  }
  if (verbose_flag)
  {
    fprintf (stderr,
             " %d predicate calls changed to function calls (on total of %d) (%.1f%%)\n", lift_count, total_nr_of_mems,
             (total_nr_of_mems != 0) ? 100 * (float) lift_count / (float) total_nr_of_mems : (float) 0);
  }
  fclose (output);
}


start_code ()
{

  if (!MARKED (root, external))
    fprintf (output, "static void U%d();\n", root);
  else
    fprintf (output, "void U%s();\n", REPR (root));

  if (memo_flag)
    fprintf (output, "#define INPUT_MEMO_SIZE %ld\n", runtime_input_size / (8 * sizeof (int)));

  code_includes ();



  for (ulhs = laststdpred; ulhs != nil; ulhs = BROTHER (ulhs))
  {
    if (!MARKED (ulhs, rule_used))
      continue;
    if (MARKED (ulhs, deterministic))
      fprintf (output, " int D%s();\n", REPR (ulhs));
    if (!det_flag)
      fprintf (output, " void U%s();\n", REPR (ulhs));
  }

  for (ulhs = BROTHER (root); ulhs != laststdpred; ulhs = BROTHER (ulhs))
    if (MARKED (ulhs, external))
    {
      if (MARKED (ulhs, deterministic))
        fprintf (output, " int D%s();\n", REPR (ulhs));
      if (!det_flag)
        fprintf (output, " void U%s();\n", REPR (ulhs));
    }
    else
    {
      if (MARKED (ulhs, deterministic))
        fprintf (output, " static int D_%d();\n", ulhs);
      if (!det_flag)
        fprintf (output, " static void U%d();\n", ulhs);
    }
  if (!separate_comp_flag)
  {
    main_code ();
  }
  else if (MARKED (root, docompile))
    main_code ();

  if (stat_flag)
    statistic_table ();
  choice_switch_statement_coder ();
}


cut_in_alt (alt)
int alt;
{
  int mem;

  for (; alt != nil; alt = BROTHER (alt))
    for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
      if (DEF (mem) == cut)
        return true;
  return false;
}


cut_in_mem (mem)
int mem;
{
  for (; mem != nil; mem = BROTHER (mem))
    if (DEF (mem) == cut)
      return true;
  return false;
}


lookahead_in_alt (alt)
int alt;
{
  int mem;

  for (; alt != nil; alt = BROTHER (alt))
    for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
      if (LKH (mem))
        return true;
  return false;
}



memo_in_alt (alt)
int alt;
{
  for (; alt != nil; alt = BROTHER (alt))
    if (MEMOIZE (alt))
      return true;
  return false;
}



lifted_mem_in_alt (alt)
int alt;
{
  for (; alt != nil; alt = BROTHER (alt))
    if (lift_element (SON (alt)))
      return true;
  return false;
}



alts_code (rule, alt)
int rule, alt;
{
  int affix = (AFFIXDEF (alt)), c_in_mem, c_in_alt, memo_save;


  no_inherited_afx = true;
  c_in_alt = cut_in_alt (SON (ulhs));
  get_affixes (affix, 0);

/*  save_lattice_afx();  */
  if (backtrace_mark > 0)
    fprintf (output, " rcount += 1;\n");
  if (trace_flag)
    trace_code (MARKED (rule, built_aliased), affix);
  for (; alt != nil; alt = BROTHER (alt))
  {
    memo_save = DEF (alt);
    set_number_of_mems (SON (alt));
    c_in_mem = cut_in_mem (SON (alt));
    if (memo_flag)
      memo_alt = MEMOIZE (alt);
    if (memo_flag && memo_alt)
    {
      fprintf (output, "   if (SET(%d)) { \n", alt_nr);
    }
    else
      fprintf (output, "  {\n");
    alt_code (alt);
    fprintf (output, "  }\n");
    DEF (alt) = memo_save;
    if (ambiguous_flag)
      fprintf (output, "  ambiguous_trace(&pq,&aq,\"%s\");\n", REPR (rule));
    fprintf (output, "   c = rc; ip = rip ; af = raf; \n");
    if (trace_flag)
      fprintf (output, "  level = ltr;\n");
    /* restore_lattice_afx(); */
    if (c_in_mem)
      fprintf (output, "  if (cut_set) goto done;\n");
  }
  if (c_in_alt)
    fprintf (output, "done:\n");
  push_affixes (affix, 0);

}


set_number_of_mems (mem)
int mem;
{
  for (nr_of_mems = 0; mem != nil; mem = BROTHER (mem))
    nr_of_mems += 1;
  total_nr_of_mems += nr_of_mems;
}


get_affixes (affix, count)
int affix, count;
{
  if (affix == nil)
  {
    nraffixes = -count;
    return;
  }
  get_affixes (BROTHER (affix), count + 1);
  if (NODENAME (affix) == inherited)
    no_inherited_afx = false;
  if (count > 4)
    fprintf (output, "  AFFIX A_%d = (rq+%d)-> a;\n", count, count + nraffixes + 1);
  else
    fprintf (output, "  register AFFIX A_%d = (rq+%d)-> a;\n", count, count + nraffixes + 1);
}


push_affixes (affix, count)
int affix, count;
{
  if (affix == nil)
    return;
  fprintf (output, "  (rq+%d)->a = A_%d;\n", nraffixes + count + 1, count);
  push_affixes (BROTHER (affix), count + 1);
}


trace_code (built_als, affix)
int affix;
{
  int count = 0;

  fprintf (output, "   begin1_trace(ntname);\n");
  for (; affix != nil; affix = BROTHER (affix))
  {
    if (NODENAME (affix) == inherited)
      fprintf (output, "  trace_affix(%d,A_%d);\n", built_als ? 1 : 0, count);
    count += 1;
  }
  fprintf (output, "   begin2_trace();\n");

}


define_all_derived_affixes (alt)
int alt;
{
  int affix, mem;

  for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
    for (affix = AFFIXTREE (mem); affix != nil; affix = BROTHER (affix))
    {
      int term = SON (affix);

      if (NODENAME (affix) == derived)
        if ((APPLY_BOUND_AFFIX (term)) && (!IS_LEFTDEF (term)))
          fprintf (output, "  affix a_%s;\n", REPR (term));
      if ((LATTICE (affix)) && (!IS_LEFTDEF (term)))
        fprintf (output, "  affix a_%s ;\n", REPR (term));
    }
}


initialize_all_derived_affixes (alt)
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
            fprintf (output, "   A_%ld -> t = undefined;\n", LEFTDEF (term));
        }
        else
          fprintf (output, "   a_%s.t = undefined;\n", REPR (term));
      }
    }
}


nice_tree (lt, rt)
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
      fprintf (output, "  T%d.t = \"%s\"; T%d.l = nil; T%d.r = nil;\n", aCount, REPR (lt), aCount, aCount);
    }
    return;
  }
  else if ((BROTHER (term) == rt) && (NODENAME (term) != affixtm))
  {
    aCount += 1;
    if (NODENAME (lt) == affixtm)
    {
      if (IS_LEFTDEF (term))
        fprintf (output, "  T%d.t = \"%s\"; T%d.l = nil; T%d.r = A_%ld;\n",
                 aCount, REPR (lt), aCount, aCount, LEFTDEF (term));
      else
        fprintf (output, "  T%d.t = \"%s\"; T%d.l = nil; T%d.r = &a_%s;\n",
                 aCount, REPR (lt), aCount, aCount, REPR (term));
    }
    else if (IS_LEFTDEF (lt))
    {
      if (IS_LEFTDEF (term))
        fprintf (output, "  T%d.t = empty; T%d.l = A_%ld; T%d.r =  A_%ld;\n",
                 aCount, aCount, LEFTDEF (lt), aCount, LEFTDEF (term));
      else
        fprintf (output, "  T%d.t = empty; T%d.l = A_%ld; T%d.r = &a_%s;\n",
                 aCount, aCount, LEFTDEF (lt), aCount, REPR (term));
    }
    else if (IS_LEFTDEF (term))
      fprintf (output, "  T%d.t = empty; T%d.l = &a_%s; T%d.r = A_%ld;\n",
               aCount, aCount, REPR (lt), aCount, LEFTDEF (term));
    else
      fprintf (output, "  T%d.t = empty; T%d.l = &a_%s; T%d.r = &a_%s;\n",
               aCount, aCount, REPR (lt), aCount, REPR (term));
    return;
  }
  if (NODENAME (lt) != affixtm)
  {
    aCount += 1;
    if (IS_LEFTDEF (lt))
      fprintf (output, "  T%d.t = empty;T%d.l = A_%ld;T%d.r = &T%d;\n",
               aCount, aCount, LEFTDEF (lt), aCount, aCount + 1);
    else
      fprintf (output, "  T%d.t = empty;T%d.l = &a_%s;T%d.r = &T%d;\n", aCount, aCount, REPR (lt), aCount, aCount + 1);
    nice_tree (BROTHER (lt), rt);
  }
  else
  {
    aCount += 1;
    fprintf (output, "  T%d.t = \"%s\";T%d.l = nil;T%d.r = &T%d;\n", aCount, REPR (lt), aCount, aCount, aCount + 1);
    nice_tree (BROTHER (lt), rt);
  }
}


nice_lift_tree (lt, rt)
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
      fprintf (output, "T%d.t = \"%s\",T%d.l = nil,T%d.r = nil,", aCount, REPR (lt), aCount, aCount);
    }
    return;
  }
  else if ((BROTHER (term) == rt) && (NODENAME (term) != affixtm))
  {
    aCount += 1;
    if (NODENAME (lt) == affixtm)
    {
      if (IS_LEFTDEF (term))
        fprintf (output, "T%d.t = \"%s\",T%d.l = nil,T%d.r = A_%ld,",
                 aCount, REPR (lt), aCount, aCount, LEFTDEF (term));
      else
        fprintf (output, "T%d.t = \"%s\",T%d.l = nil,T%d.r = &a_%s,", aCount, REPR (lt), aCount, aCount, REPR (term));
    }
    else if (IS_LEFTDEF (lt))
    {
      if (IS_LEFTDEF (term))
        fprintf (output, "T%d.t = empty,T%d.l = A_%ld,T%d.r = A_%ld,",
                 aCount, aCount, LEFTDEF (lt), aCount, LEFTDEF (term));
      else
        fprintf (output, "T%d.t = empty,T%d.l = A_%ld,T%d.r = &a_%s,",
                 aCount, aCount, LEFTDEF (lt), aCount, REPR (term));
    }
    else if (IS_LEFTDEF (term))
      fprintf (output, "T%d.t = empty,T%d.l = &a_%s,T%d.r = A_%ld,", aCount, aCount, REPR (lt), aCount, LEFTDEF (term));
    else
      fprintf (output, "T%d.t = empty,T%d.l = &a_%s,T%d.r = &a_%s,", aCount, aCount, REPR (lt), aCount, REPR (term));
    return;
  }
  if (NODENAME (lt) != affixtm)
  {
    aCount += 1;
    if (IS_LEFTDEF (lt))
      fprintf (output, "T%d.t = empty,T%d.l = A_%ld,T%d.r = &T%d,", aCount, aCount, LEFTDEF (lt), aCount, aCount + 1);
    else
      fprintf (output, "T%d.t = empty,T%d.l = &a_%s,T%d.r = &T%d,", aCount, aCount, REPR (lt), aCount, aCount + 1);
    nice_lift_tree (BROTHER (lt), rt);
  }
  else
  {
    aCount += 1;
    fprintf (output, "T%d.t = \"%s\",T%d.l = nil,T%d.r = &T%d,", aCount, REPR (lt), aCount, aCount, aCount + 1);
    nice_lift_tree (BROTHER (lt), rt);
  }
}


affix_code (mem)
int mem;
{
  int affix;

  for (affix = AFFIXTREE (mem); affix != nil; affix = BROTHER (affix))
  {
    int term = SON (affix);

    if (NODENAME (affix) == inherited)
    {
      if (BROTHER (term) != nil)
      {
        int this_a_count = aCount + 1;

        nice_tree (term, nil);
        fprintf (output, "  (rq+%d)->a = &T%d;\n", qCount + 1, this_a_count);
      }
      else if (NODENAME (term) == affixtm)
      {
        aCount += 1;
        fprintf (output, "  T%d.t = \"%s\"; T%d.l = nil; T%d.r = nil;\n", aCount, REPR (term), aCount, aCount);
        fprintf (output, "  (rq+%d)->a = &T%d;\n", qCount + 1, aCount);
      }
      else if (IS_LEFTDEF (term))
        fprintf (output, "  (rq+%d)->a = A_%ld;\n", qCount + 1, LEFTDEF (term));
      else
        fprintf (output, "  (rq+%d)->a = &a_%s;\n", qCount + 1, REPR (term));
    }
    else
    {
      if (IS_LEFTDEF (term))
        fprintf (output, "  (rq+%d)->a = A_%ld;\n", qCount + 1, LEFTDEF (term));
      else
        fprintf (output, "  (rq+%d)->a = &a_%s;\n", qCount + 1, REPR (term));
    }
    qCount += 1;
  }
}


code_lifted_affixes (affix)
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
        nice_lift_tree (term, nil);
        fprintf (output, "  &T%d)", this_a_count);
      }
      else if (NODENAME (term) == affixtm)
      {
        aCount += 1;
        fprintf (output, "  (T%d.t = \"%s\", T%d.l = nil, T%d.r = nil,&T%d)",
                 aCount, REPR (term), aCount, aCount, aCount);
      }
      else if (IS_LEFTDEF (term))
        fprintf (output, "  A_%ld", LEFTDEF (term));
      else
        fprintf (output, "  &a_%s", REPR (term));
    }
    else
    {
      if (IS_LEFTDEF (term))
        fprintf (output, "  A_%ld", LEFTDEF (term));
      else
        fprintf (output, "  &a_%s", REPR (term));
    }
    if (BROTHER (affix) != nil)
      fprintf (output, ",");
  }
  fprintf (output, ")");
}


result_code (alt)
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
          fprintf (output,
                   "  A_%d -> t = \"%s\"; A_%d -> l = nil; A_%d -> r = nil;\n", count, REPR (term), count, count);
        else if (IS_LEFTDEF (term) && LEFTDEF(term) != count)
          fprintf (output, "MAKE(A_%d,A_%ld);\n ", count, LEFTDEF (term));
        else if (FREE_AFFIX (term))
        {
#ifndef NO_COPY_STRUCT
          fprintf (output, "  *A_%d  = a_%s;", count, REPR (term));
#else
          fprintf (output, "  A_%d -> t = a_%s.t;", count, REPR (term));
          fprintf (output, "A_%d -> r = nil;", count);
          fprintf (output, "A_%d -> l = nil;\n", count);
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
            fprintf (output, "  T%d.t = \"%s\";", this_a_count, REPR (bterm));
            fprintf (output, "T%d.l = nil;", this_a_count);
            fprintf (output, "T%d.r = nil;\n", this_a_count);
            fprintf (output, "  A_%d -> t = \"%s\";", count, REPR (term));
            fprintf (output, "A_%d -> r = &T%d;", count, this_a_count);
            fprintf (output, "A_%d -> l = nil;", count);
          }
          else if ((NODENAME (term) != affixtm) && (NODENAME (bterm) == affixtm))
          {
            aCount += 1;
            fprintf (output, "  T%d.t = \"%s\";", this_a_count, REPR (bterm));
            fprintf (output, "T%d.l = nil;", this_a_count);
            fprintf (output, "T%d.r = nil;\n", this_a_count);
            if (IS_LEFTDEF (term))
              fprintf (output, "  A_%d -> l = A_%ld;", count, LEFTDEF (term));
            else
              fprintf (output, "  A_%d -> l = &a_%s;", count, REPR (term));
            fprintf (output, "A_%d -> r = &T%d;", count, this_a_count);
            fprintf (output, "A_%d -> t = empty;\n", count);
          }
          else if ((NODENAME (term) == affixtm) && (NODENAME (bterm) != affixtm))
          {
            fprintf (output, "  A_%d -> t = \"%s\";", count, REPR (term));
            if (IS_LEFTDEF (bterm))
              fprintf (output, "A_%d -> r = A_%ld;", count, LEFTDEF (bterm));
            else
              fprintf (output, "A_%d -> r = &a_%s;", count, REPR (bterm));
            fprintf (output, "A_%d -> l = nil;\n", count);
          }
          else
          {
            fprintf (output, "  A_%d -> t = empty;", count);
            if (IS_LEFTDEF (term))
              fprintf (output, "A_%d -> l = A_%ld;", count, LEFTDEF (term));
            else
              fprintf (output, "A_%d -> l = &a_%s;", count, REPR (term));
            if (IS_LEFTDEF (bterm))
              fprintf (output, "A_%d -> r = A_%ld;\n", count, LEFTDEF (bterm));
            else
              fprintf (output, "A_%d -> r = &a_%s;\n", count, REPR (bterm));
          }
        }
        else if (NODENAME (term) != affixtm)
        {
          nice_tree (bterm, nil);
          fprintf (output, "  A_%d -> t = empty;", count);
          fprintf (output, "A_%d -> r = &T%d;", count, this_a_count);
          if (IS_LEFTDEF (term))
            fprintf (output, "A_%d -> l = A_%ld;\n", count, LEFTDEF (term));
          else
            fprintf (output, "A_%d -> l = &a_%s;\n", count, REPR (term));
        }
        else
        {
          nice_tree (bterm, nil);
          fprintf (output, "  A_%d -> t = \"%s\";", count, REPR (term));
          fprintf (output, "A_%d -> r = &T%d;", count, this_a_count);
          fprintf (output, "A_%d -> l = nil;\n", count);
        }
      }
    }
    else if (LATTICE (affix))
      if (NODENAME (SON (affix)) == affixtm)
      {
        int term = SON (affix);
        fprintf (output, " A_%d ->t = (char *) 0X%x;\n", count, NODENAME (LATTICE_DEF (term)));

      }
    count += 1;
  }
}

code_qstack_check (mem)
int mem;
{
  int afx, q_top = qCount;

  for (; mem != nil; mem = BROTHER (mem))
    if (!lift_element (mem))
      break;

  for (; mem != nil; mem = BROTHER (mem))
    if (STRING (mem))
      q_top += 1;
    else if (TERMINAL (mem))
      q_top += 2;
    else
    {
      for (afx = AFFIXTREE (mem); afx != nil; afx = BROTHER (afx))
        q_top += 1;
      q_top += 1;
    }
  if (q_top > 0)
    fprintf (output, " if (q+%d > q_top) need_more_qstack();\n", q_top);
}

alt_code (alt)
int alt;
{
  int mem;

  qCount = nraffixes;
  aCount = 0;
  alt_intermediate_defs (alt);
  aCount = 0;
  define_all_derived_affixes (alt);
  initialize_all_derived_affixes (alt);
  code_qstack_check (SON (alt));
  lift_code (alt);
  mem = SON (alt);
  result_code (alt);
  if (mem == nil)
    fprintf (output, "  q = rq+%d;(*(rq+%d)->q)();\n", qCount - 1, qCount);
  else if (BROTHER (mem) == nil)
  {

/*
    if ((memo_flag) && (memo_alt))
      fprintf(output, "   (rq+%d) -> q = uskip_;\n", qCount);
*/
    if (LKH (mem))
    {
      lkh_epiloque ();
      fprintf (output, "  lkh = ip;\n");
    }
    if (TERMINAL (mem))
    {
      if (backtrace_mark > 0)
        fprintf (output, "  (rq+%d)->a = A_0;\n", ++qCount);
      affix_code (mem);
      code_first_terminal (mem);
      qCount += 2;
    }
    else if (!MARKED (DEF (mem), external))
    {
      affix_code (mem);
      fprintf (output, "  q = rq+%d;U%d();\n", qCount, DEF (mem));
      qCount += 1;
    }
    else
    {
      affix_code (mem);
      if (DEF (mem) == cut)
        fprintf (output, "   (rq+%d) -> i = &cut_set;\n", ++qCount);
      fprintf (output, "  q = rq+%d; U%s();\n", qCount, REPR (mem));
      qCount += 1;
    }
  }
  else
  {
    mems_code (alt, BROTHER (mem));
    if (LKH (mem))
    {
      lkh_epiloque ();
      fprintf (output, "  lkh = ip;\n");
    }
    if (TERMINAL (mem))
    {
      if (backtrace_mark > 0)
        fprintf (output, "  (rq+%d)->a = A_0;\n", ++qCount);
      affix_code (mem);
      code_first_terminal (mem);
      qCount += 2;
    }
    else if (!MARKED (DEF (mem), external))
    {
      affix_code (mem);
      fprintf (output, "  q = rq+%d;U%d();\n", qCount, DEF (mem));
      qCount += 1;
    }
    else
    {
      affix_code (mem);
      if (DEF (mem) == cut)
        fprintf (output, "   (rq+%d) -> i = &cut_set;\n", ++qCount);
      if (DEF (mem) == skip)
      {
        fprintf (output, "  /* here comes that skip */\n");
        DEF (alt) = qCount;
      }
      fprintf (output, "  q = rq+%d;U%s();\n", qCount, REPR (mem));
      qCount += 1;
    }
  }
  if (memo_flag && memo_alt)
  {
    fprintf (output,
             "  rq = q - %d;\n   if ((rq+ %d)->q == uskip_) { CLEAR(%d); rq -=1;}\n", qCount - 1, DEF (alt), alt_nr++);
  }
  else if (memo_flag)
    fprintf (output, "  rq = q - %d; \n", qCount);
  if (mems_lifted)
    fprintf (output, "   }\n");
}


lift_code (alt)
int alt;
{
  int mem = SON (alt);

  mems_lifted = false;
  if ((mem == nil) || (!lift_element (mem)))
    return;
  mems_lifted = true;
  for (mem = SON (alt); (mem != nil) && (lift_element (mem)); mem = BROTHER (mem))
  {
    lift_count += 1;
    if (LKH (mem))
      fprintf (output, "   if ((lkh = ip))\n");
    if (TERMINAL (mem))
      code_lifted_terminal (mem, AFFIXTREE (mem));
    else
    {
      if (DEF (mem) == cut)
        fprintf (output, "   if ((cut_set = 1))\n");
      else if (!MARKED (DEF (mem), external))
      {
        fprintf (output, "   if ( D_%d", DEF (mem));
        code_lifted_affixes (AFFIXTREE (mem));
        fprintf (output, ")\n");
      }
      else
      {
        fprintf (output, "   if ( D%s", REPR (mem));
        code_lifted_affixes (AFFIXTREE (mem));
        fprintf (output, ")\n");
      }
    }
    if (LKH (mem))
      fprintf (output, "   if ((ip = lkh))\n");
  }
  SON (alt) = mem;
  fprintf (output, "   {");
}


mems_code (alt, mem)
int alt, mem;
{
  if (mem != nil)
  {

    mems_code (alt, BROTHER (mem));
    if (LKH (mem))
      lkh_epiloque ();

    if (TERMINAL (mem))
    {
      if (backtrace_mark > 0)
        fprintf (output, "  (rq+%d)->a = A_0;\n", ++qCount);
      affix_code (mem);
      code_terminal (mem);
      qCount += 2;
    }
    else if (!MARKED (DEF (mem), external))
    {
      affix_code (mem);
      fprintf (output, "  (rq+%d)->q = U%d;\n", ++qCount, DEF (mem));
    }
    else
    {
      affix_code (mem);
      if (DEF (mem) == skip)
      {
        fprintf (output, "  /* here comes that skip */\n");
        DEF (alt) = qCount;
      }
      if (DEF (mem) == cut)
        fprintf (output, "   (rq+%d) -> i = &cut_set;\n", ++qCount);
      fprintf (output, "  (rq+%d)->q = U%s;\n", ++qCount, REPR (mem));
    }
    if (LKH (mem))
      lkh_proloque ();
  }
}


lkh_epiloque ()
{
  fprintf (output, "  (rq+%d)->l = &lkh;\n", ++qCount);
  fprintf (output, "  (rq+%d)->q = lkh_epiloque;\n", ++qCount);
}


lkh_proloque ()
{
  fprintf (output, "  (rq+%d)->l = &lkh;\n", ++qCount);
  fprintf (output, "  (rq+%d)->q = lkh_proloque;\n", ++qCount);
}


code_first_terminal (termi)
int termi;
{
  if (STRING (termi))
    fprintf (output, "  (rq+%d)->s= \"%s\"; q = rq +%d;%sUterminal_();\n", qCount + 1, REPR (termi), qCount + 1, rrrt);
  else if (COMPLEMENT (termi))
  {
    if (EXCLAMATIONSTARCHOICE (termi))
      fprintf (output,
               "  (rq+%d)->s =\"%s\";q = rq +%d;%sUx_star_ex_choice();\n", qCount + 1, REPR (termi), qCount + 1, rrrt);
    else if (EXCLAMATIONPLUSCHOICE (termi))
      fprintf (output,
               "  (rq+%d)->s =\"%s\";q = rq +%d;%sUx_plus_ex_choice();\n", qCount + 1, REPR (termi), qCount + 1, rrrt);
    else
      fprintf (output, "  (rq+%d)->s =\"%s\";q = rq +%d;%sUex_choice();\n", qCount + 1, REPR (termi), qCount + 1, rrrt);
  }
  else if (EXCLAMATIONSTARCHOICE (termi))
    fprintf (output,
             "  (rq+%d)->s =\"%s\";q = rq +%d;%sUx_star_choice();\n", qCount + 1, REPR (termi), qCount + 1, rrrt);
  else if (EXCLAMATIONPLUSCHOICE (termi))
    fprintf (output,
             "  (rq+%d)->s =\"%s\";q = rq +%d;%sUx_plus_choice();\n", qCount + 1, REPR (termi), qCount + 1, rrrt);
  else
    fprintf (output, "  (rq+%d)->s =\"%s\";q = rq +%d;%sUchoice_();\n", qCount + 1, REPR (termi), qCount + 1, rrrt);
}


code_terminal (termi)
int termi;
{
  if (STRING (termi))
    fprintf (output, "  (rq+%d)->s = \"%s\";(rq+%d)->q = %sUterminal_;\n", qCount + 1, REPR (termi), qCount + 2, rrrt);
  else if (COMPLEMENT (termi))
  {
    if (EXCLAMATIONSTARCHOICE (termi))
      fprintf (output,
               "  (rq+%d)->s = \"%s\";(rq+%d)->q = %sUx_star_ex_choice;\n", qCount + 1, REPR (termi), qCount + 2, rrrt);
    else if (EXCLAMATIONPLUSCHOICE (termi))
      fprintf (output,
               "  (rq+%d)->s = \"%s\";(rq+%d)->q = %sUx_plus_ex_choice;\n", qCount + 1, REPR (termi), qCount + 2, rrrt);
    else
      fprintf (output,
               "  (rq+%d)->s = \"%s\";(rq+%d)->q = %sUex_choice;\n", qCount + 1, REPR (termi), qCount + 2, rrrt);
  }
  else if (EXCLAMATIONSTARCHOICE (termi))
    fprintf (output,
             "  (rq+%d)->s = \"%s\";(rq+%d)->q = %sUx_star_choice;\n", qCount + 1, REPR (termi), qCount + 2, rrrt);
  else if (EXCLAMATIONPLUSCHOICE (termi))
    fprintf (output,
             "  (rq+%d)->s = \"%s\";(rq+%d)->q = %sUx_plus_choice;\n", qCount + 1, REPR (termi), qCount + 2, rrrt);
  else
    fprintf (output, "  (rq+%d)->s = \"%s\";(rq+%d)->q = %sUchoice_;\n", qCount + 1, REPR (termi), qCount + 2, rrrt);
}


code_lifted_terminal (termi, afx)
int termi, afx;
{
  int trm;

  if (STRING (termi))
  {
    fprintf (output, "  if (Dterminal_(\"%s\"))\n", REPR (termi));
    return;
  }
  trm = SON (afx);
  if (IS_LEFTDEF (trm))
  {
    if (COMPLEMENT (termi))
    {
      if (EXCLAMATIONSTARCHOICE (termi))
        fprintf (output, "  if ( Dx_star_ex_choice(\"%s\",A_%ld))\n", REPR (termi), LEFTDEF (trm));
      else if (EXCLAMATIONPLUSCHOICE (termi))
        fprintf (output, "  if (Dx_plus_ex_choice(\"%s\",A_%ld))\n", REPR (termi), LEFTDEF (trm));
      else
        fprintf (output, "  if (Dex_choice(\"%s\",A_%ld))\n", REPR (termi), LEFTDEF (trm));
    }
    else if (EXCLAMATIONSTARCHOICE (termi))
      fprintf (output, "  if (Dx_star_choice(\"%s\",A_%ld))\n", REPR (termi), LEFTDEF (trm));
    else if (EXCLAMATIONPLUSCHOICE (termi))
      fprintf (output, "  if (Dx_plus_choice(\"%s\",A_%ld))\n", REPR (termi), LEFTDEF (trm));
    else
      fprintf (output, "  if (Dchoice_(\"%s\",A_%ld))\n", REPR (termi), LEFTDEF (trm));
  }
  else
  {
    if (COMPLEMENT (termi))
    {
      if (EXCLAMATIONSTARCHOICE (termi))
        fprintf (output, "  if (Dx_star_ex_choice(\"%s\",&a_%s))\n", REPR (termi), REPR (trm));
      else if (EXCLAMATIONPLUSCHOICE (termi))
        fprintf (output, "  if (Dx_plus_ex_choice(\"%s\",&a_%s))\n", REPR (termi), REPR (trm));
      else
        fprintf (output, "  if (Dex_choice(\"%s\",&a_%s))\n", REPR (termi), REPR (trm));
    }
    else if (EXCLAMATIONSTARCHOICE (termi))
      fprintf (output, "  if (Dx_star_choice(\"%s\",&a_%s))\n", REPR (termi), REPR (trm));
    else if (EXCLAMATIONPLUSCHOICE (termi))
      fprintf (output, "  if (Dx_plus_choice(\"%s\",&a_%s))\n", REPR (termi), REPR (trm));
    else
      fprintf (output, "  if (Dchoice_(\"%s\",&a_%s))\n", REPR (termi), REPR (trm));
  }
}


count_intermediate_defs_in_tree (lt, rt)
int lt, rt;
{
  int term;

  if (lt == rt)
    return;
  term = BROTHER (lt);
  if (term == rt)
  {
    if (NODENAME (lt) == affixtm)
      aCount += 1;
    return;
  }
  else if ((BROTHER (term) == rt) && (NODENAME (term) != affixtm))
  {
    aCount += 1;
    return;
  }
  if (NODENAME (lt) != affixtm)
  {
    aCount += 1;
    highest_tempory = aCount + 1;
    count_intermediate_defs_in_tree (BROTHER (lt), rt);
  }
  else
  {
    aCount += 1;
    highest_tempory = aCount + 1;
    count_intermediate_defs_in_tree (BROTHER (lt), rt);
  }
}


affix_intermediate_defs (mem)
int mem;
{
  int affix;

  for (affix = AFFIXTREE (mem); affix != nil; affix = BROTHER (affix))
  {
    int term = SON (affix);

    if (NODENAME (affix) == inherited)
    {
      if (BROTHER (term) != nil)
        count_intermediate_defs_in_tree (term, nil);
      else if (NODENAME (term) == affixtm)
        aCount += 1;
    }
  }
}


result_intermediate_defs (alt)
int alt;
{
  int affix;

  for (affix = AFFIXDEF (alt); affix != nil; affix = BROTHER (affix))
  {
    if (NODENAME (affix) == derived)
    {
      int term = SON (affix), bterm = BROTHER (term);

      if (bterm == nil);
      else if (BROTHER (bterm) == nil)
      {
        if ((NODENAME (term) == affixtm) && (NODENAME (bterm) == affixtm))
          aCount += 1;
        else if ((NODENAME (term) != affixtm) && (NODENAME (bterm) == affixtm))
          aCount += 1;
      }
      else
        count_intermediate_defs_in_tree (bterm, nil);
    }
  }
}


alt_intermediate_defs (alt)
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
  if (aCount == 0);
  else if (aCount == 1)
    fprintf (output, "   affix T1;\n");
  else
  {
    fprintf (output, "   affix T1");
    for (count = 2; count != aCount + 1; count++)
      fprintf (output, ",T%d", count);
    fprintf (output, ";\n");
  }
}


mems_intermediate_defs (mem)
int mem;
{
  if (mem != nil)
  {
    mems_intermediate_defs (BROTHER (mem));
    affix_intermediate_defs (mem);
  }
}


def_extern_meta_affixes ()
{
  int mrule;
  fprintf (output, "typedef struct affix{char *t; struct affix *l; struct affix *r;} affix,*AFFIX ;\n");
  for (mrule = lastmetarule; mrule != nil; mrule = BROTHER (mrule))
    fprintf (output, "   extern affix a_%s;\n", REPR (mrule));
}

def_meta_affixes ()
{
  int mrule, prev = lastmetarule;

  fprintf (output, "typedef struct affix{char *t; struct affix *l; struct affix *r;} affix,*AFFIX ;\n");
  for (mrule = lastmetarule; mrule != nil; mrule = BROTHER (mrule))
  {
    int this;
    prev = mrule;
    if ((!separate_comp_flag) && (mrule > laststdmetarule) && 0)
      fprintf (output, "  static ");
    if (NODENAME (mrule) == meta_prod_rule)
      /* && (NODENAME(SON(mrule)) == affixtm)) */
      fprintf (output, " affix a_%s = {\"%s\",0,0};\n", REPR (mrule), REPR (SON (mrule)));
    else
      fprintf (output, " affix a_%s = {\"!@!\",0,0};\n", REPR (mrule));
  }
  fprintf (output, "AFFIX first_meta = &a_%s;\n", REPR (lastmetarule));
  fprintf (output, "AFFIX last_meta = &a_%s;\n", REPR (prev));
}

lift_element (mem)
int mem;
{
  if (mem == nil)
    return false;
  return ((TERMINAL (mem)) || (MARKED (DEF (mem), deterministic)));
}


main_code ()
{
  fprintf (output, "\n\
\n\
main (arg_count,arguments)\n\
int arg_count;\n\
char **arguments;\n\
{\n\
  affix r,e;\n\
  fla.t = fast_list_acces;\n\
  e.t = undefined;\n\
  r.t = e.t;\n\
  pntname = \"%s\"; \n\
  readinput(arg_count,arguments, %s);\n\
  cstore  = input + nrofchars+10;\n\
  c = cstore; \n\
  ct = cstore;\n\
  q = q_stack;\n\
  af = affix_heap; \n", FREPR (root), start_predicate_flag ? "false" : "true");
  if (MARKED (init_one_star, deterministic))
    fprintf (output, " DInit_One_Star();\n");

  fprintf (output, "\
  q -> a =  &r;\n\
  (++q) -> q = found; \n");

  if (!MARKED (init_one_star, deterministic))
    fprintf (output, "  (++q) -> q = UInit_One_Star;\n");

  fprintf (output, "\
  mip = ip;\n\
  argc = arg_count; arg_v = arguments;\n\
  parsecount=0;\n");

  if (memo_flag && nr_of_memo_alts)
  {
    fprintf (output, "\n\
{       int i,j;\n\
          for (i=0; i< MEMOIZE; i+=1) \n\
                  for (j=0; j<INPUT_MEMO_SIZE; )\n\
                         memo_table[i] [j++] = -1;\n\
}\n");
  }



  if (backtrace_mark > 0)
    fprintf (output, "  e.t = \" %s\";e.r = nil; e.l = nil;(++q) -> a = &e;\n", REPR (root));
  fprintf (output, "  (++q) -> a = &r;\n");
  if (!MARKED (root, external))
    fprintf (output, "set_and_test_limits(1,%d);loop_line_mode(U%d);\n", memo_flag, root);
  else
    fprintf (output, "set_and_test_limits(1,%d);loop_line_mode(U%s);\n", memo_flag, REPR (root));
  if (stat_flag)
    fprintf (output, "print_stat();\n\n");
  if (noerrmsg_flag)
    fprintf (output, "if (parsecount == 0) exit(error_code);\n}\n");
  else
    fprintf (output, "exit(errmsg());\n}\n");


}
