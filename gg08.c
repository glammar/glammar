
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 

 *  file  : compute properties 
 *                  deterministic rules
 *                  empty
 *                  left recursive rules
 *                  which rules to (partly) memoize
 */

#include "gg1.h"
#include "gg2.h"
long det_count = 0, nondet_count = 0, memo_count = 0, recursive_count = 0;
long last_alt = true, memo_gain;
long rec_count = 0;
long adp_rule (long rule, long count);
void left_rec_rule (long rule);
long get_affix (long afx, long count);
void recursive_rule (long rule);

init_builtins ()
{
  long rule, alt;

  equal = get_builtin ("equal");
  if (meta_uniq_flag)
    *(REPR (equal)) = 'f';
  notequal = get_builtin ("notequal");
  if (meta_uniq_flag)
    *(REPR (notequal)) = 'f';

  endofsentence = get_builtin ("endofsentence");
  evalmeta = get_builtin ("evalmeta");
  explintersect = get_builtin ("explintersect_");
  falseip = get_builtin ("falseip_");
  getip = get_builtin ("getip_");
  initmeta = get_builtin ("initmeta");
  initmint = get_builtin ("initmint_");
  intersect = get_builtin ("intersect_");
  metaterminal = get_builtin ("metaterminal");
  metaterminal2 = get_builtin ("metaterminal2_");
  nestaralt = get_builtin ("nestaralt_");
  nestarset = get_builtin ("nestarset");
  detnestarset = get_builtin ("detnestarset");
  detprefix = get_builtin ("detprefix");
  detprefix2 = get_builtin ("detprefix2_");
  equalsempty = get_builtin ("equalsempty");
  nlcr = get_builtin ("nlcr");
  pair = get_builtin ("pair");
  repair = get_builtin ("repair");
  resetinputptr = get_builtin ("resetinputptr_");
  restoreip = get_builtin ("restoreip_");
  setinputptrto = get_builtin ("setinputptrto_");
  skip = get_builtin ("skip_");
  tltraditional = get_builtin ("tltraditional_");
  tltraditionalterm = get_builtin ("tltraditionalterm_");
  transformlattice = get_builtin ("transformlattice_");
  transformlatticeterm = get_builtin ("transformlatticeterm_");
  unpair = get_builtin ("unpair");
  where = get_builtin ("where");
  getlist_ = get_builtin ("getlist_");
  add_to = get_builtin ("addto");
  assign = get_builtin ("assign");

  meta_empty = get_meta_builtin ("empty");
}

get_builtin (repr)
register char *repr;
{
  register long rule, rr;
  for (rule = laststdpred; (rule != nil) && (!mystrcmp (REPR (rule), repr)); rule = BROTHER (rule));
  if (rule == nil)
  {
    fprintf (stderr, "glammar: installation error: %s: not a builtin\n", repr);
    exit (12);
  }

  for (rr = root; rr != laststdpred; rr = BROTHER (rr))
    if (REPR (rule) == REPR (rr))
    {
      if (input_from_partlist)
        fprintf (stderr, "In %s:\n", PART (rr));
      fprintf (stderr, "line %ld: %s: already in use do not redefine\n", LINE ((rr)), FREPR (rr));
      exit (12);
    }
  return rule;
}

get_meta_builtin (repr)
register char *repr;
{
  register long rule, rr;
  rr = nil;
  for (rule = lastmetarule; rule != nil; rule = BROTHER (rule))
    if (mystrcmp (REPR (rule), repr))
    {
      if (rr != nil)
      {
        if (input_from_partlist)
          fprintf (stderr, "In %s:\n", PART (rr));
        fprintf (stderr, "line %ld: %s: name already in use; do not redefine\n", LINE (rr), REPR (rr));
        exit (12);
      }
      else
        rr = rule;
    }

  if (rr == nil)
  {
    fprintf (stderr, "glammar: installation error: %s: not a meta builtin\n", repr);
    exit (12);
  }

  return rr;

}

determ ()
{
  long rule;
  if (!lift_flag)
  {
    if (det_flag)
    {
      fprintf (stderr, "glammar: -d ignored because -f overrules\n");
      det_flag = false;
    }
    fprintf (stderr, "glammar: builtins nondeterministc\n");
    for (rule = root; rule != nil; rule = BROTHER (rule))
      SET (rule, nondeterministic);
    UNSET (evalmeta, nondeterministic);
    UNSET (initmint, nondeterministic);
    UNSET (initmeta, nondeterministic);
    UNSET (pair, nondeterministic);
    UNSET (init_one_star, nondeterministic);
    SET (evalmeta, deterministic);
    SET (initmint, deterministic);
    SET (pair, deterministic);
    SET (initmeta, deterministic);
    SET (init_one_star, deterministic);
    return;
  }

/* 
 * builtins classification
 *    a:.        ----- deterministic
 *    a:;a:.     ----- deterministic if -d flag is on , else nondeterministic
 *    a:;a:;a:.  ----- nondeterministic 
 *
 */
  for (rule = laststdpred; rule != nil; rule = BROTHER (rule))
    if (BROTHER (SON (rule)) == nil)
      SET (rule, deterministic);
    else if (BROTHER (BROTHER (SON (rule))) == nil)
      if (det_flag)
        SET (rule, deterministic);
      else
        SET (rule, nondeterministic);
    else
      SET (rule, nondeterministic);

  compute_determ (root);
  check_determ ();


  if ((det_flag) && (nondet_count > 0))
  {
    exit (-1);
  }
  if ((MARKED (root, deterministic)) && (!det_flag))
  {
    if (verbose_flag)
      fprintf (stderr, "glammar semantic compute: Grammar is deterministic.\n");
    /* det_flag = true; */
  }
  if (verbose_flag)
    fprintf (stderr, "glammar semantic compute: Deterministic: (%ld,%ld)\n", det_count, nondet_count);
}

check_determ ()
{
  long alt, rule, again, over = 0, mem;
  do
  {
    again = 0;
    for (rule = root; rule != laststdpred; rule = BROTHER (rule))
      if (MARKED (rule, deterministic))
        for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
          for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
            if (NONTERMINAL (mem) && MARKED (DEF (mem), nondeterministic))
            {
              if (verbose_flag)
              {
                fprintf (stderr, "glammar: %s  is nondeterministic.\n", FREPR (mem));
              }

              again += 1;
              SET (rule, nondeterministic);
              UNSET (rule, deterministic);
            }
    if (again > 0)
      over += 1;
  }
  while (again > 0);
  if (over > 2)
    fprintf (stderr, "glammar semantic compute: deterministic: needed %ld re-computations\n", over);
}

compute_determ (rule)
long rule;
{
  if (rule == laststdpred)
    return;
  compute_determ (BROTHER (rule));
  if (determ_rule (rule))
  {
    SET (rule, deterministic);
    det_count += 1;
  }
}

determ_rule (rule)
long rule;
{
  if (MARKED (rule, nondeterministic))
  {
    return false;
  }
  if (MARKED (rule, processing))
  {
    SET (rule, recursive);
    return true;
  }
  if (MARKED (rule, deterministic))
  {
    return true;
  }
  SET (rule, processing);
  if (determ_alts (SON (rule)))
  {
    UNSET (rule, processing);
    if (!MARKED (rule, recursive))
    {
      det_count += 1;
      SET (rule, deterministic);

/*       fprintf(stderr, "line %ld: `%s' deterministic.\n",DEF(rule),REPR(rule)); */
    }
    /* else fprintf(stderr, "line %ld: `%s' recursive.\n",DEF(rule),REPR(rule)); */

/* */
    return true;
  }
  else
  {
    UNSET (rule, processing);
    nondet_count += 1;
    if (det_flag)
    {
      if (input_from_partlist)
        fprintf (stderr, "In %s:\n", PART (rule));
      fprintf (stderr, "line %ld: `%s' nondeterministic.\n", LINE ((rule)), FREPR (rule));

/*          SET(rule, deterministic); */
      return true;
    }
    else
      SET (rule, nondeterministic);
    return false;
  }
}


determ_alts (alt)
long alt;
{
  for (; alt != nil; alt = BROTHER (alt))
  {
    if (BROTHER (alt) == nil)
      last_alt = true;
    else
      last_alt = false;

    if (!determ_mems (SON (alt)))
      return false;
  }
  return true;
}


determ_mems (member)
long member;
{
  long cut_in_alt = last_alt;
  register long mem = member;
  if (!cut_in_alt)
    for (; mem != nil; mem = BROTHER (mem))
      if (DEF (mem) == cut)
        cut_in_alt = true;

  if (!cut_in_alt)
    return false;

  for (mem = member; mem != nil; mem = BROTHER (mem))
    if ((TERMINAL (mem)));
    else if (!determ_rule (DEF (mem)))
    {
      return false;
    }
  return true;
}

compute_predicates ()
{
  set_nr_of_mems_preds ();
  decrease_mem_count_preds (-99);
  set_nonpreds ();
}

empty ()
{
  set_nr_of_mems ();
  decrease_mem_count (-99);
  set_notempty ();
}

set_nr_of_mems ()
{
  long rule, alt, mem;
  for (rule = root; rule != nil; rule = BROTHER (rule))
    for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
    {
      long nr_mems = 0;
      for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
        if (FLAG_MARKED (mem, redirected_input))
          continue;
        else if TERMINAL
          (mem)
        {
          SET (rule, notemptyrule);
          for (; BROTHER (alt) != nil; alt = BROTHER (alt));
          break;
        }
        else
          nr_mems += 1;
      DEF (alt) = nr_mems;
    }
}

set_nr_of_mems_preds ()
{
  long rule, alt, mem, no_mems;
  for (rule = root; rule != nil; rule = BROTHER (rule))
  {
    long nr_mems = 0;
    for (alt = SON (rule); !MARKED (rule, not_a_predicate) && alt != nil; alt = BROTHER (alt))
    {
      for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
      {
        if (FLAG_MARKED (mem, redirected_input))
          continue;
        if (TERMINAL (mem))
        {
          SET (rule, not_a_predicate);
          break;
        }
        else if (DEF (mem) != rule)
          nr_mems += 1;
      }
    }
    DEF (SON (rule)) = nr_mems;
  }
}

decrease_mem_count_preds (rule)
long rule;
{
  long rl, alt, mem;
  for (rl = root; rl != nil; rl = BROTHER (rl))
    if ((!MARKED (rl, not_a_predicate)) && (!MARKED (rl, is_predicate)))
    {
      for (alt = SON (rl); alt != nil; alt = BROTHER (alt))
        for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
          if (FLAG_MARKED (mem, redirected_input))
            continue;
          else if (TERMINAL (mem));
          else if (DEF (mem) == rule)
            DEF (SON (rl)) -= 1;
      if (DEF (SON (rl)) == 0)
      {
        SET (rl, is_predicate);
        decrease_mem_count_preds (rl);
      }
    }
}

set_nonpreds ()
{
  long rule, preds = 0, non_preds = 0;
  for (rule = root; rule != nil; rule = BROTHER (rule))
    if (!MARKED (rule, is_predicate))
    {
      SET (rule, not_a_predicate);
      non_preds += 1;
    }
    else
      preds += 1;
  if (verbose_flag)
    fprintf (stderr, "predicates: (%ld,%ld)\n", preds, non_preds);
}

decrease_mem_count (rule)
long rule;
{
  long rl, alt, mem;
  for (rl = root; rl != nil; rl = BROTHER (rl))
    if ((!MARKED (rl, emptyrule)) && (!MARKED (rl, notemptyrule)))
      for (alt = SON (rl); alt != nil; alt = BROTHER (alt))
      {
        for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
          if (TERMINAL (mem) || FLAG_MARKED (mem, redirected_input));
          else if (DEF (mem) == rule)
            DEF (alt) -= 1;

        if (DEF (alt) == 0)
        {
          SET (rl, emptyrule);
          decrease_mem_count (rl);
          break;
        }
      }
}


long et = 0, net = 0;

set_notempty ()
{
  long rule;
  for (rule = root; rule != nil; rule = BROTHER (rule))
    if (!MARKED (rule, emptyrule))
    {
      SET (rule, notemptyrule);
      net += 1;
    }
    else
      et += 1;
  if (verbose_flag)
    fprintf (stderr, "empty: (%ld,%ld)\n", et, net);
}

adp_walk ()
{

  adp_clear ();
  adp_stddefs ();
  adp_rest ();
  add_skip ();

  if (verbose_flag)
    fprintf (stderr, "%ld productions memoized (table size = %ldk)\n",
             memo_count, (memo_count * (runtime_input_size >> 3)) >> 10);

  compute_memo_gain ();
  if (verbose_flag)
    fprintf (stderr, "glammar: %ld more productions memorized because of semantic computations\n", memo_gain);

}

add_skip ()
{
  register long rule, alt;
  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
    if (MARKED (rule, nondeterministic))
      for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
        if ((AM_flag) || (no_iha (AFFIXDEF (alt))))
          add_skip_in_alt (alt);
}

no_iha (afx)
long afx;
{
  for (; afx != nil; afx = BROTHER (afx))
    if (INHERITED (afx))
      return false;
  return true;
}

add_skip_in_alt (alt)
long alt;
{
  register long afx, skip_mem, max_count = 10000, count, mem, prev_mem, af, tm;
  char *reprterm;
  long all_empty;
  for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
    if (TERMINAL (mem));
    else if (MARKED (DEF (mem), nondeterministic))
      break;

  for (; mem != nil; mem = BROTHER (mem))
    if (TERMINAL (mem))
      break;
    else if (MARKED (DEF (mem), notemptyrule))
      break;

  if (mem == nil)
  {
    DEF (alt) = -1;
    return;
  }

  if ((BROTHER (mem) == nil) && (no_iha (AFFIXDEF (SON (DEF (mem))))) && (SON (alt) == mem))
  {
    DEF (alt) = -1;
    return;
  }
  brother = nil;
  newdefnode (ntnode, nil, nil, skip, REPR (skip));
  skip_mem = brother;
  for (afx = AFFIXDEF (alt); afx != nil; afx = BROTHER (afx))
  {
    if ((NODENAME (afx) == inherited) && (MARKED_ADP (afx, affix_directed_parsing)))
    {
      reprterm = REPR (SON (afx));
      for (mem = SON (alt), count = 0; ((mem != nil) &&
                                        (MARKED (DEF (mem), deterministic))); mem = BROTHER (mem), count++);
      for (; mem != nil; mem = BROTHER (mem), count++)
        for (af = AFFIXTREE (mem); af != nil; af = BROTHER (af))
          if (!DERIVED (af))
            for (tm = SON (af); tm != nil; tm = BROTHER (tm))
              if ((REPR (tm) == reprterm) && (count < max_count))
                max_count = count;

    }
  }
  all_empty = true;
  prev_mem = nil;
  for (mem = SON (alt); mem != nil; prev_mem = mem, mem = BROTHER (mem))

    if ((max_count-- == 0) && (!all_empty))
    {
      DEF (alt) = domemo;
      memo_count += 1;
      BROTHER (prev_mem) = skip_mem;
      BROTHER (skip_mem) = mem;
    }
    else if (TERMINAL (mem))
      all_empty = false;
    else if ((all_empty) && (MARKED (DEF (mem), notemptyrule)))
      all_empty = false;

  if ((max_count > 0) && (prev_mem != nil) && (!all_empty))
  {
    DEF (alt) = domemo;
    memo_count += 1;
    BROTHER (prev_mem) = skip_mem;
    BROTHER (skip_mem) = nil;
  }

}

adp_clear ()
{
  register long rule, afx, alt;

  for (rule = root; rule != nil; rule = BROTHER (rule))
    for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
    {
      DEF (alt) = -1;
      for (afx = AFFIXDEF (alt); afx != nil; afx = BROTHER (afx))
        DEF (afx) = 0;
    }
}

adp_stddefs ()
{
  register long rule, afx, alt;
  for (rule = laststdpred; rule != nil; rule = BROTHER (rule))
    for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
      for (afx = AFFIXDEF (alt); afx != nil; afx = BROTHER (afx))
        if (INHERITED (afx))
          if (NODENAME (SON (afx)) == affixtm)
            SET_ADP (afx, affix_directed_parsing);
          else
            SET_ADP (afx, no_affix_directed_parsing);
}

adp_rest ()
{
  register long rule, afx, alt, count;
  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
    for (afx = AFFIXDEF (SON (rule)), count = 0; afx != nil; afx = BROTHER (afx), count++)
      if (INHERITED (afx))
        adp_rule (rule, count);
}

alt_adp (reprtrm, mem)
char *reprtrm;
long mem;
{
  long count = 0;
  register long trm, afx;
  for (; mem != nil; mem = BROTHER (mem))
    for (count = 0, afx = AFFIXTREE (mem); afx != nil; afx = BROTHER (afx), count += 1)
      if (INHERITED (afx))
        for (trm = SON (afx); trm != nil; trm = BROTHER (trm))
          if (REPR (trm) == reprtrm)
            if (adp_rule (DEF (mem), count))
              return 1;
            else
            {
              long af;
              for (af = AFFIXTREE (mem); af != nil; af = BROTHER (af))
                if (NODENAME (af) == derived)
                  if (alt_adp (REPR (SON (af)), BROTHER (mem)))
                    return 1;
            }
  return 0;
}

long adp_rule (rule, count)
long rule, count;
{
  long afx, alt;
  for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
  {
    afx = get_affix (AFFIXDEF (alt), count);
    if (MARKED_ADP (afx, affix_directed_parsing))
      return true;
    if (MARKED_ADP (afx, no_affix_directed_parsing))
      return false;
    else if (MARKED_ADP (afx, processing));
    else
    {
      SET_ADP (afx, processing);
      if (!alt_adp (REPR (SON (afx)), SON (alt)))
      {
        SET_ADP (afx, no_affix_directed_parsing);
        UNSET_ADP (afx, processing);

      }
      else
      {
        UNSET_ADP (afx, processing);
        for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
          for (afx = AFFIXDEF (alt); afx != nil; afx = BROTHER (afx))
            if (NODENAME (afx) == inherited)
            {
              UNSET_ADP (afx, no_affix_directed_parsing);
              SET_ADP (afx, affix_directed_parsing);
            }
        return true;
      }
    }
  }
  return false;
}

long get_affix (afx, count)
long afx, count;
{
  for (; afx != nil; afx = BROTHER (afx), count -= 1)
    if (INHERITED (afx))
      if (count == 0)
        return afx;
  fprintf (stderr, "glammar: fatal compiler error in adp.get_affix (count was %ld)\n", count);
  exit (-1);
  return 0;                     /* Bypass compiler warning */
}

left_rec ()
{
  long rule;
  left_rec_rule (root);
  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
    if (MARKED (rule, leftrec))
    {
      if (input_from_partlist)
        fprintf (stderr, "In %s:\n", PART (rule));
      fprintf (stderr, "line %ld: `%s' left recursive\n", LINE ((rule)), FREPR (rule));
      exit (-1);
    }
}

void left_rec_rule (long rule)
{
  if (MARKED (rule, processing))
  {
    SET (rule, leftrec);
    return;
  }
  if (MARKED (rule, leftrec))
    return;
  if (MARKED (rule, notleftrec))
    return;
  SET (rule, processing);
  left_rec_alts (SON (rule));
  UNSET (rule, processing);
  if (!MARKED (rule, leftrec))
    SET (rule, notleftrec);
}


left_rec_alts (alt)
long alt;
{
  for (; alt != nil; alt = BROTHER (alt))
    left_rec_mem (SON (alt));
}


left_rec_mem (member)
long member;
{
  for (; member != nil; member = BROTHER (member))
  {
    long rule = DEF (member);
    if (TERMINAL (member))
      return;
    else if (!MARKED (rule, emptyrule))
    {
      left_rec_rule (rule);
      return;
    }
    else
      left_rec_rule (rule);
  }
}


recursive_ ()
{
  long rule;
  recursive_rule (root);
  if (verbose_flag)
  {
    for (rule = root; rule != laststdpred; rule = BROTHER (rule))
      if (MARKED (rule, recursive))
        recursive_count += 1;
    fprintf (stderr, "glammar semantic compute: %ld rules are recursive\n", recursive_count);
  }
}

void recursive_rule (long rule)
{
  if (MARKED (rule, processing))
  {
    SET (rule, recursive);
    return;
  }
  if (MARKED (rule, recursive))
    return;
  if (MARKED (rule, notrecursive))
    return;
  SET (rule, processing);
  recursive_alts (SON (rule));
  UNSET (rule, processing);
  if (!MARKED (rule, recursive))
    SET (rule, notrecursive);
}


recursive_alts (alt)
long alt;
{
  register long member;
  for (; alt != nil; alt = BROTHER (alt))
    for (member = SON (alt); member != nil; member = BROTHER (member))
      if (TERMINAL (member));
      else
        recursive_rule (DEF (member));
}


compute_memo_gain ()
{
  register long rule, alt;

  nr_of_memo_alts = memo_count;
  memo_gain = 0;
  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
    for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
      if (MEMOIZE (alt))
      {
        if (inherited_afx (alt))
          memo_gain += 1;
      }
}



inherited_afx (alt)
long alt;
{
  long afx;

  for (afx = AFFIXDEF (alt); afx != nil; afx = BROTHER (afx))
    if (NODENAME (afx) == inherited)
      return true;
  return false;
}


hint_on_non_used_hyperrules (local)
long local;
{
  long rule;
  long l = 0;

  if (local)
    l = external;
  for (rule = root; rule > laststdpred; rule = BROTHER (rule))
    if (!MARKED (rule, rule_used + l))
    {
      char *r;
      if (rule == root || rule == init_one_star)
        continue;
      if (input_from_partlist)
        fprintf (stderr, "In %s:\n", PART (rule));
      r = FREPR (rule);
      fprintf (stderr, "line %ld: (hint) lhs '%s' not used\n", LINE ((rule)), r);
    }
}

int tag_cmp (const void *e1, const void *e2)
{
  long r1, r2;
  r1 = *((long *) e1);
  r2 = *((long *) e2);
  return strcmp (REPR (r1), REPR (r2));
}

tag_index ()
{
  long alt, rule, rrule, mem;
  long *list, nr = 1;

  if (output == stdout)
  {
    tagindexfile = stdout;
  }
  else
  {
    tagindexfile = fopen (tagindexfilename, "w");
  }
  if (!tagindexfile)
    return;
  for (rrule = root; rrule != laststdpred; rrule = BROTHER (rrule))
  {
    if (strchr (REPR (rrule), '_') || !LINE (rrule))
      continue;

    nr++;

    if (!input_from_partlist)
      PART (rrule) = inputfilename;
  }
  for (rrule = lastmetarule; rrule != laststdmetarule; rrule = BROTHER (rrule))
  {
    if (strchr (REPR (rrule), '_'))
      continue;
    nr++;

    if (!input_from_partlist)
      PART (rrule) = inputfilename;
  }

  for (rrule = laststdpred; rrule != nil; rrule = BROTHER (rrule))
  {
    if (MARKED (rrule, rule_used) && !strchr (REPR (rrule), '_'))
    {
      nr++;
      PART (rrule) = stddefs_g;
    }
  }

  list = malloc (sizeof (long) * nr);
  nr = 0;
  for (rrule = root; rrule != laststdpred; rrule = BROTHER (rrule))
  {
    if (strchr (REPR (rrule), '_') || !LINE (rrule))
      continue;
    list[nr++] = rrule;
  }
  for (rrule = lastmetarule; rrule != laststdmetarule; rrule = BROTHER (rrule))
  {
    if (strchr (REPR (rrule), '_'))
      continue;
    list[nr++] = rrule;
  }
  for (rrule = laststdpred; rrule != nil; rrule = BROTHER (rrule))
    if (MARKED (rrule, rule_used) && !strchr (REPR (rrule), '_'))
    {
      list[nr++] = rrule;
    }

  qsort (list, (size_t) nr, sizeof (long), tag_cmp);

  for (rule = 0; rule < nr; rule++)
  {
    rrule = list[rule];
    if (fprintf (tagindexfile, "%s\t%s\t%ld\n", FREPR (rrule), PART (rrule), LINE (rrule)) == EOF)
      fprintf (stderr, "glammar: Write to tag index file failed\n");
  }

}


better_index ()
{
  long alt, rule, rrule, mem;
  for (rrule = root; rrule != laststdpred; rrule = BROTHER (rrule))

  {
    if (input_from_partlist)
    {
      if (fprintf (indexfile, "%s in %s used by:\n", FREPR (rrule), PART (rrule)) == EOF)
        fprintf (stderr, "glammar: Write to index file failed\n");
    }
    else if (fprintf (indexfile, "%s used by:\n", FREPR (rrule)) == EOF)
      fprintf (stderr, "glammar: Write to index file failed\n");

    for (rule = root; rule != laststdpred; rule = BROTHER (rule))
      for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
        for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
          if (NONTERMINAL (mem) && DEF (mem) == rrule)
          {
            if (input_from_partlist)
              fprintf (indexfile, "  %s %s,%ld\n", PART (rule), FREPR (rule), LINE (mem));
            else
              fprintf (indexfile, "  %s,%ld\n", FREPR (rule), LINE (mem));
          }
  }
}



long vol_count, nonvol_count;
set_volatile ()
{
  long alt, rule, mem;
  SET (assign, isvolatile);
  SET (repair, isvolatile);
  SET (add_to, isvolatile);
  SET (initmeta, isvolatile);
  SET (evalmeta, isvolatile);

  vol_count = 0;
  nonvol_count = 0;
  for (rule = laststdpred; rule != nil; rule = BROTHER (rule))
  {
    if ((!MARKED (rule, isvolatile)))
      SET (rule, nonvolatile);
  }

  compute_volatile (root);
  check_volatile ();


  if ((MARKED (root, isvolatile)))
  {
    if (verbose_flag)
      fprintf (stderr, "glammar semantic compute: Grammar is volatile (%ld, %ld).\n", vol_count, nonvol_count);
  }

  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
    for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
    {
      long afx;
      long noderived = true;
      for (afx = AFFIXDEF (alt); afx != nil; afx = BROTHER (afx))
        if (DERIVED (afx))
          break;

      if (afx != nil)
        break;

      mem = SON (alt);
      if (mem == nil)
        continue;


      for (; mem != nil; mem = BROTHER (mem))
        if (NONTERMINAL (mem) && MARKED (DEF (mem), isvolatile))
          break;
      if (mem != nil)
        continue;


      FLAG_SET (alt, freestacks_f);
    }
}

check_volatile ()
{
  long alt, rule, again, over = 0, mem;
  do
  {
    again = 0;
    for (rule = root; rule != laststdpred; rule = BROTHER (rule))
      if (MARKED (rule, nonvolatile))
        for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
          for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
            if (NONTERMINAL (mem) && MARKED (DEF (mem), isvolatile))
            {
              if (verbose_flag && !again)
              {
                fprintf (stderr, "glammar: %s  is isvolatile.\n", FREPR (mem));
              }

              again += 1;
              UNSET (rule, nonvolatile);
              SET (rule, isvolatile);
            }
    if (again > 0)
      over += 1;
  }
  while (again > 0);
  if (over > 3)
    fprintf (stderr, "glammar semantic compute: isvolatile: needed %ld re-computations\n", over);
}

compute_volatile (rule)
long rule;
{
  if (rule == laststdpred)
    return;
  compute_volatile (BROTHER (rule));
  volatile_rule (rule);
}

volatile_rule (rule)
long rule;
{
  if (MARKED (rule, nonvolatile))
  {
    return false;
  }
  if (MARKED (rule, isvolatile))
  {
    return true;
  }
  if (MARKED (rule, processing))
  {
    return false;
  }
  SET (rule, processing);
  if (volatile_alts (SON (rule)))
  {
    UNSET (rule, processing);
    SET (rule, isvolatile);
    vol_count += 1;
    return true;
  }
  else
  {
    UNSET (rule, processing);
    SET (rule, nonvolatile);
    nonvol_count += 1;
    return false;
  }
}


volatile_alts (alt)
long alt;
{
  for (; alt != nil; alt = BROTHER (alt))
  {
    if (volatile_mems (SON (alt)))
      return true;
  }
  return false;
}


volatile_mems (member)
long member;
{
  long mem = member;

  for (mem = member; mem != nil; mem = BROTHER (mem))
    if ((TERMINAL (mem)));
    else if (volatile_rule (DEF (mem)))
    {
      return true;
    }
  return false;
}
