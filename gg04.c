
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
long unique_e_count = 0, total_e_count = 0;
long dc_count = 0;
char *lran = "error_affiX";
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

long detnestars = 0;


/* exports
   void ie ();
   void altdetnestarset (long alt);
   void affixuse ();
   */

static void altie (long alt);
static void lefthsie (long alt);
static char *get_ie ();
static void rightsie (long alt, long member, long affix);
static long lhs_implicite_intersect (long alt, long affix);
static void lhs_explicite_intersect (long alt, long affix, long count);
static long right_implicite_intersect (long alt, long member, long affix);
static void right_explicite_intersect (long member, long affix);
static char *hex_repr (long repr);
static long alt_checksum_afx_terms (long alt);
static long affixtm_sum (char *repr);
static void mem_dont_care_affixes (long mem);
static void alt_not_used_affixes (long rule, long alt);
static long affix_not_used_in_lhs (long afx, char *repr);
static long affix_not_used_in_rhs (long mem, char *repr);
static void check_affix_case (long rule, long linenr, long term);

void print_rhs (long mem_item);
void ie () 
{
  long rule, alt;
  for (rule = root; rule != laststdpred; rule = BROTHER (rule))

    /*    if (MARKED(rule,docompile)) */
    for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
      altie (alt);
  if (verbose_flag)
    fprintf (stderr, "glammar transformation phase: %ld implied equals resolved\n", total_e_count);
  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
    for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
    {
      altdetnestarset (alt);
      altdetnestarset (alt);
    }
  if (verbose_flag)
    fprintf (stderr, "glammar transformation phase: %ld  phase 1 nestarsets replaced\n", detnestars);
}

void optimize_detnestarset (long alt)
{
  long member, bm, trm;
  for (member = SON (alt); member != nil; member = BROTHER (member))
  {
    if (DEF(member) != detnestarset)
      continue;
    for (bm = BROTHER(member), trm = SON(BROTHER(AFFIXTREE(member)));
	trm != nil;
	bm = BROTHER(member), trm = BROTHER(trm))
    { 
      BROTHER(member) = BROTHER(bm);
    }
  }
}

void altdetnestarset (long alt) 
{
  long member, affix, bt;
  long sb1 = nil;
  long sb2 = nil;
  long b3 = nil;
  for (member = SON (alt); member != nil; member = BROTHER (member))
  {
    sb1 = nil;
    sb2 = nil;
    b3 = nil;
    sb1 = BROTHER (member);
    if (sb1 != nil)
    {
      sb2 = BROTHER (sb1);
      if (sb2 != nil)
      {
	b3 = BROTHER (sb2);
      }
    }
    if (b3 != nil && (DEF (member) == setinputptrto) && (DEF (sb1) == detnestarset)
	&& (STRING (sb2) || DEF (sb2) == metaterminal) && (DEF (b3) == resetinputptr))
    {
      long af1, af2, af3;
      DEF (member) = detprefix;
      REPR (member) = REPR (detprefix);
      BROTHER (member) = BROTHER (b3);
      af1 = AFFIXTREE (member);
      af2 = AFFIXTREE (sb1);
      if (STRING (sb2))
      {
	newnode (affixtm, nil, nil, REPR (sb2));
      }
      else
      {
	long t = SON (AFFIXTREE (sb2));
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
    if (sb2 != nil && (DEF (member) == detnestarset)
	&& (STRING (sb1) || DEF (sb1) == metaterminal) && (DEF (sb2) == resetinputptr))
    {
      long af1, af2;
      DEF (member) = detprefix2;
      REPR (member) = REPR (detprefix2);
      BROTHER (member) = BROTHER (sb2);
      af2 = AFFIXTREE (member);
      af1 = AFFIXTREE (sb2);
      BROTHER (af1) = af2;
      AFFIXTREE (member) = af1;
      SET (detprefix2, rule_used);
      continue;
    }
    if (sb1 != nil && (DEF (member) == detnestarset) && (DEF (sb1) == equal))
    {
      long af1, t1;
      long bf1, bf2, s1;
      af1 = AFFIXTREE (member);
      t1 = SON (af1);
      bf1 = AFFIXTREE (sb1);
      bf2 = BROTHER (bf1);
      s1 = SON (bf1);
      if (REPR (t1) == REPR (s1))
      {
	DEF (member) = metaterminal;
	REPR (member) = REPR (metaterminal);
	AFFIXTREE (member) = bf2;
	BROTHER (member) = sb2;
	SET (metaterminal, rule_used);
      }
    }
    if (DEF (member) == equal)
    {
      long af1, af2, t1, t2;
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
    if (sb2 != nil && (DEF (member) == detnestarset)
	&& (STRING (sb1) || DEF (sb1) == metaterminal) && (STRING(sb2) || DEF (sb2) == metaterminal))
    {
      /*      Transform: 
       *      ..., detnestarset(p>,>":"), ":", metaterminal(>b), ..
       *       INTO
       *      ...  detnestarset(p>,>":"+b), ":", metaterminal(>b), ...
       */
      for (;  STRING(sb2) || DEF (sb2) == metaterminal && !FLAG_MARKED(sb2, metatermopt); sb2 = BROTHER(sb2) )
      {
	long af1, trm, trm2;
	af1 = BROTHER(AFFIXTREE (member)); 
	for (trm = SON(af1);  BROTHER(trm) != nil; trm = BROTHER(trm))
	  ;

	if (STRING(sb2))
	{
	  newnode(affixtm, nil, nil, REPR(sb2));
	}
	else
	{
	  trm2  = SON (AFFIXTREE(sb2));
	  newnode (NODENAME (trm2), nil, nil, REPR (trm2));
	}
	BROTHER(trm)  = brother;
	FLAG_SET(sb2, metatermopt);
      }
    }
  }
}

static void altie (long alt) 
{
  long member, affix;
  unique_e_count = 0;
  lefthsie (alt);
  for (member = SON (alt); member != nil; member = BROTHER (member))
    for (affix = AFFIXTREE (member); affix != nil; affix = BROTHER (affix))
      if (REPR (SON (affix)) == dont_care);
      else if (DERIVED (affix))
	rightsie (alt, member, affix);
      else if (0 && LATTICE (affix))
      {
	if (!right_implicite_intersect (alt, member, affix))
	  right_explicite_intersect (member, affix);
      }
  total_e_count += unique_e_count;
}

static void lefthsie (long alt) 
{
  long affix, count=0;
  for (affix = AFFIXDEF (alt); affix != nil; affix = BROTHER (affix),count++)
  {
    if (INHERITED (affix) && REPR (SON (affix)) != dont_care)
    {
      if (APPLY_BOUND_AFFIX (SON (affix)))
      {
	long afx;
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
	long term = SON (affix), atype = NODENAME (term);
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
    else if (LATTICE (affix))
    {
      if (!lhs_implicite_intersect (alt, affix))
	lhs_explicite_intersect (alt, affix,count);
    }
  }
}

static char *get_ie () 
{
  if (unique_e_count > 9)
  {
    char *name;
    name = &chartable[++charindex];
    (void) sprintf (&chartable[charindex], "E_%ld", unique_e_count++);
    charindex += 10;
    if (charindex > maxchars)
      alloc_chartable ();
    return name;
  }
  return small_ie[unique_e_count++];
}

static void rightsie (long alt, long member, long affix) 
{
  long mem, afx;
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
    long term = SON (affix), atype = NODENAME (term);
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

static long right_implicite_intersect (long alt, long member, long affix) 
{
  long mem, afx;
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

static void right_explicite_intersect (long member, long affix) 
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

static long lhs_implicite_intersect (long alt, long affix)
{
  long afx;
  for (afx = AFFIXDEF (alt); afx != nil; afx = BROTHER (afx))
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
      newdefnode (ntnode, SON (alt), brother, intersect, REPR (intersect));
      SET (intersect, rule_used);
      SON (alt) = brother;
      return true;
    }
  }
  return false;
}

static void lhs_explicite_intersect (long alt, long affix, long count) 
{
  long b;
  newnode (affixtm, nil, nil, hex_repr (NODENAME (LATTICE_DEF (SON (affix)))));
  newnode (inherited, nil, brother, "(nil)");
  b = brother;
  newnode (affixnt, nil, nil, REPR(SON(affix)));
  LEFTDEF(brother) = count;
  newnode (inherited, b, brother, "(nil)");
  newdefnode (ntnode, SON (alt), brother, explintersect, REPR (explintersect));
  SET (explintersect, rule_used);
  SON (alt) = brother;
}


static char *hex_repr (long repr) 
{
  char *name;
  name = &chartable[++charindex];
  (void) sprintf (&chartable[charindex], "%ld", repr);
  charindex += 20;
  if (charindex > maxchars)
    alloc_chartable ();
  return name;
}

void affixuse () 
{
  long term, mem, affix;
  long rule, alt, count, afx, optimized = 0;
  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
  {
    sum = init_sum;
    for (alt = SON (rule); alt != nil; SUM_ALT, alt = BROTHER (alt))
    {
      count = 0;
      for (afx = AFFIXDEF (alt); afx != nil; afx = BROTHER (afx))
      {
	long trm = SON (afx);
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
	    FLAG_SET (trm, wasmeta);
	  }
	trm = SON (afx);
	if ((BROTHER (trm) == nil) && (NODENAME (trm) == affixnt))
	{
	  long occurs_after = false;
	  if (INHERITED (afx))
	  {
	    check_affix_case (rule, LINE(alt),trm);
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
	    if (DEF(mem) != explintersect)
	      for (affix = AFFIXTREE (mem); affix != nil; affix = BROTHER (affix))
	      {
		for (term = SON (affix); term != nil; term = BROTHER (term))
		  if (NODENAME (term) == superaffix)
		  {
		    NODENAME (term) = affixnt;
		    LEFTDEF (term) = -1;
		    FLAG_SET (trm, wasmeta);
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
    fprintf (stderr, "glammar optimizer: %ld left defined affixes used stand alone in right positions.\n", optimized);
    fprintf (stderr, "glammar optimizer: %ld affixes marked don't care.\n", dc_count);
  }
}

static long a_s, m_s;

static long alt_checksum_afx_terms (long alt) 
{
  long mem, affix, term;
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
  return 0;
}

static long affixtm_sum (char *repr) 
{
  long r = m_s + a_s, pos = r;
  while (*repr != '\0')
    r += (*repr++ + m_s + a_s) << (pos++ & 15);
  return r & 32767;
}

static void mem_dont_care_affixes (long mem) 
{
  long afx;
  for (; mem != nil; SUM_MEM, mem = BROTHER (mem))
    for (afx = AFFIXTREE (mem); afx != nil; SUM_AFX, afx = BROTHER (afx))
      if (REPR (SON (afx)) == dont_care)
      {
	SUM_DC;
	DONTCARE (SON (afx));
      }
}

static void alt_not_used_affixes (long rule, long alt) 
{
  long mem, afx, term;
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
	check_affix_case (rule, LINE(alt),SON (afx));
    }
}

static long affix_not_used_in_lhs (long afx, char *repr) 
{
  long term;
  for (; afx != nil; afx = BROTHER (afx))
    if (DERIVED (afx))
      for (term = SON (afx); term != nil; SUM_TERM, term = BROTHER (term))
	if (REPR (term) == repr)
	  return false;
  return false;
}

static long affix_not_used_in_rhs (long mem, char *repr) 
{
  long afx, term;
  for (; mem != nil; mem = BROTHER (mem))
    for (afx = AFFIXTREE (mem); afx != nil; afx = BROTHER (afx))
      if (INHERITED (afx))
	for (term = SON (afx); term != nil; term = BROTHER (term))
	  if (REPR (term) == repr)
	    return false;

  /*   return true; */
  return false;
}

static void check_affix_case (long rule, long linenr, long term)
{
  char *x = REPR (term);
  if (FLAG_MARKED(term, wasmeta)) 
    return;
  while (*x)
    if (*x == '_' || islower (*x))
      return;
    else
      x++;
  if (input_from_partlist)
    fprintf (stderr, "In %s:\n", PART (rule));
  fprintf (stderr, "line %ld: `%s': all caps affix `%s' is not meta (hint)\n", linenr, FREPR (rule), FREPR (term));}
