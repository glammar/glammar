
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 

 **********
 * file  : optimize memoizer benefits by making empties 
 *                  explicite
 * created: Wed Jul 25 16:45:09 MET DST 2012
 *
 * In order to increase memoizer speedup we consider to 
 * turn empties in empty onlies.
 * For instance if we could transform:
 *   a : b , c.   (1)
 *   b: "b"; .
 *   c :"c"; .
 *  
 * in something like:
 *   a : ip(ip>), b, c , false (>ip);  (2)
 *   a : .
 *   b: "b"; .
 *   c :"c"; .
 *
 * a major memoizer gain could be expected  since (2) could 
 * benifit from memoizing while  (1) does not because it produces
 * empty.
 * In (2) false(>ip) succeeds if (2) not recognized empty .
 *********
 *
 */

#include "gg1.h"
#include "gg2.h"
static int last_alt = true;
static int last_empty_alt = nil, empty_rule = nil;
static int first_empty_rule = nil, last_empty_rule = 0;

memopt ()
{
  int r;
  make_empty_alts_empty_only ();
  if (first_empty_rule != nil)
  {
    link_empty_rules ();
    for (r = root; BROTHER (r) != init_one_star; r = BROTHER (r));
    BROTHER (last_empty_rule) = init_one_star;
    BROTHER (r) = first_empty_rule;
  }
  else
    fprintf (stderr, "No empty alts?\n");
}

char *short_repr ();
char *new_repr ();
char *ipterm = "ip_";

make_empty_alts_empty_only ()
{
  int alt, rule, mem, last_alt;
  for (rule = root; rule != init_one_star; rule = BROTHER (rule))
    if (MARKED (rule, emptyrule))
    {
      empty_rule = nil;
      last_alt = nil;
      alt = SON (rule);
      if (SON (alt) == nil && BROTHER (alt) == nil)
      {
        add_empty_only_alt_to_shadow (rule, alt);
        continue;
      }
      for (; alt != nil; alt = BROTHER (alt))
      {
        for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
          if (MARKED (DEF (mem), notemptyrule))
            break;
        if (mem == nil)
        {
          add_empty_only_alt_to_shadow (rule, alt);

          if (SON (alt) == nil)
          {
            if (last_alt == nil)
              SON (rule) = BROTHER (alt);
            else
              BROTHER (last_alt) = BROTHER (alt);
          }
          else
          {
            /* get ip (ip>) */
            newnode (affixnt, nil, nil, ipterm);
            newnode (derived, nil, brother, "(nil)");
            newdefnode (ntnode, SON (alt), brother, getip, REPR (getip));
            SON (alt) = brother;

            for (mem = SON (alt); BROTHER (mem) != nil; mem = BROTHER (mem));

            /* false ip(>ip) */
            newnode (affixnt, nil, nil, ipterm);
            newnode (inherited, nil, brother, "(nil)");
            newdefnode (ntnode, nil, brother, falseip, REPR (falseip));
            BROTHER (mem) = brother;
          }
        }
        last_alt = alt;
      }
      for (last_alt = SON (rule); BROTHER (last_alt) != nil; last_alt = BROTHER (last_alt));
      add_final_empty_alt (last_alt);
    }
}

add_final_empty_alt (alt)
int alt;
{
  int d1, d2;
  brother = nil;
  new_display (AFFIXDEF (alt), 0);
  d1 = brother;
  brother = nil;
  new_display (AFFIXDEF (alt), 0);
  d2 = brother;

  newdefnode (ntnode, nil, d2, last_empty_rule, REPR (last_empty_rule));
  newnode (d1, nil, brother, "nil");
  BROTHER (alt) = brother;
}

new_display (afx, count)
int afx, count;
{
  int b;
  if (afx == nil)
    return;
  new_display (BROTHER (afx), count + 1);
  b = brother;
  newnode (affixnt, nil, nil, short_repr (count));
  newnode (NODENAME (afx), b, brother, "nil");
}

add_empty_only_alt_to_shadow (rule, alt)
int alt, rule;
{
  int b;
  brother = nil;

  shadow_empty_mems (SON (alt));
  if (empty_rule != nil && no_derived_afx (AFFIXDEF (alt)))
    return;

  newnode (NODENAME (alt), nil, brother, REPR (alt));
  if (empty_rule == nil)
  {
    last_empty_alt = brother;
    newnode (NODENAME (rule), nil, brother, new_repr (REPR (rule)));
    if (MARKED (root, docompile))
      SET (brother, docompile);

    empty_rule = brother;
    if (first_empty_rule == nil)
    {
      last_empty_rule = brother;
      first_empty_rule = brother;
    }
    else
    {
      BROTHER (last_empty_rule) = brother;
      last_empty_rule = brother;
    }
  }
  else
  {
    BROTHER (last_empty_alt) = brother;
    last_empty_alt = brother;
  }
}

shadow_empty_mems (mem)
int mem;
{
  if (mem == nil)
    return;
  shadow_empty_mems (BROTHER (mem));
  if (no_derived_afx (AFFIXTREE (mem)))
    return;
  if (DEF (mem) > BROTHER (init_one_star))
    newnode (NODENAME (mem), brother, SON (mem), new_repr (REPR (mem)));
  else
    newdefnode (NODENAME (mem), brother, SON (mem), DEF (mem), REPR (mem));
}

no_derived_afx (afx)
int afx;
{
  for (; afx != nil; afx = BROTHER (afx))
    if (DERIVED (afx))
      return false;
  return true;
}

char *new_repr (head)
char *head;
{
  int curchar;
  hashindex = 0;
  curchar = 0;
  do
  {
    chartable[charindex++] = *head;
    hashindex += *head++ << (++curchar & 7);
  }
  while (*head != '\0');
  chartable[charindex++] = '_';
  hashindex += '_' << (++curchar & 7);

  hashindex &= (maxnt - 1);
  chartable[charindex++] = '\0';
  addname ();
  return string;
}

link_empty_rules ()
{
  register int rule, alt, mem, afx;
  for (rule = first_empty_rule; rule != nil; rule = BROTHER (rule))
  {
    int alt = SON (rule);
    for (; alt != nil; alt = BROTHER (alt))
      link_empty_members (alt, rule);
  }
}

link_empty_members (alt, rule)
int alt, rule;
{
  register int member, rrule;
  for (member = SON (alt); member != nil; member = BROTHER (member))
  {
    if (DEF (member) != -1)
      continue;
    rrule = first_empty_rule;
    while (REPR (member) != REPR (rrule))
    {
      if (rrule == nil)
      {
        fprintf (stderr, " compiler error:line %ld in %s: member `%s' is not defined\n",
                 LINE (alt), REPR (rule), REPR (member));
        exit (10);
      }
      else
        rrule = BROTHER (rrule);
    }
    if (rrule == nil)
      break;
    DEF (member) = rrule;
  }
}

char *short_repr (count)
int count;
{
  int curchar;
  char c1 = 'A' + count % 26;
  hashindex = 0;
  curchar = 0;
  chartable[charindex++] = c1;
  hashindex += c1 << (++curchar & 7);
  chartable[charindex++] = '_';
  hashindex += '_' << (++curchar & 7);

  hashindex &= (maxnt - 1);
  chartable[charindex++] = '\0';
  addname ();
  return string;
}
