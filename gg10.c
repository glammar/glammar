
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 

*/
/* file: coder 1 */
#include "gg1.h"
#include "gg2.h"
long nraffixes, alt_nr = 0, nr_of_mems = 0, no_inherited_afx, 
     highest_tempory, memo_alt, mems_lifted, lift_count = 0, total_nr_of_mems = 0;

char *rrrt = "";

/* exports
long lift_element (long mem);
long lookahead_in_alt (long alt);
void affix_intermediate_defs (long mem);
void code ();
void get_affixes (long affix, long count);
void mems_intermediate_defs (long mem);
void push_affixes (long affix, long count);
void result_intermediate_defs (long alt);
void trace_code (long built_als,long affix);
*/

static void rule_restore_lattices (long alt);
static void rule_save_lattices (long alt);
static void alt_init_lattices (long alt);
static long cut_in_alt (long alt);
static long cut_in_mem (long mem);
static long memo_in_alt (long alt);
static void affix_code (long mem);
static void alt_code (long alt);
static void alt_intermediate_defs (long alt);
static void alts_code (long rule, long alt);
static void code_first_terminal (long termi);
static void code_lifted_affixes (long affix);
static void code_lifted_terminal (long termi, long afx);
static void code_qstack_check (long mem);
static void code_terminal (long termi);
static void count_intermediate_defs_in_tree (long lt, long rt);
static void def_extern_meta_affixes ();
static void define_all_derived_affixes (long alt);
static void def_meta_affixes ();
static void initialize_all_derived_affixes (long alt);
static void lift_code (long alt);
static void lkh_epiloque ();
static void lkh_proloque ();
static void main_code ();
static void mems_code (long alt, long mem);
static void nice_lift_tree (long lt, long rt);
static void nice_tree (long lt, long rt);
static void result_code (long alt);
static void set_number_of_mems (long mem);
static void start_code ();

void code () 
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

#ifdef ATARI
  fprintf (output, "const long _stksize = %ldl;\n", runtime_stksize);
#endif

  if (separate_comp_flag && !MARKED (root, docompile))
    def_extern_meta_affixes ();
  else
  {
    fprintf (output, "int backtrace = %ld;\n", backtrace_mark);
    def_meta_affixes ();
  }
  if (memo_flag)
  {
    if (nr_of_memo_alts)
      fprintf (output, "#define MEMOIZE %ld\n", nr_of_memo_alts + 1);
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
        long alt;
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
        fprintf (output, "static void U%ld() {   /* %s */ \n", ulhs, FREPR (ulhs));
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
          fprintf (output, " (rq+1)->q = U%ld;q= rq+1;level=ltr;endtrace(pct,pntnamesv, ntname);\n }\n", ulhs);
        else
          fprintf (output, " (rq+1)->q = U%s;q= rq+1;level=ltr;endtrace(pct,pntnamesv, ntname);\n }\n", REPR (ulhs));
      }
      else if (!MARKED (ulhs, external))
        fprintf (output, "  (rq+1)-> q = U%ld; q = rq+1; }\n", ulhs);
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
             " %ld predicate calls changed to function calls (on total of %ld) (%.1f%%)\n", lift_count, total_nr_of_mems,
             (total_nr_of_mems != 0) ? 100 * (float) lift_count / (float) total_nr_of_mems : (float) 0);
  }
  fclose (output);
}

static void det_proto_helper (long afx) 
{
  long affix = afx, count = 0;

  if (afx == nil)
  {
      fprintf (output, "void");
      return;
  }
  for (; affix != nil; affix = BROTHER (affix))
    if (BROTHER (affix) != nil)
      fprintf (output, "AFFIX A_%ld,", count++);
    else
      fprintf (output, "AFFIX A_%ld", count++);
}

static void det_proto (long ulhs)
{ 
    fprintf (output, " int D%s(", REPR (ulhs));
    det_proto_helper(AFFIXDEF (SON (ulhs)));
    fprintf (output, ");\n");
}

static void start_code () 
{

  if (!MARKED (root, external))
    fprintf (output, "static void U%ld();\n", root);
  else
    fprintf (output, "void U%s();\n", REPR (root));

  if (memo_flag)
    fprintf (output, "#define INPUT_MEMO_SIZE %ld\n", runtime_input_size / (8 * sizeof (long)));

  code_includes ();



  for (ulhs = laststdpred; ulhs != nil; ulhs = BROTHER (ulhs))
  {
    if (!MARKED (ulhs, rule_used))
      continue;
    if (MARKED (ulhs, deterministic))
      det_proto(ulhs);
    if (!det_flag)
      fprintf (output, " void U%s(void);\n", REPR (ulhs));
  }

  for (ulhs = BROTHER (root); ulhs != laststdpred; ulhs = BROTHER (ulhs))
    if (MARKED (ulhs, external))
    {
      if (MARKED (ulhs, deterministic))
          det_proto(ulhs);
      if (!det_flag)
        fprintf (output, " void U%s();\n", REPR (ulhs));
    }
    else
    {
      if (MARKED (ulhs, deterministic))
      {
        fprintf (output, " static int D_%ld(", ulhs);
        det_proto_helper(AFFIXDEF (SON (ulhs)));
        fprintf (output, ");\n");
      }
      if (!det_flag)
        fprintf (output, " static void U%ld();\n", ulhs);
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
 
static void alt_init_lattices(long alt) 
{
  long afx,count=0;
  for (afx=AFFIXDEF(alt);  afx != nil; afx = BROTHER (afx),count++)
      if (LATTICE(afx))
        fprintf (output, " A_%ld ->t = (char *) 0X%lx;\n", count, NODENAME (afx));
}
static void rule_save_lattices (long alt) 
{
  long afx,count=0;

  for (afx=AFFIXDEF(alt);  afx != nil; afx = BROTHER (afx),count++)
      if (LATTICE(afx))
        fprintf (output, " char *L_%ld = A_%ld->t;\n", count, count);
}

static void rule_restore_lattices (long alt) 
{
  long afx,count=0;

  for (afx=AFFIXDEF(alt);  afx != nil; afx = BROTHER (afx),count++)
      if (LATTICE(afx))
        fprintf (output, "  A_%ld->t = L_%ld;\n", count, count);
}


static long cut_in_alt (long alt) 
{
  long mem;

  for (; alt != nil; alt = BROTHER (alt))
    for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
      if (DEF (mem) == cut)
        return true;
  return false;
}


static long cut_in_mem (long mem) 
{
  for (; mem != nil; mem = BROTHER (mem))
    if (DEF (mem) == cut)
      return true;
  return false;
}


long lookahead_in_alt (long alt) 
{
  long mem;

  for (; alt != nil; alt = BROTHER (alt))
    for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
      if (LKH (mem))
        return true;
  return false;
}



static long memo_in_alt (long alt) 
{
  for (; alt != nil; alt = BROTHER (alt))
    if (MEMOIZE (alt))
      return true;
  return false;
}



static void alts_code (long rule, long alt) 
{
  long affix = (AFFIXDEF (alt)), c_in_mem, c_in_alt, memo_save;


  no_inherited_afx = true;
  c_in_alt = cut_in_alt (SON (ulhs));
  get_affixes (affix, 0);
  rule_save_lattices(SON(ulhs)); 

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
      fprintf (output, "   if (SET(%ld)) { \n", alt_nr);
    }
    else
      fprintf (output, "  {\n");
    alt_code (alt);
    fprintf (output, "  }\n");
    DEF (alt) = memo_save;
    if (ambiguous_flag)
      fprintf (output, "  ambiguous_trace(&pq,&aq,\"%s\");\n", REPR (rule));
    rule_restore_lattices(alt);
    fprintf (output, "   c = rc; ip = rip ; af = raf; \n");
    if (trace_flag)
      fprintf (output, "  level = ltr;\n");
    if (c_in_mem)
      fprintf (output, "  if (cut_set) goto done;\n");
  }
  if (c_in_alt)
    fprintf (output, "done:\n");
  push_affixes (affix, 0);

}


static void set_number_of_mems (long mem) 
{
  for (nr_of_mems = 0; mem != nil; mem = BROTHER (mem))
    nr_of_mems += 1;
  total_nr_of_mems += nr_of_mems;
}


void get_affixes (long affix, long count) 
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
    fprintf (output, "  AFFIX A_%ld = (rq+%ld)-> a;\n", count, count + nraffixes + 1);
  else
    fprintf (output, "  register AFFIX A_%ld = (rq+%ld)-> a;\n", count, count + nraffixes + 1);
}


void push_affixes (long affix, long count) 
{
  if (affix == nil)
    return;
  fprintf (output, "  (rq+%ld)->a = A_%ld;\n", nraffixes + count + 1, count);
  push_affixes (BROTHER (affix), count + 1);
}


void trace_code (long built_als,long affix) 
{
  long count = 0;

  fprintf (output, "   begin1_trace(ntname);\n");
  for (; affix != nil; affix = BROTHER (affix))
  {
    if (FLAG_MARKED (affix,TRC_LATTICE))
      fprintf (output, "  trace_lattice(A_%ld);\n", count);
    else if (NODENAME (affix) == inherited)
      fprintf (output, "  trace_affix(%d,A_%ld);\n", built_als ? 1 : 0, count);
    count += 1;
  }
  fprintf (output, "   begin2_trace();\n");

}


static void define_all_derived_affixes (long alt) 
{
  long affix, mem;

  for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
    for (affix = AFFIXTREE (mem); affix != nil; affix = BROTHER (affix))
    {
      long term = SON (affix);

      if (NODENAME (affix) == derived)
        if ((APPLY_BOUND_AFFIX (term)) && (!IS_LEFTDEF (term)))
          fprintf (output, "  affix a_%s;\n", REPR (term));
      if ((LATTICE (affix)) && (!IS_LEFTDEF (term)) && same_lattice_not_already_encountered(alt, mem, affix))
        fprintf (output, "  affix a_%s ;\n", REPR (term));
    }
}


static void initialize_all_derived_affixes (long alt) 
{
  long affix, mem;

  for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
    for (affix = AFFIXTREE (mem); affix != nil; affix = BROTHER (affix))
    {
      long term = SON (affix);

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
      if ((LATTICE (affix)) && (!IS_LEFTDEF (term)) && same_lattice_not_already_encountered(alt, mem, affix))
        fprintf (output, "  a_%s.t = (char *) 0X%lX; a_%s.l = nil; a_%s.r = nil;\n", 
              REPR (term), NODENAME(LATTICE_DEF(term)),REPR(term),REPR(term));
    }
}

int same_lattice_not_already_encountered(long alt, long member, long affix)
{
   char *rafx= REPR(SON(affix));
   long mem = SON(alt); 
   for (; mem != nil; mem=BROTHER(mem) )
   {
     long afx=AFFIXTREE(mem);
     for (; afx != nil && afx != affix; afx=BROTHER(afx))
      if (LATTICE(afx) && REPR(SON(afx)) == rafx)
           return 0;
     if (afx == affix || mem == member) return 1;
   }
  return 1;
}

static void nice_tree (long lt, long rt) 
{
  long term;

  if (lt == rt)
    return;
  term = BROTHER (lt);
  if (term == rt)
  {
    if (NODENAME (lt) == affixtm)
    {
      aCount += 1;
      fprintf (output, "  TM_%ld.t = \"%s\"; TM_%ld.l = nil; TM_%ld.r = nil;\n", aCount, REPR (lt), aCount, aCount);
    }
    return;
  }
  else if ((BROTHER (term) == rt) && (NODENAME (term) != affixtm))
  {
    aCount += 1;
    if (NODENAME (lt) == affixtm)
    {
      if (IS_LEFTDEF (term))
        fprintf (output, "  TM_%ld.t = \"%s\"; TM_%ld.l = nil; TM_%ld.r = A_%ld;\n",
                 aCount, REPR (lt), aCount, aCount, LEFTDEF (term));
      else
        fprintf (output, "  TM_%ld.t = \"%s\"; TM_%ld.l = nil; TM_%ld.r = &a_%s;\n",
                 aCount, REPR (lt), aCount, aCount, REPR (term));
    }
    else if (IS_LEFTDEF (lt))
    {
      if (IS_LEFTDEF (term))
        fprintf (output, "  TM_%ld.t = empty; TM_%ld.l = A_%ld; TM_%ld.r =  A_%ld;\n",
                 aCount, aCount, LEFTDEF (lt), aCount, LEFTDEF (term));
      else
        fprintf (output, "  TM_%ld.t = empty; TM_%ld.l = A_%ld; TM_%ld.r = &a_%s;\n",
                 aCount, aCount, LEFTDEF (lt), aCount, REPR (term));
    }
    else if (IS_LEFTDEF (term))
      fprintf (output, "  TM_%ld.t = empty; TM_%ld.l = &a_%s; TM_%ld.r = A_%ld;\n",
               aCount, aCount, REPR (lt), aCount, LEFTDEF (term));
    else
      fprintf (output, "  TM_%ld.t = empty; TM_%ld.l = &a_%s; TM_%ld.r = &a_%s;\n",
               aCount, aCount, REPR (lt), aCount, REPR (term));
    return;
  }
  if (NODENAME (lt) != affixtm)
  {
    aCount += 1;
    if (IS_LEFTDEF (lt))
      fprintf (output, "  TM_%ld.t = empty;TM_%ld.l = A_%ld;TM_%ld.r = &TM_%ld;\n",
               aCount, aCount, LEFTDEF (lt), aCount, aCount + 1);
    else
      fprintf (output, "  TM_%ld.t = empty;TM_%ld.l = &a_%s;TM_%ld.r = &TM_%ld;\n", aCount, aCount, REPR (lt), aCount, aCount + 1);
    nice_tree (BROTHER (lt), rt);
  }
  else
  {
    aCount += 1;
    fprintf (output, "  TM_%ld.t = \"%s\";TM_%ld.l = nil;TM_%ld.r = &TM_%ld;\n", aCount, REPR (lt), aCount, aCount, aCount + 1);
    nice_tree (BROTHER (lt), rt);
  }
}


static void nice_lift_tree (long lt, long rt) 
{
  long term;

  if (lt == rt)
    return;
  term = BROTHER (lt);
  if (term == rt)
  {
    if (NODENAME (lt) == affixtm)
    {
      aCount += 1;
      fprintf (output, "TM_%ld.t = \"%s\",TM_%ld.l = nil,TM_%ld.r = nil,", aCount, REPR (lt), aCount, aCount);
    }
    return;
  }
  else if ((BROTHER (term) == rt) && (NODENAME (term) != affixtm))
  {
    aCount += 1;
    if (NODENAME (lt) == affixtm)
    {
      if (IS_LEFTDEF (term))
        fprintf (output, "TM_%ld.t = \"%s\",TM_%ld.l = nil,TM_%ld.r = A_%ld,",
                 aCount, REPR (lt), aCount, aCount, LEFTDEF (term));
      else
        fprintf (output, "TM_%ld.t = \"%s\",TM_%ld.l = nil,TM_%ld.r = &a_%s,", aCount, REPR (lt), aCount, aCount, REPR (term));
    }
    else if (IS_LEFTDEF (lt))
    {
      if (IS_LEFTDEF (term))
        fprintf (output, "TM_%ld.t = empty,TM_%ld.l = A_%ld,TM_%ld.r = A_%ld,",
                 aCount, aCount, LEFTDEF (lt), aCount, LEFTDEF (term));
      else
        fprintf (output, "TM_%ld.t = empty,TM_%ld.l = A_%ld,TM_%ld.r = &a_%s,",
                 aCount, aCount, LEFTDEF (lt), aCount, REPR (term));
    }
    else if (IS_LEFTDEF (term))
      fprintf (output, "TM_%ld.t = empty,TM_%ld.l = &a_%s,TM_%ld.r = A_%ld,", aCount, aCount, REPR (lt), aCount, LEFTDEF (term));
    else
      fprintf (output, "TM_%ld.t = empty,TM_%ld.l = &a_%s,TM_%ld.r = &a_%s,", aCount, aCount, REPR (lt), aCount, REPR (term));
    return;
  }
  if (NODENAME (lt) != affixtm)
  {
    aCount += 1;
    if (IS_LEFTDEF (lt))
      fprintf (output, "TM_%ld.t = empty,TM_%ld.l = A_%ld,TM_%ld.r = &TM_%ld,", aCount, aCount, LEFTDEF (lt), aCount, aCount + 1);
    else
      fprintf (output, "TM_%ld.t = empty,TM_%ld.l = &a_%s,TM_%ld.r = &TM_%ld,", aCount, aCount, REPR (lt), aCount, aCount + 1);
    nice_lift_tree (BROTHER (lt), rt);
  }
  else
  {
    aCount += 1;
    fprintf (output, "TM_%ld.t = \"%s\",TM_%ld.l = nil,TM_%ld.r = &TM_%ld,", aCount, REPR (lt), aCount, aCount, aCount + 1);
    nice_lift_tree (BROTHER (lt), rt);
  }
}


static void affix_code (long mem) 
{
  long affix;

  for (affix = AFFIXTREE (mem); affix != nil; affix = BROTHER (affix))
  {
    long term = SON (affix);

    if (NODENAME (affix) == inherited)
    {
      if (BROTHER (term) != nil)
      {
        long this_a_count = aCount + 1;

        nice_tree (term, nil);
        fprintf (output, "  (rq+%ld)->a = &TM_%ld;\n", qCount + 1, this_a_count);
      }
      else if (NODENAME (term) == affixtm)
      {
        aCount += 1;
        fprintf (output, "  TM_%ld.t = \"%s\"; TM_%ld.l = nil; TM_%ld.r = nil;\n", aCount, REPR (term), aCount, aCount);
        fprintf (output, "  (rq+%ld)->a = &TM_%ld;\n", qCount + 1, aCount);
      }
      else if (IS_LEFTDEF (term))
        fprintf (output, "  (rq+%ld)->a = A_%ld;\n", qCount + 1, LEFTDEF (term));
      else
        fprintf (output, "  (rq+%ld)->a = &a_%s;\n", qCount + 1, REPR (term));
    }
    else
    {
      if (IS_LEFTDEF (term))
        fprintf (output, "  (rq+%ld)->a = A_%ld;\n", qCount + 1, LEFTDEF (term));
      else
        fprintf (output, "  (rq+%ld)->a = &a_%s;\n", qCount + 1, REPR (term));
    }
    qCount += 1;
  }
}


static void code_lifted_affixes (long affix) 
{
  fprintf (output, "(");
  for (; affix != nil; affix = BROTHER (affix))
  {
    long term = SON (affix);

    if (NODENAME (affix) == inherited)
    {
      if (BROTHER (term) != nil)
      {
        long this_a_count = aCount + 1;

        fprintf (output, "(");
        nice_lift_tree (term, nil);
        fprintf (output, "  &TM_%ld)", this_a_count);
      }
      else if (NODENAME (term) == affixtm)
      {
        aCount += 1;
        fprintf (output, "  (TM_%ld.t = \"%s\", TM_%ld.l = nil, TM_%ld.r = nil,&TM_%ld)",
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


static void result_code (long alt) 
{
  long affix, count = 0;

  for (affix = AFFIXDEF (alt); affix != nil; affix = BROTHER (affix))
  {
    if (NODENAME (affix) == derived)
    {
      long term = SON (affix);

      if (BROTHER (term) == nil)
      {
        if (NODENAME (term) == affixtm)
          fprintf (output,
                   "  A_%ld -> t = \"%s\"; A_%ld -> l = nil; A_%ld -> r = nil;\n", count, REPR (term), count, count);
        else if (IS_LEFTDEF (term) && LEFTDEF(term) != count)
          fprintf (output, "MAKE(A_%ld,A_%ld);\n ", count, LEFTDEF (term));
        else if (FREE_AFFIX (term))
        {
#ifndef NO_COPY_STRUCT
          fprintf (output, "  *A_%ld  = a_%s;", count, REPR (term));
#else
          fprintf (output, "  A_%ld -> t = a_%s.t;", count, REPR (term));
          fprintf (output, "A_%ld -> r = nil;", count);
          fprintf (output, "A_%ld -> l = nil;\n", count);
#endif
        }
      }
      else
      {
        long this_a_count = aCount + 1, bterm = BROTHER (term);

        if (BROTHER (bterm) == nil)
        {
          if ((NODENAME (term) == affixtm) && (NODENAME (bterm) == affixtm))
          {
            aCount += 1;
            fprintf (output, "  TM_%ld.t = \"%s\";", this_a_count, REPR (bterm));
            fprintf (output, "TM_%ld.l = nil;", this_a_count);
            fprintf (output, "TM_%ld.r = nil;\n", this_a_count);
            fprintf (output, "  A_%ld -> t = \"%s\";", count, REPR (term));
            fprintf (output, "A_%ld -> r = &TM_%ld;", count, this_a_count);
            fprintf (output, "A_%ld -> l = nil;", count);
          }
          else if ((NODENAME (term) != affixtm) && (NODENAME (bterm) == affixtm))
          {
            aCount += 1;
            fprintf (output, "  TM_%ld.t = \"%s\";", this_a_count, REPR (bterm));
            fprintf (output, "TM_%ld.l = nil;", this_a_count);
            fprintf (output, "TM_%ld.r = nil;\n", this_a_count);
            if (IS_LEFTDEF (term))
              fprintf (output, "  A_%ld -> l = A_%ld;", count, LEFTDEF (term));
            else
              fprintf (output, "  A_%ld -> l = &a_%s;", count, REPR (term));
            fprintf (output, "A_%ld -> r = &TM_%ld;", count, this_a_count);
            fprintf (output, "A_%ld -> t = empty;\n", count);
          }
          else if ((NODENAME (term) == affixtm) && (NODENAME (bterm) != affixtm))
          {
            fprintf (output, "  A_%ld -> t = \"%s\";", count, REPR (term));
            if (IS_LEFTDEF (bterm))
              fprintf (output, "A_%ld -> r = A_%ld;", count, LEFTDEF (bterm));
            else
              fprintf (output, "A_%ld -> r = &a_%s;", count, REPR (bterm));
            fprintf (output, "A_%ld -> l = nil;\n", count);
          }
          else
          {
            fprintf (output, "  A_%ld -> t = empty;", count);
            if (IS_LEFTDEF (term))
              fprintf (output, "A_%ld -> l = A_%ld;", count, LEFTDEF (term));
            else
              fprintf (output, "A_%ld -> l = &a_%s;", count, REPR (term));
            if (IS_LEFTDEF (bterm))
              fprintf (output, "A_%ld -> r = A_%ld;\n", count, LEFTDEF (bterm));
            else
              fprintf (output, "A_%ld -> r = &a_%s;\n", count, REPR (bterm));
          }
        }
        else if (NODENAME (term) != affixtm)
        {
          nice_tree (bterm, nil);
          fprintf (output, "  A_%ld -> t = empty;", count);
          fprintf (output, "A_%ld -> r = &TM_%ld;", count, this_a_count);
          if (IS_LEFTDEF (term))
            fprintf (output, "A_%ld -> l = A_%ld;\n", count, LEFTDEF (term));
          else
            fprintf (output, "A_%ld -> l = &a_%s;\n", count, REPR (term));
        }
        else
        {
          nice_tree (bterm, nil);
          fprintf (output, "  A_%ld -> t = \"%s\";", count, REPR (term));
          fprintf (output, "A_%ld -> r = &TM_%ld;", count, this_a_count);
          fprintf (output, "A_%ld -> l = nil;\n", count);
        }
      }
    }
    else if (0 && LATTICE (affix))  /* semantic change in lattice evaluation, threating it as defining */
      if (NODENAME (SON (affix)) == affixtm)
      {
        long term = SON (affix);
        fprintf (output, " A_%ld ->t = (char *) 0X%lx;\n", count, NODENAME (LATTICE_DEF (term)));

      }
    count += 1;
  }
}

static void code_qstack_check (long mem) 
{
  long afx, q_top = qCount;

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
    fprintf (output, " if (q+%ld > q_top) need_more_qstack();\n", q_top);
}

static void alt_code (long alt) 
{
  long mem;

  qCount = nraffixes;
  aCount = 0;
  alt_intermediate_defs (alt);
  aCount = 0;
  define_all_derived_affixes (alt);
  initialize_all_derived_affixes (alt);
  /* alt_init_lattices(alt); */
  code_qstack_check (SON (alt));
  lift_code (alt);
  mem = SON (alt);
  result_code (alt);
  if (mem == nil)
    fprintf (output, "  q = rq+%ld;(*(rq+%ld)->q)();\n", qCount - 1, qCount);
  else if (BROTHER (mem) == nil)
  {

/*
    if ((memo_flag) && (memo_alt))
      fprintf(output, "   (rq+%ld) -> q = uskip_;\n", qCount);
*/
    if (LKH (mem))
    {
      lkh_epiloque ();
      fprintf (output, "  lkh = ip;\n");
    }
    if (TERMINAL (mem))
    {
      if (backtrace_mark > 0)
        fprintf (output, "  (rq+%ld)->a = A_0;\n", ++qCount);
      affix_code (mem);
      code_first_terminal (mem);
      qCount += 2;
    }
    else if (!MARKED (DEF (mem), external))
    {
      affix_code (mem);
      fprintf (output, "  q = rq+%ld;U%ld();\n", qCount, DEF (mem));
      qCount += 1;
    }
    else
    {
      affix_code (mem);
      if (DEF (mem) == cut)
        fprintf (output, "   (rq+%ld) -> i = &cut_set;\n", ++qCount);
      fprintf (output, "  q = rq+%ld; U%s();\n", qCount, REPR (mem));
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
        fprintf (output, "  (rq+%ld)->a = A_0;\n", ++qCount);
      affix_code (mem);
      code_first_terminal (mem);
      qCount += 2;
    }
    else if (!MARKED (DEF (mem), external))
    {
      affix_code (mem);
      fprintf (output, "  q = rq+%ld;U%ld();\n", qCount, DEF (mem));
      qCount += 1;
    }
    else
    {
      affix_code (mem);
      if (DEF (mem) == cut)
        fprintf (output, "   (rq+%ld) -> i = &cut_set;\n", ++qCount);
      if (DEF (mem) == skip)
      {
        fprintf (output, "  /* here comes that skip */\n");
        DEF (alt) = qCount;
      }
      fprintf (output, "  q = rq+%ld;U%s();\n", qCount, REPR (mem));
      qCount += 1;
    }
  }
  if (memo_flag && memo_alt)
  {
    fprintf (output,
             "  rq = q - %ld;\n   if ((rq+ %ld)->q == uskip_) { CLEAR(%ld); rq -=1;}\n", qCount - 1, DEF (alt), alt_nr++);
  }
  else if (memo_flag)
    fprintf (output, "  rq = q - %ld; \n", qCount);
  if (mems_lifted)
    fprintf (output, "   }\n");
}


static void lift_code (long alt)
{
  long mem = SON (alt);

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
        fprintf (output, "   if ( D_%ld", DEF (mem));
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


static void mems_code (long alt, long mem)
{
  if (mem != nil)
  {
    mems_code (alt, BROTHER (mem));
    if (LKH (mem))
      lkh_epiloque ();

    if (TERMINAL (mem))
    {
      if (backtrace_mark > 0)
        fprintf (output, "  (rq+%ld)->a = A_0;\n", ++qCount);
      affix_code (mem);
      code_terminal (mem);
      qCount += 2;
    }
    else if (!MARKED (DEF (mem), external))
    {
      affix_code (mem);
      fprintf (output, "  (rq+%ld)->q = U%ld;\n", ++qCount, DEF (mem));
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
        fprintf (output, "   (rq+%ld) -> i = &cut_set;\n", ++qCount);
      fprintf (output, "  (rq+%ld)->q = U%s;\n", ++qCount, REPR (mem));
    }
    if (LKH (mem))
      lkh_proloque ();
  }
}


static void lkh_epiloque () 
{
  fprintf (output, "  (rq+%ld)->l = &lkh;\n", ++qCount);
  fprintf (output, "  (rq+%ld)->q = lkh_epiloque;\n", ++qCount);
}


static void lkh_proloque () 
{
  fprintf (output, "  (rq+%ld)->l = &lkh;\n", ++qCount);
  fprintf (output, "  (rq+%ld)->q = lkh_proloque;\n", ++qCount);
}


static void code_first_terminal (long termi)
{
  if (STRING (termi))
    fprintf (output, "  (rq+%ld)->s= \"%s\"; q = rq +%ld;%sUterminal_();\n", qCount + 1, REPR (termi), qCount + 1, rrrt);
  else if (COMPLEMENT (termi))
  {
    if (EXCLAMATIONSTARCHOICE (termi))
      fprintf (output,
               "  (rq+%ld)->s =\"%s\";q = rq +%ld;%sUx_star_ex_choice();\n", qCount + 1, REPR (termi), qCount + 1, rrrt);
    else if (EXCLAMATIONPLUSCHOICE (termi))
      fprintf (output,
               "  (rq+%ld)->s =\"%s\";q = rq +%ld;%sUx_plus_ex_choice();\n", qCount + 1, REPR (termi), qCount + 1, rrrt);
    else
      fprintf (output, "  (rq+%ld)->s =\"%s\";q = rq +%ld;%sUex_choice();\n", qCount + 1, REPR (termi), qCount + 1, rrrt);
  }
  else if (EXCLAMATIONSTARCHOICE (termi))
    fprintf (output,
             "  (rq+%ld)->s =\"%s\";q = rq +%ld;%sUx_star_choice();\n", qCount + 1, REPR (termi), qCount + 1, rrrt);
  else if (EXCLAMATIONPLUSCHOICE (termi))
    fprintf (output,
             "  (rq+%ld)->s =\"%s\";q = rq +%ld;%sUx_plus_choice();\n", qCount + 1, REPR (termi), qCount + 1, rrrt);
  else
    fprintf (output, "  (rq+%ld)->s =\"%s\";q = rq +%ld;%sUchoice_();\n", qCount + 1, REPR (termi), qCount + 1, rrrt);
}


static void code_terminal (long termi) 
{
  if (STRING (termi))
    fprintf (output, "  (rq+%ld)->s = \"%s\";(rq+%ld)->q = %sUterminal_;\n", qCount + 1, REPR (termi), qCount + 2, rrrt);
  else if (COMPLEMENT (termi))
  {
    if (EXCLAMATIONSTARCHOICE (termi))
      fprintf (output,
               "  (rq+%ld)->s = \"%s\";(rq+%ld)->q = %sUx_star_ex_choice;\n", qCount + 1, REPR (termi), qCount + 2, rrrt);
    else if (EXCLAMATIONPLUSCHOICE (termi))
      fprintf (output,
               "  (rq+%ld)->s = \"%s\";(rq+%ld)->q = %sUx_plus_ex_choice;\n", qCount + 1, REPR (termi), qCount + 2, rrrt);
    else
      fprintf (output,
               "  (rq+%ld)->s = \"%s\";(rq+%ld)->q = %sUex_choice;\n", qCount + 1, REPR (termi), qCount + 2, rrrt);
  }
  else if (EXCLAMATIONSTARCHOICE (termi))
    fprintf (output,
             "  (rq+%ld)->s = \"%s\";(rq+%ld)->q = %sUx_star_choice;\n", qCount + 1, REPR (termi), qCount + 2, rrrt);
  else if (EXCLAMATIONPLUSCHOICE (termi))
    fprintf (output,
             "  (rq+%ld)->s = \"%s\";(rq+%ld)->q = %sUx_plus_choice;\n", qCount + 1, REPR (termi), qCount + 2, rrrt);
  else
    fprintf (output, "  (rq+%ld)->s = \"%s\";(rq+%ld)->q = %sUchoice_;\n", qCount + 1, REPR (termi), qCount + 2, rrrt);
}


static void code_lifted_terminal (long termi, long afx) 
{
  long trm;

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


static void count_intermediate_defs_in_tree (long lt, long rt) 
{
  long term;

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


void affix_intermediate_defs (long mem) 
{
  long affix;

  for (affix = AFFIXTREE (mem); affix != nil; affix = BROTHER (affix))
  {
    long term = SON (affix);

    if (NODENAME (affix) == inherited)
    {
      if (BROTHER (term) != nil)
        count_intermediate_defs_in_tree (term, nil);
      else if (NODENAME (term) == affixtm)
        aCount += 1;
    }
  }
}


void result_intermediate_defs (long alt) 
{
  long affix;

  for (affix = AFFIXDEF (alt); affix != nil; affix = BROTHER (affix))
  {
    if (NODENAME (affix) == derived)
    {
      long term = SON (affix), bterm = BROTHER (term);

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


static void alt_intermediate_defs (long alt) 
{
  long mem = SON (alt), count;

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
    fprintf (output, "   affix TM_1;\n");
  else
  {
    fprintf (output, "   affix TM_1");
    for (count = 2; count != aCount + 1; count++)
      fprintf (output, ",TM_%ld", count);
    fprintf (output, ";\n");
  }
}


void mems_intermediate_defs (long mem) 
{
  if (mem != nil)
  {
    mems_intermediate_defs (BROTHER (mem));
    affix_intermediate_defs (mem);
  }
}


static void def_extern_meta_affixes () 
{
  long mrule;
  fprintf (output, "typedef struct affix{char *t; struct affix *l; struct affix *r;} affix,*AFFIX ;\n");
  for (mrule = lastmetarule; mrule != nil; mrule = BROTHER (mrule))
    fprintf (output, "   extern affix a_%s;\n", REPR (mrule));
  for (mrule = first_lattice; mrule != nil; mrule = BROTHER (mrule))
    fprintf (output, "   extern affix a_%s;\n", REPR (mrule));
}

static void def_meta_affixes () 
{
  long mrule, prev = lastmetarule;

  fprintf (output, "typedef struct affix{char *t; struct affix *l; struct affix *r;} affix,*AFFIX ;\n");
  for (mrule = lastmetarule; mrule != nil; mrule = BROTHER (mrule))
  {
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
#ifdef NVR
  for (mrule = first_lattice; mrule != nil; mrule = BROTHER (mrule))
  {
      if (0)
      {
        fprintf (output, " affix la_%s = {(char *)%ld,0,0};\n", REPR(mrule),NODENAME(mrule));
        fprintf (output, " AFFIX a_%s = &la_%s;\n", REPR(mrule),REPR(mrule));
      }
      else
      {
        fprintf (output, " affix a_%s = {(char *)%ld,0,0};\n", REPR(mrule),NODENAME(mrule));
      }
  }
#endif
}

long lift_element (long mem) 
{
  if (mem == nil)
    return false;
  return ((TERMINAL (mem)) || (MARKED (DEF (mem), deterministic)));
}


static void main_code () 
{
  fprintf (output, "\n\
\n\
int main(int arg_count, char **arguments)\n\
{\n\
  affix r,e;\n\
  fla.t = fast_list_acces;\n\
  e.t = undefined;\n\
  r.t = e.t;%s\n\
  pntname = \"%s\"; \n\
  readinput(arg_count,arguments, %s);\n\
  cstore  = input + nrofchars+10;\n\
  c = cstore; \n\
  ct = cstore;\n\
  q = q_stack;\n\
  af = affix_heap; \n", 
    MARKED (init_one_star, deterministic)?"q_size=1000;":"", 
    FREPR (root), 
    start_predicate_flag ? "false" : "true");
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
    fprintf (output, "set_and_test_limits(1,%ld);loop_line_mode(U%ld);\n", memo_flag, root);
  else
    fprintf (output, "set_and_test_limits(1,%ld);loop_line_mode(U%s);\n", memo_flag, REPR (root));
  if (stat_flag)
    fprintf (output, "print_stat();\n\n");
  if (noerrmsg_flag)
    fprintf (output, "if (parsecount == 0) exit(error_code);\n}\n");
  else
    fprintf (output, "exit(errmsg());\n}\n");
}
