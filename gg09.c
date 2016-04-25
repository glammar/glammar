
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 

/* 
 *  file  : trace builtins
 *                  
 */

#include "gg1.h"
#include "gg2.h"
static int builtin;
extern int resetinputptr;
extern int nestaralt;
static int new_rule;

trace ()
{
  alias_used_builtins ();
}

builtin_used ()
{
  int rule, alt, mem;
  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
    for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
      for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
        if ((builtin == DEF (mem)) && !(TERMINAL (mem)))
          return true;
  return false;
}


alias_used_builtins ()
{
  int rule, one_builtin_used = false;
  brother = laststdpred;
  for (builtin = laststdpred; builtin != nil; builtin = BROTHER (builtin))
    if ((builtin != cut) && (builtin != nestarset) &&
        (!mystrcmp (REPR (builtin), "select")) &&
        (builtin != tltraditionalterm) &&
        (builtin != transformlatticeterm) &&
        (builtin != tltraditional) &&
        (builtin != transformlattice) &&
        (builtin != skip) &&
        (builtin != explintersect) &&
        (builtin != getip) &&
        (builtin != falseip) &&
        (builtin != restoreip) &&
        (builtin != evalmeta) &&
        (builtin != initmeta) &&
        (builtin != evalmeta) && (builtin != resetinputptr) && (builtin != nestaralt) && (builtin != where))
    {
      int mem, alt;
      char *n, *m = REPR (builtin);
      if (!builtin_used ())
        continue;
      one_builtin_used = true;
      if (det_flag)
        if ((builtin == tltraditional) || (builtin == tltraditionalterm)
            || (mystrcmp (REPR (builtin), "select")) || (mystrcmp (REPR (builtin), "delete")))
          continue;
      make_new_rule (brother);
      new_rule = brother;

      n = (char *) malloc (strlen (m) + 7);
      if (n == NULL)
        fprintf (stderr, "glammar: Out of memory.\n"), exit (-1);
      strcpy (n, "built_"), strcpy (n + 6, m);
      REPR (new_rule) = n;
      SET (new_rule, external);
      PART (new_rule) = PART (root);
      for (rule = root; rule != laststdpred; rule = BROTHER (rule))
        for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
          for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
            if ((builtin == DEF (mem)) && !(TERMINAL (mem)))
            {
              DEF (mem) = new_rule;
              REPR (mem) = n;
            }
    }
  if (!one_builtin_used)
    return;
  for (rule = root; BROTHER (rule) != laststdpred; rule = BROTHER (rule));
  BROTHER (rule) = new_rule;
}

make_new_rule (b)
{
  brother = nil;
  make_affix_tree (AFFIXDEF (SON (builtin)));
  make_member (REPR (builtin));
  make_alt (SON (builtin));
  make_rule (b);
}

make_affix_tree (afx)
{
  int sbrother;
  if (afx == nil)
  {
    return;
  }
  make_affix_tree (BROTHER (afx));
  sbrother = brother;
  newnode (affixnt, nil, nil, REPR (SON (afx)));
  newnode (NODENAME (afx), sbrother, brother, "(nil)");
}

make_member (name)
char *name;
{
  newnode (ntnode, nil, brother, name);
  DEF (brother) = builtin;
}

make_alt (alt)
int alt;
{
  int sons = brother;
  brother = nil;
  make_affix_tree (AFFIXDEF (alt));
  newnode (brother, nil, sons, REPR (builtin));
}

make_rule (b)
int b;
{
  newrulenode (NODENAME (builtin), b, brother, LINE (builtin), PART (builtin), REPR (builtin));
  if (MARKED (root, docompile))
    SET (brother, docompile);
  SET (brother, built_aliased);
  SET (brother, generated_rule);
  UNSET (brother, external);
}
