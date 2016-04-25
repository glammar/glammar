
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 


/* file     general transactions on the data structure 
   * print back in glammar source
   * make a contex free grammar  : stripe affixes
   * make an identity grammar   : add some affixes
   * make a parse tree grammar   : add even more affixes
*/

#include "gg1.h"
#include "gg2.h"
static int do_trick = false;
printtree ()
{
  int rule_item;
  if (do_trick)
    trick ();
  printf ("last_stddef %d\n", laststdpred);
  for (rule_item = root; rule_item != laststdpred; rule_item = BROTHER (rule_item))
    if (!MARKED (rule_item, generated_rule))
      print_rule (rule_item);
  /* 
     for (rule_item = lastmetarule; rule_item != nil; rule_item = BROTHER(rule_item)) print_meta(rule_item); for
     (rule_item = first_lattice; rule_item != nil; rule_item = BROTHER(rule_item)) print_lattice (rule_item); */

}

print_meta (rule)
{
  int term, alt;

  printf ("%s :: ", FREPR (rule));

  switch (NODENAME (rule))
  {
  case meta_prod_rule:
  case One:
    print_meta_terms (SON (rule));
    break;
  case Mult:

    printf ("\n\t");
    for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
    {
      print_meta_terms (SON (alt));
      if (BROTHER (alt) != nil)
        printf (";\n\t");
    }
    break;
  default:
    printf ("unknown type\n");
  }
  printf (".\n\n");
}

print_meta_terms (term_item)
int term_item;
{
  int fact_item;
  for (; term_item != nil; term_item = BROTHER (term_item))
  {
    if ((NODENAME (term_item)) == supernt)
    {
      int lhs = DEF (term_item);
      printf (" @  %s", FREPR (lhs));
      printaffixtree (AFFIXTREE (term_item));
    }
    else if (NODENAME (term_item) == factor)
      for (fact_item = SON (term_item); fact_item != nil; fact_item = BROTHER (fact_item))
      {
        if (NODENAME (fact_item) == affixnt)
          printf ("%s", FREPR (fact_item));
        else
          printf ("\"%s\"", REPR (fact_item));
        if (BROTHER (fact_item) != nil)
          printf ("*");
      }
    else if (NODENAME (term_item) == affixnt)
      printf ("%s", FREPR (term_item));
    else
      printf ("\"%s\"", REPR (term_item));
    if (BROTHER (term_item) != nil)
      printf ("+");
  }
}

print_lattice (rule)
int rule;
{
  int mem;
  printf ("# Lattice %ld  @%lx\n%s :: {}\n\t", DEF (rule), NODENAME (rule), FREPR (rule));
  for (mem = SON (rule); mem != nil; mem = BROTHER (mem))
  {
    printf ("%s", FREPR (mem));
    if (BROTHER (mem) != nil)
      printf (";\t# @%lx\n\t", NODENAME (mem));
    else
      printf (".\t# @%lx\n\n", NODENAME (mem));
  }
}


print_rule (rule_item)
int rule_item;
{
  int alt_item, mem_item;
  printf ("  # rule id=%d\n", rule_item);
  if (MARKED (rule_item, is_predicate))
    printf ("  # Predicate\n");
  if (MARKED (rule_item, not_a_predicate))
    printf ("  # Not a Predicate\n");
  if (MARKED (rule_item, emptyrule))
    printf ("  # Empty\n");
  if (MARKED (rule_item, notemptyrule))
    printf ("  # Not empty\n");
  if (MARKED (rule_item, nondeterministic))
    printf ("  # Nondeterministic\n");
  if (MARKED (rule_item, deterministic))
    printf ("  # Deterministic\n");
  if (MARKED (rule_item, recursive))
    printf ("  # recursive\n");
  if (MARKED (rule_item, isvolatile))
    printf ("  # volatile\n");
  if (MARKED (rule_item, nonvolatile))
    printf ("  # nonvolatile\n");
  if (SON (rule_item))
  {
    printf ("  # %d mems\n", DEF (SON (rule_item)));
  }
  for (alt_item = SON (rule_item); alt_item != nil; alt_item = BROTHER (alt_item))
  {
    print_lhs (rule_item, alt_item);
    if (SON (alt_item) != nil)
    {
      if (BROTHER (SON (alt_item)) != nil)
        printf ("\n");
      print_rhs (SON (alt_item));
    }
    if (BROTHER (alt_item) != nil)
      printf (";\n");
  }
  printf (".\n\n");
}

print_lhs (rule_item, alt_item)
int rule_item, alt_item;
{
  printf ("%s", FREPR (rule_item));
  printaffixtree (AFFIXDEF (alt_item));
  if (MARKED (rule_item, external))
    printf ("=      ");
  else
    printf (":    ");

}

print_rhs (mem_item)
int mem_item;
{
  for (; mem_item != nil; mem_item = BROTHER (mem_item))
  {
    if (LKH (mem_item))
      printf ("?");
    if (TERMINAL (mem_item))
    {
      if (STRING (mem_item))
        printf ("\n    \"%s\"", REPR (mem_item));
      else
      {
        if (COMPLEMENT (mem_item))
          printf ("\n    ^");
        else
          printf ("\n     ");
        if (EXCLAMATIONSTARCHOICE (mem_item))
          printf ("!%s! * ", REPR (mem_item));
        else if (EXCLAMATIONPLUSCHOICE (mem_item))
          printf ("!%s! + ", REPR (mem_item));
        else
          printf ("!%s!  ", REPR (mem_item));
        printaffixtree (AFFIXTREE (mem_item));
      }
    }
    else
    {
      int lhs = DEF (mem_item);

      printf ("\n    %s", FREPR (lhs));
      printaffixtree (AFFIXTREE (mem_item));
    }
    if (BROTHER (mem_item) != nil)
      printf (",");
    if (FLAG_MARKED (mem_item, redirected_input))
      printf ("\n                                     # redirected");
    if (!TERMINAL (mem_item))
      if (MARKED (DEF (mem_item), is_predicate))
        printf ("\n                                     # predicate");
  }
}

printaffixtree (tree)
int tree;
{
  int term_item, fact_item;

  if (tree == nil)
    return;
  printf ("(");
  for (; tree != nil; tree = BROTHER (tree))
  {
    if ((NODENAME (tree) == inherited) && (DEF (tree) != -1))
    {
      if (MARKED_ADP (tree, affix_directed_parsing))
        printf ("#ADP#");
      if (MARKED_ADP (tree, no_affix_directed_parsing))
        printf ("#NO_ADP#");
    }
    if (NODENAME (tree) == inherited)
      printf (">");
    if (LATTICE (tree))
      printf ("%s", FREPR (SON (tree)));
    else
    {
      for (term_item = SON (tree); term_item != nil; term_item = BROTHER (term_item))
      {
        if (NODENAME (term_item) == factor)
          for (fact_item = SON (term_item); fact_item != nil; fact_item = BROTHER (fact_item))
          {
            if (NODENAME (fact_item) == affixnt)
              printf ("%s", FREPR (fact_item));
            else
              printf ("\"%s\"", REPR (fact_item));
            if (BROTHER (fact_item) != nil)
              printf ("^");
          }
        else if (NODENAME (term_item) == affixnt)
          printf ("%s", FREPR (term_item));
        else if (NODENAME (term_item) == metaffix)
          printf ("%s", FREPR (term_item));
        else if (NODENAME (term_item) == superaffix)
          printf ("@%s", FREPR (term_item));
        else
          printf ("\"%s\"", REPR (term_item));
        if (BROTHER (term_item) != nil)
          printf ("+");
      }
      if (NODENAME (tree) == derived)
        printf (">");
    }
    if (BROTHER (tree) != nil)
      printf (",");
  }
  printf (")");
}

print_cfg_tree ()
{
  int rule_item;
  for (rule_item = root; rule_item != laststdpred; rule_item = BROTHER (rule_item))
    print_cfg_rule (rule_item);

  for (rule_item = first_lattice; rule_item != nil; rule_item = BROTHER (rule_item))
    print_lattice (rule_item);

}

print_cfg_rule (rule_item)
int rule_item;
{
  int alt_item, mem_item;
  for (alt_item = SON (rule_item); alt_item != nil; alt_item = BROTHER (alt_item))
  {
    print_cfg_lhs (rule_item, alt_item);
    if (SON (alt_item) != nil)
    {
      if (BROTHER (SON (alt_item)) != nil)
        printf ("\n");
      print_cfg_rhs (SON (alt_item));
    }
    if (BROTHER (alt_item) != nil)
      printf (";\n");
  }
  printf (".\n\n");
}

print_cfg_lhs (rule_item, alt_item)
int rule_item, alt_item;
{
  printf ("%s", FREPR (rule_item));
  print_lattice_tree (AFFIXDEF (alt_item));
  if (MARKED (rule_item, external))
    printf (": ");
  else
    printf (": ");

}

print_cfg_rhs (mem_item)
int mem_item;
{
  int skip = 0;
  for (; mem_item != nil; mem_item = BROTHER (mem_item))
  {

    if ((!skip) && LKH (mem_item))
      printf ("?");
    if (TERMINAL (mem_item))
    {
      if (skip)
        continue;
      if (STRING (mem_item))
        printf ("    \"%s\"", REPR (mem_item));
      else
      {
        if (COMPLEMENT (mem_item))
          printf ("    ^");
        else
          printf ("     ");
        if (EXCLAMATIONSTARCHOICE (mem_item))
          printf ("!%s!*", REPR (mem_item));
        else if (EXCLAMATIONPLUSCHOICE (mem_item))
          printf ("!%s!+", REPR (mem_item));
        else
          printf ("!%s!", REPR (mem_item));
        print_lattice_tree (AFFIXTREE (mem_item));
      }
    }
    else
    {
      int lhs = DEF (mem_item);
      if (lhs == resetinputptr)
      {
        skip = 0;
        continue;
      }
      if (skip)
        continue;
      if (lhs == cut)
        printf ("->");
      else if (lhs == setinputptrto)
      {
        skip = 1;
        continue;
      }
      else
        printf ("    %s", FREPR (lhs));
      print_lattice_tree (AFFIXTREE (mem_item));
    }
    if (BROTHER (mem_item) != nil)
    {
      if (DEF (BROTHER (mem_item)) == cut)
        printf (",");
      else
        printf (",\n");
    }
  }
}

print_lattice_tree (tree)
int tree;
{
  int afx = tree;

  if (tree == nil)
    return;
  if (!lattice_in_tree (tree))
    return;

  printf ("(");
  for (; tree != nil; tree = BROTHER (tree))
    if (LATTICE (tree))
    {
      printf ("%s", FREPR (SON (tree)));
      if (lattice_in_tree (BROTHER (tree)))
        printf (",");
    }
  printf (")");
}

lattice_in_tree (afx)
int afx;
{
  for (; afx != nil; afx = BROTHER (afx))
    if (LATTICE (afx))
      return true;
  return false;
}

parse_tree ()
{
  register int ag, mem, afx, alt, rule;
  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
    for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
    {
      brother = nil;
      newnode (affixtm, brother, nil, ")\\n");
      add_parse_tree_term_lhs (SON (alt));
      newnode (affixtm, brother, nil, REPR (rule));
      newnode (affixtm, brother, nil, "(");
      newnode (derived, AFFIXDEF (alt), brother, "");
      AFFIXDEF (alt) = brother;
      for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
        if (!TERMINAL (mem))
        {
          newnode (affixnt, nil, nil, REPR (mem));
          newnode (derived, AFFIXTREE (mem), brother, "");
          AFFIXTREE (mem) = brother;
        }
    }

}

add_parse_tree_term_lhs (mem)
int mem;
{
  if (mem == nil)
    return;
  add_parse_tree_term_lhs (BROTHER (mem));
  add_parse_tree_term (mem);
}


add_parse_tree_term (mem)
{
  if (STRING (mem))
  {
    if (BROTHER (mem) != nil)
      newnode (affixtm, brother, nil, ",");
    newnode (affixtm, brother, nil, REPR (mem));
  }
  else if (TERMINAL (mem))
  {
    if (BROTHER (mem) != nil)
      newnode (affixtm, brother, nil, ",");
    newnode (affixnt, brother, nil, REPR (SON (AFFIXTREE (mem))));
  }
  else
  {
    if (BROTHER (mem) != nil)
      newnode (affixtm, brother, nil, ",");
    newnode (affixnt, brother, nil, REPR (mem));
  }
}

id_tree ()
{
  register int ag, mem, afx, alt, rule;
  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
    for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
    {
      brother = nil;
      add_id_tree_term_lhs (SON (alt));
      newnode (derived, AFFIXDEF (alt), brother, "");
      AFFIXDEF (alt) = brother;
      for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
        if ((!TERMINAL (mem)) && (mem > laststdpred))
        {
          newnode (affixnt, nil, nil, REPR (mem));
          newnode (derived, AFFIXTREE (mem), brother, "");
          AFFIXTREE (mem) = brother;
        }
    }
}

add_id_tree_term_lhs (mem)
int mem;
{
  if (mem == nil)
    return;
  add_id_tree_term_lhs (BROTHER (mem));
  if (mem > laststdpred)
    add_id_tree_term (mem);
}


add_id_tree_term (mem)
{
  if (STRING (mem))
    newnode (affixtm, brother, nil, REPR (mem));
  else if (TERMINAL (mem))
    newnode (affixnt, brother, nil, REPR (SON (AFFIXTREE (mem))));
  else
    newnode (affixnt, brother, nil, REPR (mem));

}

trick ()
{
  int rule, x;
  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
  {
    x = SON (rule);
    if (x == nil)
      continue;
    x = AFFIXDEF (x);
    if (x == nil)
      continue;
    x = SON (x);
    if (x == nil)
      continue;
    if (NODENAME (x) == affixtm)
      REPR (rule) = REPR (x) + 1;
  }
}
