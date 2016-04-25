
/*
    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 
    - fill in implied equals 
    - fill in intersections for lattices
    - compute affix usage for each rule compute its "check sum"
        used for separate compilation
*/
#include "gg1.h"
#include "gg2.h"
int unique_e_count = 0, total_e_count = 0;
int dc_count = 0;
char *lran = "error_affiX";
char *hex_repr ();
char *get_ie ();
char small_ie[10][4] = {
  "E_0",
  "E_1",
  "E_2",
  "E_3",
  "E_4",
  "E_5",
  "E_6",
  "E_7",
  "E_8",
  "E_9",
};

int detnestars = 0;
ie ()
{
  int rule, alt;
  for (rule = root; rule != laststdpred; rule = BROTHER (rule))

/*    if (MARKED(rule,docompile)) */
    for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
      altie (alt);
  if (verbose_flag)
    fprintf (stderr, "glammar transformation phase: %d implied equals resolved\n", total_e_count);
  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
    for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
      altdetnestarset (alt);
  if (verbose_flag)
    fprintf (stderr, "glammar transformation phase: %d  phase 1 nestarsets replaced\n", detnestars);
}

altdetnestarset (alt)
int alt;
{
  int member, affix, term, bt;
  int b1 = nil;
  int b2 = nil;
  int b3 = nil;
  for (member = SON (alt); member != nil; member = BROTHER (member))
  {
    b1 = nil;
    b2 = nil;
    b3 = nil;
    b1 = BROTHER (member);
    if (b1 != nil)
    {
      b2 = BROTHER (b1);
      if (b2 != nil)
      {
        b3 = BROTHER (b2);
      }
    }
    if (b3 != nil && (DEF (member) == setinputptrto) && (DEF (b1) == detnestarset)
        && (STRING (b2) || DEF (b2) == metaterminal) && (DEF (b3) == resetinputptr))
    {
      int af1, af2, af3;
      DEF (member) = detprefix;
      REPR (member) = REPR (detprefix);
      BROTHER (member) = BROTHER (b3);
      af1 = AFFIXTREE (member);
      af2 = AFFIXTREE (b1);
      if (STRING (b2))
      {
        newnode (affixtm, nil, nil, REPR (b2));
      }
      else
      {
        int t = SON (AFFIXTREE (b2));
        newnode (NODENAME (t), nil, nil, REPR (t));
      }
      newnode (inherited, nil, brother, "(nil)");
      af3 = brother;
      BROTHER (af1) = af2;
      BROTHER (af2) = af3;
      AFFIXTREE (member) = af1;
      SET (detprefix, rule_used);
      continue;
    }
    if (b2 != nil && (DEF (member) == detnestarset)
        && (STRING (b1) || DEF (b1) == metaterminal) && (DEF (b2) == resetinputptr))
    {
      int af1, af2, af3;
      DEF (member) = detprefix2;
      REPR (member) = REPR (detprefix2);
      BROTHER (member) = BROTHER (b2);
      af2 = AFFIXTREE (member);
      af3 = BROTHER (af2);
      af1 = AFFIXTREE (b2);
      BROTHER (af1) = af2;
      AFFIXTREE (member) = af1;
      SET (detprefix2, rule_used);
      continue;
    }
    if (b1 != nil && (DEF (member) == detnestarset) && (DEF (b1) == equal))
    {
      int af1, af2, t1, t2;
      int bf1, bf2, s1, s2;
      af1 = AFFIXTREE (member);
      af2 = BROTHER (af1);
      t1 = SON (af1);
      t2 = SON (af2);
      bf1 = AFFIXTREE (b1);
      bf2 = BROTHER (bf1);
      s1 = SON (bf1);
      s2 = SON (bf2);
      if (REPR (t1) == REPR (s1))
      {
        DEF (member) = metaterminal;
        REPR (member) = REPR (metaterminal);
        AFFIXTREE (member) = bf2;
        BROTHER (member) = b2;
        SET (metaterminal, rule_used);
        continue;
      }
    }
    if (DEF (member) == equal)
    {
      int af1, af2, t1, t2;
      af1 = AFFIXTREE (member);
      af2 = BROTHER (af1);
      t1 = SON (af1);
      t2 = SON (af2);
      if (NODENAME (t1) == metaffix && REPR (t1) == REPR (meta_empty) && BROTHER (t1) == nil)
      {
        DEF (member) = equalsempty;
        REPR (member) = REPR (equalsempty);
        AFFIXTREE (member) = af2;
        SET (equalsempty, rule_used);
        continue;
      }
      if (NODENAME (t2) == metaffix && REPR (t2) == REPR (meta_empty) && BROTHER (t2) == nil)
      {
        DEF (member) = equalsempty;
        REPR (member) = REPR (equalsempty);
        BROTHER (af1) = nil;
        SET (equalsempty, rule_used);
        continue;
      }
    }
    if (DEF (member) == nestarset && BROTHER (member) != nil &&
        (DEF (BROTHER (member)) == metaterminal || DEF (BROTHER (member)) == metaterminal2))
    {
      REPR (member) = REPR (detnestarset);
      DEF (member) = detnestarset;
      affix = AFFIXTREE (member);
      bt = SON (AFFIXTREE (BROTHER (member)));
      newnode (NODENAME (bt), nil, nil, REPR (bt));
      newnode (inherited, nil, brother, "(nil)");
      BROTHER (affix) = brother;;
      detnestars++;
      SET (detnestarset, rule_used);
    }
  }
}

altie (alt)
int alt;
{
  int member, affix;
  unique_e_count = 0;
  lefthsie (alt);
  for (member = SON (alt); member != nil; member = BROTHER (member))
    for (affix = AFFIXTREE (member); affix != nil; affix = BROTHER (affix))
      if (REPR (SON (affix)) == dont_care);
      else if (DERIVED (affix))
        rightsie (alt, member, affix);
      else if (LATTICE (affix))
      {
        if (!right_implicite_intersect (alt, member, affix))
          right_explicite_intersect (member, affix);
      }
  total_e_count += unique_e_count;
}

lefthsie (alt)
int alt;
{
  int affix;
  for (affix = AFFIXDEF (alt); affix != nil; affix = BROTHER (affix))
  {
    if (INHERITED (affix) && REPR (SON (affix)) != dont_care)
      if (APPLY_BOUND_AFFIX (SON (affix)))
      {
        int afx;
        for (afx = BROTHER (affix); afx != nil; afx = BROTHER (afx))
        {
          if ((INHERITED (afx)) && (REPR (SON (affix)) == REPR (SON (afx))))
          {
            char *name;
            name = get_ie ();
            REPR (SON (afx)) = name;
            LEFTDEF (SON (afx)) = -1;
            newnode (inherited, nil, SON (afx), "(nil)");
            newnode (inherited, brother, SON (affix), "(nil)");
            newdefnode (ntnode, SON (alt), brother, equal, REPR (equal));
            SET (equal, rule_used);
            SON (alt) = brother;
          }
        }
      }
      else
      {
        char *name;
        int term = SON (affix), atype = NODENAME (term);
        name = get_ie ();
        newnode (atype, BROTHER (term), nil, REPR (term));
        newnode (inherited, nil, brother, "(nil)");
        newnode (inherited, brother, term, "(nil)");
        newdefnode (ntnode, SON (alt), brother, equal, REPR (equal));
        SET (equal, rule_used);
        SON (alt) = brother;
        NODENAME (term) = affixnt;
        BROTHER (term) = nil;
        REPR (term) = name;
        LEFTDEF (term) = -1;
      }
  }
}

char *get_ie ()
{
  if (unique_e_count > 9)
  {
    char *name;
    name = &chartable[++charindex];
    (void) sprintf (&chartable[charindex], "E_%d", unique_e_count++);
    charindex += 10;
    if (charindex > maxchars)
      alloc_chartable ();
    return name;
  }
  return small_ie[unique_e_count++];
}

rightsie (alt, member, affix)
int alt, member, affix;
{
  int mem, afx;
  if (APPLY_BOUND_AFFIX (SON (affix)))
  {
    for (afx = AFFIXDEF (alt); afx != nil; afx = BROTHER (afx))
    {
      if (INHERITED (afx))
        if (REPR (SON (afx)) == REPR (SON (affix)))
        {
          if (DEF (member) == nestarset)
          {
            DEF (member) = metaterminal;
            REPR (member) = REPR (metaterminal);
            NODENAME (affix) = inherited;
            SET (metaterminal, rule_used);
          }
          else
          {
            char *name;
            name = get_ie ();
            REPR (SON (affix)) = name;
            LEFTDEF (SON (affix)) = -1;
            newnode (inherited, nil, SON (afx), "(nil)");
            newnode (inherited, brother, SON (affix), "(nil)");
            newdefnode (ntnode, BROTHER (member), brother, equal, REPR (equal));
            SET (equal, rule_used);
            BROTHER (member) = brother;
            return;
          }
        }
    }
    for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
      for (afx = AFFIXTREE (mem); afx != nil; afx = BROTHER (afx))
      {
        if (afx == affix)
          return;
        if ((NODENAME (afx) == derived)
            && (NODENAME (SON (afx)) == affixnt) && (REPR (SON (afx)) == REPR (SON (affix))))
        {
          if (DEF (member) == nestarset)
          {
            DEF (member) = metaterminal;
            REPR (member) = REPR (metaterminal);
            NODENAME (affix) = inherited;
            SET (metaterminal, rule_used);
          }
          else
          {
            char *name;
            name = get_ie ();
            REPR (SON (affix)) = name;
            LEFTDEF (SON (affix)) = -1;
            newnode (inherited, nil, SON (afx), "(nil)");
            newnode (inherited, brother, SON (affix), "(nil)");
            newdefnode (ntnode, BROTHER (member), brother, equal, REPR (equal));
            SET (equal, rule_used);
            BROTHER (member) = brother;
            return;
          }
        }
      }
  }
  else
  {
    char *name;
    int term = SON (affix), atype = NODENAME (term);
    name = get_ie ();
    newnode (atype, BROTHER (term), nil, REPR (term));
    newnode (inherited, nil, brother, "(nil)");
    newnode (inherited, brother, term, "(nil)");
    newdefnode (ntnode, BROTHER (member), brother, equal, REPR (equal));
    SET (equal, rule_used);
    BROTHER (member) = brother;
    NODENAME (term) = affixnt;
    BROTHER (term) = nil;
    REPR (term) = name;
    LEFTDEF (term) = -1;
  }
}

int right_implicite_intersect (alt, member, affix)
int alt, member, affix;
{
  int mem, afx;
  for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
    for (afx = AFFIXTREE (mem); afx != nil; afx = BROTHER (afx))
    {
      if (afx == affix)
        return false;
      if ((LATTICE (afx)) && (REPR (SON (afx)) == REPR (SON (affix))))
      {
        char *name;
        name = get_ie ();
        REPR (SON (affix)) = name;
        LEFTDEF (SON (affix)) = -1;
        newnode (inherited, nil, SON (afx), "(nil)");
        newnode (inherited, brother, SON (affix), "(nil)");
        newdefnode (ntnode, BROTHER (member), brother, intersect, REPR (intersect));
        SET (intersect, rule_used);
        BROTHER (member) = brother;
        return true;
      }
    }
  return false;
}

right_explicite_intersect (member, affix)
int member, affix;
{
  if (lattice_top (affix))
    return;
  newnode (affixtm, nil, nil, hex_repr (NODENAME (LATTICE_DEF (SON (affix)))));
  newnode (inherited, nil, brother, "(nil)");
  newnode (inherited, brother, SON (affix), "(nil)");
  newdefnode (ntnode, BROTHER (member), brother, explintersect, REPR (explintersect));
  SET (explintersect, rule_used);
  BROTHER (member) = brother;
}

char *hex_repr (repr)
int repr;
{
  char *name;
  name = &chartable[++charindex];
  (void) sprintf (&chartable[charindex], "%d", repr);
  charindex += 20;
  if (charindex > maxchars)
    alloc_chartable ();
  return name;
}

affixuse ()
{
  register int term, mem, affix;
  int rule, alt, count, afx, optimized = 0;
  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
  {
    sum = init_sum;
    for (alt = SON (rule); alt != nil; SUM_ALT, alt = BROTHER (alt))
    {
      count = 0;
      for (afx = AFFIXDEF (alt); afx != nil; afx = BROTHER (afx))
      {
        register int trm = SON (afx);
        if (REPR (trm) == dont_care)
        {
          DONTCARE (trm);
          SUM_DC;
          continue;
        }
        for (; trm != nil; trm = BROTHER (trm))
          if (NODENAME (trm) == superaffix)
          {
            NODENAME (trm) = affixnt;
            LEFTDEF (trm) = -1;
          }
        trm = SON (afx);
        if ((BROTHER (trm) == nil) && (NODENAME (trm) == affixnt))
        {
          int occurs_after = false;
          if (INHERITED (afx))
          {
            check_case (rule, trm);
            LEFTDEF (trm) = count;
          }
          for (affix = AFFIXDEF (alt); affix != nil; affix = BROTHER (affix))
          {
            term = SON (affix);
            if (BROTHER (term) != nil)
            {
              for (; term != nil; term = BROTHER (term))
                if ((NODENAME (trm) == affixnt) && (REPR (term) == REPR (trm)) && (term != trm))
                {
                  LEFTDEF (term) = count;
                  optimized += 1;
                  SUM_LD;
                }
            }
            else if ((DERIVED (affix)) && (INHERITED (afx)) && (REPR (term) == REPR (trm)))
            {
              LEFTDEF (term) = count;
              optimized += 1;
              SUM_LD;
            }
            else if ((DERIVED (affix)) && REPR (term) == REPR (trm) &&  occurs_after)
            {
              LEFTDEF (term) = count;
              LEFTDEF (trm) = count;
              optimized += 1;
              SUM_LD;
            }
            if (afx == affix) 
               occurs_after = true;
          }
          for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
            for (affix = AFFIXTREE (mem); affix != nil; affix = BROTHER (affix))
            {
              for (term = SON (affix); term != nil; term = BROTHER (term))
                if (NODENAME (term) == superaffix)
                {
                  NODENAME (term) = affixnt;
                  LEFTDEF (term) = -1;
                  SUM_NOLD;
                }
              for (term = SON (affix); term != nil; term = BROTHER (term))
                if ((NODENAME (trm) == affixnt) && (REPR (term) == REPR (trm)))
                  if ((!IS_LEFTDEF (term)) || (INHERITED (afx)))
                  {
                    LEFTDEF (term) = count;
                    LEFTDEF (trm) = count;
                    optimized += 1;
                    SUM_LD;
                  }
            }
        }
        count += 1;
      };
      mem_dont_care_affixes (SON (alt));
      alt_not_used_affixes (rule, alt);
      alt_checksum_afx_terms (alt);
    }
    SUM (rule) = sum;
    sum = init_sum;
  }
  if (verbose_flag)
  {
    fprintf (stderr, "glammar optimizer: %d left defined affixes used stand alone in right positions.\n", optimized);
    fprintf (stderr, "glammar optimizer: %d affixes marked don't care.\n", dc_count);
  }
}

static int a_s, m_s;
alt_checksum_afx_terms (alt)
int alt;
{
  int mem, affix, term;
  a_s = 0;
  for (affix = AFFIXDEF (alt); affix != nil; SUM_AFX, affix = BROTHER (affix))
    for (term = SON (affix); term != nil; SUM_TERM, term = BROTHER (term))
    {
      a_s += 1;
      sum += affixtm_sum (REPR (term));
      if (NODENAME (term) == affixtm)
        sum += 1129;
    }
  m_s = 0;
  for (mem = SON (alt); mem != nil; SUM_MEM, mem = BROTHER (mem))
  {
    m_s += 1;
    if (TERMINAL (mem))
      sum += 321;
    else if (MARKED (DEF (mem), deterministic))
      m_s += 17;
    sum -= affixtm_sum (REPR (mem));
    a_s = 0;
    for (affix = AFFIXTREE (mem); affix != nil; SUM_AFX, affix = BROTHER (affix))
      for (term = SON (affix); term != nil; SUM_TERM, term = BROTHER (term))
      {
        a_s += 1;
        sum += affixtm_sum (REPR (term));
        if (NODENAME (term) == affixtm)
          sum += 871;
      }
  }
}

affixtm_sum (repr)
register char *repr;
{
  register int r = m_s + a_s, pos = r;
  while (*repr != '\0')
    r += (*repr++ + m_s + a_s) << (pos++ & 15);
  return r & 32767;
}

mem_dont_care_affixes (mem)
register int mem;
{
  register int afx;
  for (; mem != nil; SUM_MEM, mem = BROTHER (mem))
    for (afx = AFFIXTREE (mem); afx != nil; SUM_AFX, afx = BROTHER (afx))
      if (REPR (SON (afx)) == dont_care)
      {
        SUM_DC;
        DONTCARE (SON (afx));
      }
}

alt_not_used_affixes (rule, alt)
int rule;
int alt;
{
  register int mem, afx, trm, term;
  for (mem = SON (alt); mem != nil; mem = BROTHER (mem))

/*     if (TERMINAL(mem) || DEF(mem) <= laststdpred) */
    for (afx = AFFIXTREE (mem); afx != nil; afx = BROTHER (afx))
    {
      if (DERIVED (afx) && NOT_IS_LEFTDEF (SON (afx)))
      {
        term = SON (afx);
        if (affix_not_used_in_lhs (AFFIXDEF (alt), REPR (term)) && affix_not_used_in_rhs (SON (alt), REPR (term)))
        {
          DONTCARE (term);
          if (REPR (term)[1] != '_')
          {
            if (verbose_flag)
              fprintf (stderr,
                       "line %ld: (hint):affix %s in %s.%s not used\n",
                       LINE (alt), FREPR (term), FREPR (rule), FREPR (mem));
          }
          dc_count += 1;
          SUM_DC;
        }
      }
      if (DERIVED (afx))
        check_case (rule, SON (afx));
    }
}

affix_not_used_in_lhs (afx, repr)
register int afx;
register char *repr;
{
  register int term;
  for (; afx != nil; afx = BROTHER (afx))
    if (DERIVED (afx))
      for (term = SON (afx); term != nil; SUM_TERM, term = BROTHER (term))
        if (REPR (term) == repr)
          return false;
  return false;
}

affix_not_used_in_rhs (mem, repr)
register int mem;
register char *repr;
{
  register int afx, term;
  for (; mem != nil; mem = BROTHER (mem))
    for (afx = AFFIXTREE (mem); afx != nil; afx = BROTHER (afx))
      if (INHERITED (afx))
        for (term = SON (afx); term != nil; term = BROTHER (term))
          if (REPR (term) == repr)
            return false;

/*   return true; */
  return false;
}

copyafx (afx)
int afx;
{
  if (afx == nil)
    return;
  copyafx (BROTHER (afx));
  if (INHERITED (afx))
  {
    newnode (affixtm, brother, nil, ") ");
    copyterm (SON (afx));
    newnode (affixtm, brother, nil, " (");
  }
}

copyterm (term)
int term;
{
  if (term == nil)
    return;
  copyterm (BROTHER (term));
  newnode (NODENAME (term), brother, nil, REPR (term));
}

check_case (int rule, int term)
{
  char *x = REPR (term);
  while (*x)
    if (*x == '_' || islower (*x))
      return;
    else
      x++;
  if (input_from_partlist)
    fprintf (stderr, "In %s:\n", PART (rule));
  fprintf (stderr, "line %ld: `%s': affix `%s' is not meta (hint)\n", LINE (term), FREPR (rule), FREPR (term));
}
