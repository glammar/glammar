
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 

/* file  : transform defining affix expressions */
#include "gg1.h"
#include "gg2.h"
int lastmem, prevmem, unique_eag_count = 0,
                                        total_eag_count = 0,
                                        unique_list_count = 0,
                                        total_list_count = 0,
                                        unique_empty_count = 0, where_count = 0, pair_count = 0, prev_source;

char *gterm;

char *get_L ();
char *get_G ();
char *get_GG ();

char small_L[10][4] = {
  "L_0",
  "L_1",
  "L_2",
  "L_3",
  "L_4",
  "L_5",
  "L_6",
  "L_7",
  "L_8",
  "L_9",
};

char small_GG[10][5] = {
  "GG_0",
  "GG_1",
  "GG_2",
  "GG_3",
  "GG_4",
  "GG_5",
  "GG_6",
  "GG_7",
  "GG_8",
  "GG_9",
};


char *get_L ()
{
  if (unique_list_count > 9)
  {
    char *name;
    name = &chartable[++charindex];
    (void) sprintf (&chartable[charindex], "L_%d", unique_list_count++);
    charindex += 10;
    if (charindex > maxchars)
      alloc_chartable ();
    return name;
  }
  return small_L[unique_list_count++];
}

char *get_GG ()
{
  if (unique_eag_count > 9)
  {
    char *name;
    name = &chartable[++charindex];
    (void) sprintf (&chartable[charindex], "GG_%d", unique_eag_count++);
    charindex += 10;
    if (charindex > maxchars)
      alloc_chartable ();
    return name;
  }
  return small_GG[unique_eag_count++];
}

eag ()
{
  int rule, alt;


  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
    for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
      alt_eag (alt);
  if (verbose_flag)
    fprintf (stderr, "glammar transformation fase: %d defining expressions resolved.\n", total_eag_count);
  delete_superaffix_productions ();
}

delete_superaffix_productions ()
{
  int this, prev, super_rules = 0;


  for (; lastmetarule != nil; lastmetarule = BROTHER (lastmetarule))
    if (NODENAME (SON (lastmetarule)) != supernt)
      break;
  prev = lastmetarule;
  if (prev != nil)
    for (this = BROTHER (prev); this != nil; this = BROTHER (prev))
    {
      if (NODENAME (SON (this)) == supernt)
      {
        BROTHER (prev) = BROTHER (this);
        super_rules += 1;
      }
      else
        prev = this;
    }
  if (verbose_flag)
    fprintf (stderr, "glammar transformation fase: %d super affixes deleted\n", super_rules);
}

alt_eag (alt)
int alt;
{
  int member;

  unique_eag_count = 0;
  lefths_eag (alt);
  for (member = SON (alt); member != nil; member = BROTHER (member))
    rights_eag (member);
  total_eag_count += unique_eag_count;
}


lefths_eag (alt)
int alt;
{
  int affix;

  for (affix = AFFIXDEF (alt); affix != nil; affix = BROTHER (affix))
    if (NODENAME (affix) == inherited)
    {
      if ((BROTHER (SON (affix)) != nil) || (NODENAME (SON (affix)) == superaffix))
      {
        char *name;
        int lbrother, llbrother;

        name = get_GG ();
        gterm = name + 1;
        brother = SON (alt);
        defining_expr (SON (affix));
        REPR (SON (affix)) = name;
        LEFTDEF (SON (affix)) = -1;
        BROTHER (SON (affix)) = nil;
        NODENAME (SON (affix)) = affixnt;
        lbrother = brother;
        newnode (affixnt, nil, nil, name + 1);
        newnode (derived, nil, brother, "(nil)");
        llbrother = brother;
        newnode (affixnt, nil, nil, name);
        newnode (inherited, llbrother, brother, "(nil)");
        newdefnode (ntnode, lbrother, brother, setinputptrto, REPR (setinputptrto));
        FLAG_SET (brother, redirected_input);
        SET (setinputptrto, rule_used);
        SON (alt) = brother;
      }
      /* delete_superaffixes_on_applying_positions */
    }
    else if (NODENAME (SON (affix)) == superaffix)
    {
      LEFTDEF (SON (affix)) = -1;
      NODENAME (SON (affix)) = affixnt;
    }
}


rights_eag (mem)
int mem;
{
  int affix;
  char *name;
  int lbrother, llbrother;

  if (REPR (mem) == redirect)
  {
    int m = mem;
    int b;
    name = get_GG ();
    brother = BROTHER (m);
    newnode (affixnt, nil, nil, name + 1);
    LEFTDEF (brother) = -1;
    newnode (derived, nil, brother, "(nil)");
    affix = AFFIXTREE (m);
    BROTHER (affix) = brother;
    REPR (m) = REPR (setinputptrto);
    FLAG_SET (m, redirected_input);
    DEF (m) = setinputptrto;
    SET (setinputptrto, rule_used);

    m = BROTHER (m);
    b = BROTHER (m);

    newnode (affixnt, nil, nil, name + 1);
    newnode (inherited, nil, brother, "(nil)");
    newdefnode (ntnode, b, brother, resetinputptr, REPR (resetinputptr));
    FLAG_SET (brother, redirected_input);
    SET (resetinputptr, rule_used);
    BROTHER (m) = brother;
    FLAG_SET (m, redirected_input);
  }

  for (affix = AFFIXTREE (mem); affix != nil; affix = BROTHER (affix))
    if (NODENAME (affix) == derived)
    {
      if ((BROTHER (SON (affix)) != nil) || (NODENAME (SON (affix)) == superaffix))
      {

        name = get_GG ();
        gterm = name + 1;
        brother = BROTHER (mem);
        defining_expr (SON (affix));
        REPR (SON (affix)) = name;
        LEFTDEF (SON (affix)) = -1;
        BROTHER (SON (affix)) = nil;
        NODENAME (SON (affix)) = affixnt;
        lbrother = brother;
        newnode (affixnt, nil, nil, name + 1);
        newnode (derived, nil, brother, "(nil)");
        llbrother = brother;
        newnode (affixnt, nil, nil, name);
        newnode (inherited, llbrother, brother, "(nil)");
        newdefnode (ntnode, lbrother, brother, setinputptrto, REPR (setinputptrto));
        FLAG_SET (brother, redirected_input);
        SET (resetinputptr, rule_used);
        BROTHER (mem) = brother;
        mem = lastmem;
      }
      /* delete_superaffixes_on_applying_positions */
    }
    else if (NODENAME (SON (affix)) == superaffix)
    {
      LEFTDEF (SON (affix)) = -1;
      NODENAME (SON (affix)) = affixnt;
    }
}


defining_expr (term)
int term;
{
  if (term == nil)
    return;
  if (BROTHER (term) == nil)
  {
    if (NODENAME (term) == metaffix)
      last_meta_constant (term);
    else if (NODENAME (term) == superaffix)
      last_super_hyper (term);
    else if (NODENAME (term) == affixtm)
      last_constant (term);
    else
      last_free (term);
    return;
  }
  defining_expr (BROTHER (term));
  if (NODENAME (term) == metaffix)
    meta_constant (term);
  else if (NODENAME (term) == affixtm)
    constant (term);
  else if (NODENAME (term) == superaffix)
    super_hyper (term);
  else
  {
    int bterm = BROTHER (term);
    if (NODENAME (bterm) == metaffix || NODENAME (bterm) == affixtm)
    {
      det_free_meta (term);
    }
    else
      free_meta (term);
  }
  return;
}


constant (term)
int term;
{
  newnode (tnode, brother, nil, REPR (term));
  FLAG_SET (brother, redirected_input);
}


last_constant (term)
int term;
{
  int lbrother = brother;

  newnode (affixnt, nil, nil, gterm);
  newnode (inherited, nil, brother, "(nil)");
  newdefnode (ntnode, lbrother, brother, resetinputptr, REPR (resetinputptr));
  FLAG_SET (brother, redirected_input);
  SET (resetinputptr, rule_used);
  lastmem = brother;
  newnode (tnode, brother, nil, REPR (term));
  FLAG_SET (brother, redirected_input);
}


det_free_meta (term)
int term;
{
  int ab, lbrother = brother;
  int replace = detnestarset;
  int bt = BROTHER (term);

  if (REPR (term) && (*(REPR (term)) == 'L') && (*(REPR (term) + 1) == '_'))
    return free_meta (term);

  newnode (NODENAME (bt), nil, nil, REPR (bt));
  newnode (inherited, nil, brother, "(nil)");
  ab = brother;
  newnode (affixnt, nil, nil, REPR (term));
  newnode (derived, ab, brother, "(nil)");
  newdefnode (ntnode, lbrother, brother, replace, REPR (replace));
  FLAG_SET (brother, redirected_input);
  SET (replace, rule_used);
}

free_meta (term)
int term;
{
  int lbrother = brother;
  int replace = nestarset;

  if (REPR (term) && (*(REPR (term)) == 'L') && (*(REPR (term) + 1) == '_'))
    replace = getlist_;

  newnode (affixnt, nil, nil, REPR (term));
  newnode (derived, nil, brother, REPR (term));
  newdefnode (ntnode, lbrother, brother, replace, REPR (replace));
  FLAG_SET (brother, redirected_input);
  SET (replace, rule_used);
}


last_free (term)
int term;
{
  int lbrother = brother, ab;

  newnode (affixnt, nil, nil, gterm);
  newnode (inherited, nil, brother, "(nil)");
  ab = brother;
  newnode (affixnt, nil, nil, REPR (term));
  newnode (derived, ab, brother, "(nil)");
  newdefnode (ntnode, lbrother, brother, nestaralt, REPR (nestaralt));
  FLAG_SET (brother, redirected_input);
  lastmem = brother;
  SET (nestaralt, rule_used);
}

meta_constant (term)
int term;
{
  int lbrother = brother;

  newnode (metaffix, nil, nil, REPR (term));
  newnode (inherited, nil, brother, REPR (term));
  newdefnode (ntnode, lbrother, brother, metaterminal, REPR (metaterminal));
  FLAG_SET (brother, redirected_input);
  SET (resetinputptr, rule_used);
  SET (metaterminal, rule_used);
}


last_meta_constant (term)
int term;
{
  int lbrother = brother;

  newnode (affixnt, nil, nil, gterm);
  newnode (inherited, nil, brother, "(nil)");
  newdefnode (ntnode, lbrother, brother, resetinputptr, REPR (resetinputptr));
  FLAG_SET (brother, redirected_input);
  SET (resetinputptr, rule_used);
  lastmem = brother;
  lbrother = brother;
  newnode (metaffix, nil, nil, REPR (term));
  newnode (inherited, nil, brother, "(nil)");
  newdefnode (ntnode, lbrother, brother, metaterminal, REPR (metaterminal));
  FLAG_SET (brother, redirected_input);
  SET (resetinputptr, rule_used);
  SET (metaterminal, rule_used);
}

super_hyper (term)
int term;
{
  int lbrother = brother;

  newnode (affixnt, nil, nil, REPR (term));
  newnode (derived, nil, brother, REPR (term));
  newdefnode (ntnode, lbrother, brother, DEF (term), REPR (DEF (term)));
  FLAG_SET (brother, redirected_input);
  SET (DEF (term), rule_used);
  if (!MARKED (DEF (term), docompile))
    if (!MARKED (DEF (term), external))
    {
      fprintf (stderr, "%s: super meta-notion not defined as external.\n", FREPR (DEF (term)));
      exit (-1);
    }
}

last_super_hyper (term)
int term;
{
  int lbrother = brother;

  newnode (affixnt, nil, nil, gterm);
  newnode (inherited, nil, brother, "(nil)");
  newdefnode (ntnode, lbrother, brother, resetinputptr, REPR (resetinputptr));
  FLAG_SET (brother, redirected_input);
  lastmem = brother;
  lbrother = brother;
  newnode (affixnt, nil, nil, REPR (term));
  newnode (derived, nil, brother, "(nil)");
  newdefnode (ntnode, lbrother, brother, DEF (term), REPR (DEF (term)));
  FLAG_SET (brother, redirected_input);
  SET (DEF (term), rule_used);
  if (!MARKED (DEF (term), docompile))
    if (!MARKED (DEF (term), external))
    {
      fprintf (stderr, "%s: super meta-notion not defined as external.\n", FREPR (DEF (term)));
      exit (-1);
    }
}

char *repr_empty;
list ()
{
  int rule, alt;
  for (rule = lastmetarule; (rule != nil) && (!mystrcmp (REPR (rule), "empty")); rule = BROTHER (rule));
  if (rule == nil)
  {
    fprintf (stderr, "glammar instalation error: empty missing\n");
    /* exit(12); */
    repr_empty = "-1";

  }
  else
    repr_empty = REPR (rule);


  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
  {
    for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
      alt_list (alt);
  }
  if (verbose_flag)
    fprintf (stderr, "glammar transformation fase: %d compositions resolved.\n", total_list_count);
  if (verbose_flag)
    fprintf (stderr, "glammar optimization fase: %d empty notions deleted.\n", unique_empty_count);
}


alt_list (alt)
int alt;
{
  int member;

  unique_list_count = 0, lefths_list (alt);
  prevmem = nil;
  lastmem = alt;
  for (member = SON (alt); member != nil; member = BROTHER (member))
  {
    rights_list (member);
    prevmem = member;
  }
  total_list_count += unique_list_count;
}


lefths_list (alt)
int alt;
{
  int affix, last_mem, term;

  for (affix = AFFIXDEF (alt); affix != nil; affix = BROTHER (affix))
  {
    delete_multiple_empty_terms (SON (affix));
    for (term = SON (affix); term != nil; term = BROTHER (term))
      if ((NODENAME (term) == factor) && (NODENAME (affix) == inherited))
      {
        NODENAME (term) = affixnt;
        REPR (term) = get_L ();
        brother = SON (alt);
        resolve_list (SON (term), REPR (term), 1);
        SON (alt) = brother;
      }
  }
  if (SON (alt) == nil)
    last_mem = nil;
  else
  {
    int mem;
    for (mem = SON (alt); BROTHER (mem) != nil; mem = BROTHER (mem));
    last_mem = mem;
  }
  brother = nil;
  for (affix = AFFIXDEF (alt); affix != nil; affix = BROTHER (affix))
    for (term = SON (affix); term != nil; term = BROTHER (term))
      if ((NODENAME (term) == factor) && (NODENAME (affix) == derived))
      {
        NODENAME (term) = affixnt;
        REPR (term) = get_L ();
        resolve_list (SON (term), REPR (term), 0);
      }
  if (last_mem == nil)
    SON (alt) = brother;
  else
    BROTHER (last_mem) = brother;
}

delete_multiple_empty_terms (prev)
int prev;
{
  int term = BROTHER (prev), next;
  if (term == nil)
    return;

  next = BROTHER (term);
  for (; next != nil; prev = term, term = next, next = BROTHER (next))
    if (REPR (term) == repr_empty)
    {
      BROTHER (prev) = next;
      unique_empty_count += 1;
      term = prev;
    }
}

rights_list (mem)
int mem;
{
  int affix, term;

  for (affix = AFFIXTREE (mem); affix != nil; affix = BROTHER (affix))
    for (term = SON (affix); term != nil; term = BROTHER (term))
      if (NODENAME (term) == factor)
      {
        NODENAME (term) = affixnt;
        REPR (term) = get_L ();
        if (NODENAME (affix) == derived)
          brother = BROTHER (mem);
        else
          brother = mem;
        resolve_list (SON (term), REPR (term), NODENAME (affix) == derived);
        if (NODENAME (affix) == derived)
        {
          BROTHER (mem) = brother;
        }
        else
        {
          if (prevmem == nil)
            SON (lastmem) = brother;
          else
            BROTHER (prevmem) = brother;
          for (prevmem = brother; BROTHER (prevmem) != mem; prevmem = BROTHER (prevmem));

        }
      }
}


make_pair_node (fact, lterm, llterm)
int fact;
char *lterm, *llterm;
{
  int lbrother = brother, ab;

  newnode (affixnt, nil, nil, lterm);
  newnode (inherited, nil, brother, "(nil)");
  ab = brother;
  newnode (NODENAME (fact), nil, nil, REPR (fact));
  newnode (inherited, ab, brother, "(nil)");
  ab = brother;
  newnode (affixnt, nil, nil, llterm);
  newnode (derived, ab, brother, "(nil)");
  newnode (ntnode, lbrother, brother, REPR (pair));
}


make_unpair_node (fact, lterm, llterm)
int fact;
char *lterm, *llterm;
{
  int lbrother = brother, ab;

  newnode (affixnt, nil, nil, lterm);
  newnode (derived, nil, brother, "(nil)");
  ab = brother;
  newnode (NODENAME (fact), nil, nil, REPR (fact));
  newnode (derived, ab, brother, "(nil)");
  ab = brother;
  newnode (affixnt, nil, nil, llterm);
  newnode (inherited, ab, brother, "(nil)");
  newnode (ntnode, lbrother, brother, REPR (unpair));
}


make_last_pair_node (fact, bfact, lterm)
int fact, bfact;
char *lterm;
{
  int lbrother = brother, ab;

  newnode (NODENAME (bfact), nil, nil, REPR (bfact));
  newnode (inherited, nil, brother, "(nil)");
  ab = brother;
  newnode (NODENAME (fact), nil, nil, REPR (fact));
  newnode (inherited, ab, brother, "(nil)");
  ab = brother;
  newnode (affixnt, nil, nil, lterm);
  newnode (derived, ab, brother, "(nil)");
  newnode (ntnode, lbrother, brother, REPR (pair));
}


make_last_unpair_node (fact, bfact, lterm)
int fact, bfact;
char *lterm;
{
  int lbrother = brother, ab;

  newnode (NODENAME (bfact), nil, nil, REPR (bfact));
  newnode (derived, nil, brother, "(nil)");
  ab = brother;
  newnode (NODENAME (fact), nil, nil, REPR (fact));
  newnode (derived, ab, brother, "(nil)");
  ab = brother;
  newnode (affixnt, nil, nil, lterm);
  newnode (inherited, ab, brother, "(nil)");
  newnode (ntnode, lbrother, brother, REPR (unpair));
}


resolve_list (fact, lterm, defining)
int fact, defining;
char *lterm;
{
  if (BROTHER (BROTHER (fact)) == nil)
  {
    if (defining)
      make_last_unpair_node (fact, BROTHER (fact), lterm);
    else
      make_last_pair_node (fact, BROTHER (fact), lterm);
  }
  else
  {
    char *llterm;

    llterm = get_L ();
    resolve_list (BROTHER (fact), llterm, defining);
    if (defining)
      make_unpair_node (fact, llterm, lterm);
    else
      make_pair_node (fact, llterm, lterm);
  }
}


wheres ()
{
  int rule, alt;

  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
    for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
    {
      alt_wheres (alt);
    }
  if (verbose_flag)
    fprintf (stderr, "glammar transformation fase: %d pairs re-arranged.\n", pair_count);
  if (verbose_flag)
    fprintf (stderr, "glammar optimization fase: %d wheres eliminated.\n", where_count);
}


alt_wheres (alt)
int alt;
{
  int member;

  prevmem = nil;
  lastmem = alt;
  for (member = SON (alt); member != nil; member = BROTHER (member))
  {
    if ((DEF (member) == where) && (no_affix_expressions (AFFIXTREE (member))))
    {
      resolve_where (alt, member);
      where_count += 1;
      if (prevmem == nil)
        SON (alt) = BROTHER (member);
      else
        BROTHER (prevmem) = BROTHER (member);
    }
    else
      prevmem = member;
  }
}

/* Optimizing defining mixed composition expression 
 * I.e 
     lhs(>a*b+c):
 * This is rewritten by (list removal) to 
     lhs(>L_0+c):  
        unpair(>L_0,a>,b>).
 * and after defining expression elimination:
    lhs(>GG_0):  
       setinputptrto_(>GG_0,G_0>),
       nestarset(L_0>),
       nestaralt_(c>,>G_0),
       unpair(>L_0,a>,b>).
 * Since nestarset is a non-deterministic builtin grammars
 * using this construction are non-deterministic also.
 * This is why we rename members of the form nestarset(L_>) to 
 * getlist_(L_0>)
*/

alt_opt_def_mixed_comp (alt)
int alt;
{
  int member;

  prevmem = nil;
  lastmem = alt;
  for (member = SON (alt); member != nil; member = BROTHER (member))
  {
    if ((DEF (member) == nestarset) && (no_affix_expressions (AFFIXTREE (member))))
    {
      resolve_where (alt, member);
      where_count += 1;
      if (prevmem == nil)
        SON (alt) = BROTHER (member);
      else
        BROTHER (prevmem) = BROTHER (member);
    }
    else
      prevmem = member;
  }
}


no_affix_expressions (affix)
int affix;
{
  return (BROTHER (SON (affix)) == nil) && (NODENAME (SON (affix)) ==
                                            affixnt) && (BROTHER (SON (BROTHER (affix))) == nil);
}


resolve_where (alt, wher)
int alt, wher;
{
  int a1 = AFFIXTREE (wher);
  int a2 = BROTHER (a1);

  char *source = REPR (SON (a2));
  char *destination = REPR (SON (a1));

  int affix, term, mem;

  for (affix = AFFIXDEF (alt); affix != nil; affix = BROTHER (affix))
    for (term = SON (affix); term != nil; term = BROTHER (term))
      if (REPR (term) == source)
        REPR (term) = destination;
  for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
    for (affix = AFFIXTREE (mem); affix != nil; affix = BROTHER (affix))
      for (term = SON (affix); term != nil; term = BROTHER (term))
        if (REPR (term) == source)
          REPR (term) = destination;
}


int lmem;
int pmem;

tail_rec_opt ()
{
  int rule;
  if (no_recursion_elm_flag)
    return;

  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
    tailrec_elimnation_move_pair_up (rule);
}

tailrec_elimnation_move_pair_up (rule)
int rule;
{
  int alt, afx;

#ifdef NVE
  for (afx = AFFIXDEF (SON (rule)); afx != nil; afx = BROTHER (afx))
  {
    if (DERIVED (afx))
      return;
  }
#endif
  for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
  {
    int mem;
    int afx;
    int nre = 0;
    lmem = nil;
    pmem = nil;

    tailrec_alt_lastmem (SON (alt));

    if (lmem == nil || pmem == nil || DEF (lmem) != rule)
      continue;


    /* check for affixes not passed in and used in inherited position */
    /* if so we cannot remove recursion */
    for (afx = AFFIXTREE (lmem); afx != nil; afx = BROTHER (afx))
    {
      if (INHERITED (afx))
      {
        int trm;
        for (trm = SON (afx); trm != nil; trm = BROTHER (trm))
        {
          if ((NOT_IS_LEFTDEF (trm)) && (NODENAME (trm) == affixnt))
          {
            FLAG_SET (alt, no_tail_recursion_opt_f);
            break;
          }
        }
      }
    }
    if (FLAG_MARKED (alt, no_tail_recursion_opt_f))
      continue;
    if (BROTHER (lmem) == nil)
      continue;

    for (mem = lmem; BROTHER (mem) != nil; mem = BROTHER (mem));

    BROTHER (pmem) = BROTHER (lmem);
    BROTHER (lmem) = nil;
    BROTHER (mem) = lmem;

    lmem = nil;
    pmem = nil;

    tailrec_alt_lastcut (SON (alt));

    if (lmem == nil || pmem == nil)
      continue;

    for (mem = lmem; BROTHER (mem) != nil; mem = BROTHER (mem));

    BROTHER (pmem) = BROTHER (lmem);
    BROTHER (lmem) = nil;
    BROTHER (mem) = lmem;


  }
}

tailrec_alt_lastmem (mem)
int mem;
{


  if (mem == nil)
    return;

  tailrec_alt_lastmem (BROTHER (mem));
  if (pmem != nil)
    return;
  if (lmem != nil)
  {
    pmem = mem;
    return;
  }

  if (DEF (mem) != pair && DEF (mem) != cut)
    lmem = mem;

  return;
}

tailrec_alt_lastcut (mem)
int mem;
{


  if (mem == nil)
    return;

  tailrec_alt_lastcut (BROTHER (mem));
  if (pmem != nil)
    return;
  if (lmem != nil)
  {
    pmem = mem;
    return;
  }

  if (DEF (mem) == cut)
    lmem = mem;

  return;
}


move_terms_up ()
{
  int rule, alt;

  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
    for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
    {
      move_unpair_up_mems (nil, nil, SON (alt), alt);
      move_terms_up_mems (nil, nil, SON (alt), alt);
    }
}

move_terms_up_mems (ob0, ob, mem, alt)
int ob0, ob, mem, alt;
{

  if (mem == nil)
    return;
  if (ob != nil
      && (NONTERMINAL (ob)
          || FLAG_MARKED (ob, redirected_input))
      && TERMINAL (mem)
      && !FLAG_MARKED (mem, redirected_input)
      && ((NONTERMINAL (ob) && (MARKED (DEF (ob), is_predicate))) || FLAG_MARKED (ob, redirected_input)))
  {
    if (verbose_flag)
      fprintf (stderr, "moving %s (%d) up over %s(%d) \n", REPR (mem), mem, REPR (ob), ob);
    BROTHER (ob) = BROTHER (mem);
    BROTHER (mem) = ob;
    if (ob0 == nil)
    {
      SON (alt) = mem;
      return;
    }
    else
      BROTHER (ob0) = mem;

    move_terms_up_mems (nil, nil, SON (alt), alt);
  }
  else
  {
    move_terms_up_mems (ob, mem, BROTHER (mem), alt);
  }
}

move_unpair_up_mems (ob0, ob, mem, alt)
int ob0, ob, mem, alt;
{

  if (mem == nil)
    return;
  if (ob != nil
      && DEF (mem) == unpair && DEF (ob) != unpair && term_defined_before (alt, ob, REPR (SON (AFFIXTREE (mem)))))
  {
    if (verbose_flag)
      fprintf (stderr, "moving %s (%d) up over %s(%d) \n", REPR (mem), mem, REPR (ob), ob);
    BROTHER (ob) = BROTHER (mem);
    BROTHER (mem) = ob;
    if (ob0 == nil)
    {
      SON (alt) = mem;
      return;
    }
    else
      BROTHER (ob0) = mem;

    move_unpair_up_mems (nil, nil, SON (alt), alt);
  }
  else
  {
    move_unpair_up_mems (ob, mem, BROTHER (mem), alt);
  }
}

int term_defined_before (alt, mem, term)
int alt, mem;
char *term;
{
  int m, a;
  for (m = SON (alt); m != mem; m = BROTHER (m))
    for (a = AFFIXTREE (m); a != nil; a = BROTHER (a))
      if (DERIVED (a) && REPR (SON (a)) == term)
        return true;

  for (a = AFFIXDEF (alt); a != nil; a = BROTHER (a))
    if (INHERITED (a) && REPR (SON (a)) == term)
      return true;
  return false;
}

final_nestarset_removal ()
{
  int rule, alt;

  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
    for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
      while (final_nestarset_removal_alt (nil, nil, SON (alt), alt))
        altdetnestarset (alt);
}

int final_nestarset_removal_alt (ob0, ob, mem, alt)
int ob0, ob, mem, alt;
{

  if (mem == nil)
    return 0;
  if (ob != nil && DEF (ob) == nestarset && (DEF (mem) == nestaralt || DEF (mem) == nestarset))
  {
    int m = SON (alt);
    int afx, trm;
    char *target = REPR (SON (AFFIXTREE (mem)));
    char *equal_target;
    int equal_mem;

    /* find equal target of source target y , we are looking to make the following deterministic

       # non deterministic t(>x+y+z, y+y>):^!!(y>). -> # non deterministic t(>GG_0,y+y>): setinputptrto_(>GG_0,G_0>),
       # redirected nestarset(x>), # redirected nestarset(y>), # redirected nestaralt_(z>,>G_0), # redirected ^!!
       (E_1>), equal(>E_1,>y). -> # non deterministic t(>GG_0,y+y>): ^!! (E_1>), # moved up
       setinputptrto_(>GG_0,G_0>), # redirected nestarset(x>), # redirected nestarset(y>), # redirected
       nestaralt_(z>,>G_0), # redirected equal(>E_1,>y). -> # non deterministic t(>GG_0,y+y>): ^!! (y>), # moved up
       setinputptrto_(>GG_0,G_0>), # redirected nestarset(x>), # redirected metaterminal(>y), # redirected
       nestaralt_(z>,>G_0).  # redirected -> # deterministic t(>GG_0,y+y>): ^!! (y>), # moved up
       setinputptrto_(>GG_0,G_0>), # redirected detnestar(x>,>y), # redirected metaterminal(>y), # redirected
       nestaralt_(z>,>G_0), # redirected

     */
    /* find equal target */
    for (m = SON (alt); m != nil; m = BROTHER (m))
    {
      int af = AFFIXTREE (m);
      char *t = REPR (SON (af));
      int found = false;
      if (DEF (m) == equal && (REPR (SON (BROTHER (af))) == target || t == target))
      {
        equal_target = t != target ? REPR (SON (af)) : REPR (SON (BROTHER (af)));
        equal_mem = m;
        for (m = SON (alt); m != mem; m = BROTHER (m))
        {
          for (afx = AFFIXTREE (m); afx != nil; afx = BROTHER (afx))
            if (NODENAME (afx) == derived)
            {
              if (REPR (SON (afx)) == equal_target)
              {
                found = true;
                break;
              }
            }
        }
        if (!found)
        {
          for (afx = AFFIXDEF (alt); afx != nil; afx = BROTHER (afx))
            if (NODENAME (afx) == inherited && REPR (SON (afx)) == equal_target)
            {
              found = true;
              break;
            }
        }
        if (found)
        {
          NODENAME (AFFIXTREE (mem)) = inherited;
          if (DEF (mem) == nestarset)
          {
            /* replace nestarset with metaterminal */
            REPR (mem) = REPR (metaterminal);
            DEF (mem) = metaterminal;
            SET (metaterminal, rule_used);
          }
          else
          {
            /* replace nestararalt with metaterminal2_ */
            REPR (mem) = REPR (metaterminal2);
            DEF (mem) = metaterminal2;
            SET (metaterminal2, rule_used);
          }
          remove_mem_in_alt (alt, equal_mem);
          {
            char *rt = REPR (SON (AFFIXTREE (equal_mem)));
            if (rt[0] == 'E' && rt[1] == '_')
              replace_term_in_alt (alt, rt, REPR (SON (BROTHER (AFFIXTREE (equal_mem)))));
            else
              replace_term_in_alt (alt, REPR (SON (BROTHER (AFFIXTREE (equal_mem)))), rt);

          }
          return 1;
        }
        return final_nestarset_removal_alt (ob, mem, BROTHER (mem), alt);
      }
    }


  }
  else
  {
    return final_nestarset_removal_alt (ob, mem, BROTHER (mem), alt);
  }
  return 0;
}

replace_term_in_alt (alt, fr, tr)
int alt;
char *fr, *tr;
{
  int m, t, a;
  if (verbose_flag)
    fprintf (stderr, " replacing %s with %s", fr, tr);
  for (m = SON (alt); m != nil; m = BROTHER (m))
    for (a = AFFIXTREE (m); a != nil; a = BROTHER (a))
      for (t = SON (a); t != nil; t = BROTHER (t))
        if (REPR (t) == fr)
          REPR (t) = tr;

  for (a = AFFIXDEF (alt); a != nil; a = BROTHER (a))
    for (t = SON (a); t != nil; t = BROTHER (t))
      if (REPR (t) == fr)
        REPR (t) = tr;
}

remove_mem_in_alt (alt, mem)
int alt, mem;
{
  int m;
  if (SON (alt) == nil)
    return;
  if (SON (alt) == mem)
  {
    SON (alt) = nil;
    return;
  }
  for (m = SON (alt); m != nil; m = BROTHER (m))
  {
    if (BROTHER (m) == mem)
    {
      BROTHER (m) = BROTHER (mem);
      return;
    }
  }
}
