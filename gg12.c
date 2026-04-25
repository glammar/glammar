
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
static long last_empty_alt = nil, empty_rule = nil;
static long first_empty_rule = nil, last_empty_rule = 0;


/* exports
void memopt ();
*/

static void make_empty_alts_empty_only ();
static void add_final_empty_alt (long alt);
static void new_display (long afx, long count);
static void add_empty_only_alt_to_shadow (long rule,long alt);
static void shadow_empty_mems (long mem);
static long no_derived_afx (long afx);
static char *new_repr (char *head);
static void link_empty_rules ();
static void link_empty_members (long alt, long rule);
static char *short_repr (long count);

void memopt () 
{
  long r;
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

char *ipterm = "ip_";

static void make_empty_alts_empty_only () 
{
  long alt, rule, mem, last_alt;
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

static void add_final_empty_alt (long alt) 
{
  long d1, d2;
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

static void new_display (long afx, long count) 
{
  long b;
  if (afx == nil)
    return;
  new_display (BROTHER (afx), count + 1);
  b = brother;
  newnode (affixnt, nil, nil, short_repr (count));
  newnode (NODENAME (afx), b, brother, "nil");
}

static void add_empty_only_alt_to_shadow (long rule,long alt) 
{
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

static void shadow_empty_mems (long mem) 
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

static long no_derived_afx (long afx) 
{
  for (; afx != nil; afx = BROTHER (afx))
    if (DERIVED (afx))
      return false;
  return true;
}

static char *new_repr (char *head) 
{
  long curchar;
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

static void link_empty_rules () 
{
  register long rule;
  for (rule = first_empty_rule; rule != nil; rule = BROTHER (rule))
  {
    long alt = SON (rule);
    for (; alt != nil; alt = BROTHER (alt))
      link_empty_members (alt, rule);
  }
}

static void link_empty_members (long alt, long rule) 
{
  register long member, rrule;
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

static char *short_repr (long count) 
{
  long curchar;
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
