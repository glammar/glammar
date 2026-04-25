
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 
*/
/* 
 *  file  : trace builtins
 *                  
 */

#include "gg1.h"
#include "gg2.h"
static long builtin;
extern long resetinputptr;
extern long nestaralt;
static long new_rule;


/* exports
void trace ();
void make_member (char *name);
*/
static long builtin_used ();
static void alias_used_builtins ();
static void make_new_rule (long b);
static void make_affix_tree (long afx);
static void make_alt (long alt);
static void make_rule (long b);

void trace () 
{
  alias_used_builtins ();
}

static long builtin_used () 
{
  long rule, alt, mem;
  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
    for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
      for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
        if ((builtin == DEF (mem)) && !(TERMINAL (mem)))
          return true;
  return false;
}


static void alias_used_builtins () 
{
  long rule, one_builtin_used = false;
  brother = laststdpred;
  for (builtin = laststdpred; builtin != nil; builtin = BROTHER (builtin))
    if ((builtin != cut) && (builtin != nestarset) &&
        (!mystrcmp (REPR (builtin), "select")) &&
        (!mystrcmp (REPR (builtin), "delete")) &&
        (builtin != tltraditionalterm) &&
        (builtin != transformlatticeterm) &&
        (builtin != tltraditional) &&
        (builtin != init_one_star) &&
        (builtin != transformlattice) &&
        (builtin != skip) &&
        (builtin != explintersect) &&
        (builtin != intersect) &&
        (builtin != getip) &&
        (builtin != falseip) &&
        (builtin != restoreip) &&
        (builtin != evalmeta) &&
        (builtin != initmeta) &&
        (builtin != evalmeta) && (builtin != resetinputptr) && (builtin != nestaralt) && (builtin != where))
    {
      long mem, alt;
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

static void make_new_rule (long b)
{
  brother = nil;
  make_affix_tree (AFFIXDEF (SON (builtin)));
  make_member (REPR (builtin));
  make_alt (SON (builtin));
  make_rule (b);
}

#define LATtice "LATTICE"
#define ALIst  "ALIST"
static void make_affix_tree (long afx)
{
  long sbrother;
  char *repr;
  if (afx == nil)
  {
    return;
  }
  repr= REPR(SON(afx));
  make_affix_tree (BROTHER (afx));
  sbrother = brother;
  newnode (affixnt, nil, nil, REPR (SON (afx)));
  if (!strncmp(repr, LATtice, sizeof(LATtice)-1))
     FLAG_SET(afx, TRC_LATTICE);
  else if (!strncmp(repr, ALIst, sizeof(ALIst)-1))
     FLAG_SET(afx, TRC_ALIST);
  newnode (NODENAME (afx), sbrother, brother, "(nil)");
}

void make_member (char *name) 
{
  newnode (ntnode, nil, brother, name);
  DEF (brother) = builtin;
}

static void make_alt (long alt) 
{
  long sons = brother;
  brother = nil;
  make_affix_tree (AFFIXDEF (alt));
  newnode (brother, nil, sons, REPR (builtin));
}

static void make_rule (long b) 
{
  newrulenode (NODENAME (builtin), b, brother, LINE (builtin), PART (builtin), REPR (builtin));
  if (MARKED (root, docompile))
    SET (brother, docompile);
  SET (brother, built_aliased);
  SET (brother, generated_rule);
  UNSET (brother, external);
}
